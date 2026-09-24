#ifndef LIGARIUM_CONFIG_H
#define LIGARIUM_CONFIG_H

#include <QString>

namespace Ligarium
{

// user defined table
enum class Table : uint8_t {
  Property,
  Tenant,
  Attachment,
};

[[nodiscard]]
QString Table_to_str(Table table)
{
  switch (table) {
  case Table::Property:   return "Property";
  case Table::Tenant:     return "Tenant";
  case Table::Attachment: return "Attachment";
  }
}

} // namespace Ligarium

// must be included always before user ligarium_config.h file
#include <ligarium.h>

#endif // LIGARIUM_CONFIG_H
