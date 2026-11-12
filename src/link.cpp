#include "ligarium/link.h"

#include "ligarium/database.h"
#include "ligarium/ligarium.h"

#include <QSqlDatabase>
#include <QSqlQuery>

namespace
{

QList<qsizetype> read_one_to_many(const ligarium::Database& db, const ligarium::SqlLinkSpec& spec, qsizetype owner_id)
{
  QList<qsizetype> ids;

  auto q = db.query(
      QStringLiteral(R"(SELECT id
FROM %1
WHERE %2 = :owner_id
)")
          .arg(ligarium::Table_to_str(spec.target_table), QString(spec.foreign_column)),
      {
          {":owner_id", owner_id}
  }
  );

  if (!db.execute_next(q)) return {};

  do {
    ids.append(q.value(0).toLongLong());
  } while (db.next(q));

  return ids;
}

QList<qsizetype> read_many_to_many(const ligarium::Database& db, const ligarium::SqlLinkSpec& spec, qsizetype owner_id)
{
  QList<qsizetype> ids;

  auto q = db.query(
      QStringLiteral(R"(
SELECT %1
FROM %2
WHERE %3 = :owner_id
)")
          .arg(
              QString(spec.association_target_column), ligarium::Table_to_str(spec.association_table),
              QString(spec.association_owner_column)
          ),
      {
          {":owner_id", owner_id}
  }
  );

  if (!db.execute_next(q)) return {};

  do {
    ids.append(q.value(0).toLongLong());
  } while (db.next(q));

  return ids;
}

bool save_one_to_many(
    const ligarium::Database& db, const ligarium::SqlLinkSpec& spec, qsizetype owner_id,
    const QList<qsizetype>& target_ids
)
{
  auto q = db.query(
      QStringLiteral(R"(
UPDATE %1
SET %2 = NULL
WHERE %2 = :owner_id
)")
          .arg(ligarium::Table_to_str(spec.target_table), QString(spec.foreign_column)),
      {
          {":owner_id", owner_id}
  }
  );

  if (!db.execute(q)) return false;

  // Assign the owner to each target record.
  q.prepare(QStringLiteral(R"(
UPDATE %1
SET %2 = :owner_id
WHERE id = :target_id
)")
                .arg(ligarium::Table_to_str(spec.target_table), QString(spec.foreign_column)));

  for (const qsizetype target_id : target_ids) {
    q.bindValue(":owner_id", owner_id);
    q.bindValue(":target_id", target_id);

    if (!db.execute(q)) return false;
  }

  return true;
}

bool save_many_to_many(
    const ligarium::Database& db, const ligarium::SqlLinkSpec& spec, qsizetype owner_id,
    const QList<qsizetype>& target_ids
)
{
  QSqlQuery q(db.connection());

  // Remove the existing associations.
  q.prepare(QStringLiteral(R"(
DELETE FROM %1
WHERE %2 = :owner_id
)")
                .arg(ligarium::Table_to_str(spec.association_table), QString(spec.association_owner_column)));

  q.bindValue(":owner_id", owner_id);

  if (!db.execute(q)) return false;

  // Recreate the current associations.
  q.prepare(QStringLiteral(R"(
INSERT INTO %1 (%2, %3)
VALUES (:owner_id, :target_id)
)")
                .arg(
                    ligarium::Table_to_str(spec.association_table), QString(spec.association_owner_column),
                    QString(spec.association_target_column)
                ));

  for (const qsizetype target_id : target_ids) {
    q.bindValue(":owner_id", owner_id);
    q.bindValue(":target_id", target_id);

    if (!db.execute(q)) return false;
  }

  return true;
}

} // namespace

bool ligarium::save_link_ids(
    const ligarium::Database& db, ERelation relation, const SqlLinkSpec& spec, qsizetype owner_id,
    const QList<qsizetype>& target_ids
)
{
  switch (relation) {
  case ERelation::OneToMany:  return save_one_to_many(db, spec, owner_id, target_ids);

  case ERelation::ManyToMany: return save_many_to_many(db, spec, owner_id, target_ids);

  case ERelation::OneToOne:
  case ERelation::ManyToOne:
    // These relationships are stored in the owner table.
    return true;
  default: assert(false && "Invalid relation type");
  }

  return false;
}

QVector<qsizetype>
ligarium::read_link_ids(const ligarium::Database& db, ERelation relation, const SqlLinkSpec& spec, qsizetype owner_id)
{
  QSqlQuery q(db.connection());

  switch (relation) {
  case ERelation::OneToMany: {
    q.prepare(QStringLiteral(R"(
SELECT id
FROM %1
WHERE %2 = :owner_id
)")
                  .arg(ligarium::Table_to_str(spec.target_table))
                  .arg(spec.foreign_column));

    q.bindValue(":owner_id", owner_id);

    break;
  }

  case ERelation::ManyToMany: {
    q.prepare(QString(R"(
SELECT %1
FROM %2
WHERE %3 = :owner_id
)")
                  .arg(spec.association_target_column)
                  .arg(ligarium::Table_to_str(spec.association_table))
                  .arg(spec.association_owner_column));

    q.bindValue(":owner_id", owner_id);

    break;
  }

  case ERelation::PolymorphicManyToOne: {
    q.prepare(QString(R"(
SELECT id
FROM %1
WHERE target_table = :target_table
AND target_id = :target_id
)")
                  .arg(ligarium::Table_to_str(spec.target_table)));

    q.bindValue(":target_table", static_cast<int>(spec.owner_table));

    q.bindValue(":target_id", owner_id);

    break;
  }

  default: return {};
  }

  if (!db.execute_next(q)) return {};

  QVector<qsizetype> ids;

  do {
    ids.append(q.value(0).toLongLong());
  } while (db.next(q));

  return ids;
}

bool ligarium::save_polymorphic_link(
    const ligarium::Database& db, const SqlLinkSpec& spec, qsizetype owner_id, const QVector<qsizetype>& target_ids
)
{
  QSqlQuery q(db.connection());

  q.prepare(QString(R"(
UPDATE %1
SET target_table = NULL, target_id = NULL
WHERE target_table = :target_table
AND target_id = :target_id
)")
                .arg(ligarium::Table_to_str(spec.target_table)));

  q.bindValue(":target_table", static_cast<int>(spec.owner_table));

  q.bindValue(":target_id", owner_id);

  if (!db.execute(q)) return false;

  q.prepare(QString(R"(UPDATE %1 "
SET target_table = :target_table, target_id = :target_id
WHERE id = :id)")
                .arg(ligarium::Table_to_str(spec.target_table)));

  for (const qsizetype attachment_id : target_ids) {
    q.bindValue(":target_table", static_cast<int>(spec.owner_table));

    q.bindValue(":target_id", owner_id);

    q.bindValue(":id", attachment_id);

    if (!db.execute(q)) return false;
  }

  return true;
}