# Ligarium Widgets

Ligarium provides a set of Qt Widgets to display records, edit them and manage relationships between records.

These widgets are designed to work with Ligarium's record system and can be used to build database interfaces without implementing every interaction from scratch.

## 1. Registering a record widget

To display or edit a record through Ligarium's widget system, create a widget derived from `IRecordWidget` and register it with `WidgetRegistry`.

A record widget provides three main operations:

- `load()` — load the record data into the widget.
- `save()` — save the widget data. Returns `true` if the operation succeeds.
- `reset()` — reset the widget's state.

Register the widget for its record type:

```cpp
WidgetRegistry::register_widget<MyRecord, MyRecordWidget>();
```

The registry allows Ligarium to find the appropriate widget when a record needs to be displayed or edited.

When creating a new record, Ligarium uses `INVALID_ID` as the record ID.

## 2. Record manager — `W_Manager`

`W_Manager` provides a tile-based interface for managing records from a database table.

Users can:

- View the records in a table.
- Open a record for editing.
- Delete a record.
- Create a new record.

The Create action opens the registered widget for the record type.

The current implementation does not provide built-in search, filtering, sorting or pagination.

## 3. Record tile — `W_Selectable`

`W_Selectable` displays an individual record as a selectable tile.

It provides:

- Record selection.
- An edit action.
- A delete action.

It is primarily used by `W_Manager` and `W_Tile_View`.

## 4. Record picker — `W_Picker`

`W_Picker` opens a dialog allowing the user to select a record.

When a record is selected, the widget emits:

```cpp
signal_selected(table, id);
```

The dialog can be used by relationship fields to let users choose an existing record.

The Clear action closes the dialog without selecting a record.

## 5. Single relationship field — `W_Field`

`W_Field` displays and manages a reference to a single record.

It can be used for relationships such as:

- One-to-One
- Many-to-One

The field provides:

- Display of the selected record.
- Access to the selected record ID through `id()`.
- Access to the associated table through `table()`.
- Access to the database through `database()`.
- A picker dialog to choose another record.

The `refresh()` and `set_record()` methods can be used to update the displayed record.

## 6. Multiple relationship field — `W_Many_Field`

`W_Many_Field` is intended to manage a selection of multiple related records.

It is designed for relationships such as One-to-Many and association-based relationships.

Its interface includes:

- `refresh()` — update the displayed selection.
- `set_selection()` — set the selected records.
- `selection()` — retrieve the current selection.

**Status:** this widget is still under development. The current implementation has limitations in how it handles target record IDs and association record IDs. It should not yet be considered a complete association editor.

## 7. Relationship item — `W_Many_Elem`

`W_Many_Elem` represents an individual element in a multiple-record field.

It provides a way to display an element and request its removal through a signal.

It is intended to be used as part of `W_Many_Field`.

## 8. Tile container — `W_Tile_View`

`W_Tile_View` displays widgets in a tile-based layout.

It uses `W_FlowLayout` to arrange its children in a flow layout, adapting their positions to the available space.

It is used to present collections of records or selectable elements.

## 9. Widget modes

Ligarium provides two widget modes through `setWidgetMode()`:

| Mode | Behaviour |
|---|---|
| `WidgetMode::ReadOnly` | Prevents editing through supported Qt controls. |
| `WidgetMode::ReadWrite` | Allows editing through supported Qt controls. |

In read-only mode, common input controls are made read-only or disabled.

Supported controls include line edits, text editors, spin boxes, date and time controls, combo boxes, checkboxes, radio buttons, push buttons and sliders.

Custom widgets may need to implement their own read-only behaviour.

## 10. Opening a record widget in a dialog

Ligarium provides `exec_widget_dialog()` to open a record widget in a modal dialog.

When the user confirms the dialog, Ligarium calls `save()`.

The dialog is accepted only if `save()` returns `true`. This allows the widget to prevent closing when validation or saving fails.

## Widget overview

| Widget | Purpose |
|---|---|
| `W_Manager` | Manage records from a table |
| `W_Tile_View` | Display widgets in a tile layout |
| `W_Selectable` | Display and select an individual record |
| `W_Picker` | Select a record in a dialog |
| `W_Field` | Select and display one related record |
| `W_Many_Field` | Manage multiple related records (under development) |
| `W_Many_Elem` | Display an element in a multiple-record field |
| `W_FlowLayout` | Arrange widgets in a responsive flow layout |

## Current limitations

Some features are not complete in the current implementation:

- `W_Many_Field` is not yet a complete association editor.
- `W_Manager` does not include search, filtering, sorting or pagination.
- Custom widgets may require their own implementation of read-only behaviour.

The widget API may evolve as Ligarium development continues.