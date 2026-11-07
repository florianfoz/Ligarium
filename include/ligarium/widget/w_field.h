#ifndef W_REC_FIELD_H
#define W_REC_FIELD_H

#include "ligarium/ligarium.h"

#include <QWidget>

namespace Ui
{
class W_Ligarium_Field;
}


namespace ligarium
{

class W_Field : public QWidget
{
  Q_OBJECT

public:
  explicit W_Field(QWidget* parent);
  ~W_Field();


  QWidget* tooltip  = nullptr;
  bool     readonly = false;

  void set_readonly(bool _readonly);
  void refresh(ligarium::Database& db, ligarium::Table _rec_table);
  void set_record(ligarium::Database& db, ligarium::Table _rec_table, qsizetype _id);
  void clear();

  void hide_tooltip();

  [[nodiscard]]
  qsizetype id() const
  {
    return m_id;
  }

  [[nodiscard]]
  ligarium::Table table() const
  {
    return m_table;
  }

  [[nodiscard]]
  ligarium::Database& database() const
  {
    return *m_db;
  }

private:
  ligarium::Table     m_table = ligarium::Table(0);
  qsizetype           m_id    = ligarium::INVALID_ID;
  ligarium::Database* m_db    = nullptr;

private slots:
  void enterEvent(QEnterEvent* event) override;

  void leaveEvent(QEvent* event) override;

  void mousePressEvent(QMouseEvent* event) override;

private:
  Ui::W_Ligarium_Field* ui;
};

} // namespace ligarium


#endif // W_REC_FIELD_H