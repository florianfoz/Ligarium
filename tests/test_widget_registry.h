#ifndef TEST_WIDGET_REGISTRY_H
#define TEST_WIDGET_REGISTRY_H

#include "ligarium_config.h"
//

#include <QTest>
#include <QWidget>
#include <ligarium/widget_registry.h>

class PropertyWidget : public ligarium::IRecordWidget
{
  Q_OBJECT
public:
  explicit PropertyWidget(QWidget* parent, ligarium::Database& db, qsizetype id)
    : ligarium::IRecordWidget(parent)
    , m_db(&db)
    , m_id(id)
  {
  }

  void load() override
  {
  }
  bool save() override
  {
    return true;
  }
  void reset() override
  {
  }

  [[nodiscard]]
  qsizetype id() const noexcept
  {
    return m_id;
  }

private:
  qsizetype           m_id = ligarium::INVALID_ID;
  ligarium::Database* m_db = nullptr;
};

#endif // TEST_WIDGET_REGISTRY_H