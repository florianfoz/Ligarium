#ifndef W_REC_MANAGER_H
#define W_REC_MANAGER_H

#include "forward.h"

#include <QWidget>

namespace Ui
{
class W_Rec_Manager;
}


class W_Rec_Manager : public QWidget
{
  Q_OBJECT

public:
  explicit W_Rec_Manager(ETable rec_table);
  ~W_Rec_Manager();

  ETable rec_table;

  void refresh();

private slots:
  void on_b_new_clicked();

private:
  Ui::W_Rec_Manager* ui;
};

#endif // W_REC_MANAGER_H