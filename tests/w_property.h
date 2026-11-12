#ifndef W_PROPERTY_H
#define W_PROPERTY_H

#include "records.h"

#include <QDialog>

enum class EMode : uint8_t;

namespace Ui
{
class W_property;
}

class W_property : public ligarium::IRecordWidget
{
  Q_OBJECT

public:
  explicit W_property(QWidget* parent, ligarium::Database& db, qsizetype id = ligarium::INVALID_ID);
  ~W_property();

  Property            m_rec;
  ligarium::Database& m_db;

  void load() override;
  bool save() override;
  void reset() override;

private:
  Ui::W_property* ui;
};

#endif // W_PROPERTY_H
