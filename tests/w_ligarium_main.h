#ifndef W_LIGARIUM_MAIN_H
#define W_LIGARIUM_MAIN_H

#include <QMainWindow>

class QResizeEvent;
class QPushButton;

QT_BEGIN_NAMESPACE
namespace Ui
{
class W_Ligarium_Main;
}
QT_END_NAMESPACE

class W_Ligarium_Main : public QMainWindow
{
  Q_OBJECT

public:
  W_Ligarium_Main(QWidget* parent = nullptr);
  ~W_Ligarium_Main();

private:
  Ui::W_Ligarium_Main* ui;
};

#endif // W_LIGARIUM_MAIN_H
