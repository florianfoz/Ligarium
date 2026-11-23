#ifndef W_SELECTABLE_H
#define W_SELECTABLE_H

#include "ligarium/ligarium.h"

#include <QWidget>

namespace Ui
{
class W_Ligarium_Selectable;
}


namespace ligarium
{

class W_Selectable : public QWidget
{
  Q_OBJECT

public:
  explicit W_Selectable(Database& db, Table table, qsizetype id);
  ~W_Selectable();

  Table                   m_table = Table(0);
  qsizetype               m_id    = INVALID_ID;
  Database&               m_db;
  QMetaObject::Connection connection;

  void select(bool selected);


signals:
  void signal_selected(Table table, qsizetype id);

private slots:
  void on_b_edit_clicked();
  void on_b_delete_clicked();

  void enterEvent(QEnterEvent* event) override;

  void mousePressEvent(QMouseEvent* event) override;
  void leaveEvent(QEvent* event) override;

private:
  Ui::W_Ligarium_Selectable* ui;
};

} // namespace ligarium

#endif // W_SELECTABLE_H