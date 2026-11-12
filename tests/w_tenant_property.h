#ifndef W_TENANT_PROPERTY_H
#define W_Tenant_Property_H

#include "records.h"

#include <QDialog>

class Tenant_Property;

namespace Ui
{
class W_Tenant_Property;
}

class W_Tenant_Property : public ligarium::IRecordWidget
{
  Q_OBJECT

public:
  explicit W_Tenant_Property(QWidget* parent, ligarium::Database& db, qsizetype id = ligarium::INVALID_ID);
  ~W_Tenant_Property();

  ligarium::Database& m_db;
  Tenant_Property     m_rec;

  void load() override;
  bool save() override;
  void reset() override;

private:
  Ui::W_Tenant_Property* ui;
};

#endif // W_TENANT_PROPERTY_H
