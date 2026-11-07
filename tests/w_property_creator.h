#ifndef W_PROPERTY_CREATOR_H
#define W_PROPERTY_CREATOR_H

#include "test_records.h"

#include <QDialog>

enum class EMode : uint8_t;

namespace Ui
{
class W_Property_Creator;
}

class W_Property_Creator : public QDialog
{
  Q_OBJECT

public:
  explicit W_Property_Creator(QWidget* parent, ligarium::Database& db, qsizetype id = ligarium::INVALID_ID);
  ~W_Property_Creator();

  Property property;

  void clear();
  void populate_ui();
  void inject_data();

private slots:
  void on_buttonBox_accepted();

  void on_buttonBox_rejected();

private:
  Ui::W_Property_Creator* ui;
  ligarium::Database&     m_db;
};

#endif // W_PROPERTY_CREATOR_H
