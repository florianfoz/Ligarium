#include "ligarium/database.h"

#include "ligarium/widget_registry.h"

#include <QDebug>
#include <QSqlError>

namespace ligarium
{

Database::Database(QSqlDatabase database)
  : m_database(database)
{
}

const QSqlDatabase& Database::connection() const noexcept
{
  return m_database;
}

bool Database::is_open() const noexcept
{
  return m_database.isOpen();
}

bool Database::execute(QSqlQuery& query) const
{
  if (!query.exec()) {
    qCritical() << "SQL query exec failed: " << query.lastError().text();
    return false;
  }
  return true;
}

bool Database::execute_next(QSqlQuery& query) const
{
  if (!execute(query)) return false;

  if (!query.next()) {
    qCritical() << "SQL query next failed:" << query.lastError().text();
    return false;
  }

  return true;
}

qsizetype Database::last_insert_id() const
{
  QSqlQuery query(m_database);

  if (!query.exec("SELECT last_insert_rowid()")) {
    qCritical() << "Failed to retrieve last insert ID:" << query.lastError().text();

    return INVALID_ID;
  }

  if (!query.next()) {
    qCritical() << "No last insert ID returned.";
    return INVALID_ID;
  }

  return query.value(0).toLongLong();
}

bool Database::contains(Table table, qsizetype record_id) const
{
  QSqlQuery query(m_database);

  const QString sql = QStringLiteral(R"(
SELECT EXISTS(
  SELECT 1 FROM %1 WHERE id = :id
)
)")
                          .arg(Table_to_str(table));

  if (!query.prepare(sql)) {
    qCritical() << "Failed to prepare contains query:" << query.lastError().text();

    return false;
  }

  query.bindValue(":id", record_id);

  if (!execute_next(query)) return false;

  return query.value(0).toBool();
}

std::optional<QSqlQuery> Database::find(Table table, qsizetype record_id) const
{
  QSqlQuery query(m_database);

  query.prepare(QStringLiteral(R"(
SELECT * 
FROM %1 
WHERE id = :id
)")
                    .arg(Table_to_str(table)));

  query.bindValue(":id", record_id);

  if (!execute_next(query)) return std::nullopt;

  return query;
}

std::optional<QSqlQuery> Database::all(Table table) const
{
  QSqlQuery query(m_database);

  query.prepare(QStringLiteral(R"(
SELECT *
FROM %1
)")
                    .arg(Table_to_str(table)));

  if (!execute(query)) return std::nullopt;

  return query;
}

std::optional<QSqlQuery> Database::all_ids(Table table) const
{
  QSqlQuery query(m_database);

  query.prepare(QStringLiteral(R"(
SELECT id 
FROM %1
)")
                    .arg(Table_to_str(table)));

  if (!execute(query)) return std::nullopt;

  return query;
}

qsizetype Database::record_id(Table table, const QString& column, const QVariant& value) const
{
  if (!is_valid_column(table, column)) return INVALID_ID;

  QSqlQuery query(m_database);

  query.prepare(QStringLiteral(R"(
SELECT id 
FROM %1
WHERE %2 = :value
LIMIT 1
)")
                    .arg(Table_to_str(table), column));

  query.bindValue(":value", value);

  if (!execute_next(query)) return INVALID_ID;

  return query.value("id").toLongLong();
}

bool Database::is_valid_column(Table table, const QString& column) const
{
  QSqlQuery query(m_database);

  const QString sql = QStringLiteral(
                          R"(
SELECT COUNT(*) 
FROM pragma_table_info('%1') 
WHERE name = :column
)")
                          .arg(Table_to_str(table));

  if (!query.prepare(sql)) {
    qCritical() << "Failed to prepare column validation query:" << query.lastError().text();

    return false;
  }

  query.bindValue(":column", column);

  if (!execute_next(query)) return false;

  return query.value(0).toInt() == 1;
}

qsizetype Database::insert(Table table) const
{
  QSqlQuery query(m_database);

  const QString sql = QStringLiteral("INSERT INTO %1 DEFAULT VALUES").arg(Table_to_str(table));

  if (!query.exec(sql)) {
    qCritical() << "Failed to insert record:" << query.lastError().text();

    return INVALID_ID;
  }

  return query.lastInsertId().toLongLong();
}

bool Database::save(Table table, qsizetype record_id, const QString& column, const QVariant& value) const
{
  if (!is_valid_column(table, column)) return false;

  QSqlQuery query(m_database);

  const QString sql = QStringLiteral(R"(
UPDATE %1
SET %2 = :value
WHERE id = :id
)")
                          .arg(Table_to_str(table), column);

  if (!query.prepare(sql)) {
    qCritical() << "Failed to prepare save query:" << query.lastError().text();

    return false;
  }

  query.bindValue(":value", value);
  query.bindValue(":id", record_id);

  if (!execute(query)) return false;

  return query.numRowsAffected() > 0;
}

bool Database::remove(Table table, qsizetype record_id) const
{
  QSqlQuery query(m_database);

  const QString sql = QStringLiteral(R"(
DELETE 
FROM %1 
WHERE id = :id
)")
                          .arg(Table_to_str(table));

  if (!query.prepare(sql)) {
    qCritical() << "Failed to prepare delete query:" << query.lastError().text();

    return false;
  }

  query.bindValue(":id", record_id);

  if (!execute(query)) return false;

  return query.numRowsAffected() > 0;
}

} // namespace ligarium