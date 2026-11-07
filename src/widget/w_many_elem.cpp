#include "ligarium/widget/w_many_elem.h"

#include "ligarium/database.h"
#include "ligarium/widget_registry.h"
#include "ui_w_ligarium_many_elem.h"

ligarium::W_Many_Elem::W_Many_Elem(QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::W_Ligarium_Many_Elem)
{
  ui->setupUi(this);
}

ligarium::W_Many_Elem::~W_Many_Elem()
{
  delete ui;
  hide_tooltip();
}


void ligarium::W_Many_Elem::refresh(ligarium::Database& db, ligarium::Table table)
{
  m_db    = &db;
  m_table = table;
}

void ligarium::W_Many_Elem::set_record(ligarium::Database& db, ligarium::Table table, qsizetype id)
{
  m_db    = &db;
  m_table = table;
  m_id    = id;

  auto text = ligarium::WidgetRegistry::dump_record(*m_db, m_table, m_id);
  ui->le_name->setText(text);

  if (m_id == ligarium::INVALID_ID) clear();
}

void ligarium::W_Many_Elem::enterEvent(QEnterEvent* event)
{
  hide_tooltip();

  if (m_table != ligarium::Table::NONE && m_id != ligarium::INVALID_ID)
    m_tooltip = ligarium::WidgetRegistry::new_widget_record_view(*m_db, m_table, m_id);
}

void ligarium::W_Many_Elem::hide_tooltip()
{
  if (m_tooltip) {
    m_tooltip->deleteLater();
    m_tooltip = nullptr;
  }
}

void ligarium::W_Many_Elem::clear()
{
  ui->le_name->setText(QObject::tr("Click to select a record"));
  m_id = ligarium::INVALID_ID;
  hide_tooltip();
}


void ligarium::W_Many_Elem::leaveEvent(QEvent* event)
{
  hide_tooltip();
}


void ligarium::W_Many_Elem::on_b_delete_clicked()
{
  emit signal_deleted(m_id);
}
