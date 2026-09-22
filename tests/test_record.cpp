#include "ligarium_config.h"
//

#include "database.h"
#include "field.h"
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
  using Ligarium::Record<Property>::Record;

  static constexpr Ligarium::Table static_table = Ligarium::Table::Property;

  QString name;

  [[nodiscard]]
  QString dump() const override
  {
    return name;
  }

  static constexpr auto sql_fields()
  {
    return std::tuple{
        Ligarium::field(u"name", &Property::name) //
    };
  }

  friend bool operator==(const Property& lhs, const Property& rhs)
  {
    return lhs.id() == rhs.id() && lhs.name == rhs.name;
  }
};


class Tenant final : public Ligarium::Record<Tenant>
{
public:
  using Ligarium::Record<Tenant>::Record;

  static constexpr Ligarium::Table static_table = Ligarium::Table::Tenant;

  QString name;

  [[nodiscard]]
  QString dump() const override
  {
    return name;
  }

  static constexpr auto sql_fields()
  {
    return std::tuple{
        Ligarium::field(u"name", &Tenant::name) //
    };
  }

  friend bool operator==(const Tenant& lhs, const Tenant& rhs)
  {
    return lhs.id() == rhs.id() && lhs.name == rhs.name;
  }
};


class Attachment final : public Ligarium::Record<Attachment>
{
public:
  using Ligarium::Record<Attachment>::Record;

  static constexpr Ligarium::Table static_table = Ligarium::Table::Attachment;

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
    return std::tuple{
        Ligarium::field(u"table", &Attachment::table),   //
        Ligarium::field(u"col_id", &Attachment::col_id), //
        Ligarium::field(u"path", &Attachment::path)      //
    };
  }

  friend bool operator==(const Attachment& lhs, const Attachment& rhs)
  {
    return lhs.id() == rhs.id() && lhs.table == rhs.table && lhs.col_id == rhs.col_id && lhs.path == rhs.path;
  }
};


class TestRecord : public QObject
{
  Q_OBJECT

private:
  QSqlDatabase                        m_sql_database;
  std::unique_ptr<Ligarium::Database> m_database;

private slots:

