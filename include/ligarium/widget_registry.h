#ifndef WIDGET_REGISTRY_H
#define WIDGET_REGISTRY_H

#include "ligarium/ligarium.h"
#include "ligarium/record.h"

#include <QHash>
#include <QWidget>
#include <functional>
#include <type_traits>
#include <utility>

namespace ligarium
{

class WidgetRegistry
{
public:
  WidgetRegistry() = delete;

  using FactoryWidgetRecordCreator = std::function<QWidget*(QWidget*, qsizetype)>;
  using FactoryWidgetRecordView    = std::function<QWidget*(QWidget*, qsizetype)>;
  using Dumper                     = std::function<QString(ligarium::Database& db, qsizetype id)>;

  struct RecordMetadata {
    FactoryWidgetRecordCreator w_creator;
    FactoryWidgetRecordView    w_view;
    Dumper                     dump;
  };

  static void register_widget(Table table, FactoryWidgetRecordCreator creator, FactoryWidgetRecordView view,
                              Dumper dumper)
  {
    qDebug() << "widget registered";

    m_widgets.insert(table, RecordMetadata{
                                .w_creator = std::move(creator),
                                .w_view    = std::move(view),
                                .dump      = std::move(dumper),
                            });
  }

  [[nodiscard]]
  static QWidget* new_widget_record_creator(Table table, qsizetype id, QWidget* parent = nullptr)
  {
    const auto it = m_widgets.constFind(table);

    if (it == m_widgets.cend()) return nullptr;

    return it.value().w_creator(parent, id);
  }

  [[nodiscard]]
  static QWidget* new_widget_record_view(Table table, qsizetype id, QWidget* parent = nullptr)
  {
    const auto it = m_widgets.constFind(table);

    if (it == m_widgets.cend()) return nullptr;

    return it.value().w_view(parent, id);
  }

  [[nodiscard]]
  static QString dump_record(Database& db, Table table, qsizetype id)
  {
    const auto it = m_widgets.constFind(table);

    if (it == m_widgets.cend()) return {};

    return it.value().dump(db, id);
  }

  [[nodiscard]]
  static bool contains(Table table)
  {
    return m_widgets.contains(table);
  }

  static void clear()
  {
    m_widgets.clear();
  }

private:
  inline static QHash<Table, RecordMetadata> m_widgets;
};

template <ligarium::RecordType Record, typename TWidgetCreator, typename TWidgetView>
  requires std::constructible_from<TWidgetCreator, QWidget*, qsizetype> && std::derived_from<TWidgetCreator, QWidget>
           && std::constructible_from<TWidgetView, QWidget*, qsizetype> && std::derived_from<TWidgetView, QWidget>
void register_widget()
{
  auto creator = [](QWidget* parent, qsizetype id) -> QWidget* { return new TWidgetCreator(parent, id); };
  auto view    = [](QWidget* parent, qsizetype id) -> QWidget* { return new TWidgetView(parent, id); };
  auto dump    = [](ligarium::Database& db, qsizetype id) -> QString { return ligarium::dump_record<Record>(db, id); };
  ligarium::WidgetRegistry::register_widget(Record::static_table, creator, view, dump);
}

} // namespace ligarium

#endif // WIDGET_REGISTRY_H
