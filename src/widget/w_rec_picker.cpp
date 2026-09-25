#include "widget/utils/w_rec_picker.h"

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
#include "ui_w_rec_picker.h"
#include "utils/w_rec_selectable.h"
// #include "entity/property_Feature.h"
// #include "entity/property_Room.h"


W_Rec_Picker::W_Rec_Picker(QWidget* parent)
  : QDialog(parent)
  , rec_table(rec_table)
  , ui(new Ui::W_Rec_Picker)
{
  ui->setupUi(this);
}

W_Rec_Picker::~W_Rec_Picker()
{
  delete ui;
}

template <typename T, Ligarium::RecordType<T> REC>
QList<qsizeype> all_records_id()
{
}

void W_Rec_Picker::refresh(ligarium::Table _rec_table)
{
  rec_table = _rec_table;
  ui->view->clear();

  setWindowTitle(QObject::tr("Select %1").arg(ligarium::Table_to_str(rec_table)));

#define TBL(table) ids = table::all_records_id();

  QList<qsizetype> ids = TBL(_rec_table);

  switch (rec_table) {
  case ligarium::Table::Attachment:      ids = Attachment::all_records_id(); break;
  case ligarium::Table::Damage:          ids = Damage::all_records_id(); break;
  case ligarium::Table::Invoice:         ids = Invoice::all_records_id(); break;
  case ligarium::Table::Landlord:        ids = Landlord::all_records_id(); break;
  case ligarium::Table::Lease_Agreement: ids = Lease_Agreement::all_records_id(); break;
  case ligarium::Table::Maintenance:     ids = Maintenance::all_records_id(); break;
  case ligarium::Table::Property:        ids = Property::all_records_id(); break;
  case ligarium::Table::Receipt:         ids = Receipt::all_records_id(); break;
  case ligarium::Table::Rent:            ids = Rent::all_records_id(); break;
  case ligarium::Table::Tenant:          ids = Tenant::all_records_id(); break;
  default:                               assert(false && "Invalid record table");
  }

  for (auto id : ids) {
    auto* w = new W_Rec_Selectable(rec_table, id);
    connect(w, &W_Rec_Selectable::signal_selected, [this](qsizetype id) {
      for (auto* elem : ui->view->childrens()) {
        auto* ptr = qobject_cast<W_Rec_Selectable*>(elem);
        ptr->select(ptr->id == id);
      }
      emit signal_on_selected(rec_table, id);
      accept();
    });

    ui->view->add_widget(w);
  }
}

void W_Rec_Picker::on_b_clear_clicked()
{
  reject();
}
