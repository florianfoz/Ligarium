#include "widget/utils/w_rec_selectable.h"

#include "ui_w_rec_selectable.h"
#include "widget/entity/w_attachment.h"
#include "widget/entity/w_damage.h"
#include "widget/entity/w_invoice.h"
#include "widget/entity/w_landlord.h"
// #include "widget/entity/w_lease_agreement.h"
#include "widget/entity/w_maintenance.h"
#include "widget/entity/w_property.h"
// #include "widget/entity/w_property_Feature.h"
// #include "widget/entity/w_property_Room.h"
#include "widget/entity/w_receipt.h"
#include "widget/entity/w_rent.h"
#include "widget/entity/w_tenant.h"

// #include "widget/entity/w_attachment_creator.h"
#include "widget/entity/w_damage_creator.h"
#include "widget/entity/w_invoice_creator.h"
#include "widget/entity/w_landlord_creator.h"
// #include "widget/entity/w_lease_agreement_creator.h"
#include "widget/entity/w_maintenance_creator.h"
#include "widget/entity/w_property_creator.h"
// #include "widget/entity/w_property_Feature_creator.h"
// #include "widget/entity/w_property_Room_creator.h"
#include "widget/entity/w_receipt_creator.h"
// #include "widget/entity/w_rent_creator.h"

//
#include "entity/attachment.h"
#include "entity/damage.h"
#include "entity/invoice.h"
#include "entity/landlord.h"
#include "entity/lease_agreement.h"
#include "entity/maintenance.h"
#include "entity/property.h"
#include "widget/entity/w_tenant_creator.h"
// #include "entity/property_Feature.h"
// #include "entity/property_Room.h"
#include "entity/receipt.h"
#include "entity/rent.h"
#include "entity/tenant.h"

W_Rec_Selectable::W_Rec_Selectable(ETable table, qsizetype id)
  : QWidget(nullptr)
  , table(table)
  , id(id)
  , ui(new Ui::W_Rec_Selectable)
{
  ui->setupUi(this);

  QWidget* w = nullptr;

  switch (table) {
  case ETable::Attachment:  w = new W_Attachment(id); break;
  case ETable::Damage:      w = new W_Damage(id); break;
  case ETable::Invoice:     w = new W_Invoice(id); break;
  case ETable::Landlord:    w = new W_Landlord(id); break;
  // case ETable::Lease_Agreement: w = new W_Lease_Agreement(id); break;
  case ETable::Maintenance: w = new W_Maintenance(id); break;
  case ETable::Property:    w = new W_Property(id); break;
  // case ETable::Property_Feature: w = new W_Property_Feature(id); break;
  // case ETable::Property_Room:    w = new W_Property_Room(id); break;
  case ETable::Receipt:     w = new W_Receipt(id); break;
  case ETable::Rent:        w = new W_Rent(id); break;
  case ETable::Tenant:      w = new W_Tenant(id); break;
  default:                  assert(false && "Must be a valid table");
  }

  connect(this, &W_Rec_Selectable::signal_selected, [this](qsizetype _id) { select(_id == this->id); });

  ui->layout->addWidget(w);

  ui->b_delete->hide();
  ui->b_edit->hide();
}

W_Rec_Selectable::~W_Rec_Selectable()
{
  delete ui;
}


void W_Rec_Selectable::select(bool selected)
{
  if (selected) {
    ui->f_selection->setStyleSheet("#f_selection { border: 1px solid blue; }");
  } else {
    ui->f_selection->setStyleSheet("");
  }
}

void W_Rec_Selectable::mousePressEvent(QMouseEvent* event)
{
  emit signal_selected(id);
}

void W_Rec_Selectable::on_b_edit_clicked()
{
  QDialog* w = nullptr;

  switch (table) {
  // case ETable::Attachment:       w = new W_Attachment_Creator(id); break;
  case ETable::Damage:      w = new W_Damage_Creator(id); break;
  case ETable::Invoice:     w = new W_Invoice_Creator(id); break;
  case ETable::Landlord:    w = new W_Landlord_Creator(id); break;
  // case ETable::LandlordP:        w = new W_LandlordP_Creator(id); break;
  // case ETable::Lease_Agreement:  w = new W_Lease_Agreement_Creator(id); break;
  case ETable::Maintenance: w = new W_Maintenance_Creator(id); break;
  case ETable::Property:    w = new W_Property_Creator(id); break;
  // case ETable::Property_Feature: w = new W_Property_Feature_Creator(id); break;
  // case ETable::Property_Room:    w = new W_Property_Room_Creator(id); break;
  case ETable::Receipt:     w = new W_Receipt_Creator(id); break;
  // case ETable::Rent:        w = new W_Rent_Creator(id); break;
  case ETable::Tenant:      w = new W_Tenant_Creator(id); break;

  default:                  assert(false && "Must be a valid table");
  }

  w->setModal(true);
  w->exec();
}

void W_Rec_Selectable::on_b_delete_clicked()
{
#define case_rec(_rec)                                                                                                 \
  case ETable::_rec: (void)_rec::read_record(id).delete_record(true); break;

  switch (table) {
    // case_rec(Attachment);
    case_rec(Damage);
    case_rec(Invoice);
    case_rec(Landlord);
    // case_rec(LandlordP);
    // case_rec(Lease_Agreement);
    case_rec(Maintenance);
    case_rec(Property);
    // case_rec(Property_Feature);
    // case_rec(Property_Room);
    case_rec(Receipt);
    // case_rec(Rent);
    case_rec(Tenant);

  default: assert(false && "Must be a valid table");
  }
}

void W_Rec_Selectable::enterEvent(QEnterEvent* event)
{
  ui->b_delete->show();
  ui->b_edit->show();
}

void W_Rec_Selectable::leaveEvent(QEvent* event)
{
  ui->b_delete->hide();
  ui->b_edit->hide();
}
