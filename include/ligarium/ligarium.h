#ifndef LIGARIUM_H
#define LIGARIUM_H

#include <QSqlDatabase>
#include <QString>
#include <QWidget>

#define CRITICAL_QUERY(_query, msg)                                                                                    \
  qCritical() << "ligarium query failed: " << __PRETTY_FUNCTION__ << ": " << msg << ":" << _query.lastError().text()   \
              << "\n  Last query: " << _query.lastQuery();

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
  PolymorphicManyToOne,
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

enum class WidgetMode : uint8_t { ReadOnly, ReadWrite };

class IRecordWidget;

void setWidgetMode(QWidget* parent, WidgetMode mode);
bool exec_widget_dialog(QWidget* parent, IRecordWidget* widget, const QString& title = QString());


template <typename T>
concept Is_SQL_Link = requires { typename T::Is_Link; };

template <typename T>
concept Is_SQL_Direct_Link_Poly = Is_SQL_Link<T> && (T::RELATION == ERelation::PolymorphicManyToOne);

template <typename T>
concept Is_SQL_Direct_Link = Is_SQL_Link<T>
                             && (T::RELATION == ERelation::OneToOne || T::RELATION == ERelation::ManyToOne
                                 || T::RELATION == ERelation::PolymorphicManyToOne);

template <typename T>
concept Is_SQL_Indirect_Link = Is_SQL_Link<T> && !Is_SQL_Direct_Link<T>;

template <typename T>
concept Is_SQL_Field = requires { typename T::Is_Field; };


} // namespace ligarium

#endif // LIGARIUM_H