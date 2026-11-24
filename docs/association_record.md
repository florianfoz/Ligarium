# Association Records

`AssociationRecord` is a base class for records that represent relationships between two other record types.

It is designed for many-to-many relationships and is particularly useful when a relationship has its own data.

For example, a user can belong to several projects, and each project can have several users. The relationship can also store information such as the user's role or joining date.

## 1. Defining an association record

An association record inherits from `AssociationRecord` instead of directly from `Record`.

It must contain two `ManyToOne` links: one to the owner record and one to the target record.

For example:

```cpp
struct ProjectMember : ligarium::AssociationRecord<ProjectMember>
{
    Link<User, ERelation::ManyToOne> user;
    Link<Project, ERelation::ManyToOne> project;

    // Additional fields describing the relationship
    QString role;
    QDate joined_at;
};
```

Each `ProjectMember` record represents one association between a user and a project.

The association can store its own data, independently of the linked records.

## 2. Reading associations

`AssociationRecord` provides methods to retrieve associations from the database.

### Read all associations for a record

Use `read_associations()` to retrieve all association records matching a target ID:

```cpp
auto memberships =
    ProjectMember::read_associations(
        database,
        user_id,
        &ProjectMember::user
    );
```

This returns all `ProjectMember` records associated with the specified user.

### Read associations between two records

Use `read_between()` to retrieve associations matching both linked record IDs:

```cpp
auto memberships =
    ProjectMember::read_between(
        database,
        user_id,
        &ProjectMember::user,
        project_id,
        &ProjectMember::project
    );
```

This is useful when you need to retrieve the relationship between two specific records, including its additional fields.

## 3. Reading association IDs

If you only need the IDs of association records, use:

- `read_association_ids()` to retrieve association IDs matching one linked record.
- `read_between_ids()` to retrieve association IDs matching two linked records.

For example:

```cpp
auto association_ids =
    ProjectMember::read_between_ids(
        database,
        user_id,
        &ProjectMember::user,
        project_id,
        &ProjectMember::project
    );
```

These methods return a `QList<qsizetype>`.

## 4. Counting associations

Use `count_associations()` to count the associations matching a record:

```cpp
auto count =
    ProjectMember::count_associations(
        database,
        user_id,
        &ProjectMember::user
    );
```

The result is a `std::optional<qsizetype>`:

- A value contains the number of matching associations.
- `std::nullopt` indicates that the count could not be retrieved.

## 5. Synchronizing associations

`sync_associations()` is the main method for managing a collection of associations.

It synchronizes the associations belonging to one owner with a list of target record IDs.

The method performs three operations:

1. Removes associations whose target IDs are no longer requested.
2. Creates associations for target IDs that do not yet exist.
3. Keeps existing associations and updates their additional fields through an optional callback.

For example, to synchronize the projects associated with a user:

```cpp
QList<qsizetype> project_ids = {
    website_id,
    mobile_app_id
};

bool success = ProjectMember::sync_associations(
    database,
    user_id,
    &ProjectMember::user,
    project_ids,
    &ProjectMember::project
);
```

After synchronization, the user is associated with exactly the projects in `project_ids`.

Existing association records are preserved when their target IDs remain in the requested list. Their additional fields are not modified by the default overload.

### Updating additional association fields

You can provide a callback to initialize or update fields belonging to the association record:

```cpp
bool success = ProjectMember::sync_associations(
    database,
    user_id,
    &ProjectMember::user,
    project_ids,
    &ProjectMember::project,
    [](ProjectMember& membership) {
        membership.role = "Developer";
    }
);
```

The callback is called for every requested association, whether it already existed or has just been created.

This is useful for updating relationship-specific information such as roles, dates, permissions or status.

### Transaction safety

Synchronization is performed inside a database transaction.

If an operation fails, the transaction is rolled back. If all operations succeed, the transaction is committed.

After a successful synchronization, Ligarium emits `signal_db_updated` for the association table.

## 6. Available methods

| Method | Purpose |
|---|---|
| `read_associations()` | Read complete association records for one linked record |
| `read_between()` | Read complete associations between two records |
| `read_association_ids()` | Read association record IDs for one linked record |
| `read_between_ids()` | Read association record IDs between two records |
| `count_associations()` | Count associations for one linked record |
| `sync_associations()` | Synchronize associations with a list of target IDs |

## 7. Important notes

- Both relationship members must be `Link` fields with `ERelation::ManyToOne`.
- The member pointers passed to these methods must correspond to fields declared by the association record.
- `sync_associations()` requires a valid owner ID and valid target IDs.
- An empty target ID list is treated as a successful no-op: it does not remove existing associations.
- The synchronization callback runs for every requested association and should set the desired values consistently.
- `sync_associations()` identifies existing associations by target ID for the specified owner. The design therefore assumes that an owner-target pair identifies a single association.

`AssociationRecord` is especially useful when a relationship must be managed as a first-class record rather than as a simple link.