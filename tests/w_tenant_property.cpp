#include "ligarium/widget/w_picker.h"
#include "ligarium_config.h"
//

#include "ui_w_tenant_property.h"
#include "w_tenant_property.h"

#include <QFileDialog>
#include <QPushButton>
#include <ligarium/database.h>


W_Tenant_Property::W_Tenant_Property(QWidget* parent, ligarium::Database& db, qsizetype id)
  : ligarium::IRecordWidget(parent)
  , m_db(db)
  , m_rec(Tenant_Property::read_record(db, id))
  , ui(new Ui::W_Tenant_Property)
{
  ui->setupUi(this);

  load();
}

W_Tenant_Property::~W_Tenant_Property()
{
  delete ui;
}

void W_Tenant_Property::load()
{
  if (m_rec) {
    ui->dsb_share->setValue(m_rec.share_percent);
    return;
  }

  ui->dsb_share->clear();
}

void W_Tenant_Property::reset()
{
  m_rec = Tenant_Property::read_record(m_db, m_rec.id());

  load();
}

bool W_Tenant_Property::save()
{
  m_rec.share_percent = ui->dsb_share->value();
  return m_rec.save_record();
}
