#include "widget/utils/w_tile_view.h"

#include "ui_w_tile_view.h"
#include "widget/w_flowlayout.h"

W_Tile_View::W_Tile_View(QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::W_Tile_View)
{
  ui->setupUi(this);
  flowlayout = new W_FlowLayout(ui->content->widget());
  ui->content->addLayout(flowlayout, 0, 0);
}

W_Tile_View::~W_Tile_View()
{
  delete ui;
}

void W_Tile_View::add_widget(QWidget* widget)
{
  _childrens.append(widget);
  flowlayout->addWidget(widget);
}

void W_Tile_View::clear()
{
  while (auto* item = flowlayout->takeAt(0)) {
    if (auto* widget = item->widget()) widget->deleteLater();
    delete item;
  }
}
