#include "ligarium_config.h"
//

#include "ligarium/database.h"
#include "ligarium/field.h"
#include "ligarium/polymorphic_link.h"
#include "ligarium/record.h"
#include "ligarium/schema.h"
#include "test_records.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QtTest>
#include <cstdint>
#include <memory>


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

    QSqlDatabase connection = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), connection_name);

    connection.setDatabaseName(QStringLiteral(":memory:"));

    QVERIFY(connection.open());

    Ligarium::SchemaBuilder schema(connection);

    if (!schema.create_all<Property, Tenant, Attachment>()) {
      QVERIFY(qPrintable(schema.last_error()));
    }

    m_database = std::make_unique<Ligarium::Database>(connection);
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