#include "w_property.h"

#include "test_records.h"
#include "ui_w_property.h"
#include "w_property_creator.h"

#include <QMessageBox>
#include <QRegularExpression>
#include <QSql>
#include <QSqlQuery>


W_Property::W_Property(QWidget* parent, ligarium::Database& db, qsizetype id)
  : QWidget(parent)
  , m_id(id)
  , m_db(db)
  , ui(new Ui::W_Property)
{
  assert(id != ligarium::INVALID_ID && "No instance on invalid id");
  ui->setupUi(this);

  refresh();
}

void W_Property::refresh()
{
  auto property = Property::read_record(m_db, m_id);
  ui->l_name->setText(property.name);
  ui->l_surface_size->setText(QStringLiteral("%1 m³").arg(property.surface));
  ui->tenant->set_record(m_db, ligarium::Table::Tenant, property.tenant.id());
  ui->tenant->set_readonly(true);
}

W_Property::~W_Property()
{
  delete ui;
}