  void initTestCase()
  {
    const QString connection_name = QStringLiteral("ligarium_record_test");

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

  void recordType()
  {
    static_assert(Ligarium::RecordType<Property>);

    static_assert(Ligarium::RecordType<Tenant>);

    static_assert(Ligarium::RecordType<Attachment>);
  }

  void defaultState()
  {
    Property property;

    QCOMPARE(property.id(), Ligarium::INVALID_ID);

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

    QCOMPARE(property.id(), Ligarium::INVALID_ID);

    QVERIFY(!property.is_valid());

    QVERIFY(!static_cast<bool>(property));
  }

  void tableMapping()
  {
    QCOMPARE(Property::static_table, Ligarium::Table::Property);

    QCOMPARE(Tenant::static_table, Ligarium::Table::Tenant);

    QCOMPARE(Attachment::static_table, Ligarium::Table::Attachment);

    QCOMPARE(Ligarium::Table_to_str(Ligarium::Table::Property), QStringLiteral("property"));

    QCOMPARE(Ligarium::Table_to_str(Ligarium::Table::Tenant), QStringLiteral("tenant"));

    QCOMPARE(Ligarium::Table_to_str(Ligarium::Table::Attachment), QStringLiteral("attachment"));
  }

  void createRecord()
  {
    const Property property = Property::create_record(*m_database);

    QVERIFY(property.id() != Ligarium::INVALID_ID);

    QVERIFY(property.is_valid());

    QVERIFY(static_cast<bool>(property));

    QCOMPARE(property.name, QString());
  }

  void readRecord()
  {
    QSqlQuery query(m_sql_database);

    QVERIFY2(query.exec(QStringLiteral("INSERT INTO property (name) "
                                       "VALUES ('House')")),
             qPrintable(query.lastError().text()));

    const qsizetype id = query.lastInsertId().toLongLong();

    const Property property = Property::read_record(*m_database, id);

    QCOMPARE(property.id(), id);

    QCOMPARE(property.name, QStringLiteral("House"));

    QVERIFY(property.is_valid());

    QVERIFY(static_cast<bool>(property));
  }

  void readInvalidRecord()
  {
    const Property property = Property::read_record(*m_database, Ligarium::INVALID_ID);

    QCOMPARE(property.id(), Ligarium::INVALID_ID);
  }

  void allRecords()
  {
    QSqlQuery query(m_sql_database);

    QVERIFY2(query.exec(QStringLiteral("DELETE FROM property")), qPrintable(query.lastError().text()));

    QVERIFY2(query.exec(QStringLiteral("INSERT INTO property (name) VALUES "
                                       "('House'), "
                                       "('Apartment'), "
                                       "('Office')")),
             qPrintable(query.lastError().text()));

    const QList<Property> properties = Property::all_records(*m_database);

    QCOMPARE(properties.size(), 3);

    QCOMPARE(properties.at(0).name, QStringLiteral("House"));

    QCOMPARE(properties.at(1).name, QStringLiteral("Apartment"));

    QCOMPARE(properties.at(2).name, QStringLiteral("Office"));
  }

  void allRecordIds()
  {
    QSqlQuery query(m_sql_database);

    QVERIFY2(query.exec(QStringLiteral("DELETE FROM property")), qPrintable(query.lastError().text()));

    QVERIFY2(query.exec(QStringLiteral("INSERT INTO property (name) VALUES "
                                       "('House'), "
                                       "('Apartment')")),
             qPrintable(query.lastError().text()));

    const QList<qsizetype> ids = Property::all_records_id(*m_database);

    QCOMPARE(ids.size(), 2);

    QVERIFY(ids.at(0) != Ligarium::INVALID_ID);

    QVERIFY(ids.at(1) != Ligarium::INVALID_ID);
  }

  void saveNewRecord()
  {
    Property property(m_database.get());

    property.name = QStringLiteral("House");

    QVERIFY(property.save_record());

    QVERIFY(property.id() != Ligarium::INVALID_ID);

    QVERIFY(property.is_valid());

    const Property loaded = Property::read_record(*m_database, property.id());

    QCOMPARE(loaded.name, QStringLiteral("House"));
  }

  void saveExistingRecord()
  {
    QSqlQuery query(m_sql_database);

    QVERIFY2(query.exec(QStringLiteral("INSERT INTO property (name) "
                                       "VALUES ('Before')")),
             qPrintable(query.lastError().text()));

    const qsizetype id = query.lastInsertId().toLongLong();

    Property property = Property::read_record(*m_database, id);

    property.name = QStringLiteral("After");

    QVERIFY(property.save_record());

    const Property loaded = Property::read_record(*m_database, id);

    QCOMPARE(loaded.name, QStringLiteral("After"));
  }

  void deleteRecord()
  {
    QSqlQuery query(m_sql_database);

    QVERIFY2(query.exec(QStringLiteral("INSERT INTO property (name) "
                                       "VALUES ('House')")),
             qPrintable(query.lastError().text()));

    const qsizetype id = query.lastInsertId().toLongLong();

    Property property = Property::read_record(*m_database, id);

    QVERIFY(property.is_valid());

    QVERIFY(property.delete_record());

    QCOMPARE(property.id(), Ligarium::INVALID_ID);

    QVERIFY(!property.is_valid());

    QVERIFY(!Ligarium::contains_record(*m_database, Ligarium::Table::Property, id));
  }

  void containsRecord()
  {
    QSqlQuery query(m_sql_database);

    QVERIFY2(query.exec(QStringLiteral("INSERT INTO tenant (name) "
                                       "VALUES ('John')")),
             qPrintable(query.lastError().text()));

    const qsizetype id = query.lastInsertId().toLongLong();

    QVERIFY(Ligarium::contains_record(*m_database, Ligarium::Table::Tenant, id));

    QVERIFY(!Ligarium::contains_record(*m_database, Ligarium::Table::Tenant, Ligarium::INVALID_ID));
  }

  void dump()
  {
    Property property(m_database.get());

    property.name = QStringLiteral("House");

    QCOMPARE(property.dump(), QStringLiteral("House"));
  }

  void dirtyNewRecord()
  {
    Property property(m_database.get());

    property.name = QStringLiteral("Unsaved");

    QVERIFY(property.is_dirty());
  }

  void dirtyExistingRecord()
  {
    QSqlQuery query(m_sql_database);

    QVERIFY2(query.exec(QStringLiteral("INSERT INTO property (name) "
                                       "VALUES ('House')")),
             qPrintable(query.lastError().text()));

    const qsizetype id = query.lastInsertId().toLongLong();

    Property property = Property::read_record(*m_database, id);

    QVERIFY(!property.is_dirty());

    property.name = QStringLiteral("Villa");

    QVERIFY(property.is_dirty());
  }

  void dirtyAgainstOtherRecord()
  {
    QSqlQuery query(m_sql_database);

    QVERIFY2(query.exec(QStringLiteral("INSERT INTO property (name) VALUES "
                                       "('House'), "
                                       "('Villa')")),
             qPrintable(query.lastError().text()));

    const qsizetype first_id = query.lastInsertId().toLongLong();

    QVERIFY2(query.exec(QStringLiteral("SELECT id FROM property "
                                       "ORDER BY id DESC LIMIT 1")),
             qPrintable(query.lastError().text()));

    QVERIFY(query.next());

    const qsizetype second_id = query.value(0).toLongLong();

    const Property property = Property::read_record(*m_database, first_id);

    QVERIFY(property.is_dirty(second_id));
  }

  void newerIsDirtyAlias()
  {
    Property property(m_database.get());

    property.name = QStringLiteral("Unsaved");

    QCOMPARE(property.is_newer(), property.is_dirty());
  }

  void attachmentRecord()
  {
    QSqlQuery query(m_sql_database);

    QVERIFY2(query.exec(QStringLiteral("INSERT INTO attachment "
                                       "(table, col_id, path) "
                                       "VALUES (0, 42, '/tmp/document.pdf')")),
             qPrintable(query.lastError().text()));

    const qsizetype id = query.lastInsertId().toLongLong();

    const Attachment attachment = Attachment::read_record(*m_database, id);

    QCOMPARE(attachment.id(), id);

    QCOMPARE(attachment.table, Ligarium::Table::Property);

    QCOMPARE(attachment.col_id, qsizetype(42));

    QCOMPARE(attachment.path, QStringLiteral("/tmp/document.pdf"));

    QCOMPARE(attachment.dump(), QStringLiteral("/tmp/document.pdf"));
  }
};


QTEST_MAIN(TestRecord)

#include "test_record.moc"