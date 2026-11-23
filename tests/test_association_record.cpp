
#include "ligarium/schema.h"
#include "ligarium_config.h"
//

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QtTest>
#include <memory>

// Inclure ici le header qui déclare Tenant_Property,
// ainsi que ceux de Property et Tenant.
#include "ligarium/database.h"
#include "records.h"

class TestAssociationRecord : public QObject
{
  Q_OBJECT

private:
  QSqlDatabase                        m_connection;
  std::unique_ptr<ligarium::Database> m_database;

private slots:
  void initTestCase()
  {
    const QString connection_name = QStringLiteral("association_record_test");

    m_connection = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), connection_name);

    m_connection.setDatabaseName(QStringLiteral(":memory:"));

    QVERIFY2(m_connection.open(), qPrintable(m_connection.lastError().text()));

    ligarium::SchemaBuilder schema(m_connection);

    if (!schema.create_all<Property, Tenant, Tenant_Property, Attachment>()) {
      QVERIFY(qPrintable(schema.last_error()));
    }


    m_database = std::make_unique<ligarium::Database>(std::move(m_connection));

    QVERIFY(m_database->is_open());
  }

  void cleanupTestCase()
  {
    m_database.reset();

    const QString connection_name = m_connection.connectionName();

    m_connection = QSqlDatabase();

    QSqlDatabase::removeDatabase(connection_name);
  }

  void manyToOne_foreignKeys_are_persisted()
  {
    const qsizetype property_id = Property::create_record(*m_database).id();

    const qsizetype tenant_id = Tenant::create_record(*m_database).id();

    // Arrange : construire l'association.
    Tenant_Property association;
    association.set_database(m_database.get());

    association.property.set_id(property_id);
    association.tenant.set_id(tenant_id);
    association.share_percent = 35.0;

    auto field = ligarium::link_ManyToOne(u"property_id", &Tenant_Property::property);
    using F    = std::remove_cvref_t<decltype(field)>;

    // Act
    QVERIFY2(association.save_record(), "Impossible to save Tenant_Property");

    auto result = Tenant_Property::read_record(*m_database, association.id());

    QCOMPARE(result.property.id(), property_id);

    QCOMPARE(result.tenant.id(), tenant_id);

    QCOMPARE(result.share_percent, 35.0);
  }
};

QTEST_GUILESS_MAIN(TestAssociationRecord)

#include "test_association_record.moc"