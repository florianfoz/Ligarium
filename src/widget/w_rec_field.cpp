#include "ligarium/ui/w_rec_field.h"

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
#include "ui_w_rec_field.h"
#include "widget/entity/w_maintenance.h"
#include "widget/entity/w_property.h"
#include "widget/entity/w_receipt.h"
#include "widget/entity/w_rent.h"
#include "widget/entity/w_tenant.h"
#include "widget/utils/w_rec_picker.h"

W_Rec_Field::W_Rec_Field(QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::W_Rec_Field)
{
  ui->setupUi(this);
}

W_Rec_Field::~W_Rec_Field()
{
  delete ui;
}

void W_Rec_Field::set_readonly(bool _readonly)
{
  readonly = _readonly;
}


void W_Rec_Field::refresh(ligarium::Table _rec_table)
{
  rec_table = _rec_table;
}

void W_Rec_Field::set_record(ligarium::Table _rec_table, qsizetype _id)
{
  rec_table = _rec_table;
  id        = _id;

  if (rec_table != ligarium::Table::NONE && id != ligarium::INVALID_ID) {
    switch (rec_table) {
    case ligarium::Table::Attachment:      ui->le_name->setText(Attachment::read_record(id).dump()); break;
    case ligarium::Table::Damage:          ui->le_name->setText(Damage::read_record(id).dump()); break;
    case ligarium::Table::Invoice:         ui->le_name->setText(Invoice::read_record(id).dump()); break;
    case ligarium::Table::Landlord:        ui->le_name->setText(Landlord::read_record(id).dump()); break;
    case ligarium::Table::Lease_Agreement: ui->le_name->setText(Lease_Agreement::read_record(id).dump()); break;
    case ligarium::Table::Maintenance:     ui->le_name->setText(Maintenance::read_record(id).dump()); break;
    case ligarium::Table::Property:        ui->le_name->setText(Property::read_record(id).dump()); break;
    case ligarium::Table::Receipt:         ui->le_name->setText(Receipt::read_record(id).dump()); break;
    case ligarium::Table::Rent:            ui->le_name->setText(Rent::read_record(id).dump()); break;
    case ligarium::Table::Tenant:          ui->le_name->setText(Tenant::read_record(id).dump()); break;
    default:                               assert(false && "Invalid record table");
    }
  }

  if (id == ligarium::INVALID_ID) clear();
}

void W_Rec_Field::enterEvent(QEnterEvent* event)
{
  hide_tooltip();

  if (rec_table != ligarium::Table::NONE && id != ligarium::INVALID_ID) {

    switch (rec_table) {
    case ligarium::Table::Attachment:  tooltip = new W_Attachment(id); break;
    case ligarium::Table::Damage:      tooltip = new W_Damage(id); break;
    case ligarium::Table::Invoice:     tooltip = new W_Invoice(id); break;
    case ligarium::Table::Landlord:    tooltip = new W_Landlord(id); break;
    // case ligarium::Table::Lease_Agreement: tooltip = new W_Lease_Agreement(id); break;
    case ligarium::Table::Maintenance: tooltip = new W_Maintenance(id); break;
    case ligarium::Table::Property:    tooltip = new W_Property(id); break;
    case ligarium::Table::Receipt:     tooltip = new W_Receipt(id); break;
    case ligarium::Table::Rent:        tooltip = new W_Rent(id); break;
    case ligarium::Table::Tenant:      tooltip = new W_Tenant(id); break;
    default:                           assert(false && "Invalid record table");
    }
  }
}

void W_Rec_Field::hide_tooltip()
{
  if (tooltip) {
    tooltip->deleteLater();
    tooltip = nullptr;
  }
}

void W_Rec_Field::clear()
{
  ui->le_name->setText(QObject::tr("Click to select a record"));
  id = ligarium::INVALID_ID;
  hide_tooltip();
}


void W_Rec_Field::leaveEvent(QEvent* event)
{
  hide_tooltip();
}

void W_Rec_Field::mousePressEvent(QMouseEvent* event)
{
  assert(rec_table != ligarium::Table::NONE && "Invalid record table");

  if (readonly) return;

  hide_tooltip();

  auto* w = new W_Rec_Picker(this);
  w->refresh(ligarium::Table::Property);
  connect(w, &W_Rec_Picker::signal_on_selected, this, &W_Rec_Field::set_record);
  connect(w, &QDialog::rejected, [this]() { clear(); });
  w->setModal(true);
  w->exec();
}
