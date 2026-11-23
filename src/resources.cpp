
#include "ligarium/resources.h"

#include <QResource>

// Force call inside the global namespace
static void initLigariumResources()
{
  Q_INIT_RESOURCE(icons);
}

namespace ligarium
{
void init_resources()
{
  initLigariumResources();
}
} // namespace ligarium
