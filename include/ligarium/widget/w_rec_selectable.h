#ifndef W_REC_SELECTABLE_H
#define W_REC_SELECTABLE_H

#include "forward.h"

namespace Ui
{
class W_Rec_Selectable;
}

class W_Rec_Selectable : public QWidget
{
  Q_OBJECT

public:
  explicit W_Rec_Selectable(ligarium::Table table, qsizetype id);
  ~W_Rec_Selectable();

  ligarium::Table table;
  qsizetype       id;

  void select(bool selected);

public slots:
  void mousePressEvent(QMouseEvent* event) override;

signals:
  void signal_selected(qsizetype id);

private slots:
  void on_b_edit_clicked();

  void on_b_delete_clicked();

  void enterEvent(QEnterEvent* event) override;

  void leaveEvent(QEvent* event) override;

private:
  Ui::W_Rec_Selectable* ui;
};


#endif // W_REC_SELECTABLE_H