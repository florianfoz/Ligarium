#include "ligarium/widget/w_many_elem.h"

#include "ligarium/database.h"
#include "ligarium/widget_registry.h"
#include "ui_w_ligarium_many_elem.h"

namespace ligarium
{

W_Many_Elem::W_Many_Elem(QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::W_Ligarium_Many_Elem)
{
  ui->setupUi(this);
}

W_Many_Elem::~W_Many_Elem()
{
  delete ui;
}


void W_Many_Elem::refresh(Database& db, Table table)
{
  clear();
  m_db    = &db;
  m_table = table;
  auto* w = WidgetRegistry::new_widget_record(table, m_id, *m_db);
  if (w) {
    ui->stackedWidget->setCurrentIndex(1);
    ui->l_view->addWidget(w);
  } else {
    ui->stackedWidget->setCurrentIndex(0);
    auto* name = ui->stackedWidget->currentWidget()->findChild<QLabel*>("le_name");

    auto text = WidgetRegistry::dump_record(*m_db, m_table, m_id);
    name->setText(text);
  }
}

void W_Many_Elem::set_record(Database& db, Table table, qsizetype id)
{
  clear();

  m_db    = &db;
  m_table = table;
  m_id    = id;

  auto text = WidgetRegistry::dump_record(*m_db, m_table, m_id);
  ui->le_name->setText(text);

  if (m_id == INVALID_ID) clear();
}

void W_Many_Elem::clear()
{
  ui->le_name->setText(QObject::tr("Click to select a record"));
  m_id = INVALID_ID;

  while (QLayoutItem* item = ui->l_view->takeAt(0)) {
    if (QWidget* widget = item->widget()) widget->deleteLater();
    delete item;
  }

  ui->stackedWidget->setCurrentIndex(0);
}


void W_Many_Elem::on_b_delete_clicked()
{
  emit signal_deleted(m_id);
}

} // namespace ligarium