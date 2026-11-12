#include "test_widget_registry.h"
//

#include "records.h"

#include <QTest>
#include <QWidget>
#include <ligarium/widget_registry.h>

class TestWidgetRegistry : public QObject
{
  Q_OBJECT
public:
  TestWidgetRegistry()
  {
    auto connexion = QSqlDatabase::addDatabase("QSQLITE", "my_connection_name");
    connexion.setDatabaseName(":memory:");

    m_db = new ligarium::Database(std::move(connexion));
  }

  ligarium::Database* m_db = nullptr;


private slots:
  void initTestCase()
  {
    ligarium::register_widget<Property, PropertyWidget>();
  }

  void create_registered_widget();
  void create_unknown_widget();
  void create_with_parent();
  void contains_unknown_widget();
  void register_typed_widget();
};


void TestWidgetRegistry::create_registered_widget()
{
  QWidget* widget = ligarium::WidgetRegistry::new_widget_record(ligarium::Table::Property, 42, *m_db);

  QVERIFY(widget != nullptr);

  auto* property_widget = qobject_cast<PropertyWidget*>(widget);

  QVERIFY(property_widget != nullptr);
  QCOMPARE(property_widget->id(), qsizetype(42));

  delete widget;
}


void TestWidgetRegistry::create_unknown_widget()
{
  QWidget* widget = ligarium::WidgetRegistry::new_widget_record(ligarium::Table::Tenant, 42, *m_db);

  QVERIFY(widget == nullptr);

  delete widget;
}

void TestWidgetRegistry::create_with_parent()
{
  QWidget parent;

  QWidget* widget = ligarium::WidgetRegistry::new_widget_record(ligarium::Table::Property, 42, *m_db, &parent);

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
  ligarium::register_widget<Attachment, PropertyWidget>();

  QVERIFY(ligarium::WidgetRegistry::contains(ligarium::Table::Attachment));

  QWidget* widget = ligarium::WidgetRegistry::new_widget_record(ligarium::Table::Property, 42, *m_db);

  QVERIFY(widget != nullptr);

  auto* property_widget = qobject_cast<PropertyWidget*>(widget);

  QVERIFY(property_widget != nullptr);
  QCOMPARE(property_widget->id(), qsizetype(42));

  delete widget;
}

QTEST_MAIN(TestWidgetRegistry)

#include "test_widget_registry.moc"
