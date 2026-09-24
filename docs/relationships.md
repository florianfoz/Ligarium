# Relationships

Ligarium provides typed relationships through `Link<TARGET, RELATION>`.

Supported relationship types are:

```cpp
ERelation::OneToOne
ERelation::OneToMany
ERelation::ManyToOne
ERelation::ManyToMany
ERelation::PolymorphicOneToMany
```

## Many-to-one

A property belonging to one tenant can be represented as:

```cpp
Link<Tenant, ERelation::ManyToOne> tenant;
```

The relationship stores the target identifier.

## One-to-one

```cpp
Link<Address, ERelation::OneToOne> address;
```

represents a single target record.

## One-to-many

```cpp
Link<Property, ERelation::OneToMany> properties;
```

contains multiple target identifiers.

Useful operations include:

```cpp
properties.empty();
properties.size();
```

and loading the target records.

## Many-to-many

Many-to-many relationships use an association table.

For example:

```cpp
link_ManyToMany(
    u"tenants",
    &Property::tenants,
    ligarium::Table::PropertyTenant,
    u"property_id",
    u"tenant_id"
);
```

The association table contains the relationship:

```text
property_id | tenant_id
------------+----------
1           | 10
1           | 12
2           | 10
```

## Association records

When a many-to-many relationship has additional attributes, the association can be represented as a normal `Record`.

For example:

```text
property_id
landlord_id
share_percent
```

can become:

```cpp
class Landlord_Property
    : public ligarium::Record<Landlord_Property>
{
    // ...
};
```

This is useful when the relationship itself has business data.

## Link metadata

Relationship declarations provide enough information for Ligarium to understand:

* the local column;
* the target table;
* the foreign column;
* the association table;
* association columns.

The metadata is represented internally by `SqlLinkSpec`.
