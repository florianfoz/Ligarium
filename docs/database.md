# Database

`ligarium::Database` provides the database-facing layer of the ORM.

It wraps a `QSqlDatabase` connection.

## Creating a database

With SQLite:

```cpp
QSqlDatabase connection =
    QSqlDatabase::addDatabase("QSQLITE");

connection.setDatabaseName(":memory:");

connection.open();

ligarium::Database db(connection);
```

The database connection remains a Qt SQL connection.

## Connection access

The underlying connection can be accessed with:

```cpp
const QSqlDatabase& connection =
    db.connection();
```

This allows application code to execute SQL directly when required.

## Database operations

The database abstraction provides operations such as:

```cpp
db.contains(table, id);
db.find(table, id);
db.all(table);
db.all_ids(table);
db.insert(table);
db.save(table, id, column, value);
db.remove(table, id);
```

## Queries

Query-producing operations return:

```cpp
std::optional<QSqlQuery>
```

For example:

```cpp
auto result = db.find(
    ligarium::Table::Property,
    property_id
);

if (result)
{
    QSqlQuery query = std::move(result.value());
}
```

## SQL access

Ligarium does not prohibit direct SQL.

Application-specific queries can still use:

```cpp
QSqlQuery query(db.connection());
```

This is intentional.

Ligarium provides an ORM abstraction for common persistence operations while keeping Qt SQL available for operations that do not fit the ORM model.

## SQLite

The current implementation is designed primarily around Qt SQL and SQLite-compatible operations.

Some database operations use SQLite-specific functionality such as:

```sql
last_insert_rowid()
```

and SQLite table metadata.

Additional SQL database backends may require backend-specific adaptations.
