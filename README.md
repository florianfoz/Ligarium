# Ligarium

**Ligarium** is a compile-time C++20 / Qt 6 ORM for mapping application-defined records, fields, and relationships to SQL databases.

The name comes from the Latin *ligare*, meaning **to bind, tie, or connect**.

## Why Ligarium?

Ligarium provides a lightweight ORM layer for C++/Qt applications without introducing a separate model language or generated model classes.

The application defines its own persistent types:

```cpp
class Property : public ligarium::Record<Property>
{
public:
    static constexpr auto static_table =
        ligarium::Table::Property;

    QString name;
    double surface = 0.0;

    static constexpr auto sql_fields()
    {
        return std::tuple{
            ligarium::field(u"name", &Property::name),
            ligarium::field(u"surface", &Property::surface),
        };
    }
};
```

Ligarium uses this compile-time metadata to connect the C++ model to an SQL database.

```text
Application
    │
    ├── ligarium::Table
    ├── Record types
    ├── Fields
    └── Relationships
             │
             ▼
         Ligarium
             │
             ▼
        Qt SQL / SQL database
```

## Main features

* C++20
* Qt 6
* Compile-time record metadata
* Application-defined table types
* Strongly typed records
* Typed fields
* One-to-one relationships
* One-to-many relationships
* Many-to-one relationships
* Many-to-many relationships
* Polymorphic relationships
* Qt SQL integration
* SQLite support
* SQLite schema generation from user records
* QtTest-based test suite
* Apache-2.0 licensed


## Documentation

| Document                                         | Description                                   |
| ------------------------------------------------ | --------------------------------------------- |
| [Architecture](docs/architecture.md)             | Design and internal organization              |
| [Configuration](docs/configuration.md)           | Application-defined `Table` configuration     |
| [Records](docs/records.md)                       | Defining and persisting records               |
| [Fields](docs/fields.md)                         | Mapping C++ members to SQL columns            |
| [Relationships](docs/relationships.md)           | Typed relationships between records           |
| [Polymorphic relationships](docs/polymorphic.md) | Relationships targeting multiple record types |
| [Database](docs/database.md)                     | `ligarium::Database` and Qt SQL               |
| [Registries](docs/registries.md)                 | Record and GUI registries                     |
| [Testing](docs/testing.md)                       | Test architecture and QtTest                  |
| [CMake](docs/cmake.md)                           | Building and integrating Ligarium             |

## Build

Requirements:

* CMake 3.21+
* C++20 compiler
* Qt 6
* Qt Core
* Qt SQL

Build with CMake presets:

```bash
cmake --preset debug-linux
cmake --build --preset build-debug-linux
```

For tests:

```bash
cmake --preset debug-linux
cmake --build --preset build-debug-linux
ctest --test-dir build/debug --output-on-failure
```

## Quick Start
See [INSTALLATION.md](/INSTALLATION.md)

> Please note that the project is currently under development, **and not all tests are passing yet**.

Define your configuraition, by create `ligarium_config.h`:

```cpp
#ifndef LIGARIUM_CONFIG_H
#define LIGARIUM_CONFIG_H

#include <QString>

namespace ligarium
{

// user defined table
enum class Table : uint8_t {
  NONE = 0,           // mandatory!
  Property,
  Tenant,
  Attachment,
};

// user defined table_to_str
[[nodiscard]]
inline QString Table_to_str(Table table)
{
  switch (table) {
  case Table::NONE:       return "NONE"; // mandatory!
  case Table::Property:   return "Property";
  case Table::Tenant:     return "Tenant";
  case Table::Attachment: return "Attachment";
  default:                return {};
  }
}

} // namespace ligarium

// must be included before user ligarium_config.h file
#include <ligarium/ligarium.h>

#endif // LIGARIUM_CONFIG_H

```

**The enum key `NONE = 0` is mandatory with the value 0, it's permit to determine an invalid state 

**Include `ligarium_config.h` always before any ligarium file!**

Define a record:

```cpp

class Property final : public ligarium::Record<Property>
{
public:
  static constexpr auto static_table = ligarium::Table::Property;

  QString name;

  [[nodiscard]]
  QString dump() const override
  {
    return name;
  }

  static constexpr auto sql_fields()
  {
    return std::tuple{
        ligarium::field(u"name", &Property::name) //
    };
  }

  // optional but recommended
  friend bool operator==(const Property& lhs, const Property& rhs)
  {
    return lhs.id() == rhs.id() && lhs.name == rhs.name;
  }
};
```

To use your record with ligarium widgets, you must register your records:
```cpp
int main(int argc, char* argv[])
{
  QApplication application(argc, argv);

  ligarium::register_widget<Property, W_Property_Creator, W_Property_View>();

  ...
}
```
You can register your record with a widget "creator" and a widget "view", 
they will be used by ligarium to interact with ligarium widgets to pick, select, create, edit, view your records.

> Hint: use a modification switch mode in your widget creator, check official ligarium implemented projects:
> https://github.com/florianfoz/rentalis 


Open a Qt SQL connection and use the record API:

```cpp
const QString connection_name = "my_name";

QSqlDatabase connection = QSqlDatabase::addDatabase("QSQLITE", connection_name);

connection.setDatabaseName(":memory:");

QVERIFY(connection.open());

ligarium::SchemaBuilder schema(connection);

if (!schema.create_all<Property, Tenant, Attachment>()) {
  qPrintable(schema.last_error());
}

ligarium::Database db(connection);

Property property = Property::create_record(db);
property.name = "My property";
property.save_record();
```

For the complete configuration and build setup, see [Configuration](docs/configuration.md) and [CMake](docs/cmake.md).

# AI-assisted development

Parts of `Ligarium` have been developed with the assistance of AI tools.

AI-generated or AI-assisted code is reviewed and validated before publication. This includes manual code review and automated validation through the project's test suite.

AI assistance does not replace the project's verification process: changes are expected to compile successfully and pass the relevant tests before being published.


## Project status

Ligarium is currently **pre-1.0 and under active development**.

**NOT all tests are passing yet**.

The API may evolve, particularly around relationship handling, polymorphic relationships, registries, and database integration.

## License

Ligarium is licensed under the Apache License 2.0.

See [LICENSE](LICENSE) for details.
