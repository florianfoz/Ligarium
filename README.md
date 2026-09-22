# Ligarium

**Ligarium** is a compile-time C++20 / Qt 6 ORM for mapping application-defined records, fields, and relationships to SQL databases.

The name comes from the Latin *ligare*, meaning **to bind, tie, or connect**.

## Why Ligarium?

Ligarium provides a lightweight ORM layer for C++/Qt applications without introducing a separate model language or generated model classes.

The application defines its own persistent types:

```cpp
class Property : public Ligarium::Record<Property>
{
public:
    static constexpr Ligarium::Table static_table =
        ApplicationTable::Property;

    QString name;
    double surface = 0.0;

    static constexpr auto sql_fields()
    {
        return std::tuple{
            Ligarium::field(u"name", &Property::name),
            Ligarium::field(u"surface", &Property::surface),
        };
    }
};
```

Ligarium uses this compile-time metadata to connect the C++ model to an SQL database.

```text
Application
    │
    ├── ApplicationTable
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
| [Database](docs/database.md)                     | `Ligarium::Database` and Qt SQL               |
| [Registries](docs/registries.md)                 | Record and GUI registries                     |
| [Testing](docs/testing.md)                       | Test architecture and QtTest                  |
| [CMake](docs/cmake.md)                           | Building and integrating Ligarium             |

## Minimal model

An application first defines its table type:

```cpp
DEFINE_ENUM(ApplicationTable, int,
            Property, 1,
            Tenant, 2)

#define LIGARIUM_TABLE_TYPE ApplicationTable
```

Then a record can be defined:

```cpp
class Tenant : public Ligarium::Record<Tenant>
{
public:
    static constexpr Ligarium::Table static_table =
        ApplicationTable::Tenant;

    QString name;

    Tenant(Ligarium::Database* db = nullptr)
        : Record(db)
    {
    }

    static constexpr auto sql_fields()
    {
        return std::tuple{
            Ligarium::field(u"name", &Tenant::name),
        };
    }
};
```

And persisted through the record API:

```cpp
Tenant tenant = Tenant::create_record(db);

tenant.name = u"John Doe";
tenant.save_record();
```

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

## Project status

Ligarium is currently **pre-1.0 and under active development**.

The API may evolve, particularly around relationship handling, polymorphic relationships, registries, and database integration.

## License

Ligarium is licensed under the Apache License 2.0.

See [LICENSE](LICENSE) for details.
