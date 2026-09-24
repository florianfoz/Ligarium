#ifndef W_MANY_REC_FIELD_H
#define W_MANY_REC_FIELD_H

#include "forward.h"

#include <QWidget>

namespace Ui
{
class W_Many_Rec_Field;
}


class W_Many_Rec_Field : public QWidget
{
  Q_OBJECT

public:
  explicit W_Many_Rec_Field(QWidget* parent = nullptr);
  ~W_Many_Rec_Field();

  ETable           rec_table = ETable::NONE;
  QList<qsizetype> selection;

  void refresh(ETable _rec_table);
  void clear();

private slots:
  void on_b_add_clicked();

private:
  Ui::W_Many_Rec_Field* ui;
};

#endif // W_MANY_REC_FIELD_H