
#include "ligarium_config.h"
//
#include "test_records.h"
#include "test_widget_registry.h"

#include <QApplication>
#include <QSqlDatabase>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QTest>
#include <QWidget>
#include <ligarium/database.h>
#include <ligarium/field.h>
#include <ligarium/link.h>
#include <ligarium/record.h>
#include <ligarium/schema.h>
#include <ligarium/widget_registry.h>
#include <memory>

namespace
{


class TestAll : public QObject
{
  Q_OBJECT

  ligarium::Database* m_db;

private slots:
  void database_and_records();
  void fields_and_records();
  void links();
  void polymorphic_links();
  void widget_registry();
  void complete_workflow();
};


void TestAll::database_and_records()
{
  const QString connection_name = "test_all_database_records";

  {
    QSqlDatabase connection = QSqlDatabase::addDatabase("QSQLITE", connection_name);

    connection.setDatabaseName(":memory:");

    QVERIFY(connection.open());

    ligarium::SchemaBuilder schema(connection);

    if (!schema.create_all<Property, Tenant, Attachment>()) {
      QVERIFY(qPrintable(schema.last_error()));
    }

    ligarium::Database db(connection);

    QVERIFY(db.is_open());

    auto property    = Property::create_record(db);
    property.name    = "House";
    property.surface = 120.5;

    QVERIFY(property.save_record());
    QVERIFY(property.is_valid());
    QVERIFY(property.id() != ligarium::INVALID_ID);

    const qsizetype property_id = property.id();

    Property loaded = Property::read_record(db, property_id);

    QCOMPARE(loaded.id(), property_id);
    QCOMPARE(loaded.name, "House");
    QCOMPARE(loaded.surface, 120.5);

    property.name = "Updated House";

    QVERIFY(property.save_record());

    loaded = Property::read_record(db, property_id);

    QCOMPARE(loaded.name, "Updated House");

    QVERIFY(property.delete_record());
    QVERIFY(!property.is_valid());

    QVERIFY(!ligarium::contains_record(db, ligarium::Table::Property, property_id));
  }

  QSqlDatabase::removeDatabase(connection_name);

  {
    const QString connection_name = "test_all_database_records";

    QSqlDatabase connection = QSqlDatabase::addDatabase("QSQLITE", connection_name);

    connection.setDatabaseName(":memory:");

    QVERIFY(connection.open());

    ligarium::SchemaBuilder schema(connection);

    if (!schema.create_all<Property, Tenant, Attachment>()) {
      QVERIFY(qPrintable(schema.last_error()));
    }

    m_db = new ligarium::Database(connection);

    QVERIFY(m_db->is_open());
  }
}


void TestAll::fields_and_records()
{
  const QString connection_name = "test_all_fields";

  {
    QSqlDatabase connection = QSqlDatabase::addDatabase("QSQLITE", connection_name);

    connection.setDatabaseName(":memory:");

    QVERIFY(connection.open());

    ligarium::SchemaBuilder schema(connection);

    if (!schema.create_all<Property, Tenant, Attachment>()) {
      QVERIFY(qPrintable(schema.last_error()));
    }

    ligarium::Database db(connection);

    auto property    = Property::create_record(db);
    property.name    = "Apartment";
    property.surface = 85.25;

    QVERIFY(property.save_record());

    QSqlQuery query(connection);

    QVERIFY(query.exec(QStringLiteral("SELECT name, surface "
                                      "FROM Property "
                                      "WHERE id = %1")
                           .arg(property.id())));

    QVERIFY(query.next());

    QCOMPARE(query.value("name").toString(), "Apartment");

    QCOMPARE(query.value("surface").toDouble(), 85.25);
  }

  QSqlDatabase::removeDatabase(connection_name);
}

void TestAll::links()
{
  ligarium::Link<Tenant, ligarium::ERelation::ManyToOne> link;

  QVERIFY(!link.is_valid());

  link.set_id(42);

  QVERIFY(link.is_valid());
  QCOMPARE(link.id(), qsizetype(42));

  ligarium::Link<Property, ligarium::ERelation::OneToMany> properties;

  QVERIFY(properties.empty());
  QCOMPARE(properties.size(), qsizetype(0));

  properties.set_ids({1, 2, 3});

  QVERIFY(!properties.empty());
  QCOMPARE(properties.size(), qsizetype(3));
}

void TestAll::polymorphic_links()
{
  ligarium::Link<Attachment, ligarium::ERelation::PolymorphicOneToMany> link;

  QVERIFY(link.empty());
  QCOMPARE(link.size(), qsizetype(0));

  link.set_ids({1, 2, 3});

  QVERIFY(!link.empty());
  QCOMPARE(link.size(), qsizetype(3));
}


void TestAll::widget_registry()
{
  ligarium::register_widget<Property, PropertyWidget, PropertyWidget>();

  QVERIFY(ligarium::WidgetRegistry::contains(ligarium::Table::Property));

  QVERIFY(!ligarium::WidgetRegistry::contains(ligarium::Table::Tenant));

  QWidget parent;

  QWidget* widget = ligarium::WidgetRegistry::new_widget_record_creator(ligarium::Table::Property, 42, &parent);

  QVERIFY(widget != nullptr);
  QCOMPARE(widget->parentWidget(), &parent);

  auto* property_widget = qobject_cast<PropertyWidget*>(widget);

  QVERIFY(property_widget != nullptr);
  QCOMPARE(property_widget->id(), qsizetype(42));

  delete widget;
}

void TestAll::complete_workflow()
{
  const QString connection_name = "test_all_workflow";

  {
    QSqlDatabase connection = QSqlDatabase::addDatabase("QSQLITE", connection_name);

    connection.setDatabaseName(":memory:");

    QVERIFY(connection.open());

    ligarium::SchemaBuilder schema(connection);

    if (!schema.create_all<Property, Tenant, Attachment>()) {
      QVERIFY(qPrintable(schema.last_error()));
    }

    ligarium::Database db(connection);

    auto tenant = Tenant::create_record(db);
    tenant.name = "Alice";

    QVERIFY(tenant.save_record());

    auto property    = Property::create_record(db);
    property.name    = "Main house";
    property.surface = 150.0;

    QVERIFY(property.save_record());

    ligarium::Link<Tenant, ligarium::ERelation::ManyToOne> tenant_link;

    tenant_link.set_id(tenant.id());

    QVERIFY(tenant_link.is_valid());
    QCOMPARE(tenant_link.id(), tenant.id());


    QWidget parent;

    std::unique_ptr<QWidget> widget(
        ligarium::WidgetRegistry::new_widget_record_creator(ligarium::Table::Property, property.id(), &parent));

    QVERIFY(widget != nullptr);

    auto* property_widget = qobject_cast<PropertyWidget*>(widget.get());

    QVERIFY(property_widget != nullptr);

    QCOMPARE(property_widget->id(), property.id());

    QVERIFY(property.is_valid());
    QVERIFY(tenant.is_valid());
  }

  QSqlDatabase::removeDatabase(connection_name);
}


} // namespace


int main(int argc, char* argv[])
{
  QApplication application(argc, argv);

  ligarium::register_widget<Property, PropertyWidget, PropertyWidget>();

  TestAll test;

  return QTest::qExec(&test, argc, argv);
}

#include "test_all.moc"