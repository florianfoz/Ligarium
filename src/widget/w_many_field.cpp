#include "ligarium/widget/w_many_field.h"

//
#include "ligarium/widget/w_many_elem.h"
#include "ligarium/widget/w_picker.h"
#include "ui_w_ligarium_many_field.h"

namespace ligarium
{


W_Many_Field::W_Many_Field(QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::W_Ligarium_Many_Field)
{
  ui->setupUi(this);
}

W_Many_Field::~W_Many_Field()
{
  delete ui;
}


void W_Many_Field::refresh(Database& db, Table target_table, Table association_table)
{
  m_db                = &db;
  m_target_table      = target_table;
  m_association_table = association_table;

  clear();

  ui->b_add->setText(tr("Add %1").arg(Table_to_str(m_target_table)));
  ui->groupBox->setTitle(Table_to_str(m_target_table));

  for (auto elem : m_selection) {
    auto* w = new W_Many_Elem(this);
    w->set_record(*m_db, m_association_table, elem);
    ui->layout->addWidget(w);
  }
}

void W_Many_Field::clear()
{
  while (auto* elem = ui->layout->takeAt(0)) {
    if (auto* w = elem->widget()) w->deleteLater();
    delete elem;
  }
}

void W_Many_Field::on_b_add_clicked()
{
  auto* w = new W_Picker(this, *m_db);
  w->refresh(*m_db, m_target_table);

  connect(w, &W_Picker::signal_selected, [this](Table, qsizetype _id) {
    m_selection.append(_id);
    refresh(*m_db, m_target_table, m_association_table);
  });

  w->setModal(true);
  w->exec();
}

} // namespace ligarium
