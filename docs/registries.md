# Registries

Registries provide optional runtime associations between application table types and application-defined behavior.

## Record registry

A record registry can associate a table with operations such as record dumping.

Conceptually:

```text
Ligarium::Table::Property
          │
          ▼
    RecordRegistry
          │
          ▼
     Property::dump()
```

A registry entry can contain a callable:

```cpp
using DumpFunction =
    std::function<QString(qsizetype)>;
```

The registry itself does not need to know the concrete record type.

## Why a registry?

Compile-time metadata is useful for persistence, but some application features require runtime dispatch.

For example, an application may receive:

```cpp
Ligarium::Table table;
qsizetype id;
```

and need to determine which record implementation handles that table.

A registry can provide that runtime bridge.

## Widget registry

GUI-specific registration should live outside the core ORM.

A widget registry can associate:

```text
Table
  │
  ▼
Widget factory
  │
  ▼
QWidget
```

For example:

```cpp
using Factory =
    std::function<QWidget*(qsizetype, QWidget*)>;
```

This allows a GUI application to register widgets without adding Qt Widgets dependencies to the core Ligarium library.

## Separation of concerns

The intended architecture is:

```text
Ligarium
    │
    └── RecordRegistry

LigariumWidgets
    │
    └── WidgetRegistry
```

The ORM core therefore remains usable without a GUI.
