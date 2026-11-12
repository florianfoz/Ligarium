#include "ligarium/widget/w_selectable.h"

#include "ligarium/widget_registry.h"
#include "ui_w_ligarium_selectable.h"

#include <QDialog>

namespace ligarium
{

W_Selectable::W_Selectable(Database& db, Table table, qsizetype id)
  : QWidget(nullptr)
  , m_table(table)
  , m_id(id)
  , m_db(db)
  , ui(new Ui::W_Ligarium_Selectable)
{
  ui->setupUi(this);

  QWidget* w = WidgetRegistry::new_widget_record(m_table, m_id, m_db);
  setWidgetMode(w, WidgetMode::ReadOnly);
  ui->layout->addWidget(w);

  connect(this, &W_Selectable::signal_selected, [this](Table, qsizetype _id) { select(_id == this->m_id); });

  ui->b_delete->hide();
  ui->b_edit->hide();
}

W_Selectable::~W_Selectable()
{
  delete ui;
}


void W_Selectable::select(bool selected)
{
  if (selected) {
    ui->f_selection->setStyleSheet("#f_selection { border: 1px solid blue; }");
  } else {
    ui->f_selection->setStyleSheet("");
  }
}


void W_Selectable::mousePressEvent(QMouseEvent* event)
{
  emit signal_selected(m_table, m_id);
}

void W_Selectable::on_b_edit_clicked()
{
  auto* w = WidgetRegistry::new_widget_record(m_table, m_id, m_db);
  exec_widget_dialog(parentWidget(), w);
}

void W_Selectable::on_b_delete_clicked()
{
  (void)delete_record(m_db, m_table, m_id);
}

void W_Selectable::enterEvent(QEnterEvent* event)
{
  ui->b_delete->show();
  ui->b_edit->show();
}

void W_Selectable::leaveEvent(QEvent* event)
{
  ui->b_delete->hide();
  ui->b_edit->hide();
}

} // namespace ligarium