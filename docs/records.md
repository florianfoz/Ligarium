# Records

A persistent model is represented by a class derived from:

```cpp
ligarium::Record<T>
```

where `T` is the derived class.

## Defining a record

```cpp
class Property : public ligarium::Record<Property>
{
public:
    static constexpr auto static_table =
        ligarium::Table::Property;

    QString name;
    double surface = 0.0;

    Property(ligarium::Database* db = nullptr)
        : Record(db)
    {
    }

    static constexpr auto sql_fields()
    {
        return std::tuple{
            ligarium::field(u"name", &Property::name),
            ligarium::field(u"surface", &Property::surface),
        };
    }
};
```

A record normally provides:

* `static_table`;
* `sql_fields()`;
* a constructor accepting `Database*`;
* equality when required by dirty-state detection.

## Record identity

Each record has an identifier.

```cpp
qsizetype id() const;
```

A new record has:

```cpp
ligarium::INVALID_ID
```

as its identifier.

## Creating a record

```cpp
Property property =
    Property::create_record(db);
```

## Reading a record

```cpp
Property property =
    Property::read_record(db, property_id);
```

or:

```cpp
Property property =
    ligarium::read_record<Property>(db, property_id);
```

## Saving

```cpp
property.name = u"Apartment";
property.surface = 85.0;

property.save_record();
```

## Deleting

```cpp
property.delete_record();
```

After successful deletion, the record identifier becomes invalid.

## Validity

```cpp
if (property.is_valid())
{
    // Record exists in the database.
}
```

The record also supports boolean conversion:

```cpp
if (property)
{
    // Valid persistent record.
}
```

## Loading all records

```cpp
QList<Property> properties =
    Property::all_records(db);
```

Only identifiers can be loaded with:

```cpp
QList<qsizetype> ids =
    Property::all_records_id(db);
```

## Dirty state

Ligarium can compare a record with its persisted state.

```cpp
if (property.is_dirty())
{
    // Local state differs from the database.
}
```

This requires the record to provide an appropriate equality operator.

## `dump()`

Records may override:

```cpp
QString dump() const;
```

to provide a human-readable representation.

This is particularly useful for debugging and registries.
