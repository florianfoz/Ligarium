#ifndef LIGARIUM_DATABASE_H
#define LIGARIUM_DATABASE_H

#include "ligarium.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <optional>

namespace ligarium
{

class Database final : public QObject
{
  Q_OBJECT
public:
  explicit Database(QSqlDatabase database);

  [[nodiscard]]
  const QSqlDatabase& connection() const noexcept;

  [[nodiscard]]
  bool is_open() const noexcept;

  [[nodiscard]]
  bool execute(QSqlQuery& query) const;

  [[nodiscard]]
  qsizetype last_insert_id() const;

  [[nodiscard]]
  bool contains(Table table, qsizetype record_id) const;

  [[nodiscard]]
  std::optional<QSqlQuery> find(Table table, qsizetype record_id) const;

  [[nodiscard]]
  std::optional<QSqlQuery> all(Table table) const;

  [[nodiscard]]
  std::optional<QSqlQuery> all_ids(Table table) const;

  [[nodiscard]]
  qsizetype record_id(Table table, const QString& column, const QVariant& value) const;

  [[nodiscard]]
  bool is_valid_column(Table table, const QString& column) const;

  [[nodiscard]]
  qsizetype insert(Table table) const;

  [[nodiscard]]
  bool save(Table table, qsizetype record_id, const QString& column, const QVariant& value) const;

  [[nodiscard]]
  bool remove(Table table, qsizetype record_id) const;

private:
  [[nodiscard]]
  static QString table_name(Table table);

private:
  QSqlDatabase m_database;

public:
signals:
  void signal_db_updated(Table table);
};

} // namespace ligarium

#endif // LIGARIUM_DATABASE_H
