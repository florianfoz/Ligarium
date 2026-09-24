#include "ligarium_config.h"
//

#include <QTest>
#include <QWidget>
#include <ligarium/widget_registry.h>

class PropertyWidget : public QWidget
{
  Q_OBJECT
public:
  explicit PropertyWidget(qsizetype id, QWidget* parent = nullptr)
    : QWidget(parent)
    , m_id(id)
  {
  }

  [[nodiscard]]
  qsizetype id() const noexcept
  {
    return m_id;
  }

private:
  qsizetype m_id = ligarium::INVALID_ID;
};

class TestWidgetRegistry : public QObject
{
  Q_OBJECT

private slots:
  void initially_empty();
  void register_widget();
  void create_registered_widget();
  void create_unknown_widget();
  void create_with_parent();
  void contains_registered_widget();
  void contains_unknown_widget();
  void replace_registered_widget();
  void clear_registry();
  void register_typed_widget();
};

void TestWidgetRegistry::initially_empty()
{
  ligarium::WidgetRegistry registry;

  QVERIFY(!registry.contains(ligarium::Table::Property));
  QVERIFY(registry.create(ligarium::Table::Property, 1) == nullptr);
}

void TestWidgetRegistry::register_widget()
{
  ligarium::WidgetRegistry registry;

  registry.register_widget(ligarium::Table::Property, [](qsizetype id, QWidget* parent) {
    auto* widget = new PropertyWidget(id, parent);
    return widget;
  });

  QVERIFY(registry.contains(ligarium::Table::Property));
}

void TestWidgetRegistry::create_registered_widget()
{
  ligarium::WidgetRegistry registry;

  registry.register_widget(ligarium::Table::Property,
                           [](qsizetype id, QWidget* parent) { return new PropertyWidget(id, parent); });

  QWidget* widget = registry.create(ligarium::Table::Property, 42);

  QVERIFY(widget != nullptr);

  auto* property_widget = qobject_cast<PropertyWidget*>(widget);

  QVERIFY(property_widget != nullptr);
  QCOMPARE(property_widget->id(), qsizetype(42));

  delete widget;
}

void TestWidgetRegistry::create_unknown_widget()
{
  ligarium::WidgetRegistry registry;

  registry.register_widget(ligarium::Table::Property,
                           [](qsizetype id, QWidget* parent) { return new PropertyWidget(id, parent); });

  QWidget* widget = registry.create(ligarium::Table::Tenant, 42);

  QVERIFY(widget == nullptr);
}

void TestWidgetRegistry::create_with_parent()
{
  ligarium::WidgetRegistry registry;

  registry.register_widget(ligarium::Table::Property,
                           [](qsizetype id, QWidget* parent) { return new PropertyWidget(id, parent); });

  QWidget parent;

  QWidget* widget = registry.create(ligarium::Table::Property, 42, &parent);

  QVERIFY(widget != nullptr);
  QCOMPARE(widget->parentWidget(), &parent);

  auto* property_widget = qobject_cast<PropertyWidget*>(widget);

  QVERIFY(property_widget != nullptr);
  QCOMPARE(property_widget->id(), qsizetype(42));

  delete widget;
}

void TestWidgetRegistry::contains_registered_widget()
{
  ligarium::WidgetRegistry registry;

  registry.register_widget(ligarium::Table::Property,
                           [](qsizetype, QWidget* parent) { return new PropertyWidget(1, parent); });

  QVERIFY(registry.contains(ligarium::Table::Property));
  QVERIFY(!registry.contains(ligarium::Table::Tenant));
}

void TestWidgetRegistry::contains_unknown_widget()
{
  ligarium::WidgetRegistry registry;

  QVERIFY(!registry.contains(ligarium::Table::Property));
  QVERIFY(!registry.contains(ligarium::Table::Tenant));
}

void TestWidgetRegistry::replace_registered_widget()
{
  ligarium::WidgetRegistry registry;

  registry.register_widget(ligarium::Table::Property,
                           [](qsizetype, QWidget* parent) { return new PropertyWidget(1, parent); });

  registry.register_widget(ligarium::Table::Property,
                           [](qsizetype id, QWidget* parent) { return new PropertyWidget(id + 100, parent); });

  QWidget* widget = registry.create(ligarium::Table::Property, 42);

  QVERIFY(widget != nullptr);

  auto* property_widget = qobject_cast<PropertyWidget*>(widget);

  QVERIFY(property_widget != nullptr);
  QCOMPARE(property_widget->id(), qsizetype(142));

  delete widget;
}

void TestWidgetRegistry::clear_registry()
{
  ligarium::WidgetRegistry registry;

  registry.register_widget(ligarium::Table::Property,
                           [](qsizetype id, QWidget* parent) { return new PropertyWidget(id, parent); });

  registry.register_widget(ligarium::Table::Tenant,
                           [](qsizetype id, QWidget* parent) { return new PropertyWidget(id, parent); });

  QVERIFY(registry.contains(ligarium::Table::Property));
  QVERIFY(registry.contains(ligarium::Table::Tenant));

  registry.clear();

  QVERIFY(!registry.contains(ligarium::Table::Property));
  QVERIFY(!registry.contains(ligarium::Table::Tenant));

  QVERIFY(registry.create(ligarium::Table::Property, 1) == nullptr);

  QVERIFY(registry.create(ligarium::Table::Tenant, 1) == nullptr);
}

void TestWidgetRegistry::register_typed_widget()
{
  ligarium::WidgetRegistry registry;

  ligarium::register_widget<PropertyWidget>(registry, ligarium::Table::Property);

  QVERIFY(registry.contains(ligarium::Table::Property));

  QWidget* widget = registry.create(ligarium::Table::Property, 42);

  QVERIFY(widget != nullptr);

  auto* property_widget = qobject_cast<PropertyWidget*>(widget);

  QVERIFY(property_widget != nullptr);
  QCOMPARE(property_widget->id(), qsizetype(42));

  delete widget;
}

QTEST_MAIN(TestWidgetRegistry)

#include "test_widget_registry.moc"
