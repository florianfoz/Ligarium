#include "widget/utils/w_many_rec_field.h"

//
#include "entity/attachment.h"
#include "entity/damage.h"
#include "entity/invoice.h"
#include "entity/landlord.h"
#include "entity/lease_agreement.h"
#include "entity/maintenance.h"
#include "entity/property.h"
#include "entity/receipt.h"
#include "entity/rent.h"
#include "entity/tenant.h"
// #include "entity/property_Feature.h"
// #include "entity/property_Room.h"
#include "widget/entity/w_attachment.h"
#include "widget/entity/w_damage.h"
#include "widget/entity/w_invoice.h"
#include "widget/entity/w_landlord.h"
// #include "widget/entity/w_lease_agreement.h"
#include "ui_w_many_rec_field.h"
#include "widget/entity/w_maintenance.h"
#include "widget/entity/w_property.h"
#include "widget/entity/w_receipt.h"
#include "widget/entity/w_rent.h"
#include "widget/entity/w_tenant.h"
#include "widget/utils/w_many_rec_elem.h"
#include "widget/utils/w_rec_picker.h"

W_Many_Rec_Field::W_Many_Rec_Field(QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::W_Many_Rec_Field)
{
  ui->setupUi(this);
}

W_Many_Rec_Field::~W_Many_Rec_Field()
{
  delete ui;
}


void W_Many_Rec_Field::refresh(ligarium::Table _rec_table)
{
  clear();

  rec_table = _rec_table;
  ui->b_add->setText(tr("Add %1").arg(ligarium::Table_to_str(rec_table)));
  ui->groupBox->setTitle(ligarium::Table_to_str(rec_table));

  for (auto elem : selection) {
    auto* w = new W_Many_Rec_Elem(this);
    w->set_record(rec_table, elem);
    ui->layout->addWidget(w);
  }
}

void W_Many_Rec_Field::clear()
{
  while (auto* elem = ui->layout->takeAt(0)) {
    if (auto* w = elem->widget()) w->deleteLater();
    delete elem;
  }
}

void W_Many_Rec_Field::on_b_add_clicked()
{
  auto* w = new W_Rec_Picker(this);
  w->refresh(rec_table);

  connect(w, &W_Rec_Picker::signal_on_selected, [this](ligarium::Table _rec_table, qsizetype _id) {
    selection.append(_id);
    refresh(rec_table);
  });

  w->setModal(true);
  w->exec();
}