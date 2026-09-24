#ifndef TEST_RECORDS_H
#define TEST_RECORDS_H

#include "ligarium_config.h"
//
#include "field.h"
#include "record.h"

#define DEFINE_FIELD(_type, _name) _type _name;


#define DEFINE_RECORD(_name, _fields, _dump, _other)                                                                   \
  class _name final : public Ligarium::Record<_name>                                                                   \
  {                                                                                                                    \
  public:                                                                                                              \
    using Ligarium::Record<_name>::Record;                                                                             \
                                                                                                                       \
    static constexpr Ligarium::Table static_table = Ligarium::Table::_name;                                            \
                                                                                                                       \
    QString name;                                                                                                      \
                                                                                                                       \
    [[nodiscard]]                                                                                                      \
    QString dump() const override                                                                                      \
    {                                                                                                                  \
      return _dump;                                                                                                    \
    }                                                                                                                  \
                                                                                                                       \
    static constexpr auto sql_fields()                                                                                 \
    {                                                                                                                  \
      return std::tuple{                                                                                               \
          Ligarium::field(u"name", &_name::name) /**/                                                                  \
      };                                                                                                               \
    }                                                                                                                  \
                                                                                                                       \
    friend bool operator==(const _name& lhs, const _name& rhs)                                                         \
    {                                                                                                                  \
      return lhs.id() == rhs.id();                                                                                     \
    }                                                                                                                  \
  };


class Property final : public Ligarium::Record<Property>
{
public:
  using Ligarium::Record<Property>::Record;

  static constexpr Ligarium::Table static_table = Ligarium::Table::Property;

  QString name;

  [[nodiscard]]
  QString dump() const override
  {
    return name;
  }

  static constexpr auto sql_fields()
  {
    return std::tuple{
        Ligarium::field(u"name", &Property::name) //
    };
  }

  friend bool operator==(const Property& lhs, const Property& rhs)
  {
    return lhs.id() == rhs.id() && lhs.name == rhs.name;
  }
};


class Tenant final : public Ligarium::Record<Tenant>
{
public:
  using Ligarium::Record<Tenant>::Record;

  static constexpr Ligarium::Table static_table = Ligarium::Table::Tenant;

  QString name;

  [[nodiscard]]
  QString dump() const override
  {
    return name;
  }

  static constexpr auto sql_fields()
  {
    return std::tuple{
        Ligarium::field(u"name", &Tenant::name) //
    };
  }

  friend bool operator==(const Tenant& lhs, const Tenant& rhs)
  {
    return lhs.id() == rhs.id() && lhs.name == rhs.name;
  }
};


class Attachment final : public Ligarium::Record<Attachment>
{
public:
  using Ligarium::Record<Attachment>::Record;

  static constexpr Ligarium::Table static_table = Ligarium::Table::Attachment;

  Ligarium::Table table = Ligarium::Table::Property;

  qsizetype col_id = Ligarium::INVALID_ID;
  QString   path;

  [[nodiscard]]
  QString dump() const override
  {
    return path;
  }

  static constexpr auto sql_fields()
  {
    return std::tuple{
        Ligarium::field(u"table", &Attachment::table),   //
        Ligarium::field(u"col_id", &Attachment::col_id), //
        Ligarium::field(u"path", &Attachment::path)      //
    };
  }

  friend bool operator==(const Attachment& lhs, const Attachment& rhs)
  {
    return lhs.id() == rhs.id() && lhs.table == rhs.table && lhs.col_id == rhs.col_id && lhs.path == rhs.path;
  }
};


#endif // TEST_RECORDS_H