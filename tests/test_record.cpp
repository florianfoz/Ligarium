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


class TestRecord : public QObject
{
  Q_OBJECT

private:
  QSqlDatabase                        m_sql_database;
  std::unique_ptr<ligarium::Database> m_database;

private slots:

  void initTestCase()
  {
    const QString connection_name = "ligarium_record_test";

    QSqlDatabase connection = QSqlDatabase::addDatabase("QSQLITE", connection_name);

    connection.setDatabaseName(":memory:");

    QVERIFY(connection.open());

    ligarium::SchemaBuilder schema(connection);

    if (!schema.create_all<Property, Tenant, Attachment>()) {
      QVERIFY(qPrintable(schema.last_error()));
    }

    m_database = std::make_unique<ligarium::Database>(connection);
  }

  void cleanupTestCase()
  {
    m_database.reset();

    const QString connection_name = m_sql_database.connectionName();

    m_sql_database.close();
    m_sql_database = {};

    QSqlDatabase::removeDatabase(connection_name);
  }

  void recordType()
  {
    static_assert(ligarium::RecordType<Property>);

    static_assert(ligarium::RecordType<Tenant>);

    static_assert(ligarium::RecordType<Attachment>);
  }

  void defaultState()
  {
    Property property;

    QCOMPARE(property.id(), ligarium::INVALID_ID);

    QCOMPARE(property.database(), nullptr);

    QVERIFY(property.is_record());

    QVERIFY(!property.is_relation());

    QVERIFY(!property.is_valid());

    QVERIFY(!static_cast<bool>(property));

    QCOMPARE(property.name, QString());
  }

  void databaseAssociation()
  {
    Property property(m_database.get());

    QCOMPARE(property.database(), m_database.get());

    QCOMPARE(property.id(), ligarium::INVALID_ID);

    QVERIFY(!property.is_valid());

    QVERIFY(!static_cast<bool>(property));
  }

  void tableMapping()
  {
    QCOMPARE(Property::static_table, ligarium::Table::Property);

    QCOMPARE(Tenant::static_table, ligarium::Table::Tenant);

    QCOMPARE(Attachment::static_table, ligarium::Table::Attachment);

    QCOMPARE(ligarium::Table_to_str(ligarium::Table::Property), "property");

    QCOMPARE(ligarium::Table_to_str(ligarium::Table::Tenant), "tenant");

    QCOMPARE(ligarium::Table_to_str(ligarium::Table::Attachment), "attachment");
  }

  void createRecord()
  {
    const Property property = Property::create_record(*m_database);

    QVERIFY(property.id() != ligarium::INVALID_ID);

    QVERIFY(property.is_valid());

    QVERIFY(static_cast<bool>(property));

    QCOMPARE(property.name, QString());
  }

  void readRecord()
  {
    QSqlQuery query(m_sql_database);

    QVERIFY2(query.exec("INSERT INTO property (name) "
                        "VALUES ('House')"),
             qPrintable(query.lastError().text()));

    const qsizetype id = query.lastInsertId().toLongLong();

    const Property property = Property::read_record(*m_database, id);

    QCOMPARE(property.id(), id);

    QCOMPARE(property.name, "House");

    QVERIFY(property.is_valid());

    QVERIFY(static_cast<bool>(property));
  }

  void readInvalidRecord()
  {
    const Property property = Property::read_record(*m_database, ligarium::INVALID_ID);

    QCOMPARE(property.id(), ligarium::INVALID_ID);
  }

  void allRecords()
  {
    QSqlQuery query(m_sql_database);

    QVERIFY2(query.exec("DELETE FROM property"), qPrintable(query.lastError().text()));

    QVERIFY2(query.exec("INSERT INTO property (name) VALUES "
                        "('House'), "
                        "('Apartment'), "
                        "('Office')"),
             qPrintable(query.lastError().text()));

    const QList<Property> properties = Property::all_records(*m_database);

    QCOMPARE(properties.size(), 3);

    QCOMPARE(properties.at(0).name, "House");

    QCOMPARE(properties.at(1).name, "Apartment");

    QCOMPARE(properties.at(2).name, "Office");
  }

  void allRecordIds()
  {
    QSqlQuery query(m_sql_database);

    QVERIFY2(query.exec("DELETE FROM property"), qPrintable(query.lastError().text()));

    QVERIFY2(query.exec("INSERT INTO property (name) VALUES "
                        "('House'), "
                        "('Apartment')"),
             qPrintable(query.lastError().text()));

    const QList<qsizetype> ids = Property::all_records_id(*m_database);

    QCOMPARE(ids.size(), 2);

    QVERIFY(ids.at(0) != ligarium::INVALID_ID);

    QVERIFY(ids.at(1) != ligarium::INVALID_ID);
  }

