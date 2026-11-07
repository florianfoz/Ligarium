// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef W_FLOWLAYOUT_H
#define W_FLOWLAYOUT_H

#include <QLayout>
#include <QRect>
#include <QStyle>
//! [0]
class W_FlowLayout : public QLayout
{
public:
  explicit W_FlowLayout(QWidget* parent, int margin = -1, int hSpacing = -1, int vSpacing = -1);
  explicit W_FlowLayout(int margin = -1, int hSpacing = -1, int vSpacing = -1);
  ~W_FlowLayout();

  void                           addItem(QLayoutItem* item) override;
  [[nodiscard]] int              horizontalSpacing() const;
  [[nodiscard]] int              verticalSpacing() const;
  [[nodiscard]] Qt::Orientations expandingDirections() const override;
  [[nodiscard]] bool             hasHeightForWidth() const override;
  [[nodiscard]] int              heightForWidth(int index) const override;
  [[nodiscard]] int              count() const override;
  [[nodiscard]] QLayoutItem*     itemAt(int index) const override;
  [[nodiscard]] QSize            minimumSize() const override;
  void                           setGeometry(const QRect& rect) override;
  [[nodiscard]] QSize            sizeHint() const override;
  [[nodiscard]] QLayoutItem*     takeAt(int index) override;

private:
  int doLayout(const QRect& rect, bool testOnly) const;
  int smartSpacing(QStyle::PixelMetric pm) const;

  QList<QLayoutItem*> itemList;
  int                 m_hSpace;
  int                 m_vSpace;
};
//! [0]

#endif // W_FLOWLAYOUT_H
