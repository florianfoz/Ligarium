#include "w_tenant.h"

#include "test_records.h"
#include "ui_w_tenant.h"
#include "w_tenant_creator.h"

#include <QMessageBox>
#include <QSql>
#include <QSqlQuery>


W_Tenant::W_Tenant(QWidget* parent, ligarium::Database& db, qsizetype id)
  : QWidget(parent)
  , m_db(db)
  , m_id(id)
  , ui(new Ui::W_Tenant)
{
  assert(id != ligarium::INVALID_ID && "No instance on invalid id");
  ui->setupUi(this);

  refresh();
}
void W_Tenant::refresh()
{
  auto tenant = Tenant::read_record(m_db, m_id);

  ui->l_name->setText(tenant.name);
}

W_Tenant::~W_Tenant()
{
  delete ui;
}
