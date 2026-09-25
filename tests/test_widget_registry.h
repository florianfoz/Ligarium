#ifndef TEST_WIDGET_REGISTRY_H
#define TEST_WIDGET_REGISTRY_H

#include "ligarium_config.h"
//

#include <QTest>
#include <QWidget>
#include <ligarium/widget_registry.h>

class PropertyWidget : public QWidget
{
  Q_OBJECT
public:
  explicit PropertyWidget(QWidget* parent = nullptr, qsizetype id = ligarium::INVALID_ID)
    : QWidget(parent)
    , m_id(id)
  {
  }

  [[nodiscard]]
  qsizetype id() const noexcept
  {
    return m_id;
  }

private:
  qsizetype m_id = ligarium::INVALID_ID;
};

#endif // TEST_WIDGET_REGISTRY_H