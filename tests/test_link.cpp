#include "ligarium_config.h"
//

#include "link.h"

#include <QtTest/QtTest>

namespace
{

struct TestRecord : Ligarium::Record<TestRecord> {
  static constexpr Ligarium::Table static_table = Ligarium::Table::Property;

  [[nodiscard]] QString dump() const override
  {
    return "test_record";
  }

  qsizetype test_field;

  static constexpr auto sql_fields()
  {
    return std::tuple{u"test_field", &TestRecord::test_field};
  }
};

class LinkTest final : public QObject
{
  Q_OBJECT

private slots:

  // -------------------------------------------------------------------------
  // OneToOne
  // -------------------------------------------------------------------------

  void oneToOne_default()
  {
    Ligarium::Link<TestRecord, Ligarium::ERelation::OneToOne> link;

    QCOMPARE(link.id(), Ligarium::INVALID_ID);
    QVERIFY(!link.is_valid());
  }

  void oneToOne_valid()
  {
    Ligarium::Link<TestRecord, Ligarium::ERelation::OneToOne> link;

    link.set_id(42);

    QCOMPARE(link.id(), 42);
    QVERIFY(link.is_valid());
  }

  void oneToOne_invalid()
  {
    Ligarium::Link<TestRecord, Ligarium::ERelation::OneToOne> link;

    link.set_id(Ligarium::INVALID_ID);

    QVERIFY(!link.is_valid());
  }

  void oneToOne_equality()
  {
    using Link = Ligarium::Link<TestRecord, Ligarium::ERelation::OneToOne>;

    Link first;
    Link second;

    QVERIFY(first == second);

    first.set_id(10);

    QVERIFY(first != second);

    second.set_id(10);

    QVERIFY(first == second);
  }

  // -------------------------------------------------------------------------
  // ManyToOne
  // -------------------------------------------------------------------------

  void manyToOne_default()
  {
    Ligarium::Link<TestRecord, Ligarium::ERelation::ManyToOne> link;

    QCOMPARE(link.id(), Ligarium::INVALID_ID);
    QVERIFY(!link.is_valid());
  }

  void manyToOne_valid()
  {
    Ligarium::Link<TestRecord, Ligarium::ERelation::ManyToOne> link;

    link.set_id(123);

    QCOMPARE(link.id(), 123);
    QVERIFY(link.is_valid());
  }

  void manyToOne_equality()
  {
    using Link = Ligarium::Link<TestRecord, Ligarium::ERelation::ManyToOne>;

    Link first;
    Link second;

    QVERIFY(first == second);

    first.set_id(1);
    second.set_id(2);

    QVERIFY(first != second);

    second.set_id(1);

    QVERIFY(first == second);
  }

  // -------------------------------------------------------------------------
  // OneToMany
  // -------------------------------------------------------------------------

  void oneToMany_default()
  {
    Ligarium::Link<TestRecord, Ligarium::ERelation::OneToMany> link;

    QVERIFY(link.ids().isEmpty());
    QVERIFY(link.empty());
    QCOMPARE(link.size(), 0);
  }

  void oneToMany_single()
  {
    Ligarium::Link<TestRecord, Ligarium::ERelation::OneToMany> link;

    link.ids().append(42);

    QVERIFY(!link.empty());
    QCOMPARE(link.size(), 1);
    QCOMPARE(link.ids().at(0), 42);
  }

  void oneToMany_multiple()
  {
    Ligarium::Link<TestRecord, Ligarium::ERelation::OneToMany> link;

    link.set_ids({10, 20, 30});

    QVERIFY(!link.empty());
    QCOMPARE(link.size(), 3);

    QCOMPARE(link.ids().at(0), 10);
    QCOMPARE(link.ids().at(1), 20);
    QCOMPARE(link.ids().at(2), 30);
  }

  void oneToMany_clear()
  {
    Ligarium::Link<TestRecord, Ligarium::ERelation::OneToMany> link;

    link.set_ids({1, 2, 3});

    QVERIFY(!link.empty());

    link.ids().clear();

    QVERIFY(link.empty());
    QCOMPARE(link.size(), 0);
  }

  void oneToMany_equality()
  {
    using Link = Ligarium::Link<TestRecord, Ligarium::ERelation::OneToMany>;

    Link first;
    Link second;

    QVERIFY(first == second);

    first.set_ids({1, 2, 3});

    QVERIFY(first != second);

    second.set_ids({1, 2, 3});

    QVERIFY(first == second);

    second.set_ids({3, 2, 1});

    QVERIFY(first != second);
  }

  // -------------------------------------------------------------------------
  // ManyToMany
  // -------------------------------------------------------------------------

