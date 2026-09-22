#ifndef POLYMORPHIC_LINK_H
#define POLYMORPHIC_LINK_H

#include "record.h"

#include <QList>

namespace Ligarium
{


template <RecordType TARGET>
class PolymorphicLink
{
public:
  QList<qsizetype> ids;

  [[nodiscard]] bool empty() const noexcept
  {
    return ids.isEmpty();
  }

  [[nodiscard]] qsizetype size() const noexcept
  {
    return ids.size();
  }

  [[nodiscard]] QList<TARGET> get(Database& db) const
  {
    QList<TARGET> result;
    result.reserve(ids.size());

    for (const qsizetype id : ids) result.append(read_record<TARGET>(db, id));

    return result;
  }
};

} // namespace Ligarium


#endif // POLYMORPHIC_LINK_H