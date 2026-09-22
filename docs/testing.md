# Testing

Ligarium uses QtTest for its automated test suite.

The tests are designed to behave like an application using the library rather than testing only private implementation details.

## Application configuration

The tests define their own application table type:

```cpp
DEFINE_ENUM(Ligarium::Table, int,
            Property, 1,
            Tenant, 2,
            Attachment, 3)
```

and configure Ligarium with:

```cpp
#define LIGARIUM_TABLE_TYPE Ligarium::Table
```

This verifies the same build-time configuration mechanism used by real applications.

## Test records

Tests define actual record classes:

```cpp
class Property : public Ligarium::Record<Property>
{
    // ...
};
```

The test suite therefore exercises:

* record construction;
* table mapping;
* field mapping;
* database persistence;
* relationships;
* record identity;
* dirty state;
* polymorphic links.

## SQLite

The tests use Qt SQL with SQLite.

In-memory databases are useful for isolated tests:

```cpp
connection.setDatabaseName(":memory:");
```

This avoids depending on external database servers.

## QtTest

A test class generally looks like:

```cpp
class DatabaseTest : public QObject
{
    Q_OBJECT

private slots:
    void create_record();
    void find_record();
    void delete_record();
};
```

and ends with:

```cpp
QTEST_MAIN(DatabaseTest)
#include "test_database.moc"
```

CMake must have AUTOMOC enabled for these tests.

## Running the tests

```bash
cmake --preset debug-linux
cmake --build --preset build-debug-linux
ctest --test-dir build/debug --output-on-failure
```

## Test philosophy

Tests should use the public API whenever possible.

For example, prefer:

```cpp
Property property =
    Property::create_record(db);
```

over directly calling an internal implementation function unless the lower-level API itself is being tested.

This keeps the test suite representative of actual Ligarium usage.