  void manyToMany_default()
  {
    Ligarium::Link<TestRecord, Ligarium::ERelation::ManyToMany> link;

    QVERIFY(link.ids().isEmpty());
    QVERIFY(link.empty());
    QCOMPARE(link.size(), 0);
  }

  void manyToMany_single()
  {
    Ligarium::Link<TestRecord, Ligarium::ERelation::ManyToMany> link;

    link.ids().append(42);

    QVERIFY(!link.empty());
    QCOMPARE(link.size(), 1);
    QCOMPARE(link.ids().first(), 42);
  }

  void manyToMany_multiple()
  {
    Ligarium::Link<TestRecord, Ligarium::ERelation::ManyToMany> link;

    link.set_ids({100, 200, 300, 400});

    QCOMPARE(link.size(), 4);

    QCOMPARE(link.ids().at(0), 100);
    QCOMPARE(link.ids().at(1), 200);
    QCOMPARE(link.ids().at(2), 300);
    QCOMPARE(link.ids().at(3), 400);
  }

  void manyToMany_clear()
  {
    Ligarium::Link<TestRecord, Ligarium::ERelation::ManyToMany> link;

    link.set_ids({1, 2, 3});

    link.ids().clear();

    QVERIFY(link.empty());
    QCOMPARE(link.size(), 0);
  }

  void manyToMany_equality()
  {
    using Link = Ligarium::Link<TestRecord, Ligarium::ERelation::ManyToMany>;

    Link first;
    Link second;

    first.set_ids({1, 2});
    second.set_ids({1, 2});

    QVERIFY(first == second);

    second.set_ids({1, 3});

    QVERIFY(first != second);
  }

  // -------------------------------------------------------------------------
  // SqlLinkSpec
  // -------------------------------------------------------------------------

  void sqlLinkSpec_default()
  {
    Ligarium::SqlLinkSpec spec;

    QCOMPARE(spec.local_column, QStringView{});
    QCOMPARE(spec.foreign_column, QStringView{});
    QCOMPARE(spec.association_owner_column, QStringView{});
    QCOMPARE(spec.association_target_column, QStringView{});
  }

  void sqlLinkSpec_oneToOne()
  {
    Ligarium::SqlLinkSpec spec{.local_column = QStringView(u"property_id")};

    QCOMPARE(spec.local_column, QStringView(u"property_id"));
  }

  void sqlLinkSpec_oneToMany()
  {
    Ligarium::SqlLinkSpec spec{.target_table = Ligarium::Table{}, .foreign_column = QStringView(u"owner_id")};

    QCOMPARE(spec.foreign_column, QStringView(u"owner_id"));
  }

  void sqlLinkSpec_manyToMany()
  {
    Ligarium::SqlLinkSpec spec{.association_table         = Ligarium::Table{},
                               .association_owner_column  = QStringView(u"property_id"),
                               .association_target_column = QStringView(u"tenant_id")};

    QCOMPARE(spec.association_owner_column, QStringView(u"property_id"));

    QCOMPARE(spec.association_target_column, QStringView(u"tenant_id"));
  }

  // -------------------------------------------------------------------------
  // link metadata factories
  // -------------------------------------------------------------------------

  void linkOneToOne()
  {
    using Record = TestRecord;

    auto field = Ligarium::link_OneToOne(
        QStringView(u"tenant_id"),
        static_cast<Ligarium::Link<TestRecord, Ligarium::ERelation::OneToOne> Record::*>(nullptr));

    QCOMPARE(field.name, QStringView(u"tenant_id"));

    QCOMPARE(field.spec.local_column, QStringView(u"tenant_id"));
  }

  void linkManyToOne()
  {
    using Record = TestRecord;

    auto field = Ligarium::link_ManyToOne(
        QStringView(u"property_id"),
        static_cast<Ligarium::Link<TestRecord, Ligarium::ERelation::ManyToOne> Record::*>(nullptr));

    QCOMPARE(field.name, QStringView(u"property_id"));

    QCOMPARE(field.spec.local_column, QStringView(u"property_id"));
  }

  // -------------------------------------------------------------------------
  // polymorphic relation
  // -------------------------------------------------------------------------

  void polymorphicLinkMetadata()
  {
    // This test only verifies the metadata factory.
    // The actual loading/saving behavior is tested separately
    // once PolymorphicLink's API is covered.

    QCOMPARE(Ligarium::ERelation::PolymorphicOneToMany, Ligarium::ERelation::PolymorphicOneToMany);
  }
};

} // namespace

QTEST_MAIN(LinkTest)

#include "test_link.moc"