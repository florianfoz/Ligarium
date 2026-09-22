#include "ligarium_config.h"
//
#include "field.h"
#include "record.h"
#include "record_registry.h"

#include <QTest>

class Property : public Ligarium::Record<Property>
{
public:
  static constexpr Ligarium::Table static_table = Ligarium::Table::Property;

  [[nodiscard]]
  QString dump() const override
  {
    return QStringLiteral("Property(id=%1)").arg(id());
  }

  QString name;

  static constexpr auto sql_fields()
  {
    return std::tuple{
        Ligarium::field(u"name", &Property::name),
    };
  }
};

class TestRecordRegistry : public QObject
{
  Q_OBJECT


  void set_database(Ligarium::Database* db)
  {
    m_db = db;
  }

  Ligarium::Database* database()
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
  Ligarium::Database* m_db;
};

void TestRecordRegistry::initially_empty()
{
  Ligarium::RecordRegistry registry(m_db);

  QVERIFY(!registry.contains(Ligarium::Table::Property));
  QCOMPARE(registry.dump(Ligarium::Table::Property, 1), QString{});
}

void TestRecordRegistry::register_record()
{
  Ligarium::RecordRegistry registry(m_db);

  registry.register_record(Ligarium::Table::Property,
                           [](qsizetype id) { return QStringLiteral("Property %1").arg(id); });

  QVERIFY(registry.contains(Ligarium::Table::Property));
}

void TestRecordRegistry::dump_registered_record()
{
  Ligarium::RecordRegistry registry(m_db);

  registry.register_record(Ligarium::Table::Property,
                           [](qsizetype id) { return QStringLiteral("Property %1").arg(id); });

  QCOMPARE(registry.dump(Ligarium::Table::Property, 42), QStringLiteral("Property 42"));
}

void TestRecordRegistry::dump_unknown_record()
{
  Ligarium::RecordRegistry registry(m_db);

  registry.register_record(Ligarium::Table::Property,
                           [](qsizetype id) { return QStringLiteral("Property %1").arg(id); });

  QCOMPARE(registry.dump(Ligarium::Table::Tenant, 42), QString{});
}

void TestRecordRegistry::contains_registered_record()
{
  Ligarium::RecordRegistry registry(m_db);

  registry.register_record(Ligarium::Table::Property, [](qsizetype) { return QStringLiteral("Property"); });

  QVERIFY(registry.contains(Ligarium::Table::Property));
  QVERIFY(!registry.contains(Ligarium::Table::Tenant));
}

void TestRecordRegistry::contains_unknown_record()
{
  Ligarium::RecordRegistry registry(m_db);

  QVERIFY(!registry.contains(Ligarium::Table::Property));
  QVERIFY(!registry.contains(Ligarium::Table::Tenant));
}

void TestRecordRegistry::replace_registered_record()
{
  Ligarium::RecordRegistry registry(m_db);

  registry.register_record(Ligarium::Table::Property, [](qsizetype) { return QStringLiteral("first"); });

  registry.register_record(Ligarium::Table::Property, [](qsizetype) { return QStringLiteral("second"); });

  QCOMPARE(registry.dump(Ligarium::Table::Property, 1), QStringLiteral("second"));
}

void TestRecordRegistry::clear_registry()
{
  Ligarium::RecordRegistry registry(m_db);

  registry.register_record(Ligarium::Table::Property, [](qsizetype) { return QStringLiteral("Property"); });

  registry.register_record(Ligarium::Table::Tenant, [](qsizetype) { return QStringLiteral("Tenant"); });

  QVERIFY(registry.contains(Ligarium::Table::Property));
  QVERIFY(registry.contains(Ligarium::Table::Tenant));

  registry.clear();

  QVERIFY(!registry.contains(Ligarium::Table::Property));
  QVERIFY(!registry.contains(Ligarium::Table::Tenant));

  QCOMPARE(registry.dump(Ligarium::Table::Property, 1), QString{});
  QCOMPARE(registry.dump(Ligarium::Table::Tenant, 1), QString{});
}

void TestRecordRegistry::register_typed_record()
{
  Ligarium::RecordRegistry registry(m_db);

  Ligarium::register_record<Property>(registry, Ligarium::Table::Property);

  QVERIFY(registry.contains(Ligarium::Table::Property));

  QCOMPARE(registry.dump(Ligarium::Table::Property, 42), QStringLiteral("Property(id=42)"));
}

QTEST_MAIN(TestRecordRegistry)

#include "test_record_registry.moc"
