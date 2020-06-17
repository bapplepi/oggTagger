#include "oggtagproxy.h"

/*struct tagDataNode {
    int row;

    QPersistentModelIndex fileBaseNode;
    QPersistentModelIndex proxyBaseNode;

    tagDataMap tagData;
};*/

void oggTagProxy::fill(const QString &path)
{

    //std::cout << "dir loaded " << path.toStdString() << std::endl << std::flush;

    tagDataNode* node = new tagDataNode();

    QModelIndex idx = ((QFileSystemModel*) sourceModel())->index(path);

    node->row = idx.row();
    node->fileBaseNode = new QPersistentModelIndex(idx);
    node->proxyBaseNode = new QPersistentModelIndex(QSortFilterProxyModel::mapFromSource(idx));
    node->tagData = new tagDataMap();

    tagData.insert(std::make_pair(path, node));


    QDirIterator it(path, QDir::Files | QDir::AllDirs);

    while(it.hasNext()) {

        it.next();

        node = new tagDataNode();

        idx = ((QFileSystemModel*) sourceModel())->index(it.filePath());

        //std::cout << "idx " << idx.row() << " " << idx.column() << std::endl;

        node->row = idx.row();
        node->fileBaseNode = new QPersistentModelIndex(idx);
        node->proxyBaseNode = new QPersistentModelIndex(QSortFilterProxyModel::mapFromSource(idx));
        node->tagData = new tagDataMap();

        //std::cout << node->fileBaseNode->row() << " " << node->fileBaseNode->column() << std::endl << std::flush;

        tagData.insert(std::make_pair(it.filePath(), node));

        if(it.fileName().contains(".ogg"))
            populateSong(node, it.filePath());
    }

    return;
}


oggTagProxy::oggTagProxy() {}


oggTagProxy::oggTagProxy(int ex) : extraColumns(ex) {}


QModelIndex oggTagProxy::mapToSource(const QModelIndex &proxyIndex) const
{
    if(proxyIndex.column() == 0)
        return QSortFilterProxyModel::mapToSource(proxyIndex);

    return QModelIndex();
}


QString oggTagProxy::filePath(QModelIndex index) const
{
    QModelIndex i = mapToSource(oggTagProxy::index(index.row(), 0, index.parent()));
    return ((QFileSystemModel*) sourceModel())->filePath(i);
}


QModelIndex oggTagProxy::index(int row, int column, const QModelIndex &parent) const
{

    if(column > 0) {
        //std::cout << "B" << std::endl << std::flush;
        QString path = filePath(QSortFilterProxyModel::index(row, 0, parent));
        //std::cout << "C" << std::endl << std::flush;
        if(!path.isNull() && tagData.count(path) > 0)
            //return QSortFilterProxyModel::createIndex(row, column, tagData.at(path));
            return QSortFilterProxyModel::index(row, column, parent);
    }

    return QSortFilterProxyModel::index(row, column, parent);
}


int oggTagProxy::rowCount(const QModelIndex &parent) const
{

    //std::cout << parent.column() << std::endl;
    if(parent.column() == Tag::artist) {
        QVariantList d = data(parent, Qt::DisplayRole).toList();
        std::cout << "list length " << d.size() << std::endl;
    }

    return QSortFilterProxyModel::rowCount(parent);
}


int oggTagProxy::columnCount(const QModelIndex &parent) const
{
    //std::cout << QSortFilterProxyModel::columnCount(parent) + extraColumns << std::endl;
    return QSortFilterProxyModel::columnCount(parent) + extraColumns;
}


bool oggTagProxy::hasChildren(const QModelIndex &parent) const
{
    //std::cout << parent.column() << std::endl;
    if(parent.column() == Tag::artist) {
        if(tagData.count(filePath(parent)) > 0)
            return tagData.at(filePath(parent))->tagData->at(Tag::artist).size();
    }

    return QSortFilterProxyModel::hasChildren(parent);
}


QVariant oggTagProxy::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(section > 0 && (role == Qt::DisplayRole || role == Qt::EditRole)) {
        if(section == Tag::title) return QString("Title");
        if(section == Tag::artist) return QString("Artist");
        if(section == Tag::album) return QString("Album");
    }

    return QSortFilterProxyModel::headerData(section, orientation, role);
}


QVariant oggTagProxy::data(const QModelIndex &index, int role) const
{

    if(index.column() > 0 && index.column() < columnCount()) {
        //std::cout << index.column() << std::endl;
        //std::cout << filePath.toStdString() << std::endl << std::flush;
        if(tagData.count(filePath(index)) > 0) {
            tagDataNode * n = tagData.at(filePath(index));
            if(n->tagData->count(index.column()) > 0) {
                if(n->tagData->at(index.column()).count(role) > 0) {
                    //n->tagData->at(index.column()).at(role);
                    //std::cout << index.row() << " " << index.column() << std::endl << std::flush;
                    //std::cout << n->tagData->at(index.column()).at(role).typeName() << std::endl;
                    //if(strncmp(n->tagData->at(index.column()).at(role).typeName(), "QVariantList", 100) == 0)
                        //std::cout << ((QVariantList) n->tagData->at(index.column()).at(role))
                    return n->tagData->at(index.column()).at(role);
                }
            }
        }
        return QVariant();
    }

    return QSortFilterProxyModel::data(index, role);
}