  void saveNewRecord()
  {
    Property property(m_database.get());

    property.name = "House";

    QVERIFY(property.save_record());

    QVERIFY(property.id() != ligarium::INVALID_ID);

    QVERIFY(property.is_valid());

    const Property loaded = Property::read_record(*m_database, property.id());

    QCOMPARE(loaded.name, "House");
  }

  void saveExistingRecord()
  {
    QSqlQuery query(m_sql_database);

    QVERIFY2(query.exec("INSERT INTO property (name) "
                        "VALUES ('Before')"),
             qPrintable(query.lastError().text()));

    const qsizetype id = query.lastInsertId().toLongLong();

    Property property = Property::read_record(*m_database, id);

    property.name = "After";

    QVERIFY(property.save_record());

    const Property loaded = Property::read_record(*m_database, id);

    QCOMPARE(loaded.name, "After");
  }

  void deleteRecord()
  {
    QSqlQuery query(m_sql_database);

    QVERIFY2(query.exec("INSERT INTO property (name) "
                        "VALUES ('House')"),
             qPrintable(query.lastError().text()));

    const qsizetype id = query.lastInsertId().toLongLong();

    Property property = Property::read_record(*m_database, id);

    QVERIFY(property.is_valid());

    QVERIFY(property.delete_record());

    QCOMPARE(property.id(), ligarium::INVALID_ID);

    QVERIFY(!property.is_valid());

    QVERIFY(!ligarium::contains_record(*m_database, ligarium::Table::Property, id));
  }

  void containsRecord()
  {
    QSqlQuery query(m_sql_database);

    QVERIFY2(query.exec("INSERT INTO tenant (name) "
                        "VALUES ('John')"),
             qPrintable(query.lastError().text()));

    const qsizetype id = query.lastInsertId().toLongLong();

    QVERIFY(ligarium::contains_record(*m_database, ligarium::Table::Tenant, id));

    QVERIFY(!ligarium::contains_record(*m_database, ligarium::Table::Tenant, ligarium::INVALID_ID));
  }

  void dump()
  {
    Property property(m_database.get());

    property.name = "House";

    QCOMPARE(property.dump(), "House");
  }

  void dirtyNewRecord()
  {
    Property property(m_database.get());

    property.name = "Unsaved";

    QVERIFY(property.is_dirty());
  }

  void dirtyExistingRecord()
  {
    QSqlQuery query(m_sql_database);

    QVERIFY2(query.exec("INSERT INTO property (name) "
                        "VALUES ('House')"),
             qPrintable(query.lastError().text()));

    const qsizetype id = query.lastInsertId().toLongLong();

    Property property = Property::read_record(*m_database, id);

    QVERIFY(!property.is_dirty());

    property.name = "Villa";

    QVERIFY(property.is_dirty());
  }

  void dirtyAgainstOtherRecord()
  {
    QSqlQuery query(m_sql_database);

    QVERIFY2(query.exec("INSERT INTO property (name) VALUES "
                        "('House'), "
                        "('Villa')"),
             qPrintable(query.lastError().text()));

    const qsizetype first_id = query.lastInsertId().toLongLong();

    QVERIFY2(query.exec("SELECT id FROM property "
                        "ORDER BY id DESC LIMIT 1"),
             qPrintable(query.lastError().text()));

    QVERIFY(query.next());

    const qsizetype second_id = query.value(0).toLongLong();

    const Property property = Property::read_record(*m_database, first_id);

    QVERIFY(property.is_dirty(second_id));
  }

  void newerIsDirtyAlias()
  {
    Property property(m_database.get());

    property.name = "Unsaved";

    QCOMPARE(property.is_newer(), property.is_dirty());
  }

  void attachmentRecord()
  {
    QSqlQuery query(m_sql_database);

    QVERIFY2(query.exec("INSERT INTO attachment "
                        "(table, col_id, path) "
                        "VALUES (0, 42, '/tmp/document.pdf')"),
             qPrintable(query.lastError().text()));

    const qsizetype id = query.lastInsertId().toLongLong();

    const Attachment attachment = Attachment::read_record(*m_database, id);

    QCOMPARE(attachment.id(), id);

    QCOMPARE(attachment.table, ligarium::Table::Property);

    QCOMPARE(attachment.col_id, qsizetype(42));

    QCOMPARE(attachment.path, "/tmp/document.pdf");

    QCOMPARE(attachment.dump(), "/tmp/document.pdf");
  }
};


QTEST_MAIN(TestRecord)

#include "test_record.moc"