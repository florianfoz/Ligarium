#ifndef RECORD_H
#define RECORD_H

#include "ligarium.h"
//
#include "database.h"

#include <QSqlQuery>

namespace
{

/**
 * @brief Reads fields stored directly in the main record table.
 *
 * OneToOne and ManyToOne links are stored as foreign-key columns in the
 * current record, so their values are read from the main SQL query.
 *
 * OneToMany and ManyToMany links do not belong to the main query and are
 * therefore ignored here.
 */
template <Ligarium::RecordType RECORD>
void read_record_fields(RECORD& record, const QSqlQuery& query)
{
  std::apply(
      [&](const auto&... fields) {
        (
            [&] {
              if constexpr (requires { fields.read(record, query); }) {
                fields.read(record, query);
              }
            }(),
            ...);
      },
      RECORD::sql_fields());
}


/**
 * @brief Loads external relationships for a record.
 *
 * OneToMany and ManyToMany relationships are stored outside the current
 * record table and therefore require separate SQL queries.
 */
template <Ligarium::RecordType RECORD>
void read_record_links(RECORD& record)
{
  std::apply(
      [&](const auto&... fields) {
        (
            [&] {
              // A relationship field is identified by its "member"
              // and "spec" members.
              if constexpr (requires {
                              fields.member;
                              fields.spec;
                            }) {
                using FIELD = std::remove_cvref_t<decltype(fields)>;

                if constexpr (requires { FIELD::relation; }) {
                  if constexpr (FIELD::relation == Ligarium::ERelation::OneToMany
                                || FIELD::relation == Ligarium::ERelation::ManyToMany) {
                    const QList<qsizetype> ids = read_link_ids(FIELD::relation, fields.spec, record.id);

                    (record.*(fields.member)).ids = ids;
                  }
                }
              }
            }(),
            ...);
      },
      RECORD::sql_fields());
}
} // namespace

namespace Ligarium
{
[[nodiscard]]
inline qsizetype create_record(Database& db, Table table)
{
  return db.insert(table);
}
[[nodiscard]]
inline bool contains_record(Database& db, Table table, qsizetype id)
{
  if (table == Table(0) || id == INVALID_ID) return false;

  return db.contains(table, id);
}
[[nodiscard]]
inline bool delete_record(Database& db, Table table, qsizetype id, bool wmsg = false, const QString& msg = {})
{
  if (table == Table(0) || id == INVALID_ID) return false;

  return db.remove(table, id);
}

template <RecordType T>
[[nodiscard]]
T read_record(Database& db, qsizetype id)
{
  if (id == INVALID_ID) return T{};

  auto valid_sql = db.find(T::static_table, id);

  if (!valid_sql) return T{};

  auto& query = valid_sql.value();

  if (!query.isValid()) return T{};

  return from_sql<T>(db, query);
}


template <RecordType T>
[[nodiscard]]
bool save_record(T* rec)
{
  if (!rec) return false;

  auto valid_sql = rec->database()->find(T::static_table, rec->id());

  if (!valid_sql) return false;

  return save_record_fields(valid_sql.value(), *rec);
}

template <RecordType T>
[[nodiscard]]
T from_sql(Database& db, QSqlQuery& query)
{
  T out;

  out.set_database(&db);

  if (!query.isValid()) {
    if (!query.next()) return T{};
  }

  // Read columns belonging to the current record.
  read_record_fields(out, query);

  out.set_id(query.value("id").toLongLong());

  // Read relationships stored in other tables.
  read_record_links(out);

  return out;
}

template <RecordType T>
[[nodiscard]]
QList<T> all_records(const Database& db)
{
  auto valid_sql = db.all(T::static_table);

  if (!valid_sql) return {};

  auto& query = valid_sql.value();

  if (!query.isActive()) return {};

  QList<T> records;

  while (query.next()) {
    records.append(from_sql<T>(const_cast<Database&>(db), query));
  }

  return records;
}

[[nodiscard]]
inline QList<qsizetype> all_records_id(const Database& db, Table table)
{
  auto valid_sql = db.all_ids(table);

  if (!valid_sql) return {};

  auto& query = valid_sql.value();

  if (!query.isActive()) return {};

  QList<qsizetype> ids;

  while (query.next()) {
    ids.append(query.value("id").toLongLong());
  }

  return ids;
}


template <RecordType T>
[[nodiscard]]
bool is_dirty(const Database& db, const T& _this, qsizetype other_id = INVALID_ID)
{
  if (other_id != INVALID_ID) return _this != Ligarium::read_record<T>(const_cast<Database&>(db), other_id);

  if (_this.id() == INVALID_ID) return true;

  return _this != Ligarium::read_record<T>(const_cast<Database&>(db), _this.id());
}

template <class DERIVED>
class Record
{
public:
  explicit Record(Database* db = nullptr)
    : m_db(db)
  {
  }


  [[nodiscard]]
  qsizetype id() const
  {
    return m_id;
  }

  void set_id(qsizetype id)
  {
    m_id = id;
  }

  [[nodiscard]]
  Database* database() const
  {
    return m_db;
  }

  void set_database(Database* db)
  {
    m_db = db;
  }

  [[nodiscard]]
  virtual bool is_record() const
  {
    return true;
  }

  [[nodiscard]]
  bool is_relation() const
  {
    return !is_record();
  }

  [[nodiscard]]
  static DERIVED create_record(Database& db)
  {
    auto id = Ligarium::create_record(db, DERIVED::static_table);
    return Ligarium::read_record<DERIVED>(db, id);
  }

  [[nodiscard]]
  static DERIVED read_record(Database& db, qsizetype id)
  {
    if (id == INVALID_ID) return DERIVED();
    return Ligarium::read_record<DERIVED>(db, id);
  }

  [[nodiscard]]
  static QList<DERIVED> all_records(const Database& db)
  {
    return Ligarium::all_records<DERIVED>(db);
  }

  [[nodiscard]]
  static QList<qsizetype> all_records_id(const Database& db)
  {
    return Ligarium::all_records_id(db, DERIVED::static_table);
  }

  [[nodiscard]]
  bool delete_record(bool wmsg = false, const QString& msg = {})
  {
    if (m_db == nullptr) return false;
    if (Ligarium::delete_record(*m_db, DERIVED::static_table, m_id, wmsg, msg)) m_id = INVALID_ID;
    return m_id == INVALID_ID;
  }

  [[nodiscard]]
  bool save_record()
  {
    if (m_db == nullptr) return false;
    if (m_id == INVALID_ID) m_id = Ligarium::create_record(*m_db, DERIVED::static_table);
    return Ligarium::save_record<DERIVED>(static_cast<DERIVED*>(this));
  }

  [[nodiscard]] bool is_newer(qsizetype other_id = INVALID_ID) const
  {
    return is_dirty(other_id);
  }


  // is a newer data in fact
  [[nodiscard]] bool is_dirty(qsizetype other_id = INVALID_ID) const
  {
    if (m_db == nullptr) return false;
    return Ligarium::is_dirty<DERIVED>(*m_db, *static_cast<const DERIVED*>(this), other_id);
  }

  [[nodiscard]] virtual QString dump() const
  {
    return QObject::tr("UNDEFINED RECORD");
  };


  [[nodiscard]]
  bool is_valid() const
  {
    if (m_db == nullptr) return false;
    return Ligarium::contains_record(*m_db, DERIVED::static_table, m_id);
  }

  explicit operator bool() const noexcept
  {
    return is_valid();
  }

private:
  qsizetype m_id = INVALID_ID;

  Database* m_db = nullptr;
};

} // namespace Ligarium


#endif // RECORD_H