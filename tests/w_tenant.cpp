#include "ligarium/widget/w_picker.h"
#include "ligarium_config.h"
//

#include "ui_w_tenant.h"
#include "w_tenant.h"

#include <QFileDialog>
#include <QPushButton>
#include <ligarium/database.h>

W_Tenant::W_Tenant(QWidget* parent, ligarium::Database& db, qsizetype id)
  : ligarium::IRecordWidget(parent)
  , m_db(db)
  , m_rec(Tenant::read_record(db, id))
  , ui(new Ui::W_Tenant)
{
  ui->setupUi(this);

  if (id == ligarium::INVALID_ID)
    setWindowTitle(tr("Tenant Creation"));
  else
    setWindowTitle(tr("Tenant Edition"));

  load();
}

W_Tenant::~W_Tenant()
{
  delete ui;
}


void W_Tenant::load()
{
  if (m_rec) {
    ui->le_name->setText(m_rec.name);
    const auto       recs = Tenant_Property::read_associations(m_db, m_rec.id(), &Tenant_Property::tenant);
    QList<qsizetype> properties;
    properties.reserve(recs.size());
    for (const auto& rec : recs) properties.emplace_back(rec.property.id());

    ui->properties->set_selection(m_db, ligarium::Table::Property, properties);
    return;
  }

  ui->le_name->clear();
  ui->properties->clear();
}
bool W_Tenant::save()
{
  m_rec.name   = ui->le_name->text();
  auto success = m_rec.save_record();

  const auto       tenant_id = m_rec.id();
  QList<qsizetype> tps;
  tps.reserve(ui->properties->selection().size());
  for (auto property_id : ui->properties->selection()) {
    auto tp = Tenant_Property::create_record(m_db);
    tp.tenant.set_id(tenant_id);
    tp.property.set_id(property_id);
    (void)tp.save_record();

    tps.emplace_back(tp.id());
  }

  auto sync =
      Tenant_Property::sync_associations(m_db, m_rec.id(), &Tenant_Property::tenant, tps, &Tenant_Property::property);
  if (ui->properties->selection().empty()) sync = true;
  return success && sync;
}
void W_Tenant::reset()
{
  m_rec = Tenant::read_record(m_db, m_rec.id());
  load();
}
