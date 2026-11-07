#ifndef W_PROPERTY_H
#define W_PROPERTY_H

#include "ligarium_config.h"

#include <QWidget>

namespace Ui
{
class W_Property;
}

class W_Property : public QWidget
{
  Q_OBJECT

public:
  explicit W_Property(QWidget* parent, ligarium::Database& db, qsizetype id);
  ~W_Property();

  qsizetype           m_id = ligarium::INVALID_ID;
  ligarium::Database& m_db;

  void refresh();

private:
  Ui::W_Property* ui;
};

#endif // W_PROPERTY_H
