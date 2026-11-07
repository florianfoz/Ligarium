#include "w_property_creator.h"

#include "ligarium_config.h"
#include "test_records.h"
#include "ui_w_property_creator.h"

#include <QFileDialog>
#include <QPushButton>

W_Property_Creator::W_Property_Creator(QWidget* parent, ligarium::Database& db, qsizetype id)
  : QDialog(parent)
  , property(Property::read_record(db, id))
  , m_db(db)
  , ui(new Ui::W_Property_Creator)
{
  ui->setupUi(this);

  if (id == -1)
    setWindowTitle(tr("Property Creation"));
  else
    setWindowTitle(tr("Property Edition"));

  ui->tenant->refresh(m_db, ligarium::Table::Tenant);


  populate_ui();
}

W_Property_Creator::~W_Property_Creator()
{
  delete ui;
}

void W_Property_Creator::populate_ui()
{
  clear();

  if (property) {
    ui->le_name->setText(property.name);
    ui->dsb_surface_size->setValue(property.surface);
    ui->tenant->set_record(m_db, ligarium::Table::Tenant, property.tenant.id());

    if (auto ownership = Tenant_Property::)
      ui->landlords->set_selection(m_db, ligarium::Table::Landlord, property.landlords.ids());
  }
}

void W_Property_Creator::clear()
{
  ui->le_name->clear();
  ui->dsb_surface_size->setValue(0);
}

void W_Property_Creator::inject_data()
{
  property.name    = ui->le_name->text();
  property.surface = ui->dsb_surface_size->value();
  property.tenant.set_id(ui->tenant->id());
  property.landlords.set_ids(ui->landlords->selection());
}

void W_Property_Creator::on_buttonBox_accepted()
{
  inject_data();
  (void)property.save_record();
  close();
}


void W_Property_Creator::on_buttonBox_rejected()
{
  close();
}
