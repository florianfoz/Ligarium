#include "ligarium_config.h"

//
#include "test_records.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QtTest/QtTest>
#include <ligarium/database.h>
#include <ligarium/schema.h>

namespace
{

constexpr auto connection_name = "ligarium_test_database";

class DatabaseTest : public QObject
{
  Q_OBJECT

private:
  QSqlDatabase        m_connection;
  ligarium::Database* m_database = nullptr;

private slots:

  void initTestCase()
  {
    QVERIFY2(QSqlDatabase::isDriverAvailable("QSQLITE"), "QSQLITE driver is not available");

    m_connection = QSqlDatabase::addDatabase("QSQLITE", connection_name);

    m_connection.setDatabaseName(":memory:");

    QVERIFY(m_connection.open());

    ligarium::SchemaBuilder schema(m_connection);

    if (!schema.create_all<Property, Tenant, Attachment>()) {
      QVERIFY(qPrintable(schema.last_error()));
    }

    m_database = new ligarium::Database(m_connection);
  }

  void cleanupTestCase()
  {
    delete m_database;
    m_database = nullptr;

    if (m_connection.isOpen()) m_connection.close();

    m_connection = {};

    QSqlDatabase::removeDatabase(connection_name);
  }

  void connection()
  {
    QCOMPARE(&m_database->connection(), &m_connection);

    QVERIFY(m_database->is_open());
  }

  void execute()
  {
    QSqlQuery query(m_connection);

    QVERIFY(
        query.prepare("INSERT INTO property "
                      "(name, surface, active) "
                      "VALUES (:name, :surface, :active)"));

    query.bindValue(":name", "House");
    query.bindValue(":surface", 120.5);
    query.bindValue(":active", true);

    QVERIFY(m_database->execute(query));

    QCOMPARE(query.numRowsAffected(), 1);
  }

  void insert()
  {
    const auto id = m_database->insert(ligarium::Table::Property);

    QVERIFY(id > 0);

    QVERIFY(m_database->contains(ligarium::Table::Property, id));
  }

  void last_insert_id()
  {
    const auto id = m_database->insert(ligarium::Table::Property);

    QVERIFY(id > 0);

    QCOMPARE(m_database->last_insert_id(), id);
  }

  void contains_existing_record()
  {
    const auto id = m_database->insert(ligarium::Table::Property);

    QVERIFY(m_database->contains(ligarium::Table::Property, id));
  }

  void contains_missing_record()
  {
    QVERIFY(!m_database->contains(ligarium::Table::Property, 999999));
  }

  void find_existing_record()
  {
    const auto id = m_database->insert(ligarium::Table::Property);

    QVERIFY(id > 0);

    auto result = m_database->find(ligarium::Table::Property, id);

    QVERIFY(result.has_value());

    QSqlQuery query = *result;

    QVERIFY(query.next());

    QCOMPARE(query.value("id").toLongLong(), id);
  }

  void find_missing_record()
  {
    const auto result = m_database->find(ligarium::Table::Property, 999999);

    QVERIFY(result.has_value());

    QSqlQuery query = *result;

    QVERIFY(!query.next());
  }

  void all()
  {
    QSqlQuery query(m_connection);

    QVERIFY(
        query.exec("INSERT INTO property "
                   "(name, surface) VALUES "
                   "('House A', 100), "
                   "('House B', 200), "
                   "('House C', 300)"));

    const auto result = m_database->all(ligarium::Table::Property);

    QVERIFY(result.has_value());

    QSqlQuery records = *result;

    qsizetype count = 0;

    while (records.next()) ++count;

    QVERIFY(count >= 3);
  }

  void all_ids()
  {
    QSqlQuery query(m_connection);

    QVERIFY(
        query.exec("INSERT INTO property "
                   "(name) VALUES "
                   "('ID A'), "
                   "('ID B')"));

    const auto result = m_database->all_ids(ligarium::Table::Property);

    QVERIFY(result.has_value());

    QSqlQuery ids = *result;

    qsizetype count = 0;

    while (ids.next()) {
      QVERIFY(ids.value("id").isValid());
      ++count;
    }

    QVERIFY(count >= 2);
  }

