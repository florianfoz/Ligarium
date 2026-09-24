
#include "ligarium_config.h"
//
#include <QApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTest>
#include <QWidget>
#include <ligarium/database.h>
#include <ligarium/field.h>
#include <ligarium/link.h>
#include <ligarium/polymorphic_link.h>
#include <ligarium/record.h>
#include <ligarium/record_registry.h>
#include <ligarium/schema.h>
#include <ligarium/widget_registry.h>
#include <memory>

namespace
{

class Property : public ligarium::Record<Property>
{
public:
  static constexpr auto static_table = ligarium::Table::Property;

  QString name;
  double  surface = 0.0;

  Property(ligarium::Database* db = nullptr)
    : Record(db)
  {
  }

  static constexpr auto sql_fields()
  {
    return std::tuple{
        ligarium::field(u"name", &Property::name),
        ligarium::field(u"surface", &Property::surface),
    };
  }

  [[nodiscard]]
  QString dump() const override
  {
    return QStringLiteral("Property(id=%1, name=%2)").arg(id()).arg(name);
  }

  bool operator==(const Property& other) const noexcept
  {
    return id() == other.id() && name == other.name && surface == other.surface;
  }
};

class Tenant : public ligarium::Record<Tenant>
{
public:
  static constexpr auto static_table = ligarium::Table::Tenant;

  QString name;

  Tenant(ligarium::Database* db = nullptr)
    : Record(db)
  {
  }

  static constexpr auto sql_fields()
  {
    return std::tuple{
        ligarium::field(u"name", &Tenant::name),
    };
  }

  [[nodiscard]]
  QString dump() const override
  {
    return QStringLiteral("Tenant(id=%1, name=%2)").arg(id()).arg(name);
  }

  bool operator==(const Tenant& other) const noexcept
  {
    return id() == other.id() && name == other.name;
  }
};

class Attachment : public ligarium::Record<Attachment>
{
public:
  static constexpr auto static_table = ligarium::Table::Attachment;

  ligarium::Table table{};
  qsizetype       col_id = ligarium::INVALID_ID;
  QString         path;

  Attachment(ligarium::Database* db = nullptr)
    : Record(db)
  {
  }

  static constexpr auto sql_fields()
  {
    return std::tuple{
        ligarium::field(u"table", &Attachment::table),
        ligarium::field(u"col_id", &Attachment::col_id),
        ligarium::field(u"path", &Attachment::path),
    };
  }

  [[nodiscard]]
  QString dump() const override
  {
    return QStringLiteral("Attachment(id=%1, path=%2)").arg(id()).arg(path);
  }

  bool operator==(const Attachment& other) const noexcept
  {
    return id() == other.id() && table == other.table && col_id == other.col_id && path == other.path;
  }
};

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

class TestAll : public QObject
{
  Q_OBJECT

  ligarium::Database* m_db;

private slots:
  void database_and_records();
  void fields_and_records();
  void links();
  void polymorphic_links();
  void record_registry();
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

    Property property(&db);
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

    Property property(&db);
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
  ligarium::PolymorphicLink<Attachment> link;

  QVERIFY(link.empty());
  QCOMPARE(link.size(), qsizetype(0));

  link.ids = {1, 2, 3};

  QVERIFY(!link.empty());
  QCOMPARE(link.size(), qsizetype(3));
}

void TestAll::record_registry()
{
  ligarium::RecordRegistry registry(m_db);

  ligarium::register_record<Property>(registry, ligarium::Table::Property);

  ligarium::register_record<Tenant>(registry, ligarium::Table::Tenant);

  QVERIFY(registry.contains(ligarium::Table::Property));

  QVERIFY(registry.contains(ligarium::Table::Tenant));

  QVERIFY(!registry.contains(ligarium::Table::Attachment));

  QCOMPARE(registry.dump(ligarium::Table::Property, 12), "Property(id=12, name=)");

  QCOMPARE(registry.dump(ligarium::Table::Tenant, 7), "Tenant(id=7, name=)");

  QCOMPARE(registry.dump(ligarium::Table::Attachment, 1), QString{});

  registry.clear();

  QVERIFY(!registry.contains(ligarium::Table::Property));

  QVERIFY(!registry.contains(ligarium::Table::Tenant));
}

void TestAll::widget_registry()
{
  ligarium::WidgetRegistry registry;

  ligarium::register_widget<PropertyWidget>(registry, ligarium::Table::Property);

  QVERIFY(registry.contains(ligarium::Table::Property));

  QVERIFY(!registry.contains(ligarium::Table::Tenant));

  QWidget parent;

  QWidget* widget = registry.create(ligarium::Table::Property, 42, &parent);

  QVERIFY(widget != nullptr);
  QCOMPARE(widget->parentWidget(), &parent);

  auto* property_widget = qobject_cast<PropertyWidget*>(widget);

  QVERIFY(property_widget != nullptr);
  QCOMPARE(property_widget->id(), qsizetype(42));

  delete widget;

  registry.clear();

  QVERIFY(!registry.contains(ligarium::Table::Property));

  QVERIFY(registry.create(ligarium::Table::Property, 42) == nullptr);
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

    Tenant tenant(&db);
    tenant.name = "Alice";

    QVERIFY(tenant.save_record());

    Property property(&db);
    property.name    = "Main house";
    property.surface = 150.0;

    QVERIFY(property.save_record());

    ligarium::Link<Tenant, ligarium::ERelation::ManyToOne> tenant_link;

    tenant_link.set_id(tenant.id());

    QVERIFY(tenant_link.is_valid());
    QCOMPARE(tenant_link.id(), tenant.id());

    ligarium::RecordRegistry record_registry(&db);

    record_registry.register_record(ligarium::Table::Property, [&record_registry](qsizetype id) {
      const Property property = Property::read_record(*record_registry.database(), id);

      return property.dump();
    });

    QCOMPARE(record_registry.dump(ligarium::Table::Property, property.id()),
             QStringLiteral("Property(id=%1, name=Main house)").arg(property.id()));

    ligarium::WidgetRegistry widget_registry;

    ligarium::register_widget<PropertyWidget>(widget_registry, ligarium::Table::Property);

    QWidget parent;

    std::unique_ptr<QWidget> widget(widget_registry.create(ligarium::Table::Property, property.id(), &parent));

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

  TestAll test;

  return QTest::qExec(&test, argc, argv);
}

#include "test_all.moc"