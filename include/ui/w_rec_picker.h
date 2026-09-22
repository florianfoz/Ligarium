#ifndef W_REC_PICKER_H
#define W_REC_PICKER_H

#include "ligarium.h"

#include <QWidget>

namespace Ui
{
class W_Rec_Picker;
}


class W_Rec_Picker : public QDialog
{
  Q_OBJECT

public:
  explicit W_Rec_Picker(QWidget* parent);
  ~W_Rec_Picker();

  ETable rec_table;

  void refresh(ETable _rec_table);

signals:
  void signal_on_selected(ETable _rec_table, qsizetype id);

private slots:
  void on_b_clear_clicked();

private:
  Ui::W_Rec_Picker* ui;
};

#endif // W_REC_PICKER_H