#ifndef LIGARIUM_SCHEMA_H
#define LIGARIUM_SCHEMA_H

#include "field.h"
#include "record.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>
#include <type_traits>
#include <utility>

namespace Ligarium
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
 * @brief Generates SQLite tables from Ligarium record metadata.
 */
class SchemaBuilder
{
public:
  explicit SchemaBuilder(QSqlDatabase database)
    : m_database(std::move(database))
  {
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
  bool create()
  {
    QStringList columns;

    columns.append(QStringLiteral("%1 INTEGER PRIMARY KEY AUTOINCREMENT").arg(quote(u"id")));

    std::apply([&](const auto&... fields) { (add_column_definition<RECORD>(columns, fields), ...); },
               RECORD::sql_fields());

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
    if (!(create<RECORDS>() && ...)) {
      return false;
    }

    return (create_relations<RECORDS>() && ...);
  }

private:
  [[nodiscard]]
  static QString quote(QStringView identifier)
  {
    QString result = QString(identifier);
    result.replace('"', QStringLiteral("\"\""));

    return QStringLiteral("\"%1\"").arg(result);
  }

  [[nodiscard]]
  bool execute(const QString& sql)
  {
    QSqlQuery query(m_database);

    if (query.exec(sql)) {
      return true;
    }

    m_last_error = query.lastError().text();
    return false;
  }

  template <RecordType RECORD, typename FIELD>
  void add_column_definition(QStringList& columns, const FIELD& field)
  {
    if constexpr (requires { FIELD::relation; }) {
      if constexpr (FIELD::relation == ERelation::OneToOne || FIELD::relation == ERelation::ManyToOne) {
        columns.append(QStringLiteral("%1 INTEGER").arg(quote(field.name)));
      }

      return;
    }

    if constexpr (requires { FIELD::value_type; }) {
      using Value = typename FIELD::value_type;

      if constexpr (requires { SqlType<Value>::value; }) {
        columns.append(QStringLiteral("%1 %2").arg(quote(field.name), QString(SqlType<Value>::value)));
      }
    }
  }

  template <RecordType RECORD>
  bool create_relations()
  {
    bool result = true;

    std::apply(
        [&](const auto&... fields) {
          (
              [&] {
                if (!result) {
                  return;
                }

                if constexpr (requires { fields.relation; }) {
                  result = create_relation(RECORD::static_table, fields);
                }
              }(),
              ...);
        },
        RECORD::sql_fields());

    return result;
  }

  template <RecordType RECORD, typename FIELD>
  bool create_relation(Table owner_table, const FIELD& field)
  {
    constexpr ERelation relation = FIELD::relation;

    if constexpr (relation == ERelation::OneToOne || relation == ERelation::ManyToOne) {
      Q_UNUSED(owner_table);
      Q_UNUSED(field);
      return true;
    }

    if constexpr (relation == ERelation::OneToMany) {
      return create_one_to_many(field);
    }

    if constexpr (relation == ERelation::ManyToMany) {
      return create_many_to_many(field);
    }

    if constexpr (relation == ERelation::PolymorphicOneToMany) {
      return create_polymorphic(field);
    }

    return true;
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
  bool create_many_to_many(const FIELD& field)
  {
    const QString association = quote(Table_to_str(field.spec.association_table));

    const QString owner_column = quote(field.spec.association_owner_column);

    const QString target_column = quote(field.spec.association_target_column);

    const QString sql = QStringLiteral(
                            "CREATE TABLE IF NOT EXISTS %1 "
                            "(%2 INTEGER NOT NULL, "
                            "%3 INTEGER NOT NULL, "
                            "PRIMARY KEY (%2, %3))")
                            .arg(association, owner_column, target_column);

    return execute(sql);
  }

  template <typename FIELD>
  bool create_polymorphic(const FIELD& field)
  {
    const Table target_table = field.spec.target_table;

    if (!has_column(target_table, QStringView(u"table"))) {
      const QString sql = QStringLiteral(
                              "ALTER TABLE %1 "
                              "ADD COLUMN %2 INTEGER NOT NULL DEFAULT 0")
                              .arg(quote(Table_to_str(target_table)), quote(u"table"));

      if (!execute(sql)) {
        return false;
      }
    }

    if (!has_column(target_table, QStringView(u"col_id"))) {
      const QString sql = QStringLiteral(
                              "ALTER TABLE %1 "
                              "ADD COLUMN %2 INTEGER NOT NULL DEFAULT -1")
                              .arg(quote(Table_to_str(target_table)), quote(u"col_id"));

      if (!execute(sql)) {
        return false;
      }
    }

    return true;
  }

  bool has_column(Table table, QStringView column)
  {
    QSqlQuery query(m_database);

    const QString sql = QStringLiteral(
                            "SELECT 1 "
                            "FROM pragma_table_info(%1) "
                            "WHERE name = :name "
                            "LIMIT 1")
                            .arg(quote(Table_to_str(table)));

    if (!query.prepare(sql)) {
      m_last_error = query.lastError().text();
      return false;
    }

    query.bindValue(":name", QString(column));

    if (!query.exec()) {
      m_last_error = query.lastError().text();
      return false;
    }

    return query.next();
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

} // namespace Ligarium

#endif // LIGARIUM_SCHEMA_H
