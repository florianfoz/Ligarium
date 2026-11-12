#ifndef LIGARIUM_LINK_H
#define LIGARIUM_LINK_H

#include "ligarium/ligarium.h"
#include "ligarium/record.h"

#include <QList>
#include <QStringView>
#include <utility>

class QSqlQuery;

namespace ligarium
{


template <typename TARGET, ERelation RELATION>
class Link;

template <typename TARGET>
class Link<TARGET, ERelation::ManyToMany>
{
  static_assert(false, "Invalid Link with ManyToMany, make a association table instead.");
};

/**
 * @brief A one-to-one relationship.
 */
template <typename TARGET>
class Link<TARGET, ERelation::OneToOne>
{
public:
  static constexpr bool is_sql_column = true;

  [[nodiscard]]
  qsizetype id() const noexcept
  {
    return m_id;
  }

  void set_id(qsizetype id) noexcept
  {
    m_id = id;
  }

  [[nodiscard]]
  bool is_valid() const noexcept
  {
    return m_id != INVALID_ID;
  }

  [[nodiscard]]
  TARGET get(Database& db) const
  {
    return ligarium::read_record<TARGET>(db, m_id);
  }

  friend bool operator==(const Link&, const Link&) = default;

private:
  qsizetype m_id = INVALID_ID;
};

/**
 * @brief A many-to-one relationship.
 */
template <typename TARGET>
class Link<TARGET, ERelation::ManyToOne>
{
public:
  static constexpr bool is_sql_column = true;

  [[nodiscard]]
  qsizetype id() const noexcept
  {
    return m_id;
  }

  void set_id(qsizetype id) noexcept
  {
    m_id = id;
  }

  [[nodiscard]]
  bool is_valid() const noexcept
  {
    return m_id != INVALID_ID;
  }

  [[nodiscard]]
  TARGET get(Database& db) const
  {
    return ligarium::read_record<TARGET>(db, m_id);
  }

  friend bool operator==(const Link&, const Link&) = default;

private:
  qsizetype m_id = INVALID_ID;
};

/**
 * @brief A one-to-many relationship.
 */
template <typename TARGET>
class Link<TARGET, ERelation::OneToMany>
{
public:
  static constexpr bool is_sql_column = false;

  void set_ids(QList<qsizetype> ids) noexcept
  {
    m_ids = std::move(ids);
  }

  [[nodiscard]]
  QList<qsizetype>& ids() noexcept
  {
    return m_ids;
  }

  [[nodiscard]]
  const QList<qsizetype>& ids() const noexcept
  {
    return m_ids;
  }

  [[nodiscard]]
  bool empty() const noexcept
  {
    return m_ids.isEmpty();
  }

  [[nodiscard]]
  qsizetype size() const noexcept
  {
    return m_ids.size();
  }

  [[nodiscard]]
  QList<TARGET> get(Database& db) const
  {
    QList<TARGET> records;
    records.reserve(m_ids.size());

    for (const qsizetype id : m_ids) {
      records.append(ligarium::read_record<TARGET>(db, id));
    }

    return records;
  }

  friend bool operator==(const Link&, const Link&) = default;

private:
  QList<qsizetype> m_ids;
};


/**
 * @brief A polymorphic relationship.
 */
template <typename SOURCE>
class Link<SOURCE, ERelation::PolymorphicManyToOne>
{
public:
  static constexpr bool is_sql_column = true;

  [[nodiscard]]
  qsizetype id() const noexcept
  {
    return m_id;
  }

  void set_id(qsizetype id) noexcept
  {
    m_id = id;
  }

  [[nodiscard]]
  ligarium::Table table() const noexcept
  {
    return m_table;
  }

  void set_table(ligarium::Table table) noexcept
  {
    m_table = table;
  }


  [[nodiscard]]
  bool is_valid() const noexcept
  {
    return m_id != INVALID_ID && m_table != ligarium::Table(0);
  }

  template <typename TARGET>
  [[nodiscard]]
  TARGET get(Database& db) const
  {
    if (TARGET::static_table == m_table) return ligarium::read_record<TARGET>(db, m_id);
    return TARGET::invalid();
  }


private:
  ligarium::Table m_table = ligarium::Table(0);
  qsizetype       m_id    = INVALID_ID;
};

/**
 * @brief SQL metadata for a relationship.
 */
struct SqlLinkSpec {
  Table owner_table{};

