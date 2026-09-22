#include "test_config.h"
//

#include "widget_registry.h"

#include <QTest>
#include <QWidget>

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
  qsizetype m_id = Ligarium::INVALID_ID;
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
  Ligarium::WidgetRegistry registry;

  QVERIFY(!registry.contains(ApplicationTable::Property));
  QVERIFY(registry.create(ApplicationTable::Property, 1) == nullptr);
}

void TestWidgetRegistry::register_widget()
{
  Ligarium::WidgetRegistry registry;

  registry.register_widget(ApplicationTable::Property, [](qsizetype id, QWidget* parent) {
    auto* widget = new PropertyWidget(id, parent);
    return widget;
  });

  QVERIFY(registry.contains(ApplicationTable::Property));
}

void TestWidgetRegistry::create_registered_widget()
{
  Ligarium::WidgetRegistry registry;

  registry.register_widget(ApplicationTable::Property,
                           [](qsizetype id, QWidget* parent) { return new PropertyWidget(id, parent); });

  QWidget* widget = registry.create(ApplicationTable::Property, 42);

  QVERIFY(widget != nullptr);

  auto* property_widget = qobject_cast<PropertyWidget*>(widget);

  QVERIFY(property_widget != nullptr);
  QCOMPARE(property_widget->id(), qsizetype(42));

  delete widget;
}

void TestWidgetRegistry::create_unknown_widget()
{
  Ligarium::WidgetRegistry registry;

  registry.register_widget(ApplicationTable::Property,
                           [](qsizetype id, QWidget* parent) { return new PropertyWidget(id, parent); });

  QWidget* widget = registry.create(ApplicationTable::Tenant, 42);

  QVERIFY(widget == nullptr);
}

void TestWidgetRegistry::create_with_parent()
{
  Ligarium::WidgetRegistry registry;

  registry.register_widget(ApplicationTable::Property,
                           [](qsizetype id, QWidget* parent) { return new PropertyWidget(id, parent); });

  QWidget parent;

  QWidget* widget = registry.create(ApplicationTable::Property, 42, &parent);

  QVERIFY(widget != nullptr);
  QCOMPARE(widget->parentWidget(), &parent);

  auto* property_widget = qobject_cast<PropertyWidget*>(widget);

  QVERIFY(property_widget != nullptr);
  QCOMPARE(property_widget->id(), qsizetype(42));

  delete widget;
}

void TestWidgetRegistry::contains_registered_widget()
{
  Ligarium::WidgetRegistry registry;

  registry.register_widget(ApplicationTable::Property,
                           [](qsizetype, QWidget* parent) { return new PropertyWidget(1, parent); });

  QVERIFY(registry.contains(ApplicationTable::Property));
  QVERIFY(!registry.contains(ApplicationTable::Tenant));
}

void TestWidgetRegistry::contains_unknown_widget()
{
  Ligarium::WidgetRegistry registry;

  QVERIFY(!registry.contains(ApplicationTable::Property));
  QVERIFY(!registry.contains(ApplicationTable::Tenant));
}

void TestWidgetRegistry::replace_registered_widget()
{
  Ligarium::WidgetRegistry registry;

  registry.register_widget(ApplicationTable::Property,
                           [](qsizetype, QWidget* parent) { return new PropertyWidget(1, parent); });

  registry.register_widget(ApplicationTable::Property,
                           [](qsizetype id, QWidget* parent) { return new PropertyWidget(id + 100, parent); });

  QWidget* widget = registry.create(ApplicationTable::Property, 42);

  QVERIFY(widget != nullptr);

  auto* property_widget = qobject_cast<PropertyWidget*>(widget);

  QVERIFY(property_widget != nullptr);
  QCOMPARE(property_widget->id(), qsizetype(142));

  delete widget;
}

void TestWidgetRegistry::clear_registry()
{
  Ligarium::WidgetRegistry registry;

  registry.register_widget(ApplicationTable::Property,
                           [](qsizetype id, QWidget* parent) { return new PropertyWidget(id, parent); });

  registry.register_widget(ApplicationTable::Tenant,
                           [](qsizetype id, QWidget* parent) { return new PropertyWidget(id, parent); });

  QVERIFY(registry.contains(ApplicationTable::Property));
  QVERIFY(registry.contains(ApplicationTable::Tenant));

  registry.clear();

  QVERIFY(!registry.contains(ApplicationTable::Property));
  QVERIFY(!registry.contains(ApplicationTable::Tenant));

  QVERIFY(registry.create(ApplicationTable::Property, 1) == nullptr);

  QVERIFY(registry.create(ApplicationTable::Tenant, 1) == nullptr);
}

void TestWidgetRegistry::register_typed_widget()
{
  Ligarium::WidgetRegistry registry;

  Ligarium::register_widget<PropertyWidget>(registry, ApplicationTable::Property);

  QVERIFY(registry.contains(ApplicationTable::Property));

  QWidget* widget = registry.create(ApplicationTable::Property, 42);

  QVERIFY(widget != nullptr);

  auto* property_widget = qobject_cast<PropertyWidget*>(widget);

  QVERIFY(property_widget != nullptr);
  QCOMPARE(property_widget->id(), qsizetype(42));

  delete widget;
}

QTEST_MAIN(TestWidgetRegistry)

#include "test_widget_registry.moc"
