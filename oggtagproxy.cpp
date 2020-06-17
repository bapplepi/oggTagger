#include "oggtagproxy.h"

/*struct tagDataNode {
    int row;

    QPersistentModelIndex fileBaseNode;
    QPersistentModelIndex proxyBaseNode;

    tagDataMap tagData;
};*/

void oggTagProxy::fill(const QString &path)
{

    std::cout << "dir loaded " << path.toStdString() << std::endl << std::flush;

    tagDataNode* node = new tagDataNode();

    QModelIndex idx = ((QFileSystemModel*) sourceModel())->index(path);

    node->row = idx.row();
    node->fileBaseNode = QPersistentModelIndex(idx);
    node->proxyBaseNode = QPersistentModelIndex(QSortFilterProxyModel::mapFromSource(idx));
    node->tagData = new tagDataMap();

    tagData.insert(std::make_pair(path, node));


    QDirIterator it(path, QDir::Files | QDir::AllDirs);

    while(it.hasNext()) {

        it.next();

        node = new tagDataNode();

        idx = ((QFileSystemModel*) sourceModel())->index(it.filePath());

        node->row = idx.row();
        node->fileBaseNode = QPersistentModelIndex(idx);
        node->proxyBaseNode = QPersistentModelIndex(QSortFilterProxyModel::mapFromSource(idx));
        node->tagData = new tagDataMap();

        tagData.insert(std::make_pair(it.filePath(), node));

        if(it.fileName().contains(".ogg"))
            populateSong(node, it.filePath());
    }

    return;
}

oggTagProxy::oggTagProxy() {


}


/*oggTagProxy::oggTagProxy(int extra, std::set<int> valid, QString root) : extraColumns(extra), QSortFilterProxyModel(),
                                                                             rootPath(root)
{
    validColumns = valid;
    tagDataRoot.row = 0;
    tagDataRoot.column = 0;
    tagDataRoot.data = NULL;
    tagDataRoot.parent = NULL;

    dirs.insert(std::make_pair(rootPath, &tagDataRoot));
}*/

QModelIndex oggTagProxy::parent(const QModelIndex &child) const
{
    if(child.internalPointer() != NULL) {
        QModelIndex baseChild = ((tagDataNode*) child.internalPointer())->fileBaseNode;
        QModelIndex baseParent = sourceModel()->parent(baseChild);
        return mapFromSource(baseParent);

    }

    return QModelIndex();
}


QModelIndex oggTagProxy::index(int row, int column, const QModelIndex &parent) const
{
    if(row >= 0 && column >= 0 && row < rowCount(parent) && column < columnCount(parent)) {
        //std::cout << row << " " << column << std::endl << std::flush;
        QModelIndex proxyIndex;

        if(parent.internalPointer() != NULL) {
            tagDataNode* p = (tagDataNode*) parent.internalPointer();
            proxyIndex = QSortFilterProxyModel::index(row, column, p->proxyBaseNode);
        }
        else
            proxyIndex = QSortFilterProxyModel::index(row, column, QModelIndex());

        QModelIndex sourceIndex = QSortFilterProxyModel::mapToSource(proxyIndex);
        QString filePath = ((QFileSystemModel*) sourceModel())->filePath(sourceIndex);

        //std::cout << filePath.toStdString() << std::endl << std::flush;

        if(tagData.count(filePath) > 0) {
            tagDataNode* n = tagData.at(filePath);
            return createIndex(row, column, n);
        }
        else
            return createIndex(row, column, (void *) NULL);
    }

    return QModelIndex();
}


int oggTagProxy::columnCount(const QModelIndex &parent) const
{
    return QSortFilterProxyModel::columnCount(parent) + extraColumns;
}

//assume index belongs to our model
QModelIndex oggTagProxy::mapToSource(const QModelIndex &proxyIndex) const
{
    if(proxyIndex.column() < sourceModel()->columnCount()) {
        tagDataNode* n = (tagDataNode*) proxyIndex.internalPointer();

        if(n != NULL && n->fileBaseNode.isValid()) {
            QModelIndex sourceParent = n->fileBaseNode.parent();

            return sourceModel()->index(n->fileBaseNode.row(), proxyIndex.column(), sourceParent);
        }
    }

    return QModelIndex();
}


QModelIndex oggTagProxy::mapFromSource(const QModelIndex &sourceIndex) const
{

    if(sourceIndex.isValid()) {
        QString filePath = ((QFileSystemModel*) sourceModel())->fileName(sourceIndex);

        if(tagData.count(filePath) > 0) {
            tagDataNode * n = tagData.at(filePath);
            if(n != NULL)
                return createIndex(n->row, sourceIndex.column(), n);
        }
    }

    return QModelIndex();
}


QVariant oggTagProxy::data(const QModelIndex &index, int role) const
{
    tagDataNode* n = (tagDataNode*) index.internalPointer();

    if(index.internalPointer() != NULL) {
        if(index.column() < sourceModel()->columnCount()) {
            QModelIndex sourceIndex = sourceModel()->index(n->fileBaseNode.row(), index.column(),
                                                           n->fileBaseNode.parent());
            return sourceIndex.data();
        }

        if(n->tagData->count(index.column() > 0)) {
            if(n->tagData->at(index.column()).count(role) > 0)
                return n->tagData->at(index.column()).at(role);
        }

    }


    return QVariant();

}


Qt::ItemFlags oggTagProxy::flags(const QModelIndex &index) const
{
    if(index.column() < sourceModel()->columnCount())
        return QSortFilterProxyModel::flags(index);

    return(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable);
}


void oggTagProxy::populateSong(tagDataNode * tagNode, QString path)
{
   TagLib::Ogg::Vorbis::File song(QFile::encodeName(path).constData());

   if(!song.isOpen()) {
       std::cout << "failed to open " << path.toStdString() << std::endl << std::flush;
       return;
   }

   tagDataNode * node = new tagDataNode();

   std::map<int, QVariant> titleInfo;

   if(song.tag()->contains("TITLE")) {
       titleInfo = {
           std::make_pair(Qt::DisplayRole, QString(song.tag()->title().toCString())),
           std::make_pair(Qt::EditRole, QString(song.tag()->title().toCString())),
       };
   }

   else {
       titleInfo = {
           std::make_pair(Qt::DisplayRole, QString("")),
           std::make_pair(Qt::EditRole, QString("")),
           //std::make_pair(Qt::FontRole, QString("")),
           //std::make_pair(Qt::ForegroundRole, QString(""))
       };
   }

   tagNode->tagData->insert(std::make_pair(Tag::title, titleInfo));


}
