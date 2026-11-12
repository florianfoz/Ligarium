#include "test_ui.h"

#include "ligarium_config.h"
//
#include "ligarium/database.h"
#include "ligarium/schema.h"
#include "records.h"
#include "w_ligarium_main.h"
#include "w_property.h"
#include "w_tenant.h"

#include <QApplication>
#include <ligarium/widget_registry.h>


int main(int argc, char* argv[])
{
  QApplication application(argc, argv);

  // register widgets
  ligarium::register_widget<Property, W_property>();
  ligarium::register_widget<Tenant, W_Tenant>();

  auto connexion = QSqlDatabase::addDatabase("QSQLITE", "my_connection_name");
  connexion.setDatabaseName(":memory:");
  connexion.open();

  my_db = new ligarium::Database(std::move(connexion));

  // create database
  ligarium::SchemaBuilder builder(my_db->connection());
  builder.create_all<Property, Tenant, Tenant_Property>();

  qDebug() << builder.dump_schema();

  auto* menu = new W_Ligarium_Main();
  menu->show();

  return QApplication::exec();
}