  void record_id()
  {
    QSqlQuery query(m_connection);

    QVERIFY(
        query.prepare("INSERT INTO property "
                      "(name, surface) "
                      "VALUES (:name, :surface)"));

    query.bindValue(":name", "Unique Property");
    query.bindValue(":surface", 456.75);

    QVERIFY(query.exec());

    const auto id = m_database->record_id(ligarium::Table::Property, "name", "Unique Property");

    QVERIFY(id > 0);

    const auto id_by_surface = m_database->record_id(ligarium::Table::Property, "surface", 456.75);

    QCOMPARE(id_by_surface, id);
  }

  void record_id_missing_value()
  {
    QCOMPARE(m_database->record_id(ligarium::Table::Property, "name", "Does not exist"), ligarium::INVALID_ID);
  }

  void record_id_invalid_column()
  {
    QCOMPARE(m_database->record_id(ligarium::Table::Property, "does_not_exist", "value"), ligarium::INVALID_ID);
  }

  void is_valid_column()
  {
    QVERIFY(m_database->is_valid_column(ligarium::Table::Property, "id"));

    QVERIFY(m_database->is_valid_column(ligarium::Table::Property, "name"));

    QVERIFY(m_database->is_valid_column(ligarium::Table::Property, "surface"));

    QVERIFY(m_database->is_valid_column(ligarium::Table::Property, "active"));

    QVERIFY(!m_database->is_valid_column(ligarium::Table::Property, "unknown_column"));
  }

  void save()
  {
    const auto id = m_database->insert(ligarium::Table::Property);

    QVERIFY(id > 0);

    QVERIFY(m_database->save(ligarium::Table::Property, id, "name", "Saved property"));

    auto result = m_database->find(ligarium::Table::Property, id);

    QVERIFY(result.has_value());

    QSqlQuery query = std::move(*result);

    QVERIFY(query.next());

    QCOMPARE(query.value("name").toString(), "Saved property");
  }

  void save_numeric_value()
  {
    const auto id = m_database->insert(ligarium::Table::Property);

    QVERIFY(m_database->save(ligarium::Table::Property, id, "surface", 123.45));

    const auto result = m_database->find(ligarium::Table::Property, id);

    QVERIFY(result.has_value());

    QSqlQuery query = *result;

    QVERIFY(query.next());

    QCOMPARE(query.value("surface").toDouble(), 123.45);
  }

  void save_invalid_column()
  {
    const auto id = m_database->insert(ligarium::Table::Property);

    QVERIFY(!m_database->save(ligarium::Table::Property, id, "does_not_exist", "value"));
  }

  void save_missing_record()
  {
    QVERIFY(!m_database->save(ligarium::Table::Property, 999999, "name", "value"));
  }

  void remove()
  {
    const auto id = m_database->insert(ligarium::Table::Property);

    QVERIFY(m_database->contains(ligarium::Table::Property, id));

    QVERIFY(m_database->remove(ligarium::Table::Property, id));

    QVERIFY(!m_database->contains(ligarium::Table::Property, id));
  }

  void remove_missing_record()
  {
    QVERIFY(!m_database->remove(ligarium::Table::Property, 999999));
  }

  void signal_db_updated()
  {
    QSignalSpy spy(m_database, &ligarium::Database::signal_db_updated);

    QVERIFY(spy.isValid());

    // This test currently documents the API.
    // Database::insert/save/remove do not emit the signal yet.
    const auto id = m_database->insert(ligarium::Table::Property);

    QVERIFY(id > 0);

    QCOMPARE(spy.count(), 0);
  }
};

} // namespace

QTEST_MAIN(DatabaseTest)

#include "test_database.moc"