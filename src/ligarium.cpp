#include "ligarium/ligarium.h"

#include "ligarium/widget_registry.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDateEdit>
#include <QDateTimeEdit>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QSlider>
#include <QSpinBox>
#include <QTextEdit>
#include <QTimeEdit>
#include <QVBoxLayout>
#include <QWidget>

namespace ligarium
{


void setWidgetMode(QWidget* parent, WidgetMode mode)
{
  if (!parent) return;

  const bool readOnly = (mode == WidgetMode::ReadOnly);

  // Récupération de tous les widgets enfants, récursivement
  const auto widgets = parent->findChildren<QWidget*>();

  for (QWidget* widget : widgets) {
    if (auto* lineEdit = qobject_cast<QLineEdit*>(widget)) {
      lineEdit->setReadOnly(readOnly);
    } else if (auto* textEdit = qobject_cast<QTextEdit*>(widget)) {
      textEdit->setReadOnly(readOnly);
    } else if (auto* plainTextEdit = qobject_cast<QPlainTextEdit*>(widget)) {
      plainTextEdit->setReadOnly(readOnly);
    } else if (auto* spinBox = qobject_cast<QSpinBox*>(widget)) {
      spinBox->setReadOnly(readOnly);
    } else if (auto* doubleSpinBox = qobject_cast<QDoubleSpinBox*>(widget)) {
      doubleSpinBox->setReadOnly(readOnly);
    } else if (auto* comboBox = qobject_cast<QComboBox*>(widget)) {
      comboBox->setEnabled(!readOnly);
    } else if (auto* checkBox = qobject_cast<QCheckBox*>(widget)) {
      checkBox->setEnabled(!readOnly);
    } else if (auto* radioButton = qobject_cast<QRadioButton*>(widget)) {
      radioButton->setEnabled(!readOnly);
    } else if (auto* button = qobject_cast<QPushButton*>(widget)) {
      button->setEnabled(!readOnly);
    } else if (auto* slider = qobject_cast<QSlider*>(widget)) {
      slider->setEnabled(!readOnly);
    } else if (auto* dateEdit = qobject_cast<QDateEdit*>(widget)) {
      dateEdit->setReadOnly(readOnly);
    } else if (auto* timeEdit = qobject_cast<QTimeEdit*>(widget)) {
      timeEdit->setReadOnly(readOnly);
    } else if (auto* dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget)) {
      dateTimeEdit->setReadOnly(readOnly);
    }
  }
}

bool exec_widget_dialog(QWidget* parent, IRecordWidget* widget, const QString& title)
{
  if (!widget) return false;

  QDialog dialog(parent);
  dialog.setWindowTitle(title);
  dialog.setModal(true);

  auto* layout = new QVBoxLayout(&dialog);
  layout->addWidget(widget);

  auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);

  layout->addWidget(buttons);

  QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog, [&]() {
    if (widget->save()) dialog.accept();
  });

  QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

  return dialog.exec() == QDialog::Accepted;
}

} // namespace ligarium
