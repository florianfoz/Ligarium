#ifndef LIGARIUM_CONFIG_H
#define LIGARIUM_CONFIG_H

#include <QString>

namespace ligarium
{

// user defined table
enum class Table : uint8_t {
  NONE = 0, // mandatory
  Property,
  Tenant,
  Tenant_Property,
  Attachment,
};

[[nodiscard]]
inline QString Table_to_str(Table table)
{
  switch (table) {
  case Table::NONE:            return "NONE";
  case Table::Property:        return "Property";
  case Table::Tenant:          return "Tenant";
  case Table::Tenant_Property: return "Tenant_Property";
  case Table::Attachment:      return "Attachment";
  default:                     return {};
  }
}

} // namespace ligarium

// must be included before user ligarium_config.h file
#include <ligarium/ligarium.h>

#endif // LIGARIUM_CONFIG_H
