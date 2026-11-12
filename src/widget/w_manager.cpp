#include "ligarium/widget/w_manager.h"

#include "ligarium/database.h"
#include "ligarium/widget/w_selectable.h"
#include "ligarium/widget_registry.h"
#include "ui_w_ligarium_manager.h"

#include <QDialog>


ligarium::W_Manager::W_Manager(QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::W_Ligarium_Manager)
{
  ui->setupUi(this);
}

ligarium::W_Manager::~W_Manager()
{
  delete ui;
}

void ligarium::W_Manager::refresh(ligarium::Database& db, ligarium::Table table)
{
  m_db    = &db;
  m_table = table;

  refresh_connection();

  ui->tile_view->clear();

  ui->b_new->setText(tr("Create %1").arg(ligarium::Table_to_str(table)));

  QList<qsizetype> ids = ligarium::all_records_id(*m_db, m_table);

  for (auto id : ids) {
    auto* w = new W_Selectable(*m_db, m_table, id);
    connect(w, &W_Selectable::signal_selected, [this](ligarium::Table, qsizetype id) {
      for (auto* elem : ui->tile_view->childrens()) {
        if (auto* ptr = qobject_cast<W_Selectable*>(elem)) ptr->select(ptr->m_id == id);
      }
    });

    ui->tile_view->add_widget(w);
  }
}

void ligarium::W_Manager::refresh_connection()
{
  if (connection) disconnect(connection);

  connection = connect(m_db, &ligarium::Database::signal_db_updated, [this](ligarium::Table _table) {
    if (_table == m_table || _table == ligarium::Table(0)) refresh(*m_db, _table);
  });
}


void ligarium::W_Manager::on_b_new_clicked()
{
  auto* w = ligarium::WidgetRegistry::new_widget_record(m_table, INVALID_ID, *m_db);
  exec_widget_dialog(parentWidget(), w);
}
