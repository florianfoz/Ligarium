#include "ligarium_config.h"
//

#include "database.h"
#include "field.h"
#include "polymorphic_link.h"
#include "record.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QtTest>
#include <cstdint>
#include <memory>


class Property final : public Ligarium::Record<Property>
{
public:
  static constexpr Ligarium::Table static_table = Ligarium::Table::Property;

  explicit Property(Ligarium::Database* db = nullptr)
    : Record(db)
  {
  }

  QString name;

  [[nodiscard]]
  QString dump() const override
  {
    return name;
  }

  static constexpr auto sql_fields()
  {
    return std::tuple{Ligarium::field(u"name", &Property::name)};
  }

  friend bool operator==(const Property& lhs, const Property& rhs)
  {
    return lhs.id() == rhs.id() && lhs.name == rhs.name;
  }
};

class Tenant final : public Ligarium::Record<Tenant>
{
public:
  static constexpr Ligarium::Table static_table = Ligarium::Table::Tenant;

  explicit Tenant(Ligarium::Database* db = nullptr)
    : Record(db)
  {
  }

  QString name;

  [[nodiscard]]
  QString dump() const override
  {
    return name;
  }

  static constexpr auto sql_fields()
  {
    return std::tuple{Ligarium::field(u"name", &Tenant::name)};
  }

  friend bool operator==(const Tenant& lhs, const Tenant& rhs)
  {
    return lhs.id() == rhs.id() && lhs.name == rhs.name;
  }
};

class Attachment final : public Ligarium::Record<Attachment>
{
public:
  static constexpr Ligarium::Table static_table = Ligarium::Table::Attachment;

  explicit Attachment(Ligarium::Database* db = nullptr)
    : Record(db)
  {
  }

  Ligarium::Table table = Ligarium::Table::Property;

  qsizetype col_id = Ligarium::INVALID_ID;
  QString   path;

  [[nodiscard]]
  QString dump() const override
  {
    return path;
  }

  static constexpr auto sql_fields()
  {
    return std::tuple{Ligarium::field(u"table", &Attachment::table), Ligarium::field(u"col_id", &Attachment::col_id),
                      Ligarium::field(u"path", &Attachment::path)};
  }

  friend bool operator==(const Attachment& lhs, const Attachment& rhs)
  {
    return lhs.id() == rhs.id() && lhs.table == rhs.table && lhs.col_id == rhs.col_id && lhs.path == rhs.path;
  }
};

