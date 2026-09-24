# Installation

This document explains how to build and install Ligarium from source.

## Requirements

Ligarium requires:

* C++20 compatible compiler
* CMake 3.21 or newer
* Qt 6 with:
  * Qt Core
  * Qt SQL
  * Qt Widgets
* SQLite support through the Qt SQL SQLite driver
* Ninja is recommended

Ligarium currently uses [EnumLite](https://github.com/florianfoz/enumlite) as an internal dependency.

## Supported platforms

Ligarium is designed to work on platforms supported by Qt 6 and a C++20 compiler.

The primary development environment is:

* Linux / Windows
* GCC or Clang
* CMake
* Ninja
* Qt 6

Other platforms should work when the required Qt and C++ toolchain components are available.


## Getting the source

Clone the repository:

```bash
git clone https://github.com/<owner>/ligarium.git
cd ligarium
```

Replace `<owner>` with the GitHub account or organization containing the repository.


## Qt installation

Ligarium requires Qt 6 with the Core and SQL modules.

For example, if Qt is installed under:

```text
$HOME/Qt/6.x.x/gcc_64
```

CMake can be configured with:

```bash
cmake \
    -S . \
    -B build \
    -G Ninja \
    -DCMAKE_PREFIX_PATH="$HOME/Qt/6.x.x/gcc_64"
```

The SQLite Qt SQL driver must also be available.

You can verify the available SQL drivers from Qt:

```cpp
qDebug() << QSqlDatabase::drivers();
```

The list should contain:

```text
QSQLITE
```

## Installing

Ligarium provides a CMake installation configuration.

Install it with:

```bash
cmake --install build
```

A custom installation prefix can be specified during configuration:

```bash
cmake \
    -S . \
    -B build \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/path/to/install
```

Then:

```bash
cmake --build build
cmake --install build
```

The installation contains the public headers, library and CMake package configuration.


## Building with tests

Tests are disabled by default.

Enable them with:

```bash
cmake --build --preset build-debug-linux \
    -DLIGARIUM_BUILD_TESTS=ON
```

Then build:

```bash
cmake --build build
```

Run the test suite:

```bash
ctest \
    --test-dir build \
    --output-on-failure
```

The test suite uses an application-defined `Table` type through the test configuration header.

This is intentional: Ligarium is configured at build time with the application's table type.


## Using the provided CMake preset

The repository provides CMake presets for development environments.

> presets `linux` `windows` `macos`

For the Linux debug configuration:

```bash
cmake --preset debug-linux
```

Build:

```bash
cmake --build --preset build-debug-linux
```

Run the tests:

```bash
./ligarium-test.sh
```

> check `CMakePresets.json` then `CMAKE_PREFIX_PATH` if your Qt root is installed in other path

---

## Using the development test script

The repository may also provide:

```bash
./ligarium-test.sh
```

The script performs a clean debug configuration, builds the project and runs the test suite.

Make sure the script is executable:

```bash
chmod +x ligarium-test.sh
```

Then:

```bash
./ligarium-test.sh
```

---

## Integrating Ligarium into a CMake project

After installing Ligarium, an application can use the exported CMake target:

```cmake
find_package(Ligarium REQUIRED)

target_link_libraries(MyApplication
    PRIVATE
        ligarium::Ligarium
)
```

Ligarium exposes its required Qt dependencies through its CMake target.

The application must still provide the Qt installation used to configure the project.

---

## Application table configuration

Ligarium uses an application-defined `Table` type.

The application must define its table enumeration before including Ligarium:

```cpp
DEFINE_ENUM(ApplicationTable, int,
            Property, 1,
            Tenant, 2,
            Attachment, 3)

#define LIGARIUM_TABLE_TYPE ApplicationTable
```

It must also provide the conversion expected by Ligarium:

```cpp
namespace Ligarium
{

inline QString Table_to_str(ApplicationTable table)
{
    return ApplicationTable_to_str(table);
}

}
```

Then:

```cpp
#include <ligarium.h>
```

This configuration is part of Ligarium's compile-time architecture.

### Important

Ligarium is not designed around a single universal precompiled library that can be used with arbitrary application `Table` types.

The application table type must be visible when Ligarium is compiled.

For development builds, this can be achieved with a compiler forced-include option:

```text
GCC / Clang:
-include/path/to/application_config.h

MSVC:
/FI/path/to/application_config.h
```

The application's build system is therefore responsible for providing the Ligarium configuration header.

---

## SQLite database

Ligarium uses Qt SQL and SQLite.

A database can be stored in a regular file:

```cpp
QSqlDatabase database =
    QSqlDatabase::addDatabase("QSQLITE");

database.setDatabaseName("application.db");

if (!database.open()) {
    // Handle the error.
}
```

For tests or temporary databases, SQLite's in-memory database can be used:

```cpp
database.setDatabaseName(":memory:");
```

The database must have the schema expected by the records.

Ligarium provides `SchemaBuilder` to generate this schema from record metadata.

For example:

```cpp
ligarium::SchemaBuilder schema(database);

if (!schema.create_all<Property, Tenant, Attachment>()) {
    qFatal(
        "Schema creation failed: %s",
        qPrintable(schema.last_error()));
}
```

See [`docs/schema.md`](docs/schema.md) for details.

---

## Verifying the installation

A minimal application can verify the installation with:

```cpp
#include <ligarium.h>

int main()
{
    return 0;
}
```

Compile it against:

```text
ligarium::Ligarium
```

If the application configures and links successfully, the basic Ligarium installation is available.

For a more complete verification, build and run the Ligarium test suite.


## Troubleshooting

### CMake cannot find Qt

Specify the Qt installation path:

```bash
cmake \
    -S . \
    -B build \
    -DCMAKE_PREFIX_PATH="/path/to/Qt/6.x.x/gcc_64"
```

Alternatively, configure:

```bash
export CMAKE_PREFIX_PATH="/path/to/Qt/6.x.x/gcc_64"
```

and run CMake again.

### `QSQLITE` is unavailable

Check the Qt SQL drivers:

```cpp
qDebug() << QSqlDatabase::drivers();
```

If `QSQLITE` is missing, install or enable the Qt SQLite SQL driver for the Qt installation being used.

### `LIGARIUM_TABLE_TYPE` is not defined

Ligarium requires an application-defined table type.

Define:

```cpp
#define LIGARIUM_TABLE_TYPE ApplicationTable
```

before including Ligarium.

For library builds, ensure the configuration header is force-included before Ligarium sources are compiled.

### Schema creation fails

Check:

```cpp
schema.last_error()
```

Typical causes include:

* a referenced target table was not included in `create_all`;
* the SQLite connection is not open;
* an invalid SQL identifier was supplied;
* the SQLite SQL driver is unavailable.

For example, if `Property` references `Tenant`, create both:

```cpp
schema.create_all<Property, Tenant>();
```

rather than creating only:

```cpp
schema.create<Property>();
```


## Development workflow

A typical development cycle is:

```bash
git clone https://github.com/<owner>/ligarium.git
cd ligarium

cmake --preset debug-linux

cmake --build --preset build-debug-linux

ctest \
    --test-dir build/debug \
    --output-on-failure
```

For a clean build:

```bash
rm -rf build
```

then repeat the configuration and build steps.


## Next steps

After installation, see:

* [`README.md`](README.md) — project overview and quick start
* [`docs/configuration.md`](docs/configuration.md) — application configuration
* [`docs/records.md`](docs/records.md) — records
* [`docs/fields.md`](docs/fields.md) — SQL fields
* [`docs/relationships.md`](docs/relationships.md) — relationships
* [`docs/polymorphic.md`](docs/polymorphic.md) — polymorphic relationships
* [`docs/schema.md`](docs/schema.md) — schema generation
* [`docs/database.md`](docs/database.md) — database API
* [`docs/testing.md`](docs/testing.md) — test architecture
* [`docs/cmake.md`](docs/cmake.md) — CMake integration
