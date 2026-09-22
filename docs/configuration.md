# Build Configuration

Ligarium is configured by the application that builds it.

The application provides its own table enumeration and exposes it to Ligarium through `LIGARIUM_TABLE_TYPE`.

## Application table type

For example:

```cpp
DEFINE_ENUM(ApplicationTable, int,
            Property, 1,
            Tenant, 2,
            Attachment, 3)

#define LIGARIUM_TABLE_TYPE ApplicationTable
```

Ligarium then uses:

```cpp
Ligarium::Table
```

as an alias for the application-defined table type.

## Why application-defined tables?

The ORM cannot know the tables belonging to an application.

One application might have:

```text
Property
Tenant
Attachment
```

while another might have:

```text
User
Project
Invoice
Document
```

The library therefore does not impose a universal table enumeration.

## Configuration header

A typical application configuration header is:

```cpp
#ifndef APPLICATION_LIGARIUM_CONFIG_H
#define APPLICATION_LIGARIUM_CONFIG_H

#include <enumlite/enumlite_backend_qt.h>

#define ENUMLITE_DEFAULT_BACKEND enumlite::qt_backend
#include <enumlite/enumlite.h>

DEFINE_ENUM(ApplicationTable, int,
            Property, 1,
            Tenant, 2,
            Attachment, 3)

#define LIGARIUM_TABLE_TYPE ApplicationTable

#endif
```

## Build-time inclusion

The configuration header must be visible while compiling Ligarium itself.

For GCC and Clang, this can be achieved with:

```text
-include application_ligarium_config.h
```

For MSVC:

```text
/FIapplication_ligarium_config.h
```

CMake can configure the target accordingly.

This is an intentional part of Ligarium's architecture.

## Table string conversion

The application also provides the conversion from its table enumeration to a string:

```cpp
namespace Ligarium
{

inline QString Table_to_str(Table table)
{
    return ApplicationTable_to_str(table);
}

}
```

The function must be `inline` when defined in a shared configuration header.

## Important consequence

Ligarium is not designed around a single precompiled binary that can transparently support arbitrary application table types.

Instead:

```text
Application configuration
          │
          ▼
      Ligarium build
          │
          ▼
   Application-specific
      Ligarium binary
```

This allows the library implementation to use the application's table type directly.
