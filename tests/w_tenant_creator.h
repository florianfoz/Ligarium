#ifndef W_TENANT_CREATOR_H
#define W_TENANT_CREATOR_H

#include "test_records.h"

#include <QDialog>

class Tenant;

namespace Ui
{
class W_Tenant_Creator;
}

class W_Tenant_Creator : public QDialog
{
  Q_OBJECT

public:
  explicit W_Tenant_Creator(QWidget* parent, ligarium::Database& db, qsizetype id = ligarium::INVALID_ID);
  ~W_Tenant_Creator();

  Tenant              m_tenant;
  ligarium::Database& m_db;


  void clear();
  void populate_ui();
  void inject_data();

private slots:
  void on_buttonBox_accepted();

  void on_buttonBox_rejected();

private:
  Ui::W_Tenant_Creator* ui;
};

#endif // W_TENANT_CREATOR_H
