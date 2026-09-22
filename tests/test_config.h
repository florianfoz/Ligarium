#ifndef TEST_CONFIG_H
#define TEST_CONFIG_H

#include <enumlite/enumlite_backend_qt.h>
#define ENUMLITE_DEFAULT_BACKEND enumlite::qt_backend
#include <enumlite/enumlite.h>

DEFINE_ENUM(ApplicationTable, int, //
            Property, 1,           //
            Tenant, 2,             //
            Attachment, 3          //
)

#define LIGARIUM_TABLE_TYPE ApplicationTable

namespace Ligarium
{

inline QString Table_to_str(ApplicationTable table)
{
  return ApplicationTable_to_str(table);
}

} // namespace Ligarium


#include <ligarium.h>

#endif // TEST_CONFIG_H
