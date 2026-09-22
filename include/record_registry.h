#ifndef RECORD_REGISTRY_H
#define RECORD_REGISTRY_H

#include "ligarium.h"

#include <QHash>
#include <QString>
#include <functional>
#include <utility>

namespace Ligarium
{

class RecordRegistry
{
public:
  explicit RecordRegistry(Database* db)
    : m_db(db)
  {
  }

  [[nodiscard]]
  Database* database()
  {
    return m_db;
  }

  using DumpFunction = std::function<QString(qsizetype)>;

  void register_record(Table table, DumpFunction dump)
  {
    m_records.insert(table, std::move(dump));
  }

  [[nodiscard]]
  QString dump(Table table, qsizetype id) const
  {
    const auto it = m_records.constFind(table);

    if (it == m_records.cend()) return {};

    return it.value()(id);
  }

  [[nodiscard]]
  bool contains(Table table) const
  {
    return m_records.contains(table);
  }

  void clear()
  {
    m_records.clear();
  }

private:
  Database*                  m_db = nullptr;
  QHash<Table, DumpFunction> m_records;
};

template <RecordType TRecord>
void register_record(RecordRegistry& registry, Table table)
{
  registry.register_record(
      table, [&registry](qsizetype id) -> QString { return TRecord::read_record(*registry.database(), id).dump(); });
}

} // namespace Ligarium

#endif // RECORD_REGISTRY_H
