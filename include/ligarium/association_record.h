#ifndef ASSOCIATION_RECORD_H
#define ASSOCIATION_RECORD_H

#include "link.h"
#include "record.h"

#include <QSqlDriver>
#include <QSqlError>
#include <concepts>
#include <optional>
#include <type_traits>

namespace ligarium
{

template <typename DERIVED>
class AssociationRecord : public Record<DERIVED>
{
public:
  // ---------------------------------------------------------------------------
  // Read complete associations
  // ---------------------------------------------------------------------------

  template <typename TARGET>
  static QList<DERIVED>
  read_associations(Database& database, qsizetype target_id, Link<TARGET, ERelation::ManyToOne> DERIVED::* member)
  {
    const auto column = column_for_member(member);
    if (!column) return {};

    auto query = database.query(
        QStringLiteral("SELECT * FROM %1 WHERE %2 = :target_id").arg(Table_to_str(DERIVED::static_table), *column),
        {
            {":target_id", target_id}
    }
    );

    if (!database.execute(query)) return {};

    QList<DERIVED> records;

    while (database.next(query)) records.append(from_sql<DERIVED>(database, query));

    return records;
  }

  template <typename LEFT, typename RIGHT>
  static QList<DERIVED> read_between(
      Database& database, qsizetype left_id, Link<LEFT, ERelation::ManyToOne> DERIVED::* left_member,
      qsizetype right_id, Link<RIGHT, ERelation::ManyToOne> DERIVED::* right_member
  )
  {
    const auto left_column  = column_for_member(left_member);
    const auto right_column = column_for_member(right_member);

    if (!left_column || !right_column) return {};

    auto query = database.query(
        QStringLiteral("SELECT * FROM %1 WHERE %2 = :left_id AND %3 = :right_id")
            .arg(Table_to_str(DERIVED::static_table), *left_column, *right_column),
        {
            {":left_id",  left_id },
            {":right_id", right_id}
    }
    );

    if (!database.execute(query)) return {};

    QList<DERIVED> records;

    while (database.next(query)) records.append(from_sql<DERIVED>(database, query));

    return records;
  }

  // ---------------------------------------------------------------------------
  // Read association IDs only
  // ---------------------------------------------------------------------------

  template <typename TARGET>
  static QList<qsizetype>
  read_association_ids(Database& database, qsizetype target_id, Link<TARGET, ERelation::ManyToOne> DERIVED::* member)
  {
    const auto column = column_for_member(member);
    if (!column) return {};

    auto query = database.query(
        QStringLiteral("SELECT * FROM %1 WHERE %2 = :target_id").arg(Table_to_str(DERIVED::static_table), *column),
        {
            {":target_id", target_id}
    }
    );

    if (!query.exec()) {
      qDebug() << "Exec error:" << query.lastError();
      return {};
    }

    if (!database.execute(query)) return {};

    QList<qsizetype> ids;

    while (database.next(query)) ids.append(query.value(0).toLongLong());

    return ids;
  }

  template <typename LEFT, typename RIGHT>
  static QList<qsizetype> read_between_ids(
      Database& database, qsizetype left_id, Link<LEFT, ERelation::ManyToOne> DERIVED::* left_member,
      qsizetype right_id, Link<RIGHT, ERelation::ManyToOne> DERIVED::* right_member
  )
  {
    const auto left_column  = column_for_member(left_member);
    const auto right_column = column_for_member(right_member);

    if (!left_column || !right_column) return {};

    auto query = database.query(
        QStringLiteral("SELECT id FROM %1 WHERE %2 = :left_id AND %3 = :right_id")
            .arg(Table_to_str(DERIVED::static_table), *left_column, *right_column),
        {
            {":left_id",  left_id },
            {":right_id", right_id}
    }
    );

    if (!database.execute(query)) return {};

    QList<qsizetype> ids;

    while (database.next(query)) ids.append(query.value(0).toLongLong());

    return ids;
  }

  // ---------------------------------------------------------------------------
  // Count associations
  // ---------------------------------------------------------------------------

  template <typename TARGET>
  static std::optional<qsizetype>
  count_associations(Database& database, qsizetype target_id, Link<TARGET, ERelation::ManyToOne> DERIVED::* member)
  {
    const auto column = column_for_member(member);
    if (!column) return std::nullopt;

    auto query = database.query(
        QStringLiteral("SELECT COUNT(*) FROM %1 WHERE %2 = :target_id")
            .arg(Table_to_str(DERIVED::static_table), *column),
        {
            {":target_id", target_id}
    }
    );

    if (!database.execute_next(query)) return std::nullopt;

    return query.value(0).toLongLong();
  }


