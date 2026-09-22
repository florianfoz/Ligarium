#include "widget/utils/w_rec_manager.h"

#include "database/manager.h"
#include "ui_w_rec_manager.h"

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
// #include "widget/entity/w_attachment_creator.h"
#include "utils/w_rec_selectable.h"
#include "widget/entity/w_damage_creator.h"
#include "widget/entity/w_invoice_creator.h"
#include "widget/entity/w_landlord_creator.h"
// #include "widget/entity/w_lease_agreement_creator.h"
#include "widget/entity/w_maintenance_creator.h"
#include "widget/entity/w_property_creator.h"
#include "widget/entity/w_receipt_creator.h"
#include "widget/entity/w_tenant_creator.h"


W_Rec_Manager::W_Rec_Manager(ETable rec_table)
  : QWidget(nullptr)
  , ui(new Ui::W_Rec_Manager)
  , rec_table(rec_table)
{
  ui->setupUi(this);

  switch (rec_table) {
  case ETable::Attachment:
  case ETable::Damage:
  case ETable::Invoice:
  case ETable::Landlord:
  case ETable::Lease_Agreement:
  case ETable::Maintenance:
  case ETable::Property:
  case ETable::Receipt:
  case ETable::Rent:
  case ETable::Tenant:          break;
  default:                      assert(false && "Invalid record table");
  }

  ui->b_new->setText(tr("Create %1").arg(ETable_to_str(rec_table)));

  auto& db = Database_Manager::instance();

  connect(&db, &Database_Manager::signal_db_updated, [this](ETable table) {
    if (table == ETable::Tenant || table == ETable::NONE) refresh();
  });
  connect(&db, &Database_Manager::signal_db_changed, [this]() { refresh(); });

  refresh();
}

W_Rec_Manager::~W_Rec_Manager()
{
  delete ui;
}

void W_Rec_Manager::refresh()
{
  ui->tile_view->clear();

  QList<qsizetype> ids;

  switch (rec_table) {
  case ETable::Attachment:      ids = Attachment::all_records_id(); break;
  case ETable::Damage:          ids = Damage::all_records_id(); break;
  case ETable::Invoice:         ids = Invoice::all_records_id(); break;
  case ETable::Landlord:        ids = Landlord::all_records_id(); break;
  case ETable::Lease_Agreement: ids = Lease_Agreement::all_records_id(); break;
  case ETable::Maintenance:     ids = Maintenance::all_records_id(); break;
  case ETable::Property:        ids = Property::all_records_id(); break;
  case ETable::Receipt:         ids = Receipt::all_records_id(); break;
  case ETable::Rent:            ids = Rent::all_records_id(); break;
  case ETable::Tenant:          ids = Tenant::all_records_id(); break;
  default:                      assert(false && "Invalid record table");
  }

  for (auto id : ids) {
    auto* w = new W_Rec_Selectable(rec_table, id);
    connect(w, &W_Rec_Selectable::signal_selected, [this](qsizetype id) {
      for (auto* elem : ui->tile_view->childrens()) {
        auto* ptr = qobject_cast<W_Rec_Selectable*>(elem);
        ptr->select(ptr->id == id);
      }
    });

    ui->tile_view->add_widget(w);
  }
}


void W_Rec_Manager::on_b_new_clicked()
{
  QDialog* d = nullptr;

  switch (rec_table) {
  // case ETable::Attachment:       d = new W_Attachment_Creator(); break;
  case ETable::Damage:      d = new W_Damage_Creator(); break;
  case ETable::Invoice:     d = new W_Invoice_Creator(); break;
  case ETable::Landlord:    d = new W_Landlord_Creator(); break;
  // case ETable::Lease_Agreement:  d = new W_Lease_Agreement_Creator(); break;
  case ETable::Maintenance: d = new W_Maintenance_Creator(); break;
  case ETable::Property:    d = new W_Property_Creator(); break;
  case ETable::Receipt:     d = new W_Receipt_Creator(); break;
  case ETable::Tenant:      d = new W_Tenant_Creator(); break;
  default:                  assert(false && "Invalid record table");
  }

  d->setModal(true);
  d->exec();
}
