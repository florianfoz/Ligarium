#include "ligarium_config.h"
//
#include "test_records.h"

#include <QTest>
#include <ligarium/field.h>
#include <ligarium/record.h>
#include <ligarium/record_registry.h>


class TestRecordRegistry : public QObject
{
  Q_OBJECT


  void set_database(ligarium::Database* db)
  {
    m_db = db;
  }

  ligarium::Database* database()
  {
    return m_db;
  }

private slots:
  void initially_empty();
  void register_record();
  void dump_registered_record();
  void dump_unknown_record();
  void contains_registered_record();
  void contains_unknown_record();
  void replace_registered_record();
  void clear_registry();
  void register_typed_record();

private:
  ligarium::Database* m_db;
};

void TestRecordRegistry::initially_empty()
{
  ligarium::RecordRegistry registry(m_db);

  QVERIFY(!registry.contains(ligarium::Table::Property));
  QCOMPARE(registry.dump(ligarium::Table::Property, 1), QString{});
}

void TestRecordRegistry::register_record()
{
  ligarium::RecordRegistry registry(m_db);

  registry.register_record(ligarium::Table::Property,
                           [](qsizetype id) { return QStringLiteral("Property %1").arg(id); });

  QVERIFY(registry.contains(ligarium::Table::Property));
}

void TestRecordRegistry::dump_registered_record()
{
  ligarium::RecordRegistry registry(m_db);

  registry.register_record(ligarium::Table::Property,
                           [](qsizetype id) { return QStringLiteral("Property %1").arg(id); });

  QCOMPARE(registry.dump(ligarium::Table::Property, 42), "Property 42");
}

void TestRecordRegistry::dump_unknown_record()
{
  ligarium::RecordRegistry registry(m_db);

  registry.register_record(ligarium::Table::Property,
                           [](qsizetype id) { return QStringLiteral("Property %1").arg(id); });

  QCOMPARE(registry.dump(ligarium::Table::Tenant, 42), QString{});
}

void TestRecordRegistry::contains_registered_record()
{
  ligarium::RecordRegistry registry(m_db);

  registry.register_record(ligarium::Table::Property, [](qsizetype) { return "Property"; });

  QVERIFY(registry.contains(ligarium::Table::Property));
  QVERIFY(!registry.contains(ligarium::Table::Tenant));
}

void TestRecordRegistry::contains_unknown_record()
{
  ligarium::RecordRegistry registry(m_db);

  QVERIFY(!registry.contains(ligarium::Table::Property));
  QVERIFY(!registry.contains(ligarium::Table::Tenant));
}

void TestRecordRegistry::replace_registered_record()
{
  ligarium::RecordRegistry registry(m_db);

  registry.register_record(ligarium::Table::Property, [](qsizetype) { return "first"; });

  registry.register_record(ligarium::Table::Property, [](qsizetype) { return "second"; });

  QCOMPARE(registry.dump(ligarium::Table::Property, 1), "second");
}

void TestRecordRegistry::clear_registry()
{
  ligarium::RecordRegistry registry(m_db);

  registry.register_record(ligarium::Table::Property, [](qsizetype) { return "Property"; });

  registry.register_record(ligarium::Table::Tenant, [](qsizetype) { return "Tenant"; });

  QVERIFY(registry.contains(ligarium::Table::Property));
  QVERIFY(registry.contains(ligarium::Table::Tenant));

  registry.clear();

  QVERIFY(!registry.contains(ligarium::Table::Property));
  QVERIFY(!registry.contains(ligarium::Table::Tenant));

  QCOMPARE(registry.dump(ligarium::Table::Property, 1), QString{});
  QCOMPARE(registry.dump(ligarium::Table::Tenant, 1), QString{});
}

void TestRecordRegistry::register_typed_record()
{
  ligarium::RecordRegistry registry(m_db);

  ligarium::register_record<Property>(registry, ligarium::Table::Property);

  QVERIFY(registry.contains(ligarium::Table::Property));

  QCOMPARE(registry.dump(ligarium::Table::Property, 42), "Property(id=42)");
}

QTEST_MAIN(TestRecordRegistry)

#include "test_record_registry.moc"
