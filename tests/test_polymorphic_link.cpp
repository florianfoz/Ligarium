#include "ligarium_config.h"
//

#include "test_records.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QtTest>
#include <cstdint>
#include <ligarium/database.h>
#include <ligarium/field.h>
#include <ligarium/record.h>
#include <ligarium/schema.h>
#include <memory>


class TestPolymorphicLink : public QObject
{
  Q_OBJECT

private:
  QSqlDatabase        m_sql_database;
  ligarium::Database* m_database;

private slots:
  void initTestCase()
  {
    const QString connection_name = "ligarium_polymorphic_link_test";

    m_sql_database = QSqlDatabase::addDatabase("QSQLITE", connection_name);

    m_sql_database.setDatabaseName(":memory:");

    QVERIFY(m_sql_database.open());

    ligarium::SchemaBuilder schema(m_sql_database);

    if (!schema.create_all<Property, Tenant, Attachment>()) {
      QVERIFY(qPrintable(schema.last_error()));
    }

    m_database = new ligarium::Database(m_sql_database);
  }

  void cleanupTestCase()
  {
    delete m_database;

    const QString connection_name = m_sql_database.connectionName();

    m_sql_database.close();
    m_sql_database = {};

    QSqlDatabase::removeDatabase(connection_name);
  }

  void defaultState()
  {
    ligarium::Link<Property, ligarium::ERelation::PolymorphicOneToMany> link;

    QVERIFY(link.empty());
    QCOMPARE(link.size(), qsizetype(0));
    QVERIFY(link.empty());
  }

  void ids()
  {
    ligarium::Link<Property, ligarium::ERelation::PolymorphicOneToMany> link;

    link.set_ids({1, 2, 3});

    QVERIFY(!link.empty());
    QCOMPARE(link.size(), qsizetype(3));
    QCOMPARE(link.ids(), QList<qsizetype>({1, 2, 3}));
  }

  void clear()
  {
    ligarium::Link<Property, ligarium::ERelation::PolymorphicOneToMany> link;

    link.set_ids({1, 2});

    QVERIFY(!link.empty());

    link.ids().clear();

    QVERIFY(link.empty());
    QCOMPARE(link.size(), qsizetype(0));
  }

  void getEmpty()
  {
    ligarium::Link<Property, ligarium::ERelation::PolymorphicOneToMany> link;

    const QList<Property> records = link.get(*m_database);

    QVERIFY(records.isEmpty());
    QCOMPARE(records.size(), qsizetype(0));
  }

  void getProperties()
  {
    QSqlQuery query(m_sql_database);

    QVERIFY2(query.exec("INSERT INTO property (name) VALUES "
                        "('House'), "
                        "('Apartment'), "
                        "('Office')"),
             qPrintable(query.lastError().text()));

    const QList<qsizetype> ids = {1, 2, 3};

    ligarium::Link<Property, ligarium::ERelation::PolymorphicOneToMany> link;
    link.set_ids(ids);

    const QList<Property> properties = link.get(*m_database);

    QCOMPARE(properties.size(), qsizetype(3));

    QCOMPARE(properties.at(0).id(), qsizetype(1));
    QCOMPARE(properties.at(0).name, "House");

    QCOMPARE(properties.at(1).id(), qsizetype(2));
    QCOMPARE(properties.at(1).name, "Apartment");

    QCOMPARE(properties.at(2).id(), qsizetype(3));
    QCOMPARE(properties.at(2).name, "Office");
  }

  void getTenants()
  {
    QSqlQuery query(m_sql_database);

    QVERIFY2(query.exec("INSERT INTO tenant (name) VALUES "
                        "('Alice'), "
                        "('Bob')"),
             qPrintable(query.lastError().text()));

    ligarium::Link<Tenant, ligarium::ERelation::PolymorphicOneToMany> link;

    link.set_ids({1, 2});

    const QList<Tenant> tenants = link.get(*m_database);

    QCOMPARE(tenants.size(), qsizetype(2));

    QCOMPARE(tenants.at(0).id(), qsizetype(1));
    QCOMPARE(tenants.at(0).name, "Alice");

    QCOMPARE(tenants.at(1).id(), qsizetype(2));
    QCOMPARE(tenants.at(1).name, "Bob");
  }

  void preservesIdOrder()
  {
    QSqlQuery query(m_sql_database);

    QVERIFY2(query.exec("INSERT INTO property (name) VALUES "
                        "('House'), "
                        "('Apartment'), "
                        "('Office')"),
             qPrintable(query.lastError().text()));

    ligarium::Link<Property, ligarium::ERelation::PolymorphicOneToMany> link;

    link.set_ids({3, 1, 2});

    const QList<Property> properties = link.get(*m_database);

    QCOMPARE(properties.size(), qsizetype(3));

    QCOMPARE(properties.at(0).id(), qsizetype(3));
    QCOMPARE(properties.at(0).name, "Office");

    QCOMPARE(properties.at(1).id(), qsizetype(1));
    QCOMPARE(properties.at(1).name, "House");

    QCOMPARE(properties.at(2).id(), qsizetype(2));
    QCOMPARE(properties.at(2).name, "Apartment");
  }

  void polymorphicTarget()
  {
    QSqlQuery query(m_sql_database);

    QVERIFY2(query.exec("INSERT INTO property (name) "
                        "VALUES ('House')"),
             qPrintable(query.lastError().text()));

    const qsizetype property_id = query.lastInsertId().toLongLong();

    ligarium::Link<Property, ligarium::ERelation::PolymorphicOneToMany> link;
    link.set_ids({property_id});

    const QList<Property> properties = link.get(*m_database);

    QCOMPARE(properties.size(), qsizetype(1));
    QCOMPARE(properties.first().id(), property_id);
    QCOMPARE(properties.first().name, "House");
  }
};

QTEST_MAIN(TestPolymorphicLink)

#include "test_polymorphic_link.moc"