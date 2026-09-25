#ifndef W_MANY_REC_ELEM_H
#define W_MANY_REC_ELEM_H

#include "ligarium/ligarium.h"

#include <QWidget>

namespace Ui
{
class W_Many_Rec_Elem;
}


class W_Many_Rec_Elem : public QWidget
{
  Q_OBJECT

public:
  explicit W_Many_Rec_Elem(QWidget* parent = nullptr);
  ~W_Many_Rec_Elem();

  ligarium::Table     rec_table = ligarium::Table::NONE;
  qsizetype           id        = ligarium::INVALID_ID;
  QWidget*            tooltip   = nullptr;
  ligarium::Database* m_db      = nullptr;

  QList<qsizetype> selection;

  void refresh(ligarium::Database& db, ligarium::Table _rec_table);
  void set_record(ligarium::Database& db, ligarium::Table _rec_table, qsizetype _id);
  void clear();

  void hide_tooltip();

private slots:
  void enterEvent(QEnterEvent* event) override;

  void leaveEvent(QEvent* event) override;

  void on_b_delete_clicked();

signals:
  void signal_deleted(qsizetype id);

private:
  Ui::W_Many_Rec_Elem* ui;
};

#endif // W_MANY_REC_ELEM_H