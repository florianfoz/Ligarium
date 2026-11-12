#ifndef LIGARIUM_SCHEMA_H
#define LIGARIUM_SCHEMA_H


#include "ligarium/ligarium.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QStringList>
#include <type_traits>
#include <utility>

namespace ligarium
{

/**
 * @brief Maps a C++ type to a SQLite storage type.
 */
template <typename T>
struct SqlType;

/* Integral types. */
template <>
struct SqlType<bool> {
  static constexpr QStringView value = u"INTEGER";
};

template <typename T>
  requires(std::is_integral_v<T> && !std::is_same_v<T, bool>)
struct SqlType<T> {
  static constexpr QStringView value = u"INTEGER";
};

/* Floating-point types. */
template <typename T>
  requires(std::is_floating_point_v<T>)
struct SqlType<T> {
  static constexpr QStringView value = u"REAL";
};

/* Text and binary data. */
template <>
struct SqlType<QString> {
  static constexpr QStringView value = u"TEXT";
};

template <>
struct SqlType<QByteArray> {
  static constexpr QStringView value = u"BLOB";
};

/* Qt date/time types. */
template <>
struct SqlType<QDate> {
  static constexpr QStringView value = u"TEXT";
};

template <>
struct SqlType<QDateTime> {
  static constexpr QStringView value = u"TEXT";
};

/**
 * @brief Generates SQLite tables from ligarium record metadata.
 */
class SchemaBuilder
{
public:
  explicit SchemaBuilder(QSqlDatabase database)
    : m_database(database)
  {
  }


  [[nodiscard]]
  QString dump_schema() const
  {
    QString out;
    out.append("Driver:" + m_database.driverName() + " ");
    out.append("Database:" + m_database.databaseName() + " ");

    const QStringList tables = m_database.tables(QSql::Tables);

    for (const QString& table : tables) {
      out.append("Table: " + table + " [");
      QSqlRecord record = m_database.record(table);

      for (int i = 0; i < record.count(); ++i) {
        QSqlField field = record.field(i);
        out.append(field.name() + ", ");
      }
      out.chop(2);
      out.append("], ");
    }
    out.chop(2);


    return out;
  }

  /**
   * @brief Returns the last SQL error.
   */
  [[nodiscard]]
  QString last_error() const
  {
    return m_last_error;
  }

  /**
   * @brief Creates the table for a record.
   *
   * Only regular fields and scalar relationships are represented
   * directly in the owner table.
   */
  template <RecordType RECORD>
  bool create_fields()
  {
    QStringList columns;

    columns.append(QStringLiteral("%1 INTEGER PRIMARY KEY AUTOINCREMENT").arg(quote(u"id")));

    std::apply(
        [&](const auto&... fields) {
          (
              [&] {
                using FIELD = std::decay_t<decltype(fields)>;

                if constexpr (Is_SQL_Direct_Link_Poly<FIELD>) {
                  columns.append(QStringLiteral("%1 INTEGER").arg(quote(fields.name + "_table")));
                  columns.append(QStringLiteral("%1 INTEGER").arg(quote(fields.name + "_id")));
                } else if constexpr (Is_SQL_Direct_Link<FIELD>) {
                  columns.append(QStringLiteral("%1 INTEGER").arg(quote(fields.name)));
                } else if constexpr (!Is_SQL_Link<FIELD>) {
                  using Value = typename FIELD::value_type;
                  columns.append(QStringLiteral("%1 %2").arg(quote(fields.name), QString(SqlType<Value>::value)));
                }
              }(),
              ...
          );
        },
        RECORD::sql_fields()
    );

    const QString sql = QStringLiteral("CREATE TABLE IF NOT EXISTS %1 (%2)")
                            .arg(quote(Table_to_str(RECORD::static_table)), columns.join(", "));


    return execute(sql);
  }

  /**
   * @brief Creates all record tables first, then their relationships.
   *
   * All record tables must be included in RECORDS when their
   * relationships reference each other.
   */
  template <RecordType... RECORDS>
  bool create_all()
  {
    if (!(create_fields<RECORDS>() && ...)) {
      return false;
    }

    // return (create_relations<RECORDS>() && ...);
    return true;
  }

private:
  [[nodiscard]]
  static QString quote(QStringView identifier)
  {
    QString result = QString(identifier);
    result.replace('"', "\"\"");

    return QStringLiteral("\"%1\"").arg(result);
  }

  [[nodiscard]]
  bool execute(const QString& sql)
  {
    QSqlQuery q(m_database);

    if (q.exec(sql)) {
      return true;
    }

    m_last_error = q.lastError().text();
    return false;
  }


  template <RecordType RECORD>
  bool create_relations()
  {
    bool result = true;

    std::apply(
        [&](const auto&... fields) {
          (
              [&] {
                if (!result) return;

                using FIELD = std::decay_t<decltype(fields)>;

                if constexpr (Is_SQL_Indirect_Link<FIELD>) {
                  if (fields.RELATION == ERelation::OneToMany) return create_one_to_many(fields);
                }
              }(),
              ...
          );
        },
        RECORD::sql_fields()
    );

    return result;
  }

  template <typename FIELD>
  bool create_one_to_many(const FIELD& field)
  {
    const Table target_table = field.spec.target_table;

    const QString target = quote(Table_to_str(target_table));

    const QString column = quote(field.spec.foreign_column);

    if (has_column(target_table, field.spec.foreign_column)) {
      return true;
    }

    const QString sql = QStringLiteral("ALTER TABLE %1 ADD COLUMN %2 INTEGER").arg(target, column);

    return execute(sql);
  }


  template <typename FIELD>
  bool create_polymorphic(const FIELD& field)
  {
    {
      const QString sql = QStringLiteral(
                              "ALTER TABLE %1 "
                              "ADD COLUMN %2 INTEGER NOT NULL DEFAULT 0"
      )
                              .arg(quote(Table_to_str(FIELD::record_type::static_table)), quote(u"target_table"));

      if (!execute(sql)) return false;
    }

    {
      const QString sql = QStringLiteral(
                              "ALTER TABLE %1 "
                              "ADD COLUMN %2 INTEGER NOT NULL DEFAULT -1"
      )
                              .arg(quote(Table_to_str(FIELD::record_type::static_table)), quote(u"target_id"));

      if (!execute(sql)) return false;
    }

    return true;
  }

  bool has_column(Table table, QStringView column)
  {
    QSqlQuery q(m_database);

    const QString sql = QStringLiteral(
                            "SELECT 1 "
                            "FROM pragma_table_info(%1) "
                            "WHERE name = :name "
                            "LIMIT 1"
    )
                            .arg(quote(Table_to_str(table)));

    if (!q.prepare(sql)) {
      m_last_error = q.lastError().text();
      return false;
    }

    q.bindValue(":name", QString(column));

    if (!q.exec()) {
      m_last_error = q.lastError().text();
      return false;
    }

    return q.next();
  }

private:
  QSqlDatabase m_database;
  QString      m_last_error;
};

/**
 * @brief Creates the schema for the specified records.
 */
template <RecordType... RECORDS>
bool create_schema(QSqlDatabase database)
{
  SchemaBuilder schema(std::move(database));
  return schema.create_all<RECORDS...>();
}

} // namespace ligarium

#endif // LIGARIUM_SCHEMA_H
