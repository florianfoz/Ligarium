
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

    qDebug() << schema.dump_schema() << "\n";


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
    // Arrange : créer les deux enregistrements référencés.
    QSqlQuery query(m_connection);

    QVERIFY2(query.exec(QStringLiteral("INSERT INTO Property DEFAULT VALUES")), qPrintable(query.lastError().text()));

    const qsizetype property_id = query.lastInsertId().toLongLong();

    QVERIFY2(query.exec(QStringLiteral("INSERT INTO Tenant DEFAULT VALUES")), qPrintable(query.lastError().text()));

    const qsizetype tenant_id = query.lastInsertId().toLongLong();

    // Arrange : construire l'association.
    Tenant_Property association;
    association.set_database(m_database.get());

    association.property.set_id(property_id);
    association.tenant.set_id(tenant_id);
    association.share_percent = 35.0;

    // Act
    QVERIFY2(association.save_record(), "Impossible de sauvegarder Tenant_Property");

    // Assert : vérifier les valeurs réellement enregistrées.
    QVERIFY2(
        query.exec(QStringLiteral("SELECT property_id, tenant_id, share_percent "
                                  "FROM Tenant_Property")),
        qPrintable(query.lastError().text())
    );

    QVERIFY(query.next());

    QCOMPARE(query.value(QStringLiteral("property_id")).toLongLong(), property_id);

    QCOMPARE(query.value(QStringLiteral("tenant_id")).toLongLong(), tenant_id);

    QCOMPARE(query.value(QStringLiteral("share_percent")).toDouble(), 35.0);

    QVERIFY(!query.next());
  }
};

QTEST_GUILESS_MAIN(TestAssociationRecord)

#include "test_association_record.moc"