#ifndef W_TENANT_H
#define W_TENANT_H

#include "ligarium_config.h"

#include <QWidget>


namespace Ui
{
class W_Tenant;
}

class W_Tenant : public QWidget
{
  Q_OBJECT

public:
  explicit W_Tenant(QWidget* parent, ligarium::Database& db, qsizetype id);
  ~W_Tenant();

  qsizetype           m_id = ligarium::INVALID_ID;
  ligarium::Database& m_db;

  bool check_data();
  void refresh();
  void select(bool selected);

private:
  Ui::W_Tenant* ui;
};

#endif // W_TENANT_H
