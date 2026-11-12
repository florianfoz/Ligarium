#include "ligarium/widget/w_tile_view.h"

#include "ligarium/widget/w_flowlayout.h"
#include "ui_w_tile_view.h"

ligarium::W_Tile_View::W_Tile_View(QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::W_Tile_View)
{
  ui->setupUi(this);
  flowlayout = new W_FlowLayout(ui->content->widget());
  ui->content->addLayout(flowlayout, 0, 0);
}

ligarium::W_Tile_View::~W_Tile_View()
{
  delete ui;
}

void ligarium::W_Tile_View::add_widget(QWidget* widget)
{
  _childrens.append(widget);
  flowlayout->addWidget(widget);
}

void ligarium::W_Tile_View::clear()
{
  while (auto* item = flowlayout->takeAt(0)) {
    if (auto* widget = item->widget()) widget->deleteLater();
    delete item;
  }
}
