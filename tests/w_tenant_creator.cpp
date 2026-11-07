#include "w_tenant_creator.h"

#include "ui_w_tenant_creator.h"

#include <QFileDialog>
#include <QPushButton>
#include <ligarium/database.h>

W_Tenant_Creator::W_Tenant_Creator(QWidget* parent, ligarium::Database& db, qsizetype id)
  : QDialog(parent)
  , m_db(db)
  , m_tenant(Tenant::read_record(db, id))
  , ui(new Ui::W_Tenant_Creator)
{
  ui->setupUi(this);

  if (id == ligarium::INVALID_ID)
    setWindowTitle(tr("Tenant Creation"));
  else
    setWindowTitle(tr("Tenant Edition"));

  populate_ui();
}

W_Tenant_Creator::~W_Tenant_Creator()
{
  delete ui;
}

void W_Tenant_Creator::populate_ui()
{
  clear();

  if (m_tenant) {
    ui->le_name->setText(m_tenant.name);
  }
}

void W_Tenant_Creator::clear()
{
  ui->le_name->clear();
}

void W_Tenant_Creator::inject_data()
{
  m_tenant.name = ui->le_name->text();
}

void W_Tenant_Creator::on_buttonBox_accepted()
{
  inject_data();
  (void)m_tenant.save_record();
  close();
}


void W_Tenant_Creator::on_buttonBox_rejected()
{
  close();
}
