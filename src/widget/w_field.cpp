#include "ligarium/widget/w_field.h"

//
#include "ligarium/widget/w_picker.h"
#include "ligarium/widget_registry.h"
#include "ui_w_ligarium_field.h"

ligarium::W_Field::W_Field(QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::W_Ligarium_Field)
{
  ui->setupUi(this);
}

ligarium::W_Field::~W_Field()
{
  delete ui;
}

void ligarium::W_Field::set_readonly(bool _readonly)
{
  readonly = _readonly;
}


void ligarium::W_Field::refresh(ligarium::Database& db, ligarium::Table _rec_table)
{
  m_db    = &db;
  m_table = _rec_table;
}

void ligarium::W_Field::set_record(ligarium::Database& db, ligarium::Table _rec_table, qsizetype _id)
{
  m_db    = &db;
  m_table = _rec_table;
  m_id    = _id;

  if (m_table != ligarium::Table::NONE && m_id != ligarium::INVALID_ID) {
    ui->le_name->setText(ligarium::WidgetRegistry::dump_record(*m_db, m_table, m_id));
  }

  if (m_id == ligarium::INVALID_ID) clear();
}

void ligarium::W_Field::enterEvent(QEnterEvent* event)
{
  hide_tooltip();

  if (m_table != ligarium::Table::NONE && m_id != ligarium::INVALID_ID) {
    tooltip = ligarium::WidgetRegistry::new_widget_record_view(*m_db, m_table, m_id);
  }
}

void ligarium::W_Field::hide_tooltip()
{
  if (tooltip) {
    tooltip->deleteLater();
    tooltip = nullptr;
  }
}

void ligarium::W_Field::clear()
{
  ui->le_name->setText(QObject::tr("Click to select a record"));
  m_id = ligarium::INVALID_ID;
  hide_tooltip();
}


void ligarium::W_Field::leaveEvent(QEvent* event)
{
  hide_tooltip();
}

void ligarium::W_Field::mousePressEvent(QMouseEvent* event)
{
  assert(m_table != ligarium::Table::NONE && "Invalid record table");

  if (readonly) return;

  hide_tooltip();

  auto* w = new W_Picker(this, *m_db);
  w->refresh(*m_db, m_table);
  connect(w, &W_Picker::signal_on_selected, this, &ligarium::W_Field::set_record);
  connect(w, &QDialog::rejected, [this]() { clear(); });
  w->setModal(true);
  w->exec();
}
