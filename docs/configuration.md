# Build Configuration

Ligarium is configured by the application that builds it.

## Application table type

For example:

```cpp
enum class Table : uint8_t {
  NONE = 0,
  Property,
  Tenant, 
  Attachment,
};
```

Ligarium then uses:

```cpp
ligarium::Table
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
#ifndef LIGARIUM_CONFIG_H
#define LIGARIUM_CONFIG_H

#include <QString>

namespace ligarium
{

// user defined table
enum class Table : uint8_t {
  NONE,
  Attachment,
  Damage,
  Invoice,
  Landlord,
  LandlordP,
  Lease_Agreement,
  Maintenance,
  Property,
  Property_Feature,
  Property_Room,
  Receipt,
  Rent,
  Tenant,
};

// user defined table_to_str
[[nodiscard]]
inline QString Table_to_str(Table table)
{
  switch (table) {
  case Table::NONE:             return "NONE";
  case Table::Attachment:       return "Attachment";
  case Table::Damage:           return "Damage";
  case Table::Invoice:          return "Invoice";
  case Table::Landlord:         return "Landlord";
  case Table::LandlordP:        return "LandlordP";
  case Table::Lease_Agreement:  return "Lease_Agreement";
  case Table::Maintenance:      return "Maintenance";
  case Table::Property:         return "Property";
  case Table::Property_Feature: return "Property_Feature";
  case Table::Property_Room:    return "Property_Room";
  case Table::Receipt:          return "Receipt";
  case Table::Rent:             return "Rent";
  case Table::Tenant:           return "Tenant";
  }
}

} // namespace ligarium

// must be included before user ligarium_config.h file
#include <ligarium/ligarium.h>

// Current database used
extern std::optional<ligarium::Database> DATABASE;

#endif // LIGARIUM_CONFIG_H
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
    return ligarium::Table_to_str(table);
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
