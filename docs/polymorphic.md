# Polymorphic Relationships

A polymorphic relationship is a relationship where the target may belong to different tables.

Instead of storing only:

```text
target_id
```

the database stores:

```text
__poly_table
__poly_id
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

class Attachment final : public ligarium::Record<Attachment>
{
public:
  static constexpr auto static_table = ligarium::Table::Attachment;

  ligarium::Link<Attachment, ligarium::ERelation::PolymorphicManyToOne> target;

  QString path;

  ...

  static constexpr auto sql_fields()
  {
    return std::tuple{
        ligarium::link_PolymorphicManyToOne(&Attachment::target), //
        ligarium::field(u"path", &Attachment::path),              //
    };
  }

  ...
};
```

## Use cases

Polymorphic relationships are useful for:

* attachments;
* comments;
* activity logs;
* tags;
* audit records;
* generic metadata;
* other objects that can belong to multiple record types.
