#include "field.h"

#include "database.h"

void Ligarium::emit_on_record_saved(Ligarium::Database& db, Table table)
{
  emit db.signal_db_updated(table);
}

template <>
struct Ligarium::SqlConverter<QString> {
  static QString from_sql(const QVariant& value)
  {
    return value.toString();
  }

  static QVariant to_sql(const QString& value)
  {
    return value;
  }
};

template <>
struct Ligarium::SqlConverter<int> {
  static int from_sql(const QVariant& value)
  {
    return value.toInt();
  }

  static QVariant to_sql(int value)
  {
    return value;
  }
};

template <>
struct Ligarium::SqlConverter<qsizetype> {
  static qsizetype from_sql(const QVariant& value)
  {
    return value.toInt();
  }

  static QVariant to_sql(qsizetype value)
  {
    return value;
  }
};

template <>
struct Ligarium::SqlConverter<float> {
  static float from_sql(const QVariant& value)
  {
    return value.toFloat();
  }

  static QVariant to_sql(float value)
  {
    return value;
  }
};

template <>
struct Ligarium::SqlConverter<double> {
  static double from_sql(const QVariant& value)
  {
    return value.toDouble();
  }

  static QVariant to_sql(double value)
  {
    return value;
  }
};

template <>
struct Ligarium::SqlConverter<QDate> {
  static QDate from_sql(const QVariant& value)
  {
    return value.toDate();
  }

  static QVariant to_sql(const QDate& value)
  {
    return value;
  }
};

template <>
struct Ligarium::SqlConverter<QDateTime> {
  static QDateTime from_sql(const QVariant& value)
  {
    return value.toDateTime();
  }

  static QVariant to_sql(const QDateTime& value)
  {
    return value;
  }
};


template <typename T>
  requires std::is_enum_v<T>
struct Ligarium::SqlConverter<T> {
  static T from_sql(const QVariant& value)
  {
    return static_cast<T>(value.toInt());
  }

  static QVariant to_sql(T value)
  {
    return static_cast<int>(value);
  }
};