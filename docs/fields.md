# Fields

Ligarium maps C++ members to SQL columns through compile-time field descriptors.

## Basic field

```cpp
QString name;
```

can be mapped with:

```cpp
field(u"name", &Property::name)
```

A complete definition:

```cpp
static constexpr auto sql_fields()
{
    return std::tuple{
        Ligarium::field(u"name", &Property::name),
        Ligarium::field(u"surface", &Property::surface),
    };
}
```

## Column name

The first argument identifies the SQL column:

```cpp
field(u"name", &Property::name)
```

The second argument is a pointer-to-member:

```cpp
&Property::name
```

This provides a direct compile-time relationship between the SQL column and C++ member.

## Compile-time metadata

`sql_fields()` returns a tuple containing the field descriptors.

Ligarium can use this metadata to implement generic operations such as:

* SQL value extraction;
* member assignment;
* SQL parameter binding;
* record persistence.

## Why pointer-to-member?

A pointer-to-member avoids string-based C++ property lookup.

Instead of:

```text
"name" → runtime reflection → member
```

the mapping is:

```text
"name"
   +
&Property::name
```

and the compiler verifies the C++ member.

## Fields and relationships

Scalar fields are described with `field()`.

Relationships are described with the `Link` API.

For example:

```cpp
field(u"name", &Property::name)
```

versus:

```cpp
link_ManyToOne(
    u"tenant_id",
    &Property::tenant)
```

The two mechanisms can coexist in the same `sql_fields()` tuple.
