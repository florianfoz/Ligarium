#ifndef TEST_RECORDS_H
#define TEST_RECORDS_H

#include "ligarium_config.h"
//
#include <ligarium/field.h>
#include <ligarium/record.h>


class Property final : public ligarium::Record<Property>
{
public:
  using ligarium::Record<Property>::Record;

  static constexpr auto static_table = ligarium::Table::Property;

  QString name;

  [[nodiscard]]
  QString dump() const override
  {
    return name;
  }

  static constexpr auto sql_fields()
  {
    return std::tuple{
        ligarium::field(u"name", &Property::name) //
    };
  }

  friend bool operator==(const Property& lhs, const Property& rhs)
  {
    return lhs.id() == rhs.id() && lhs.name == rhs.name;
  }
};


class Tenant final : public ligarium::Record<Tenant>
{
public:
  using ligarium::Record<Tenant>::Record;

  static constexpr auto static_table = ligarium::Table::Tenant;

  QString name;

  [[nodiscard]]
  QString dump() const override
  {
    return name;
  }

  static constexpr auto sql_fields()
  {
    return std::tuple{
        ligarium::field(u"name", &Tenant::name) //
    };
  }

  friend bool operator==(const Tenant& lhs, const Tenant& rhs)
  {
    return lhs.id() == rhs.id() && lhs.name == rhs.name;
  }
};


class Attachment final : public ligarium::Record<Attachment>
{
public:
  using ligarium::Record<Attachment>::Record;

  static constexpr auto static_table = ligarium::Table::Attachment;

  ligarium::Table table = ligarium::Table::Property;

  qsizetype col_id = ligarium::INVALID_ID;
  QString   path;

  [[nodiscard]]
  QString dump() const override
  {
    return path;
  }

  static constexpr auto sql_fields()
  {
    return std::tuple{
        ligarium::field(u"table", &Attachment::table),   //
        ligarium::field(u"col_id", &Attachment::col_id), //
        ligarium::field(u"path", &Attachment::path)      //
    };
  }

  friend bool operator==(const Attachment& lhs, const Attachment& rhs)
  {
    return lhs.id() == rhs.id() && lhs.table == rhs.table && lhs.col_id == rhs.col_id && lhs.path == rhs.path;
  }
};


#endif // TEST_RECORDS_H