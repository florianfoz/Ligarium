#ifndef WIDGET_REGISTRY_H
#define WIDGET_REGISTRY_H

#include "ligarium.h"

#include <QHash>
#include <QWidget>
#include <functional>
#include <utility>

namespace Ligarium
{

class WidgetRegistry
{
public:
  using Factory = std::function<QWidget*(qsizetype, QWidget*)>;

  void set_database(Database* db)
  {
    m_db = db;
  }

  [[nodiscard]]
  Database* database()
  {
    return m_db;
  }

  void register_widget(Table table, Factory factory)
  {
    m_widgets.insert(table, std::move(factory));
  }

  [[nodiscard]]
  QWidget* create(Table table, qsizetype id, QWidget* parent = nullptr) const
  {
    const auto it = m_widgets.constFind(table);

    if (it == m_widgets.cend()) return nullptr;

    return it.value()(id, parent);
  }

  [[nodiscard]]
  bool contains(Table table) const
  {
    return m_widgets.contains(table);
  }

  void clear()
  {
    m_widgets.clear();
  }

private:
  Database*             m_db = nullptr;
  QHash<Table, Factory> m_widgets;
};

template <typename TWidget>
void register_widget(WidgetRegistry& registry, Table table)
{
  registry.register_widget(table, [](qsizetype id, QWidget* parent) { return new TWidget(id, parent); });
}

} // namespace Ligarium

#endif // WIDGET_REGISTRY_H