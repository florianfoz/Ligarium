#include "test_ui.h"

#include "ligarium_config.h"
//
#include "ligarium/database.h"
#include "ligarium/schema.h"
#include "test_records.h"
#include "w_ligarium_main.h"
#include "w_property.h"
#include "w_property_creator.h"
#include "w_tenant.h"
#include "w_tenant_creator.h"

#include <QApplication>
#include <ligarium/widget_registry.h>


int main(int argc, char* argv[])
{
  QApplication application(argc, argv);

  // register widgets
  ligarium::register_widget<Property, W_Property_Creator, W_Property>();
  ligarium::register_widget<Tenant, W_Tenant_Creator, W_Tenant>();

  auto connexion = QSqlDatabase::addDatabase("QSQLITE", "my_connection_name");
  connexion.setDatabaseName(":memory:");

  my_db = new ligarium::Database(connexion);

  // create database
  ligarium::SchemaBuilder builder(my_db->connection());
  builder.create_all<Property, Tenant, Landlord, Tenant_Property>();

  auto* menu = new W_Ligarium_Main();
  menu->show();

  return QApplication::exec();
}