class TestPolymorphicLink : public QObject
{
  Q_OBJECT

private:
  QSqlDatabase                        m_sql_database;
  std::unique_ptr<Ligarium::Database> m_database;

private slots:
  void initTestCase()
  {
    const QString connection_name = QStringLiteral("ligarium_polymorphic_link_test");

    m_sql_database = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), connection_name);

    m_sql_database.setDatabaseName(QStringLiteral(":memory:"));

    QVERIFY2(m_sql_database.open(), qPrintable(m_sql_database.lastError().text()));

    m_database = std::make_unique<Ligarium::Database>(m_sql_database);

    QSqlQuery query(m_sql_database);

    QVERIFY2(query.exec(QStringLiteral("CREATE TABLE property ("
                                       "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                       "name TEXT NOT NULL"
                                       ")")),
             qPrintable(query.lastError().text()));

    QVERIFY2(query.exec(QStringLiteral("CREATE TABLE tenant ("
                                       "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                       "name TEXT NOT NULL"
                                       ")")),
             qPrintable(query.lastError().text()));

    QVERIFY2(query.exec(QStringLiteral("CREATE TABLE attachment ("
                                       "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                       "table INTEGER NOT NULL,"
                                       "col_id INTEGER NOT NULL,"
                                       "path TEXT NOT NULL"
                                       ")")),
             qPrintable(query.lastError().text()));
  }

  void cleanupTestCase()
  {
    m_database.reset();

    const QString connection_name = m_sql_database.connectionName();

    m_sql_database.close();
    m_sql_database = {};

    QSqlDatabase::removeDatabase(connection_name);
  }

  void defaultState()
  {
    Ligarium::PolymorphicLink<Property> link;

    QVERIFY(link.empty());
    QCOMPARE(link.size(), qsizetype(0));
    QVERIFY(link.ids.isEmpty());
  }

  void ids()
  {
    Ligarium::PolymorphicLink<Property> link;

    link.ids = {1, 2, 3};

    QVERIFY(!link.empty());
    QCOMPARE(link.size(), qsizetype(3));
    QCOMPARE(link.ids, QList<qsizetype>({1, 2, 3}));
  }

  void clear()
  {
    Ligarium::PolymorphicLink<Property> link;

    link.ids = {1, 2};

    QVERIFY(!link.empty());

    link.ids.clear();

    QVERIFY(link.empty());
    QCOMPARE(link.size(), qsizetype(0));
  }

  void getEmpty()
  {
    Ligarium::PolymorphicLink<Property> link;

    const QList<Property> records = link.get(*m_database);

    QVERIFY(records.isEmpty());
    QCOMPARE(records.size(), qsizetype(0));
  }

  void getProperties()
  {
    QSqlQuery query(m_sql_database);

    QVERIFY2(query.exec(QStringLiteral("INSERT INTO property (name) VALUES "
                                       "('House'), "
                                       "('Apartment'), "
                                       "('Office')")),
             qPrintable(query.lastError().text()));

    const QList<qsizetype> ids = {1, 2, 3};

    Ligarium::PolymorphicLink<Property> link;
    link.ids = ids;

    const QList<Property> properties = link.get(*m_database);

    QCOMPARE(properties.size(), qsizetype(3));

    QCOMPARE(properties.at(0).id(), qsizetype(1));
    QCOMPARE(properties.at(0).name, QStringLiteral("House"));

    QCOMPARE(properties.at(1).id(), qsizetype(2));
    QCOMPARE(properties.at(1).name, QStringLiteral("Apartment"));

    QCOMPARE(properties.at(2).id(), qsizetype(3));
    QCOMPARE(properties.at(2).name, QStringLiteral("Office"));
  }

  void getTenants()
  {
    QSqlQuery query(m_sql_database);

    QVERIFY2(query.exec(QStringLiteral("INSERT INTO tenant (name) VALUES "
                                       "('Alice'), "
                                       "('Bob')")),
             qPrintable(query.lastError().text()));

    Ligarium::PolymorphicLink<Tenant> link;

    link.ids = {1, 2};

    const QList<Tenant> tenants = link.get(*m_database);

    QCOMPARE(tenants.size(), qsizetype(2));

    QCOMPARE(tenants.at(0).id(), qsizetype(1));
    QCOMPARE(tenants.at(0).name, QStringLiteral("Alice"));

    QCOMPARE(tenants.at(1).id(), qsizetype(2));
    QCOMPARE(tenants.at(1).name, QStringLiteral("Bob"));
  }

  void preservesIdOrder()
  {
    QSqlQuery query(m_sql_database);

    QVERIFY2(query.exec(QStringLiteral("INSERT INTO property (name) VALUES "
                                       "('House'), "
                                       "('Apartment'), "
                                       "('Office')")),
             qPrintable(query.lastError().text()));

    Ligarium::PolymorphicLink<Property> link;

    link.ids = {3, 1, 2};

    const QList<Property> properties = link.get(*m_database);

    QCOMPARE(properties.size(), qsizetype(3));

    QCOMPARE(properties.at(0).id(), qsizetype(3));
    QCOMPARE(properties.at(0).name, QStringLiteral("Office"));

    QCOMPARE(properties.at(1).id(), qsizetype(1));
    QCOMPARE(properties.at(1).name, QStringLiteral("House"));

    QCOMPARE(properties.at(2).id(), qsizetype(2));
    QCOMPARE(properties.at(2).name, QStringLiteral("Apartment"));
  }

  void polymorphicTarget()
  {
    QSqlQuery query(m_sql_database);

    QVERIFY2(query.exec(QStringLiteral("INSERT INTO property (name) "
                                       "VALUES ('House')")),
             qPrintable(query.lastError().text()));

    const qsizetype property_id = query.lastInsertId().toLongLong();

    Ligarium::PolymorphicLink<Property> link;
    link.ids = {property_id};

    const QList<Property> properties = link.get(*m_database);

    QCOMPARE(properties.size(), qsizetype(1));
    QCOMPARE(properties.first().id(), property_id);
    QCOMPARE(properties.first().name, QStringLiteral("House"));
  }
};

QTEST_MAIN(TestPolymorphicLink)

#include "test_polymorphic_link.moc"