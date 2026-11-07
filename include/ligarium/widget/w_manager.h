#ifndef W_REC_MANAGER_H
#define W_REC_MANAGER_H

#include "ligarium/ligarium.h"

#include <QWidget>

namespace Ui
{
class W_Ligarium_Manager;
}

namespace ligarium
{

class W_Manager : public QWidget
{
  Q_OBJECT

public:
  explicit W_Manager(QWidget* parent);
  ~W_Manager();

  ligarium::Table         m_table = ligarium::Table(0);
  ligarium::Database*     m_db    = nullptr;
  QMetaObject::Connection connection;

  void refresh(ligarium::Database& db, ligarium::Table table);

private:
  void refresh_connection();

private slots:
  void on_b_new_clicked();

private:
  Ui::W_Ligarium_Manager* ui;
};

} // namespace ligarium

#endif // W_REC_MANAGER_H