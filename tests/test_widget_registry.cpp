#include "test_widget_registry.h"
//

#include "test_records.h"

#include <QTest>
#include <QWidget>
#include <ligarium/widget_registry.h>

class TestWidgetRegistry : public QObject
{
  Q_OBJECT

private slots:
  void initTestCase()
  {
    ligarium::register_widget<Property, PropertyWidget, PropertyWidget>();
  }

  void create_registered_widget();
  void create_unknown_widget();
  void create_with_parent();
  void contains_unknown_widget();
  void register_typed_widget();
};


void TestWidgetRegistry::create_registered_widget()
{
  QWidget* widget = ligarium::WidgetRegistry::new_widget_record_creator(ligarium::Table::Property, 42);

  QVERIFY(widget != nullptr);

  auto* property_widget = qobject_cast<PropertyWidget*>(widget);

  QVERIFY(property_widget != nullptr);
  QCOMPARE(property_widget->id(), qsizetype(42));

  delete widget;
}


void TestWidgetRegistry::create_unknown_widget()
{
  QWidget* widget = ligarium::WidgetRegistry::new_widget_record_creator(ligarium::Table::Tenant, 42);

  QVERIFY(widget == nullptr);

  delete widget;
}

void TestWidgetRegistry::create_with_parent()
{
  QWidget parent;

  QWidget* widget = ligarium::WidgetRegistry::new_widget_record_creator(ligarium::Table::Property, 42, &parent);

  QVERIFY(widget != nullptr);
  QCOMPARE(widget->parentWidget(), &parent);

  auto* property_widget = qobject_cast<PropertyWidget*>(widget);

  QVERIFY(property_widget != nullptr);
  QCOMPARE(property_widget->id(), qsizetype(42));

  delete widget;
}

void TestWidgetRegistry::contains_unknown_widget()
{
  QVERIFY(!ligarium::WidgetRegistry::contains(ligarium::Table::Tenant));
}


void TestWidgetRegistry::register_typed_widget()
{
  ligarium::register_widget<Attachment, PropertyWidget, PropertyWidget>();

  QVERIFY(ligarium::WidgetRegistry::contains(ligarium::Table::Attachment));

  QWidget* widget = ligarium::WidgetRegistry::new_widget_record_creator(ligarium::Table::Property, 42);

  QVERIFY(widget != nullptr);

  auto* property_widget = qobject_cast<PropertyWidget*>(widget);

  QVERIFY(property_widget != nullptr);
  QCOMPARE(property_widget->id(), qsizetype(42));

  delete widget;
}

QTEST_MAIN(TestWidgetRegistry)

#include "test_widget_registry.moc"
