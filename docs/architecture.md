# Ligarium Architecture

## Overview

Ligarium is organized around four layers:

```text
Application model
       │
       ▼
Compile-time metadata
       │
       ▼
Ligarium persistence layer
       │
       ▼
Qt SQL
       │
       ▼
SQL database
```

The application owns the domain model. Ligarium provides the machinery required to persist that model.

## Core components

```text
include/
├── ligarium.h
├── database.h
├── record.h
├── field.h
├── link.h
├── polymorphic_link.h
└── record_registry.h

src/
├── database.cpp
├── field.cpp
├── link.cpp
└── record.cpp
```

### `ligarium.h`

Defines the global Ligarium configuration and exposes the application-defined table type as:

```cpp
ligarium::Table
```

### `Database`

Provides the SQL-facing API around `QSqlDatabase`.

### `Record`

Provides the generic persistence API for application records.

### `Field`

Describes the mapping between a C++ member and an SQL column.

### `Link`

Describes typed relationships between records.

### `PolymorphicLink`

Represents relationships whose target may belong to different tables.

### `RecordRegistry`

Provides optional runtime registration of record-related behavior.

## Compile-time model

Ligarium deliberately keeps the model description in C++.

For example:

```cpp
static constexpr auto sql_fields()
{
    return std::tuple{
        field(u"name", &Property::name),
        field(u"surface", &Property::surface),
    };
}
```

This metadata can then be consumed by generic Ligarium algorithms.

## Application ownership

Ligarium does not define the application's domain model.

The application defines:

* its tables;
* its records;
* its SQL schema;
* its business logic.

Ligarium provides the persistence mechanism.

## No GUI dependency in the core

The core library depends on Qt Core and Qt SQL.

Qt Widgets functionality should remain in a separate layer so that the ORM can also be used by:

* command-line applications;
* services;
* background processes;
* tests;
* non-GUI applications.

## SQL remains accessible

Ligarium is not intended to hide SQL completely.

Applications can access the underlying:

```cpp
const QSqlDatabase& connection() const noexcept;
```

and use `QSqlQuery` directly when necessary.

This allows Ligarium to coexist with application-specific SQL.
