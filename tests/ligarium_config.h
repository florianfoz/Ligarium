#ifndef LIGARIUM_CONFIG_H
#define LIGARIUM_CONFIG_H

#include <enumlite/enumlite_backend_qt.h>
#define ENUMLITE_DEFAULT_BACKEND enumlite::qt_backend
#include <enumlite/enumlite.h>

namespace Ligarium
{

DEFINE_ENUM(Table, int,   // user defined table
            Property, 1,  //
            Tenant, 2,    //
            Attachment, 3 //
)

} // namespace Ligarium


#include <ligarium.h>

#endif // LIGARIUM_CONFIG_H
