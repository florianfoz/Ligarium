#ifndef W_TENANT_H
#define W_TENANT_H

#include "records.h"

class Tenant;

namespace Ui
{
class W_Tenant;
}

class W_Tenant : public ligarium::IRecordWidget
{
  Q_OBJECT

public:
  explicit W_Tenant(QWidget* parent, ligarium::Database& db, qsizetype id = ligarium::INVALID_ID);
  ~W_Tenant();

  Tenant              m_rec;
  ligarium::Database& m_db;

  void load() override;
  bool save() override;
  void reset() override;

private:
  Ui::W_Tenant* ui;
};

#endif // W_TENANT_H