  // ---------------------------------------------------------------------------
  // Synchronize associations
  // ---------------------------------------------------------------------------

  template <typename OWNER, typename TARGET, typename ASSIGNER>
  static bool sync_associations(
      Database& database, qsizetype owner_id, Link<OWNER, ERelation::ManyToOne> DERIVED::* owner_member,
      const QList<qsizetype>& target_ids, Link<TARGET, ERelation::ManyToOne> DERIVED::* target_member, ASSIGNER&& assign
  )
  {
    if (target_ids.empty()) return true;
    // Validate members.
    const auto owner_column  = column_for_member(owner_member);
    const auto target_column = column_for_member(target_member);

    if (!owner_column || !target_column || owner_id == INVALID_ID) return false;

    // Build requested associations.
    QSet<qsizetype> requested;

    for (qsizetype target_id : target_ids) {
      if (target_id == INVALID_ID) return false;

      requested.insert(target_id);
    }

    // Start transaction.
    if (!database.connection().transaction()) return false;

    const auto rollback = [&database]() {
      database.connection().rollback();
      return false;
    };

    // Load existing associations.

    auto query = database.query(
        QStringLiteral("SELECT * FROM %1 WHERE %2 = :owner_id").arg(Table_to_str(DERIVED::static_table), *owner_column),
        {
            {":owner_id", owner_id}
    }
    );

    if (!database.execute(query)) return rollback();

    QHash<qsizetype, DERIVED> existing;

    while (database.next(query)) {
      DERIVED association = from_sql<DERIVED>(database, query);

      const qsizetype target_id = (association.*target_member).id();

      existing.insert(target_id, std::move(association));
    }

    // Remove obsolete associations.
    for (auto it = existing.begin(); it != existing.end();) {
      if (!requested.contains(it.key())) {
        if (!database.remove(DERIVED::static_table, it.value().id())) return rollback();

        it = existing.erase(it);
      } else {
        ++it;
      }
    }

    // Create or update associations.
    for (qsizetype target_id : requested) {
      DERIVED* association = nullptr;

      if (existing.contains(target_id)) {
        association = &existing[target_id];
      } else {
        DERIVED record;
        record.set_database(&database);

        (record.*owner_member).set_id(owner_id);
        (record.*target_member).set_id(target_id);

        existing.insert(target_id, std::move(record));
        association = &existing[target_id];
      }

      // Initialize additional business fields.
      std::invoke(assign, *association);

      if (!association->save_record()) return rollback();
    }

    // Commit transaction.
    if (!database.connection().commit()) return rollback();

    return true;
  }


  template <typename OWNER, typename TARGET>
  static bool sync_associations(
      Database& database, qsizetype owner_id, Link<OWNER, ERelation::ManyToOne> DERIVED::* owner_member,
      const QList<qsizetype>& target_ids, Link<TARGET, ERelation::ManyToOne> DERIVED::* target_member
  )
  {
    return sync_associations(database, owner_id, owner_member, target_ids, target_member, [](DERIVED&) {});
  }

private:
  // ---------------------------------------------------------------------------
  // Resolve a Link member to its SQL column
  // ---------------------------------------------------------------------------

  template <typename TARGET>
  static std::optional<QString> column_for_member(Link<TARGET, ERelation::ManyToOne> DERIVED::* member)
  {
    QString   column;
    qsizetype matches = 0;

    std::apply(
        [&](const auto&... fields) {
          (
              [&] {
                using Field = std::remove_cvref_t<decltype(fields)>;

                if constexpr (requires {
                                fields.member;
                                fields.spec.local_column;
                              }) {
                  if constexpr (std::same_as<decltype(fields.member), Link<TARGET, ERelation::ManyToOne> DERIVED::*>) {
                    if (fields.member == member) {
                      column = fields.spec.local_column.toString();
                      ++matches;
                    }
                  }
                }
              }(),
              ...
          );
        },
        DERIVED::sql_fields()
    );

    if (matches != 1) {
      qCritical() << "AssociationRecord: unable to resolve association member."
                  << "Expected one matching field, found:" << matches;

      return std::nullopt;
    }

    return column;
  }
};

} // namespace ligarium

#endif // ASSOCIATION_RECORD_H
