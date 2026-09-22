# Polymorphic Relationships

A polymorphic relationship is a relationship where the target may belong to different tables.

Instead of storing only:

```text
target_id
```

the database stores:

```text
target_table
target_id
```

## Example

An attachment may belong to either a property or a tenant:

```text
Attachment
    │
    ├── Property / 12
    │
    ├── Tenant / 4
    │
    └── Property / 19
```

A typical record can contain:

```cpp
class Attachment
    : public Ligarium::Record<Attachment>
{
public:
    static constexpr Ligarium::Table static_table =
        ApplicationTable::Attachment;

    Ligarium::Table table;
    qsizetype col_id;
    QString path;
};
```

The pair:

```text
(table, col_id)
```

identifies the target.

## `PolymorphicLink`

Ligarium provides:

```cpp
PolymorphicLink<TARGET>
```

for polymorphic target collections.

The link stores target identifiers and can load the corresponding records when given the database context.

## Database indexing

A polymorphic relationship generally benefits from an index covering:

```text
(table, col_id)
```

For example:

```sql
CREATE INDEX attachment_target
ON attachment(table, col_id);
```

The exact schema remains the application's responsibility.

## Use cases

Polymorphic relationships are useful for:

* attachments;
* comments;
* activity logs;
* tags;
* audit records;
* generic metadata;
* other objects that can belong to multiple record types.
