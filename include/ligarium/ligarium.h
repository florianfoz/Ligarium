#ifndef LIGARIUM_H
#define LIGARIUM_H

#include <QSqlDatabase>


namespace ligarium
{

enum class Table : uint8_t; // user must define

QString Table_to_str(Table table); // user must define

class Database;

constexpr qsizetype INVALID_ID = -1;

enum class ERelation : uint8_t {
  OneToOne,
  OneToMany,
  ManyToOne,
  ManyToMany,
  PolymorphicOneToMany,
};

template <class T>
class Record;

template <class T>
concept RecordDerivedType = requires(T t) {
  requires std::same_as<std::remove_cvref_t<decltype(T::static_table)>, Table>;
  { T::sql_fields() };
  requires std::same_as<decltype(t.database()), Database*>;
  requires std::same_as<decltype(t.id()), qsizetype>;
};

template <class T>
concept RecordType = std::derived_from<T, Record<T>> && RecordDerivedType<T>;

template <class T>
concept RecordFieldDerivedType = requires(T t) {
  requires std::same_as<std::remove_cvref_t<decltype(T::static_table)>, Table>;
  // { T::sql_fields() }; // can't be evaluated at the same time of concept
  requires std::same_as<decltype(t.database()), Database*>;
  requires std::same_as<decltype(t.id()), qsizetype>;
};

template <class T>
concept RecordFieldType = std::derived_from<T, Record<T>> && RecordFieldDerivedType<T>;


} // namespace ligarium

#endif // LIGARIUM_H