  QStringView local_column;

  Table       target_table{};
  QStringView foreign_column;

  Table       association_table{};
  QStringView association_owner_column;
  QStringView association_target_column;
};

/**
 * @brief SQL metadata for a Link member.
 */
template <RecordFieldType RECORD, typename TARGET, ERelation _RELATION>
struct SqlLinkField {
  static constexpr ERelation RELATION = _RELATION;

  static_assert(RELATION != ERelation::ManyToMany, "Illegal link field type");

  using Is_Link = void;

  using record_type = RECORD;
  using target_type = TARGET;

  QStringView            name;
  Link<TARGET, RELATION> RECORD::* member;
  SqlLinkSpec                      spec;


  void read(RECORD& record, const QSqlQuery& query) const
  {
    // polymorphic convention : name_table, name_id
    if constexpr (RELATION == ERelation::PolymorphicManyToOne) {
      (record.*member).set_table(ligarium::Table(query.value(name + "_table").toLongLong()));
      (record.*member).set_id(query.value(name + "_id").toLongLong());
      return;
    }
    // record field
    else if constexpr (RELATION == ERelation::ManyToOne || RELATION == ERelation::ManyToOne) {
      (record.*member).set_id(query.value(name).toLongLong());
      return;
    }
    // non record field
  }

  void bind(QSqlQuery& query, const RECORD& record) const
  {
    using T = std::remove_cvref_t<decltype(this)>;

    // polymorphic convention : name_table, name_id
    if constexpr (RELATION == ERelation::PolymorphicManyToOne) {
      query.bindValue(":" + name + "_table", qsizetype((record.*member).table()));
      query.bindValue(":" + name + "_id", (record.*member).id());
      return;
    }
    // record field
    else if constexpr (RELATION == ERelation::ManyToOne || RELATION == ERelation::ManyToOne) {
      query.bindValue(":" + name, (record.*member).id());
      return;
    }
    // non record field
  }
};

template <RecordFieldType RECORD, typename TARGET>
constexpr auto link_OneToOne(QStringView name, Link<TARGET, ERelation::OneToOne> RECORD::* member)
{
  return SqlLinkField<RECORD, TARGET, ERelation::OneToOne>{
      .name   = name,
      .member = member,
      .spec =
          SqlLinkSpec{
                      .owner_table  = RECORD::static_table,
                      .local_column = name,
                      },
  };
}

template <RecordFieldType RECORD, typename TARGET>
constexpr auto link_ManyToOne(QStringView name, Link<TARGET, ERelation::ManyToOne> RECORD::* member)
{
  return SqlLinkField<RECORD, TARGET, ERelation::ManyToOne>{
      .name   = name,
      .member = member,
      .spec =
          SqlLinkSpec{
                      .owner_table  = RECORD::static_table,
                      .local_column = name,
                      },
  };
}

template <RecordFieldType RECORD, typename TARGET>
constexpr auto link_OneToMany(
    QStringView name, Link<TARGET, ERelation::OneToMany> RECORD::* member, Table target_table,
    QStringView foreign_column
)
{
  return SqlLinkField<RECORD, TARGET, ERelation::OneToMany>{
      .name   = name,
      .member = member,
      .spec =
          SqlLinkSpec{
                      .owner_table    = RECORD::static_table,
                      .target_table   = target_table,
                      .foreign_column = foreign_column,
                      },
  };
}

template <RecordFieldType RECORD, RecordFieldType TARGET>
constexpr auto
link_PolymorphicManyToOne(QStringView name, Link<TARGET, ERelation::PolymorphicManyToOne> RECORD::* member)
{
  return SqlLinkField<RECORD, TARGET, ERelation::PolymorphicManyToOne>{
      .name   = name,
      .member = member,
      .spec =
          SqlLinkSpec{
                      .owner_table = RECORD::static_table,
                      },
  };
}

QList<qsizetype> read_link_ids(const Database& db, ERelation relation, const SqlLinkSpec& spec, qsizetype owner_id);

bool save_link_ids(
    const Database& db, ERelation relation, const SqlLinkSpec& spec, qsizetype owner_id,
    const QList<qsizetype>& target_ids
);

bool save_polymorphic_link(
    const Database& db, const SqlLinkSpec& spec, qsizetype owner_id, const QVector<qsizetype>& target_ids
);

} // namespace ligarium

#endif // LIGARIUM_LINK_H
