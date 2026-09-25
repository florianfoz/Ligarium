#ifndef W_REC_FIELD_H
#define W_REC_FIELD_H

#include "ligarium/ligarium.h"

#include <QWidget>

namespace Ui
{
class W_Rec_Field;
}


class W_Rec_Field : public QWidget
{
  Q_OBJECT

public:
  explicit W_Rec_Field(QWidget* parent = nullptr);
  ~W_Rec_Field();

  ligarium::Table rec_table = ligarium::Table::NONE;
  qsizetype       id        = ligarium::INVALID_ID;
  QWidget*        tooltip   = nullptr;
  bool            readonly  = false;

  void set_readonly(bool _readonly);
  void refresh(ligarium::Table _rec_table);
  void set_record(ligarium::Table _rec_table, qsizetype _id);
  void clear();

  void hide_tooltip();

private slots:
  void enterEvent(QEnterEvent* event) override;

  void leaveEvent(QEvent* event) override;

  void mousePressEvent(QMouseEvent* event) override;

private:
  Ui::W_Rec_Field* ui;
};

#endif // W_REC_FIELD_H