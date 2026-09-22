#include "widget/utils/w_many_rec_elem.h"

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
#include "ui_w_many_rec_elem.h"
#include "widget/entity/w_maintenance.h"
#include "widget/entity/w_property.h"
#include "widget/entity/w_receipt.h"
#include "widget/entity/w_rent.h"
#include "widget/entity/w_tenant.h"
#include "widget/utils/w_rec_picker.h"

W_Many_Rec_Elem::W_Many_Rec_Elem(QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::W_Many_Rec_Elem)
{
  ui->setupUi(this);
}

W_Many_Rec_Elem::~W_Many_Rec_Elem()
{
  delete ui;
}


void W_Many_Rec_Elem::refresh(ETable _rec_table)
{
  rec_table = _rec_table;
}

void W_Many_Rec_Elem::set_record(ETable _rec_table, qsizetype _id)
{
  rec_table = _rec_table;
  id        = _id;

  if (rec_table != ETable::NONE && id != INVALID_ID) {
    switch (rec_table) {
    case ETable::Attachment:      ui->le_name->setText(Attachment::read_record(id).dump()); break;
    case ETable::Damage:          ui->le_name->setText(Damage::read_record(id).dump()); break;
    case ETable::Invoice:         ui->le_name->setText(Invoice::read_record(id).dump()); break;
    case ETable::Landlord:        ui->le_name->setText(Landlord::read_record(id).dump()); break;
    case ETable::Lease_Agreement: ui->le_name->setText(Lease_Agreement::read_record(id).dump()); break;
    case ETable::Maintenance:     ui->le_name->setText(Maintenance::read_record(id).dump()); break;
    case ETable::Property:        ui->le_name->setText(Property::read_record(id).dump()); break;
    case ETable::Receipt:         ui->le_name->setText(Receipt::read_record(id).dump()); break;
    case ETable::Rent:            ui->le_name->setText(Rent::read_record(id).dump()); break;
    case ETable::Tenant:          ui->le_name->setText(Tenant::read_record(id).dump()); break;
    default:                      assert(false && "Invalid record table");
    }
  }

  if (id == INVALID_ID) clear();
}

void W_Many_Rec_Elem::enterEvent(QEnterEvent* event)
{
  hide_tooltip();

  if (rec_table != ETable::NONE && id != INVALID_ID) {

    switch (rec_table) {
    case ETable::Attachment:  tooltip = new W_Attachment(id); break;
    case ETable::Damage:      tooltip = new W_Damage(id); break;
    case ETable::Invoice:     tooltip = new W_Invoice(id); break;
    case ETable::Landlord:    tooltip = new W_Landlord(id); break;
    // case ETable::Lease_Agreement: tooltip = new W_Lease_Agreement(id); break;
    case ETable::Maintenance: tooltip = new W_Maintenance(id); break;
    case ETable::Property:    tooltip = new W_Property(id); break;
    case ETable::Receipt:     tooltip = new W_Receipt(id); break;
    case ETable::Rent:        tooltip = new W_Rent(id); break;
    case ETable::Tenant:      tooltip = new W_Tenant(id); break;
    default:                  assert(false && "Invalid record table");
    }
  }
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
  id = INVALID_ID;
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
