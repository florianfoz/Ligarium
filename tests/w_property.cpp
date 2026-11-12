#include "w_property.h"

#include "ligarium_config.h"
#include "records.h"
#include "ui_w_property.h"

#include <QFileDialog>
#include <QPushButton>


W_property::W_property(QWidget* parent, ligarium::Database& db, qsizetype id)
  : ligarium::IRecordWidget(parent)
  , m_rec(Property::read_record(db, id))
  , m_db(db)
  , ui(new Ui::W_property)
{
  ui->setupUi(this);

  if (id == -1)
    setWindowTitle(tr("Property Creation"));
  else
    setWindowTitle(tr("Property Edition"));

  load();
}

W_property::~W_property()
{
  delete ui;
}

void W_property::load()
{
  if (m_rec) {
    ui->le_name->setText(m_rec.name);
    ui->dsb_surface_size->setValue(m_rec.surface);
    return;
  }

  ui->le_name->clear();
  ui->dsb_surface_size->clear();
}

void W_property::reset()
{
  m_rec = Property::read_record(m_db, m_rec.id());
  load();
}

bool W_property::save()
{
  m_rec.name    = ui->le_name->text();
  m_rec.surface = ui->dsb_surface_size->value();
  return m_rec.save_record();
}
