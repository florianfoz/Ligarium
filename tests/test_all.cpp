
#include "ligarium_config.h"
//
#include "database.h"
#include "field.h"
#include "link.h"
#include "polymorphic_link.h"
#include "record.h"
#include "record_registry.h"
#include "schema.h"
#include "widget_registry.h"

#include <QApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTest>
#include <QWidget>
#include <memory>

namespace
{

class Property : public Ligarium::Record<Property>
{
public:
  static constexpr Ligarium::Table static_table = Ligarium::Table::Property;

  QString name;
  double  surface = 0.0;

  Property(Ligarium::Database* db = nullptr)
    : Record(db)
  {
  }

  static constexpr auto sql_fields()
  {
    return std::tuple{
        Ligarium::field(u"name", &Property::name),
        Ligarium::field(u"surface", &Property::surface),
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

class Tenant : public Ligarium::Record<Tenant>
{
public:
  static constexpr Ligarium::Table static_table = Ligarium::Table::Tenant;

  QString name;

  Tenant(Ligarium::Database* db = nullptr)
    : Record(db)
  {
  }

  static constexpr auto sql_fields()
  {
    return std::tuple{
        Ligarium::field(u"name", &Tenant::name),
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

class Attachment : public Ligarium::Record<Attachment>
{
public:
  static constexpr Ligarium::Table static_table = Ligarium::Table::Attachment;

  Ligarium::Table table{};
  qsizetype       col_id = Ligarium::INVALID_ID;
  QString         path;

  Attachment(Ligarium::Database* db = nullptr)
    : Record(db)
  {
  }

  static constexpr auto sql_fields()
  {
    return std::tuple{
        Ligarium::field(u"table", &Attachment::table),
        Ligarium::field(u"col_id", &Attachment::col_id),
        Ligarium::field(u"path", &Attachment::path),
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
  qsizetype m_id = Ligarium::INVALID_ID;
};

class TestAll : public QObject
{
  Q_OBJECT

  Ligarium::Database* m_db;

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
  const QString connection_name = QStringLiteral("test_all_database_records");

  {
    QSqlDatabase connection = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), connection_name);

    connection.setDatabaseName(QStringLiteral(":memory:"));

    QVERIFY(connection.open());

    Ligarium::SchemaBuilder schema(connection);

    if (!schema.create_all<Property, Tenant, Attachment>()) {
      QVERIFY(qPrintable(schema.last_error()));
    }

    Ligarium::Database db(connection);

    QVERIFY(db.is_open());

    Property property(&db);
    property.name    = QStringLiteral("House");
    property.surface = 120.5;

    QVERIFY(property.save_record());
    QVERIFY(property.is_valid());
    QVERIFY(property.id() != Ligarium::INVALID_ID);

    const qsizetype property_id = property.id();

    Property loaded = Property::read_record(db, property_id);

    QCOMPARE(loaded.id(), property_id);
    QCOMPARE(loaded.name, QStringLiteral("House"));
    QCOMPARE(loaded.surface, 120.5);

    property.name = QStringLiteral("Updated House");

    QVERIFY(property.save_record());

    loaded = Property::read_record(db, property_id);

    QCOMPARE(loaded.name, QStringLiteral("Updated House"));

    QVERIFY(property.delete_record());
    QVERIFY(!property.is_valid());

    QVERIFY(!Ligarium::contains_record(db, Ligarium::Table::Property, property_id));
  }

  QSqlDatabase::removeDatabase(connection_name);

  {
    const QString connection_name = QStringLiteral("test_all_database_records");

    QSqlDatabase connection = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), connection_name);

    connection.setDatabaseName(QStringLiteral(":memory:"));

    QVERIFY(connection.open());

    QSqlQuery schema(connection);
    m_db = new Ligarium::Database(connection);

    QVERIFY(m_db->is_open());
  }
}


void TestAll::fields_and_records()
{
  const QString connection_name = QStringLiteral("test_all_fields");

  {
    QSqlDatabase connection = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), connection_name);

    connection.setDatabaseName(QStringLiteral(":memory:"));

    QVERIFY(connection.open());

    QSqlQuery schema(connection);

    QVERIFY(
        schema.exec(QStringLiteral("CREATE TABLE Property ("
                                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                   "name TEXT NOT NULL,"
                                   "surface REAL NOT NULL"
                                   ")")));

    Ligarium::Database db(connection);

    Property property(&db);
    property.name    = QStringLiteral("Apartment");
    property.surface = 85.25;

    QVERIFY(property.save_record());

    QSqlQuery query(connection);

    QVERIFY(query.exec(QStringLiteral("SELECT name, surface "
                                      "FROM Property "
                                      "WHERE id = %1")
                           .arg(property.id())));

    QVERIFY(query.next());

    QCOMPARE(query.value(QStringLiteral("name")).toString(), QStringLiteral("Apartment"));

    QCOMPARE(query.value(QStringLiteral("surface")).toDouble(), 85.25);
  }

  QSqlDatabase::removeDatabase(connection_name);
}

void TestAll::links()
{
  Ligarium::Link<Tenant, Ligarium::ERelation::ManyToOne> link;

  QVERIFY(!link.is_valid());

  link.set_id(42);

  QVERIFY(link.is_valid());
  QCOMPARE(link.id(), qsizetype(42));

  Ligarium::Link<Property, Ligarium::ERelation::OneToMany> properties;

  QVERIFY(properties.empty());
  QCOMPARE(properties.size(), qsizetype(0));

  properties.set_ids({1, 2, 3});

  QVERIFY(!properties.empty());
  QCOMPARE(properties.size(), qsizetype(3));
}

void TestAll::polymorphic_links()
{
  Ligarium::PolymorphicLink<Attachment> link;

  QVERIFY(link.empty());
  QCOMPARE(link.size(), qsizetype(0));

  link.ids = {1, 2, 3};

  QVERIFY(!link.empty());
  QCOMPARE(link.size(), qsizetype(3));
}

void TestAll::record_registry()
{
  Ligarium::RecordRegistry registry(m_db);

  Ligarium::register_record<Property>(registry, Ligarium::Table::Property);

  Ligarium::register_record<Tenant>(registry, Ligarium::Table::Tenant);

  QVERIFY(registry.contains(Ligarium::Table::Property));

  QVERIFY(registry.contains(Ligarium::Table::Tenant));

  QVERIFY(!registry.contains(Ligarium::Table::Attachment));

  QCOMPARE(registry.dump(Ligarium::Table::Property, 12), QStringLiteral("Property(id=12, name=)"));

  QCOMPARE(registry.dump(Ligarium::Table::Tenant, 7), QStringLiteral("Tenant(id=7, name=)"));

  QCOMPARE(registry.dump(Ligarium::Table::Attachment, 1), QString{});

  registry.clear();

  QVERIFY(!registry.contains(Ligarium::Table::Property));

  QVERIFY(!registry.contains(Ligarium::Table::Tenant));
}

void TestAll::widget_registry()
{
  Ligarium::WidgetRegistry registry;

  Ligarium::register_widget<PropertyWidget>(registry, Ligarium::Table::Property);

  QVERIFY(registry.contains(Ligarium::Table::Property));

  QVERIFY(!registry.contains(Ligarium::Table::Tenant));

  QWidget parent;

  QWidget* widget = registry.create(Ligarium::Table::Property, 42, &parent);

  QVERIFY(widget != nullptr);
  QCOMPARE(widget->parentWidget(), &parent);

  auto* property_widget = qobject_cast<PropertyWidget*>(widget);

  QVERIFY(property_widget != nullptr);
  QCOMPARE(property_widget->id(), qsizetype(42));

  delete widget;

  registry.clear();

  QVERIFY(!registry.contains(Ligarium::Table::Property));

  QVERIFY(registry.create(Ligarium::Table::Property, 42) == nullptr);
}

void TestAll::complete_workflow()
{
  const QString connection_name = QStringLiteral("test_all_workflow");

  {
    QSqlDatabase connection = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), connection_name);

    connection.setDatabaseName(QStringLiteral(":memory:"));

    QVERIFY(connection.open());

    QSqlQuery schema(connection);

    QVERIFY(
        schema.exec(QStringLiteral("CREATE TABLE Tenant ("
                                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                   "name TEXT NOT NULL"
                                   ")")));

    QVERIFY(
        schema.exec(QStringLiteral("CREATE TABLE Property ("
                                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                   "name TEXT NOT NULL,"
                                   "surface REAL NOT NULL"
                                   ")")));

    Ligarium::Database db(connection);

    Tenant tenant(&db);
    tenant.name = QStringLiteral("Alice");

    QVERIFY(tenant.save_record());

    Property property(&db);
    property.name    = QStringLiteral("Main house");
    property.surface = 150.0;

    QVERIFY(property.save_record());

    Ligarium::Link<Tenant, Ligarium::ERelation::ManyToOne> tenant_link;

    tenant_link.set_id(tenant.id());

    QVERIFY(tenant_link.is_valid());
    QCOMPARE(tenant_link.id(), tenant.id());

    Ligarium::RecordRegistry record_registry(&db);

    record_registry.register_record(Ligarium::Table::Property, [&record_registry](qsizetype id) {
      const Property property = Property::read_record(*record_registry.database(), id);

      return property.dump();
    });

    QCOMPARE(record_registry.dump(Ligarium::Table::Property, property.id()),
             QStringLiteral("Property(id=%1, name=Main house)").arg(property.id()));

    Ligarium::WidgetRegistry widget_registry;

    Ligarium::register_widget<PropertyWidget>(widget_registry, Ligarium::Table::Property);

    QWidget parent;

    std::unique_ptr<QWidget> widget(widget_registry.create(Ligarium::Table::Property, property.id(), &parent));

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