bool oggTagProxy::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(index.column() == 0)
        return QSortFilterProxyModel::setData(index, value, role);

    //tagDataNode * n = (tagDataNode *) index.internalPointer();
    //std::cout << n->row << std::endl << std::flush;

    QString path = filePath(index);
    Tag tag = (Tag) index.column();

    if((index.column() < columnCount()) && (role == Qt::DisplayRole || role == Qt::EditRole) && tagData.count(path) > 0) {
        //QFileSystemModel* s = (QFileSystemModel*) sourceModel();
        //std::cout << n->fileBaseNode << " " << index.column() << std::endl << std::flush;

        //QPersistentModelIndex* node = n->fileBaseNode;
        //std::cout << "col: " << node->column() << " col count: " << columnCount()
          //        << " ok? " << (index.column() < columnCount()) << std::endl << std::flush;
        //QString p = s->filePath(node->operator const QModelIndex &());
        //std::cout << "A" << std::endl << std::flush;

        //QString path = ((QFileSystemModel*) sourceModel())->filePath(*n->fileBaseNode);
        //std::cout << "train" << std::endl << std::flush;
        if(tagData.at(path)->tagData->count(tag) > 0) {
            tagData.at(path)->tagData->at(tag).erase(Qt::DisplayRole);
            tagData.at(path)->tagData->at(tag).erase(Qt::EditRole);
            //tagData.at(path)->tagData->at(tag).insert(std::make_pair(Qt::DisplayRole, value));
            //tagData.at(path)->tagData->at(tag).insert(std::make_pair(Qt::EditRole, value));
            return editField(path, tag, value);
        }
    }

    return false;


}


Qt::ItemFlags oggTagProxy::flags(const QModelIndex &index) const
{
    if(index.column() == 0)
        return QSortFilterProxyModel::flags(index);

    return(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable);
}


bool oggTagProxy::editField(QString path, Tag tag, const QVariant &newVal)
{
    TagLib::Ogg::Vorbis::File song(QFile::encodeName(path).constData());

    if(!song.isOpen()) {
        std::cout << "failed to open " << path.toStdString() << std::endl << std::flush;
        return false;
    }

    //std::cout << tag << std::endl;

    if(tag == Tag::title)
        song.tag()->setTitle(newVal.toString().toStdString());
    if(tag == Tag::artist)
        song.tag()->setArtist(newVal.toString().toStdString());
    if(tag == Tag::album)
        song.tag()->setAlbum(newVal.toString().toStdString());

    return song.save();
}


void oggTagProxy::populateSong(tagDataNode * tagNode, QString path)
{
    TagLib::Ogg::Vorbis::File song(QFile::encodeName(path).constData());

    if(!song.isOpen()) {
       std::cout << "failed to open " << path.toStdString() << std::endl << std::flush;
       return;
    }

    QStringListModel* title = new QStringListModel();
    //QStringListModel* artists = new QStringListModel();
    //QStringListModel* album = new QStringListModel();

    for(auto tag : song.tag()->fieldListMap()) {
        if(tag.first == "TITLE")
            title->setStringList(QStringList() << QString(tag.second.toString().toCString()));
      /*  else if(tag.first == "ARTIST")
            artists->setStringList(QStringList() << QString(tag.second.toString().toCString()));
        else if(tag.first == "ALBUM")
            artists->setStringList(QStringList() << QString(tag.second.toString().toCString()));*/

    }


   /* std::map<int, QVariant> artistInfo =
    {
        std::make_pair(Qt::DisplayRole, artists),
        std::make_pair(Qt::EditRole, artists)
    };
    std::map<int, QVariant> albumInfo =
    {
        std::make_pair(Qt::DisplayRole, album),
        std::make_pair(Qt::EditRole, album)
    };*/


    QListView* titleView = new QListView();
    titleView->setModel(title);


    std::map<int, QVariant> titleInfo =
    {
        std::make_pair(Qt::DisplayRole, QVariant::fromValue<QListView*>(titleView)),
        std::make_pair(Qt::EditRole, QVariant::fromValue<QListView*>(titleView))
    };


    tagNode->tagData->insert(std::make_pair(Tag::title, titleInfo));
    //tagNode->tagData->insert(std::make_pair(Tag::artist, artistInfo));
    //tagNode->tagData->insert(std::make_pair(Tag::album, albumInfo));


}
