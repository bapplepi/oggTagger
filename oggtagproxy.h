#ifndef OGGTAGPROXY_H
#define OGGTAGPROXY_H

#include <QObject>
#include <QSortFilterProxyModel>
#include <QFileSystemModel>
#include <QDirIterator>
#include <QTableWidget>
#include <QStandardItem>
#include <QStringListModel>
#include <QListView>
#include <iostream>
#include <map>
#include <queue>
#include <set>

#include "tree.h"
#include "ogg/vorbis/vorbisfile.h"

//Q_DECLARE_METATYPE(QStringListModel *)

enum Tag {
    title=1, artist=2, album=3, genre=4, track=5, year=6
};


using tagDataMap = std::map<int, std::map<int, QVariant>>;


struct tagDataNode {
    int row;

    QPersistentModelIndex* fileBaseNode = NULL;
    QPersistentModelIndex* proxyBaseNode = NULL;

    //std::map<int, tagDataNode*> * children;

    tagDataMap* tagData = NULL;
};


class oggTagProxy : public QSortFilterProxyModel
{
    Q_OBJECT
public slots:
    void fill(const QString &path);

public:
    oggTagProxy();
    oggTagProxy(int ex);
    //oggTagProxy(int extra, std::set<int> valid, QString rootPath);
    //QModelIndex parent(const QModelIndex &child) const override;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;
    //QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;
    //QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    //bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    //void setSource(QFileSystemModel *sourceModel);
    //QFileSystemModel* sourceModel() const;

    void populateSong(tagDataNode* node, QString path);
    bool editField(QString path, Tag role, const QVariant &newVal);

private:
    QString filePath(QModelIndex index) const;
    //QModelIndex index(tagDataNode * node) const;
    //QString rootPath;
    //maps directory names to nodes
    std::map<QString, tagDataNode*> tagData;
    int extraColumns=0;
    //std::set<int> validColumns;
};

#endif // OGGTAGPROXY_H
