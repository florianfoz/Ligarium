#include "ligarium/widget/w_many_rec_elem.h"

#include "ligarium/database.h"
#include "ligarium/widget_registry.h"
#include "ui_w_many_rec_elem.h"

W_Many_Rec_Elem::W_Many_Rec_Elem(QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::W_Many_Rec_Elem)
{
  ui->setupUi(this);
}

W_Many_Rec_Elem::~W_Many_Rec_Elem()
{
  delete ui;
  hide_tooltip();
}


void W_Many_Rec_Elem::refresh(ligarium::Database& db, ligarium::Table _rec_table)
{
  m_db      = &db;
  rec_table = _rec_table;
}

void W_Many_Rec_Elem::set_record(ligarium::Database& db, ligarium::Table _rec_table, qsizetype _id)
{
  m_db      = &db;
  rec_table = _rec_table;
  id        = _id;

  auto text = ligarium::WidgetRegistry::dump_record(db, rec_table, id);
  ui->le_name->setText(text);

  if (id == ligarium::INVALID_ID) clear();
}

void W_Many_Rec_Elem::enterEvent(QEnterEvent* event)
{
  hide_tooltip();

  if (rec_table != ligarium::Table::NONE && id != ligarium::INVALID_ID)
    tooltip = ligarium::WidgetRegistry::new_widget_record_view(rec_table, id);
}

void W_Many_Rec_Elem::hide_tooltip()
{
  if (tooltip) {
    tooltip->deleteLater();
    tooltip = nullptr;
  }
}

void W_Many_Rec_Elem::clear()
{
  ui->le_name->setText(QObject::tr("Click to select a record"));
  id = ligarium::INVALID_ID;
  hide_tooltip();
}


void W_Many_Rec_Elem::leaveEvent(QEvent* event)
{
  hide_tooltip();
}


void W_Many_Rec_Elem::on_b_delete_clicked()
{
  emit signal_deleted(id);
}
