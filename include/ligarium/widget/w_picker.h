#ifndef W_REC_PICKER_H
#define W_REC_PICKER_H

#include "ligarium/ligarium.h"

#include <QDialog>

namespace Ui
{
class W_Ligarium_Picker;
}

namespace ligarium
{

class W_Picker : public QDialog
{
  Q_OBJECT

public:
  explicit W_Picker(QWidget* parent, Database& db);
  ~W_Picker();

  Table     m_table = Table(0);
  Database* m_db    = nullptr;

  void refresh(Database& db, Table _rec_table);

signals:
  void signal_selected(Table _rec_table, qsizetype id);

private slots:
  void on_b_clear_clicked();

private:
  Ui::W_Ligarium_Picker* ui;
};

} // namespace ligarium

#endif // W_REC_PICKER_H