#include "ligarium/widget/w_selectable.h"

#include "ligarium/widget_registry.h"
#include "ui_w_ligarium_selectable.h"

#include <QDialog>

ligarium::W_Selectable::W_Selectable(ligarium::Database& db, ligarium::Table table, qsizetype id)
  : QWidget(nullptr)
  , m_table(table)
  , m_id(id)
  , m_db(db)
  , ui(new Ui::W_Ligarium_Selectable)
{
  ui->setupUi(this);

  QWidget* w = ligarium::WidgetRegistry::new_widget_record_view(m_db, m_table, m_id);

  connect(this, &ligarium::W_Selectable::signal_selected,
          [this](ligarium::Database&, ligarium::Table, qsizetype _id) { select(_id == this->m_id); });

  ui->layout->addWidget(w);

  ui->b_delete->hide();
  ui->b_edit->hide();
}

ligarium::W_Selectable::~W_Selectable()
{
  delete ui;
}


void ligarium::W_Selectable::select(bool selected)
{
  if (selected) {
    ui->f_selection->setStyleSheet("#f_selection { border: 1px solid blue; }");
  } else {
    ui->f_selection->setStyleSheet("");
  }
}


void ligarium::W_Selectable::mousePressEvent(QMouseEvent* event)
{
  emit signal_selected(m_db, m_table, m_id);
}

void ligarium::W_Selectable::on_b_edit_clicked()
{
  QDialog* w = ligarium::WidgetRegistry::new_widget_record_creator(m_db, m_table, m_id);
  w->setModal(true);
  w->exec();
}

void ligarium::W_Selectable::on_b_delete_clicked()
{
  (void)ligarium::delete_record(m_db, m_table, m_id);
}

void ligarium::W_Selectable::enterEvent(QEnterEvent* event)
{
  ui->b_delete->show();
  ui->b_edit->show();
}

void ligarium::W_Selectable::leaveEvent(QEvent* event)
{
  ui->b_delete->hide();
  ui->b_edit->hide();
}
