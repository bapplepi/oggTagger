#include "oggview.h"

oggView::oggView()
{

}

void oggView::h(QMouseEvent *e) {
    std::cout << "Clicked: " << e->x() <<","<<e->y() << std::endl;
    std::cout << geometry().width() << " by " << geometry().height() << std::endl;
}

oggView::oggView(QWidget* parent) : QTreeView(parent)
{
    //this->setItemDelegate(new ItemDelegate());
    //connect(this,&oggView::mouseMoveEvent,this,&oggView::h);
}

void oggView::mouseMoveEvent(QMouseEvent *e)
{
    std::cout << "mouseover: " << e->x() <<","<<e->y() << std::endl;
    std::cout << geometry().width() << " by " << geometry().height() << std::endl;
    QTreeView::mouseMoveEvent(e);
}

void oggView::mouseReleaseEvent(QMouseEvent *e)
{
    QModelIndex index = indexAt(e->localPos().toPoint());

    std::cout << "Clicked: " << e->x() <<","<<e->y() << std::endl;
    std::cout << "Index " << index.row() << "," << index.column() << std::endl;
    std::cout << geometry().width() << " by " << geometry().height() << std::endl;
    //std::cout << "num children = " << children().size() << std::endl;
    //std::cout << "viewport s = " << viewport()->width() << " by " << viewport()->height() << std::endl;
    //std::cout << "viewport children " << viewport()-> << std::endl;
    std::string indt = "  ";


/*
    std::deque<std::pair<int, QObject*>> q;
    for(int i = 0; i < children().size(); i++) {
        q.push_back(std::make_pair(0, children().toVector()[i]));
    }

    while(!q.empty()) {
        QObject * n = q.front().second;
        int ind = q.front().first;
        q.pop_front();

        if(n->children().size() > 0) {
            for(int i = 0; i < n->children().size(); i++) {
                q.push_front(std::make_pair(ind + 1, n->children().toVector()[i]));
            }
        }

        for(int i = 0; i < ind; i++)
            std::cout << indt;

        //std::cout << n->objectName().toStdString() << " with " << n->children().size()
          //        << " children, children rect " << n->fi


    }*/
}


QSize oggView::viewportSizeHint() const
{
    return QSize(1400, 760);
}

QModelIndex oggView::indexAt(const QPoint &p) const
{
    QHeaderView* h = header();
    int col = columnAt(p.x());

    //std::cout << "clicked " << columnAt(p.x()) << " " << h->logicalIndexAt(p) << std::endl;

    //model()->index()

    return QTreeView::indexAt(p);
}
