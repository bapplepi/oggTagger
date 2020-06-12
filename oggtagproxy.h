#ifndef OGGTAGPROXY_H
#define OGGTAGPROXY_H

#include <QObject>
#include <QSortFilterProxyModel>
#include <QFileSystemModel>
#include <QDirIterator>
#include <iostream>
#include <map>
#include <queue>
#include <set>

#include "tree.h"
#include "ogg/vorbis/vorbisfile.h"

typedef std::pair<int, int> coords;

enum Tag {
    title=4, artist=5, album=6, albumArtist, genre, track, year
};


using tagDataMap = std::map<int, QVariant>;


struct tagDataNode {
    int row;
    int column;
    //quintptr baseInternalId;
    tagDataNode* parent;

    void * data;

    //QString path;
    //tagDataMap tagData;
    std::map<std::pair<int, int>, tagDataNode*> children;
};


class oggTagProxy : public QSortFilterProxyModel
{
    Q_OBJECT
public slots:
    void fill(const QString &path);

public:
    oggTagProxy();
    oggTagProxy(int extra, std::set<int> valid, QString rootPath);
    QModelIndex parent(const QModelIndex &child) const override;
    QModelIndex sibling(int row, int column, const QModelIndex &idx) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    void setSource(QFileSystemModel *sourceModel);
    QFileSystemModel* sourceModel() const;

    void populateSong(tagDataNode* node, QString path);

private:
    QModelIndex index(tagDataNode * node) const;
    QModelIndex getParentFromNode(void * n) const;
    //maps name to children/tag data
    tagDataNode tagDataRoot;
    QString rootPath;
    //maps directory names to nodes
    std::map<QString, tagDataNode*> dirs;
    int extraColumns=0;
    int extraColumnsStart=0;
    std::set<int> validColumns;
};

#endif // OGGTAGPROXY_H
