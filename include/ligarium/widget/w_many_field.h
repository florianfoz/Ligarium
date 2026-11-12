#ifndef W_MANY_REC_FIELD_H
#define W_MANY_REC_FIELD_H

#include "ligarium/ligarium.h"

#include <QWidget>

namespace Ui
{
class W_Ligarium_Many_Field;
}

namespace ligarium
{

class W_Many_Field : public QWidget
{
  Q_OBJECT

public:
  explicit W_Many_Field(QWidget* parent);
  ~W_Many_Field();

  void refresh(ligarium::Database& db, ligarium::Table table);
  void clear();

  void set_selection(ligarium::Database& db, ligarium::Table table, const QList<qsizetype>& ids)
  {
    m_selection = ids;
    refresh(db, table);
  }

  [[nodiscard]]
  QList<qsizetype> selection() const
  {
    return m_selection;
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
  ligarium::Database* m_db;
  QList<qsizetype>    m_selection;

private slots:
  void on_b_add_clicked();

private:
  Ui::W_Ligarium_Many_Field* ui;
};

} // namespace ligarium

#endif // W_MANY_REC_FIELD_H