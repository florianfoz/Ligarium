
#ifndef LIGARIUM_FIELD_H
#define LIGARIUM_FIELD_H

#include "ligarium/database.h"
#include "ligarium/ligarium.h"
#include "ligarium/link.h"

#include <QDate>
#include <QDateTime>
#include <QSqlQuery>
#include <QStringList>
#include <QVariant>
#include <tuple>
#include <utility>

namespace ligarium
{

void emit_on_record_saved(Database& db, Table table);

/**
 * @brief Converts a C++ value to and from SQL.
 */
template <typename VAL>
struct SqlConverter {
  static VAL from_sql(const QVariant& value)
  {
    return value.value<VAL>();
  }

  static QVariant to_sql(const VAL& value)
  {
    return QVariant::fromValue(value);
  }
};

/**
 * @brief Metadata for a regular SQL field.
 */
template <RecordFieldType RECORD, typename MEMBER>
struct SqlField {
  using record_type = RECORD;
  using value_type  = MEMBER;

  QStringView name;
  MEMBER RECORD::* member;

  void read(RECORD& record, const QSqlQuery& query) const
  {
    record.*member = SqlConverter<MEMBER>::from_sql(query.value(name));
  }

  void bind(QSqlQuery& query, const RECORD& record) const
  {
    query.bindValue(":" + name, SqlConverter<MEMBER>::to_sql(record.*member));
  }
};

/**
 * @brief Creates metadata for a regular SQL field.
 */
template <RecordFieldType RECORD, typename MEMBER>
constexpr auto field(QStringView name, MEMBER RECORD::* member)
{
  return SqlField<RECORD, MEMBER>{.name = name, .member = member};
}

/**
 * @brief Reads all fields represented by the owner query.
 */
template <RecordType RECORD>
void read_sql_fields(RECORD& record, const QSqlQuery& query)
{
  std::apply(
      [&](const auto&... fields) {
        (
            [&] {
              if constexpr (requires { fields.read(record, query); }) {
                fields.read(record, query);
              }
            }(),
            ...);
      },
      RECORD::sql_fields());
}

/**
 * @brief Generates the SELECT query for a record.
 */
template <RecordType RECORD>
QString make_select_sql()
{
  QStringList columns;

  std::apply(
      [&](const auto&... fields) {
        (
            [&] {
              if constexpr (requires { fields.read(std::declval<RECORD&>(), std::declval<const QSqlQuery&>()); }) {
                if constexpr (requires { fields.name; }) {
                  columns.append(QString(fields.name));
                }
              }
            }(),
            ...);
      },
      RECORD::sql_fields());

  const QString table = Table_to_str(RECORD::static_table);

  if (columns.isEmpty()) {
    return QStringLiteral("SELECT id FROM %1 WHERE id = :id").arg(table);
  }

  return QStringLiteral("SELECT id, %1 FROM %2 WHERE id = :id").arg(columns.join(", "), table);
}

/**
 * @brief Generates the UPDATE query for a record.
 */
template <RecordType RECORD>
QString make_update_sql()
{
  QStringList assignments;

  std::apply(
      [&](const auto&... fields) {
        (
            [&] {
              if constexpr (requires { fields.bind(std::declval<QSqlQuery&>(), std::declval<const RECORD&>()); }) {
                if constexpr (requires { fields.name; }) {
                  assignments.append(QStringLiteral("%1 = :%1").arg(QString(fields.name)));
                }
              }
            }(),
            ...);
      },
      RECORD::sql_fields());

  if (assignments.isEmpty()) {
    return {};
  }

  return QStringLiteral("UPDATE %1 SET %2 WHERE id = :id")
      .arg(Table_to_str(RECORD::static_table), assignments.join(", "));
}

/**
 * @brief Saves the regular fields of a record.
 */
template <RecordType RECORD>
bool save_record_fields(QSqlQuery& query, const RECORD& record)
{
  const QString sql = make_update_sql<RECORD>();

  if (sql.isEmpty()) return false;


  query.prepare(sql);

  std::apply(
      [&](const auto&... fields) {
        (
            [&] {
              if constexpr (requires { fields.bind(query, record); }) {
                fields.bind(query, record);
              }
            }(),
            ...);
      },
      RECORD::sql_fields());

  query.bindValue(":id", record.id());

  if (!record.database()->execute(query)) return false;

  emit_on_record_saved(*record.database(), RECORD::static_table);

  return true;
}

} // namespace ligarium

#endif // LIGARIUM_FIELD_H
