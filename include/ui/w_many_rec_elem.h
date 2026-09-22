#ifndef W_MANY_REC_ELEM_H
#define W_MANY_REC_ELEM_H

#include "forward.h"

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

  ETable    rec_table = ETable::NONE;
  qsizetype id        = INVALID_ID;
  QWidget*  tooltip   = nullptr;

  QList<qsizetype> selection;

  void refresh(ETable _rec_table);
  void set_record(ETable _rec_table, qsizetype _id);
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