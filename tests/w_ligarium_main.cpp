#include "w_ligarium_main.h"

#include "test_ui.h"
#include "ui_w_ligarium_main.h"

W_Ligarium_Main::W_Ligarium_Main(QWidget* parent)
  : QMainWindow(parent)
  , ui(new Ui::W_Ligarium_Main)
{
  ui->setupUi(this);

  ui->property->refresh(*my_db, ligarium::Table::Property);
  ui->tenant->refresh(*my_db, ligarium::Table::Tenant);
}
W_Ligarium_Main::~W_Ligarium_Main()
{
  delete ui;
}