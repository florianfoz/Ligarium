#ifndef W_TILE_VIEW_H
#define W_TILE_VIEW_H

#include <QWidget>

class W_FlowLayout;

namespace Ui
{
class W_Tile_View;
}

class W_Tile_View : public QWidget
{
  Q_OBJECT

public:
  explicit W_Tile_View(QWidget* parent = nullptr);
  ~W_Tile_View();

  void         set_column_size(int size);
  void         add_widget(QWidget* widget);
  void         clear();
  QWidgetList& childrens()
  {
    return _childrens;
  }

  W_FlowLayout* flowlayout = nullptr;

private:
  Ui::W_Tile_View* ui;
  QWidgetList      _childrens;
};

#endif // W_TILE_VIEW_H
