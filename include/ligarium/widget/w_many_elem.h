#ifndef W_MANY_REC_ELEM_H
#define W_MANY_REC_ELEM_H

#include "ligarium/ligarium.h"

#include <QWidget>

namespace Ui
{
class W_Ligarium_Many_Elem;
}

namespace ligarium
{

class W_Many_Elem : public QWidget
{
  Q_OBJECT

public:
  explicit W_Many_Elem(QWidget* parent = nullptr);
  ~W_Many_Elem();

  void refresh(Database& db, Table table);
  void set_record(Database& db, Table table, qsizetype id);

  [[nodiscard]]
  qsizetype id() const
  {
    return m_id;
  }

  [[nodiscard]]
  Table table() const
  {
    return m_table;
  }

  [[nodiscard]]
  Database& database() const
  {
    return *m_db;
  }

private:
  Table     m_table = Table(0);
  qsizetype m_id    = INVALID_ID;
  Database* m_db    = nullptr;

private slots:
  void on_b_delete_clicked();

signals:
  void signal_deleted(qsizetype id);

private:
  Ui::W_Ligarium_Many_Elem* ui;
};

} // namespace ligarium

#endif // W_MANY_REC_ELEM_H