#ifndef LIGARIUM_H
#define LIGARIUM_H

#include <enumlite/enumlite_backend_qt.h>
#define ENUMLITE_DEFAULT_BACKEND enumlite::qt_backend
#include <enumlite/enumlite.h>
//
#include <QSqlDatabase>

#ifndef LIGARIUM_TABLE_TYPE
#error LIGARIUM_TABLE_TYPE macro must be defined to use Ligarium!
#define LIGARIUM_TABLE_TYPE enum class _T_ : uint8_t {};
#endif


namespace Ligarium
{

class Database;

constexpr qsizetype INVALID_ID = -1;

using Table = LIGARIUM_TABLE_TYPE;

QString Table_to_str(Table table);

DEFINE_ENUM(ERelation, uint8_t,     //
            OneToOne, 1,            //
            OneToMany, 2,           //
            ManyToOne, 3,           //
            ManyToMany, 4,          //
            PolymorphicOneToMany, 5 //
)

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


} // namespace Ligarium

#endif // LIGARIUM_H