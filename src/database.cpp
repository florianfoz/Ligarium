#include "database.h"

#include <QDebug>
#include <QSqlError>

namespace Ligarium
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
  if (query.exec()) return true;

  qCritical() << "SQL query failed:" << query.lastError().text();

  return false;
}

qsizetype Database::last_insert_id() const
{
  QSqlQuery query(m_database);

  if (!query.exec(QStringLiteral("SELECT last_insert_rowid()"))) {
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

  const QString sql = QStringLiteral(
                          "SELECT EXISTS("
                          "SELECT 1 FROM %1 WHERE id = :id"
                          ")")
                          .arg(table_name(table));

  if (!query.prepare(sql)) {
    qCritical() << "Failed to prepare contains query:" << query.lastError().text();

    return false;
  }

  query.bindValue(QStringLiteral(":id"), record_id);

  if (!execute(query)) return false;

  return query.next() && query.value(0).toBool();
}

std::optional<QSqlQuery> Database::find(Table table, qsizetype record_id) const
{
  QSqlQuery query(m_database);

  const QString sql = QStringLiteral("SELECT * FROM %1 WHERE id = :id").arg(table_name(table));

  if (!query.prepare(sql)) {
    qCritical() << "Failed to prepare find query:" << query.lastError().text();

    return std::nullopt;
  }

  query.bindValue(QStringLiteral(":id"), record_id);

  if (!execute(query)) return std::nullopt;

  return query;
}

std::optional<QSqlQuery> Database::all(Table table) const
{
  QSqlQuery query(m_database);

  const QString sql = QStringLiteral("SELECT * FROM %1").arg(table_name(table));

  if (!query.exec(sql)) {
    qCritical() << "Failed to retrieve records:" << query.lastError().text();

    return std::nullopt;
  }

  return query;
}

std::optional<QSqlQuery> Database::all_ids(Table table) const
{
  QSqlQuery query(m_database);

  const QString sql = QStringLiteral("SELECT id FROM %1").arg(table_name(table));

  if (!query.exec(sql)) {
    qCritical() << "Failed to retrieve record IDs:" << query.lastError().text();

    return std::nullopt;
  }

  return query;
}

qsizetype Database::record_id(Table table, const QString& column, const QVariant& value) const
{
  if (!is_valid_column(table, column)) return INVALID_ID;

  QSqlQuery query(m_database);

  const QString sql = QStringLiteral(
                          "SELECT id "
                          "FROM %1 "
                          "WHERE %2 = :value "
                          "LIMIT 1")
                          .arg(table_name(table), column);

  if (!query.prepare(sql)) {
    qCritical() << "Failed to prepare record ID query:" << query.lastError().text();

    return INVALID_ID;
  }

  query.bindValue(QStringLiteral(":value"), value);

  if (!execute(query)) return INVALID_ID;

  if (!query.next()) return INVALID_ID;

  return query.value(QStringLiteral("id")).toLongLong();
}

bool Database::is_valid_column(Table table, const QString& column) const
{
  QSqlQuery query(m_database);

  const QString sql = QStringLiteral(
                          "SELECT COUNT(*) "
                          "FROM pragma_table_info('%1') "
                          "WHERE name = :column")
                          .arg(table_name(table));

  if (!query.prepare(sql)) {
    qCritical() << "Failed to prepare column validation query:" << query.lastError().text();

    return false;
  }

  query.bindValue(QStringLiteral(":column"), column);

  if (!execute(query)) return false;

  return query.next() && query.value(0).toInt() == 1;
}

qsizetype Database::insert(Table table) const
{
  QSqlQuery query(m_database);

  const QString sql = QStringLiteral("INSERT INTO %1 DEFAULT VALUES").arg(table_name(table));

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

  const QString sql = QStringLiteral(
                          "UPDATE %1 "
                          "SET %2 = :value "
                          "WHERE id = :id")
                          .arg(table_name(table), column);

  if (!query.prepare(sql)) {
    qCritical() << "Failed to prepare save query:" << query.lastError().text();

    return false;
  }

  query.bindValue(QStringLiteral(":value"), value);
  query.bindValue(QStringLiteral(":id"), record_id);

  if (!execute(query)) return false;

  return query.numRowsAffected() > 0;
}

bool Database::remove(Table table, qsizetype record_id) const
{
  QSqlQuery query(m_database);

  const QString sql = QStringLiteral("DELETE FROM %1 WHERE id = :id").arg(table_name(table));

  if (!query.prepare(sql)) {
    qCritical() << "Failed to prepare delete query:" << query.lastError().text();

    return false;
  }

  query.bindValue(QStringLiteral(":id"), record_id);

  if (!execute(query)) return false;

  return query.numRowsAffected() > 0;
}

QString Database::table_name(Table table)
{
  return Table_to_str(table);
}

} // namespace Ligarium