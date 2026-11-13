# Relationships

Ligarium provides typed relationships through `Link<TARGET, RELATION>`.

Supported relationship types are:

```cpp
ERelation::OneToOne
ERelation::OneToMany
ERelation::ManyToOne
ERelation::ManyToMany
ERelation::PolymorphicManyToOne
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

## Association records

Designed for a many-to-many relationship, it's possible to specify additional attributes, the association is represented as a normal `Record`.

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
