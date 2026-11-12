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

  using FactoryWidgetRecord = std::function<IRecordWidget*(QWidget*, ligarium::Database&, qsizetype)>;
  using Dumper              = std::function<QString(ligarium::Database&, qsizetype)>;

  struct RecordMetadata {
    FactoryWidgetRecord factory;
    Dumper              dump;
  };

  static void register_widget(Table table, FactoryWidgetRecord factory, Dumper dumper)
  {
    qDebug() << "widget registered";

    m_widgets.insert(
        table,
        RecordMetadata{
            .factory = std::move(factory),
            .dump    = std::move(dumper),
        }
    );
  }

  [[nodiscard]]
  static IRecordWidget* new_widget_record(Table table, qsizetype id, ligarium::Database& db, QWidget* parent = nullptr)
  {
    const auto it = m_widgets.constFind(table);

    if (it == m_widgets.cend()) return nullptr;

    return it.value().factory(parent, db, id);
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

template <ligarium::RecordType Record, typename TWidget>
  requires std::constructible_from<TWidget, IRecordWidget*, ligarium::Database&, qsizetype>
           && std::derived_from<TWidget, IRecordWidget>
void register_widget()
{
  auto factory = [](QWidget* parent, ligarium::Database& db, qsizetype id) -> TWidget* {
    return new TWidget(parent, db, id);
  };
  auto dump = [](ligarium::Database& db, qsizetype id) -> QString { return ligarium::dump_record<Record>(db, id); };
  ligarium::WidgetRegistry::register_widget(Record::static_table, factory, dump);
}

class IRecordWidget : public QWidget
{
  Q_OBJECT

public:
  explicit IRecordWidget(QWidget* parent = nullptr)
    : QWidget(parent)
  {
  }

  virtual ~IRecordWidget() = default;

  virtual void load()
  {
  }

  virtual bool save()
  {
    return true;
  }

  virtual void reset()
  {
  }
};


} // namespace ligarium

#endif // WIDGET_REGISTRY_H
