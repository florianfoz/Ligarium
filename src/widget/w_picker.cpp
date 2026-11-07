#include "ligarium/widget/w_picker.h"

#include "ligarium/record.h"
#include "ligarium/widget/w_selectable.h"
#include "ui_w_ligarium_picker.h"


ligarium::W_Picker::W_Picker(QWidget* parent, ligarium::Database& db)
  : QDialog(parent)
  , m_db(&db)
  , ui(new Ui::W_Ligarium_Picker)
{
  ui->setupUi(this);
}

ligarium::W_Picker::~W_Picker()
{
  delete ui;
}

void ligarium::W_Picker::refresh(ligarium::Database& db, ligarium::Table _rec_table)
{
  m_db    = &db;
  m_table = _rec_table;
  ui->view->clear();

  setWindowTitle(QObject::tr("Select %1").arg(ligarium::Table_to_str(m_table)));

  auto ids = ligarium::all_records_id(*m_db, m_table);

  for (auto id : ids) {
    auto* w = new W_Selectable(*m_db, m_table, id);
    connect(w, &W_Selectable::signal_selected, [this](ligarium::Database&, ligarium::Table, qsizetype id) {
      for (auto* elem : ui->view->childrens()) {
        auto* ptr = qobject_cast<W_Selectable*>(elem);
        ptr->select(ptr->m_id == id);
      }
      emit signal_on_selected(*m_db, m_table, id);
      accept();
    });

    ui->view->add_widget(w);
  }
}

void ligarium::W_Picker::on_b_clear_clicked()
{
  reject();
}
