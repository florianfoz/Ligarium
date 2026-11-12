#include "ligarium_config.h"
//

#include "records.h"

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

    m_database = new ligarium::Database(std::move(m_sql_database));
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
    ligarium::Link<Attachment, ligarium::ERelation::PolymorphicManyToOne> link;

    QVERIFY(!link.is_valid());
  }

  void id_n_table()
  {
    ligarium::Link<Attachment, ligarium::ERelation::PolymorphicManyToOne> link;

    link.set_id(1);
    link.set_table(ligarium::Table::Tenant);

    QVERIFY(link.is_valid());
    QCOMPARE(link.id(), 1);
    QCOMPARE(link.table(), ligarium::Table::Tenant);
  }


  void clear()
  {
    ligarium::Link<Attachment, ligarium::ERelation::PolymorphicManyToOne> link;
    link.set_id(1);

    QCOMPARE(link.id(), 1);

    link.set_id(ligarium::INVALID_ID);

    QVERIFY(!link.is_valid());
    QCOMPARE(link.id(), ligarium::INVALID_ID);
  }

  void getEmpty()
  {
    ligarium::Link<Attachment, ligarium::ERelation::PolymorphicManyToOne> link;

    const auto records = link.get<Property>(*m_database);

    QVERIFY(!records.is_valid());
  }

  void getProperties()
  {
    QSqlQuery query(m_sql_database);

    QVERIFY2(
        query.exec("INSERT INTO property (name) VALUES "
                   "('House'), "
                   "('Apartment'), "
                   "('Office')"),
        qPrintable(query.lastError().text())
    );

    ligarium::Link<Attachment, ligarium::ERelation::PolymorphicManyToOne> link1;
    link1.set_id(1);
    link1.set_table(ligarium::Table::Property);
    ligarium::Link<Attachment, ligarium::ERelation::PolymorphicManyToOne> link2;
    link2.set_id(2);
    link2.set_table(ligarium::Table::Property);
    ligarium::Link<Attachment, ligarium::ERelation::PolymorphicManyToOne> link3;
    link3.set_id(3);
    link3.set_table(ligarium::Table::Property);

    const auto property1 = link1.get<Property>(*m_database);
    const auto property2 = link2.get<Property>(*m_database);
    const auto property3 = link3.get<Property>(*m_database);

    QCOMPARE(property1.id(), qsizetype(1));
    QCOMPARE(property1.name, "House");
    QCOMPARE(property2.id(), qsizetype(2));
    QCOMPARE(property2.name, "Apartment");
    QCOMPARE(property3.id(), qsizetype(3));
    QCOMPARE(property3.name, "Office");
  }

  void getTenants()
  {
    QSqlQuery query(m_sql_database);

    QVERIFY2(
        query.exec("INSERT INTO tenant (name) VALUES "
                   "('Alice'), "
                   "('Bob')"),
        qPrintable(query.lastError().text())
    );

    ligarium::Link<Attachment, ligarium::ERelation::PolymorphicManyToOne> link1;
    link1.set_id(1);
    link1.set_table(ligarium::Table::Tenant);
    ligarium::Link<Attachment, ligarium::ERelation::PolymorphicManyToOne> link2;
    link2.set_id(2);
    link2.set_table(ligarium::Table::Tenant);

    const auto tenant1 = link1.get<Tenant>(*m_database);
    const auto tenant2 = link2.get<Tenant>(*m_database);

    QCOMPARE(tenant1.id(), qsizetype(1));
    QCOMPARE(tenant1.name, "Alice");
    QCOMPARE(tenant2.id(), qsizetype(2));
    QCOMPARE(tenant2.name, "Bob");
  }
};

QTEST_MAIN(TestPolymorphicLink)

#include "test_polymorphic_link.moc"