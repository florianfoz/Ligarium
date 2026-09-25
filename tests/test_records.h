#ifndef TEST_RECORDS_H
#define TEST_RECORDS_H

#include "ligarium_config.h"
//
#include <ligarium/field.h>
#include <ligarium/record.h>
#include <ligarium/widget_registry.h>


class Property final : public ligarium::Record<Property>
{
public:
  static constexpr auto static_table = ligarium::Table::Property;

  QString name;
  float   surface;

  [[nodiscard]]
  QString dump() const override
  {
    return QStringLiteral("property: %1, %2 m²").arg(name).arg(surface);
  }

  static constexpr auto sql_fields()
  {
    return std::tuple{
        ligarium::field(u"name", &Property::name),       //
        ligarium::field(u"surface", &Property::surface), //
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
  static constexpr auto static_table = ligarium::Table::Tenant;

  QString name;

  [[nodiscard]]
  QString dump() const override
  {
    return QStringLiteral("tenant: %1").arg(name);
  }

  static constexpr auto sql_fields()
  {
    return std::tuple{
        ligarium::field(u"name", &Tenant::name), //
    };
  }

  friend bool operator==(const Tenant& lhs, const Tenant& rhs)
  {
    return lhs.id() == rhs.id() && lhs.name == rhs.name;
  }
};

class Tenant_Property : public ligarium::Record<Tenant_Property>
{
public:
  static constexpr auto static_table = ligarium::Table::Tenant_Property;

  ligarium::Link<Property, ligarium::ERelation::ManyToOne> property_id;
  ligarium::Link<Tenant, ligarium::ERelation::ManyToOne>   tenant_id;
  double                                                   share_percent = 0.0F;

  [[nodiscard]] QString dump() const override
  {
    auto tenant   = tenant_id.get(*database());
    auto property = property_id.get(*database());

    return QStringLiteral("%1 in %2").arg(tenant.dump(), property.dump());
  }

  static constexpr auto sql_fields()
  {
    return std::tuple{
        ligarium::link_ManyToOne(u"property_id", &Tenant_Property::property_id),
        ligarium::link_ManyToOne(u"tenant_id", &Tenant_Property::tenant_id),
        ligarium::field(u"share_percent", &Tenant_Property::share_percent),
    };
  }
};


class Attachment final : public ligarium::Record<Attachment>
{
public:
  static constexpr auto static_table = ligarium::Table::Attachment;

  QString   target_table = ligarium::Table_to_str(ligarium::Table::Property);
  qsizetype target_id    = ligarium::INVALID_ID;
  QString   path;

  [[nodiscard]]
  QString dump() const override
  {
    return QStringLiteral("attachment: %1 in %2, id: %3").arg(path).arg(target_table).arg(target_id);
  }

  static constexpr auto sql_fields()
  {
    return std::tuple{
        ligarium::field(u"target_table", &Attachment::target_table), //
        ligarium::field(u"target_id", &Attachment::target_id),       //
        ligarium::field(u"path", &Attachment::path)                  //
    };
  }

  friend bool operator==(const Attachment& lhs, const Attachment& rhs)
  {
    return lhs.id() == rhs.id() && lhs.target_table == rhs.target_table && lhs.target_id == rhs.target_id
           && lhs.path == rhs.path;
  }
};


#endif // TEST_RECORDS_H