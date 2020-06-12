#ifndef OGGVIEW_H
#define OGGVIEW_H

#include <QWidget>
#include <QMouseEvent>
#include <QTreeView>
#include <QHeaderView>
#include <QScrollArea>
#include <iostream>
#include <queue>

class oggView : public QTreeView
{
    Q_OBJECT

public:
    oggView();
    oggView(QWidget* parent=nullptr);
    //void mouseReleaseEvent(QMouseEvent *e) override;
    QSize viewportSizeHint() const override;
    QModelIndex indexAt(const QPoint &p) const override;

    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;

    void h(QMouseEvent *e);

};

#endif // OGGVIEW_H
