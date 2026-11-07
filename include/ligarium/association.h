#ifndef ASSOCIATION_H
#define ASSOCIATION_H

#include "ligarium/database.h"

namespace ligarium
{

template <typename T>
concept AssociationType = requires(T t) {
  typename T::KEY_TYPE;
  typename T::VAL_TYPE;
  requires std::same_as<decltype(t.database()), Database*>;
  { T::sql_fields() };
};

namespace
{
/**
 * @brief Reads fields stored directly in the main association table.
 *
 * OneToOne and ManyToOne links are stored as foreign-key columns in the
 * current record, so their values are read from the main SQL query.
 *
 * OneToMany link do not belong to the main query and are
 * therefore ignored here.
 */
template <ligarium::AssociationType ASSO>
void read_association_fields(ASSO& association, const QSqlQuery& query)
{
  std::apply(
      [&](const auto&... fields) {
        (
            [&] {
              if constexpr (requires { fields.read(association, query); }) {
                fields.read(association, query);
              }
            }(),
            ...);
      },
      ASSO::sql_fields());
}


/**
 * @brief Loads external relationships for a record.
 *
 * OneToMany relationship is stored outside the current
 * record table and therefore require separate SQL queries.
 */
template <ligarium::AssociationType ASSO>
void read_assocition_links(ASSO& association)
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
                  if constexpr (FIELD::relation == ligarium::ERelation::OneToMany) {
                    const QList<qsizetype> ids =
                        read_link_ids(*association.database(), FIELD::relation, fields.spec, association.id());

                    (association.*(fields.member)).set_ids(ids);
                  }
                }
              }
            }(),
            ...);
      },
      ASSO::sql_fields());
}
} // namespace

namespace ligarium
{
[[nodiscard]]
inline qsizetype create_association(Database& db, Table table1, Table table2)
{
  return db.insert_association(table1, table2);
}
[[nodiscard]]
inline bool contains_association(Database& db, Table table1, Table table2, Table search_table, qsizetype id)
{
  if (table1 == Table(0) || table2 == Table(0) || search_table == Table(0) || id == INVALID_ID) return false;

  return db.contains_association(table1, table2, search_table, id);
}
[[nodiscard]]
inline bool delete_association(Database& db, Table table1, Table table2, Table search_table, qsizetype id,
                               bool wmsg = false, const QString& msg = {})
{
  if (table1 == Table(0) || table2 == Table(0) || id == INVALID_ID) return false;

  return db.remove_association(table1, table2, search_table, id);
}

template <AssociationType ASSO>
[[nodiscard]]
ASSO read_association(Database& db, QSqlQuery& query)
{
  ASSO out;

  out.set_database(&db);

  if (!query.isValid()) return ASSO::invalid();

  return db.
}


template <AssociationType ASSO>
[[nodiscard]]
ASSO read_association(Database& db, Table search_table, qsizetype search_id)
{
  if (!db.is_open() || search_id == INVALID_ID || search_table == Table(0)) return ASSO::invalid();

  auto valid_sql =
      db.find_association(ASSO::KEY_TYPE::static_table, ASSO::VAL_TYPE::static_table, search_table, search_id);

  if (!valid_sql) return ASSO::invalid();

  QSqlQuery& query = valid_sql.value();

  if (!query.isValid()) return ASSO::invalid();

  return association_from_sql()
}


template <RecordType KEY, RecordType VAL, typename DERIVED>
class Association
{
public:
  using KEY_TYPE = KEY;
  using VAL_TYPE = VAL;

  [[nodiscard]]
  Database* database() const
  {
    return m_db;
  }

  void set_database(Database* db)
  {
    m_db = db;
  }


  KEY_TYPE& get_key()
  {
    return m_key;
  }

  const KEY_TYPE& get_key() const
  {
    return m_key;
  }

  VAL_TYPE& get_val()
  {
    return m_key;
  }

  const VAL_TYPE& get_val() const
  {
    return m_val;
  }

  DERIVED read_association(Database& db, qsizetype id)
  {
    if (!db.is_open()) return DERIVED{};
    if (id == INVALID_ID) {
      DERIVED out{};
      out.set_database(&db);
      return out;
    }
    return ligarium::read_association<DERIVED>(db, id);
  }

  DERIVED create_association(Database& db)
  {
  }

  [[nodiscard]]
  static DERIVED invalid()
  {
    return DERIVED{};
  }


private:
  KEY_TYPE m_key;
  VAL_TYPE m_val;

  Database* m_db = nullptr;
};

} // namespace ligarium

#endif // ASSOCIATION_H