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


template <RecordFieldType TARGET, ERelation RELATION>
class Link;

/**
 * @brief A one-to-one relationship.
 */
template <RecordFieldType TARGET>
class Link<TARGET, ERelation::OneToOne>
{
public:
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
template <RecordFieldType TARGET>
class Link<TARGET, ERelation::ManyToOne>
{
public:
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
template <RecordFieldType TARGET>
class Link<TARGET, ERelation::OneToMany>
{
public:
  void set_ids(QList<qsizetype> ids)
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
 * @brief A many-to-many relationship.
 */
template <RecordFieldType TARGET>
class Link<TARGET, ERelation::ManyToMany>
{
public:
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
template <RecordFieldType TARGET>
class Link<TARGET, ERelation::PolymorphicOneToMany>
{
public:
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

  [[nodiscard]] bool empty() const noexcept
  {
    return m_ids.isEmpty();
  }

  [[nodiscard]] qsizetype size() const noexcept
  {
    return m_ids.size();
  }

  [[nodiscard]] QList<TARGET> get(Database& db) const
  {
    QList<TARGET> result;
    result.reserve(m_ids.size());

    for (const qsizetype id : m_ids) result.append(read_record<TARGET>(db, id));

    return result;
  }


private:
  QList<qsizetype> m_ids;
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
template <RecordFieldType RECORD, RecordFieldType TARGET, ERelation RELATION>
struct SqlLinkField {
  static constexpr ERelation relation = RELATION;

  using record_type = RECORD;
  using target_type = TARGET;

  QStringView            name;
  Link<TARGET, RELATION> RECORD::* member;
  SqlLinkSpec                      spec;
};

template <RecordFieldType RECORD, RecordFieldType TARGET>
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

template <RecordFieldType RECORD, RecordFieldType TARGET>
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

template <RecordFieldType RECORD, RecordFieldType TARGET>
constexpr auto link_OneToMany(QStringView name, Link<TARGET, ERelation::OneToMany> RECORD::* member, Table target_table,
                              QStringView foreign_column)
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
constexpr auto link_ManyToMany(QStringView name, Link<TARGET, ERelation::ManyToMany> RECORD::* member,
                               Table association_table, QStringView association_owner_column,
                               QStringView association_target_column)
{
  return SqlLinkField<RECORD, TARGET, ERelation::ManyToMany>{
      .name   = name,
      .member = member,
      .spec =
          SqlLinkSpec{
                      .owner_table               = RECORD::static_table,
                      .association_table         = association_table,
                      .association_owner_column  = association_owner_column,
                      .association_target_column = association_target_column,
                      },
  };
}

template <RecordFieldType RECORD, RecordFieldType TARGET>
constexpr auto link_PolymorphicOneToMany(QStringView                                   name,
                                         Link<TARGET, ERelation::PolymorphicOneToMany> RECORD::* member,
                                         Table                                                   target_table)
{
  return SqlLinkField<RECORD, TARGET, ERelation::PolymorphicOneToMany>{
      .name   = name,
      .member = member,
      .spec =
          SqlLinkSpec{
                      .owner_table  = RECORD::static_table,
                      .target_table = target_table,
                      },
  };
}

QList<qsizetype> read_link_ids(const Database& db, ERelation relation, const SqlLinkSpec& spec, qsizetype owner_id);

bool save_link_ids(const Database& db, ERelation relation, const SqlLinkSpec& spec, qsizetype owner_id,
                   const QList<qsizetype>& target_ids);

bool save_polymorphic_link(const Database& db, const SqlLinkSpec& spec, qsizetype owner_id,
                           const QVector<qsizetype>& target_ids);

} // namespace ligarium

#endif // LIGARIUM_LINK_H
