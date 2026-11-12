#include "ligarium/database.h"

#include "ligarium/widget_registry.h"

#include <QDebug>
#include <QSqlError>

namespace ligarium
{

Database::Database(QSqlDatabase&& database)
  : m_database(std::move(database))
{
}

Database::~Database()
{
  const QString connectionName = m_database.connectionName();

  m_database.close();
  m_database = QSqlDatabase();

  QSqlDatabase::removeDatabase(connectionName);
}

const QSqlDatabase& Database::connection() const noexcept
{
  return m_database;
}

QSqlDatabase& Database::connection() noexcept
{
  return m_database;
}

bool Database::is_open() const noexcept
{
  return m_database.isOpen();
}

QSqlQuery Database::query(const QString& query, const BindArgs& binds) const
{
  auto q = QSqlQuery(connection());
  q.prepare(query);
  for (const auto& [placeholder, val] : binds) q.bindValue(placeholder, val);
  return q;
}

bool Database::execute(QSqlQuery& query) const
{
  if (query.exec()) return true;

  if (query.lastError().isValid()) {
    qCritical() << "ligarium query failed: " << __PRETTY_FUNCTION__;
    qCritical() << "  Driver error:" << query.lastError().driverText();
    qCritical() << "  Database error:" << query.lastError().databaseText();
    qCritical() << "  Last query:" << query.lastQuery();
    return false;
  }
  return false;
}

bool Database::execute_next(QSqlQuery& query) const
{
  if (!execute(query)) return false;

  return next(query);
}

bool Database::next(QSqlQuery& query) const
{
  if (query.next()) return true;

  if (query.lastError().isValid()) {
    qCritical() << "ligarium query failed: " << __PRETTY_FUNCTION__;
    qCritical() << "  Driver error:" << query.lastError().driverText();
    qCritical() << "  Database error:" << query.lastError().databaseText();
    qCritical() << "  Last query:" << query.lastQuery();
    return false;
  }

  return false;
}

qsizetype Database::last_insert_id() const
{
  auto q = query("SELECT last_insert_rowid()");

  if (!execute_next(q)) return INVALID_ID;

  return q.value(0).toLongLong();
}

bool Database::contains(Table table, qsizetype record_id) const
{
  auto q = query(
      QStringLiteral(R"(
SELECT EXISTS(
  SELECT 1 FROM %1 WHERE id = :id
)
)")
          .arg(Table_to_str(table)),
      {
          {":id", record_id}
  }
  );

  if (!execute_next(q)) return false;

  return q.value(0).toBool();
}

std::optional<QSqlQuery> Database::find(Table table, qsizetype record_id) const
{
  auto q = query(
      QStringLiteral(R"(
SELECT * 
FROM %1 
WHERE id = :id
)")
          .arg(Table_to_str(table)),
      {
          {":id", record_id}
  }
  );

  if (!execute_next(q)) return std::nullopt;

  return q;
}

std::optional<QSqlQuery> Database::all(Table table) const
{
  auto q = query(QStringLiteral(R"(
SELECT *
FROM %1
)")
                     .arg(Table_to_str(table)));

  if (!execute(q)) return std::nullopt;

  return q;
}

std::optional<QSqlQuery> Database::all_ids(Table table) const
{
  auto q = query(QStringLiteral(R"(
SELECT id 
FROM %1
)")
                     .arg(Table_to_str(table)));

  if (!execute(q)) return std::nullopt;

  return q;
}

qsizetype Database::record_id(Table table, const QString& column, const QVariant& value) const
{
  if (!is_valid_column(table, column)) return INVALID_ID;

  auto q = query(
      QStringLiteral(R"(
SELECT id 
FROM %1
WHERE %2 = :value
LIMIT 1
)")
          .arg(Table_to_str(table), column),
      {
          {":value", value}
  }
  );

  if (!execute_next(q)) return INVALID_ID;

  return q.value("id").toLongLong();
}

bool Database::is_valid_column(Table table, const QString& column) const
{
  auto q = query(
      QStringLiteral(
          R"(
SELECT COUNT(*) 
FROM pragma_table_info('%1') 
WHERE name = :column
)"
      )
          .arg(Table_to_str(table)),
      {
          {":column", column}
  }
  );

  if (!execute_next(q)) return false;

  return q.value(0).toInt() == 1;
}

qsizetype Database::insert(Table table) const
{
  auto q = query(QStringLiteral("INSERT INTO %1 DEFAULT VALUES").arg(Table_to_str(table)));

  if (!execute(q)) return INVALID_ID;

  return q.lastInsertId().toLongLong();
}

bool Database::save(Table table, qsizetype record_id, const QString& column, const QVariant& value) const
{
  if (!is_valid_column(table, column)) return false;

  auto q = query(
      QStringLiteral(R"(
UPDATE %1
SET %2 = :value
WHERE id = :id
)")
          .arg(Table_to_str(table), column),
      {
          {":value", value    },
          {":id",    record_id}
  }
  );


  if (!execute(q)) return false;

  return q.numRowsAffected() > 0;
}

bool Database::remove(Table table, qsizetype record_id) const
{
  auto q = query(
      QStringLiteral(R"(
DELETE 
FROM %1 
WHERE id = :id
)")
          .arg(Table_to_str(table)),
      {
          {":id", record_id}
  }
  );

  if (!execute(q)) return false;

  return q.numRowsAffected() > 0;
}

} // namespace ligarium