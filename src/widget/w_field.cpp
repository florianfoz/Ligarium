#include "ligarium/widget/w_field.h"

//
#include "ligarium/widget/w_picker.h"
#include "ligarium/widget_registry.h"
#include "ui_w_ligarium_field.h"

namespace ligarium
{


W_Field::W_Field(QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::W_Ligarium_Field)
{
  ui->setupUi(this);
}

W_Field::~W_Field()
{
  delete ui;
}

void W_Field::set_readonly(bool _readonly)
{
}


void W_Field::refresh(Database& db, Table _rec_table)
{
  m_db    = &db;
  m_table = _rec_table;
}

void W_Field::set_record(Database& db, Table _rec_table, qsizetype _id)
{
  m_db    = &db;
  m_table = _rec_table;
  m_id    = _id;

  if (m_table != Table(0) && m_id != INVALID_ID) {
    ui->le_name->setText(WidgetRegistry::dump_record(*m_db, m_table, m_id));
  }

  if (m_id == INVALID_ID) clear();
}

void W_Field::enterEvent(QEnterEvent* event)
{
  hide_tooltip();

  if (m_table != Table(0) && m_id != INVALID_ID) {
    tooltip = WidgetRegistry::new_widget_record(m_table, m_id, *m_db);
  }
}

void W_Field::hide_tooltip()
{
  if (tooltip) {
    tooltip->deleteLater();
    tooltip = nullptr;
  }
}

void W_Field::clear()
{
  ui->le_name->setText(QObject::tr("Click to select a record"));
  m_id = INVALID_ID;
  hide_tooltip();
}


void W_Field::leaveEvent(QEvent* event)
{
  hide_tooltip();
}

void W_Field::mousePressEvent(QMouseEvent* event)
{
  assert(m_table != Table(0) && "Invalid record table");

  if (readonly) return;

  hide_tooltip();

  auto* w = new W_Picker(this, *m_db);
  w->refresh(*m_db, m_table);
  connect(w, &W_Picker::signal_selected, [&](Table rec_table, qsizetype id) {
    W_Field::set_record(*m_db, rec_table, id);
  });
  connect(w, &QDialog::rejected, [this]() { clear(); });
  w->setModal(true);
  w->exec();
}

} // namespace ligarium
