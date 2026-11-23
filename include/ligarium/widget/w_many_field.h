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

  void refresh(Database& db, Table target_table, Table association_table);
  void clear();

  void set_selection(Database& db, Table target_table, Table association_table, const QList<qsizetype>& ids)
  {
    m_selection = ids;
    refresh(db, target_table, association_table);
  }

  [[nodiscard]]
  QList<qsizetype> selection() const
  {
    return m_selection;
  }

  [[nodiscard]]
  Table table() const
  {
    return m_target_table;
  }

  [[nodiscard]]
  Database& database() const
  {
    return *m_db;
  }


private:
  Table            m_target_table      = Table(0);
  Table            m_association_table = Table(0);
  Database*        m_db;
  QList<qsizetype> m_selection;

private slots:
  void on_b_add_clicked();

private:
  Ui::W_Ligarium_Many_Field* ui;
};

} // namespace ligarium

#endif // W_MANY_REC_FIELD_H