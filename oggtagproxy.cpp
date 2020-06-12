#include "oggtagproxy.h"

void oggTagProxy::fill(const QString &path)
{
    QStringList pathComponents = path.split("/");
    QString dirName = pathComponents.takeLast();

    tagDataNode* node;

    if(path.compare(rootPath) == 0) {
        node = &tagDataRoot;
    }

    else {
        node = new tagDataNode();

        node->row = sourceModel()->index(path).row();
        node->column = 0;

        QString parentPath = path.left(path.size() - dirName.size());

        if(dirs.count(parentPath) > 0)
            node->parent = dirs.at(parentPath);

        dirs.insert(std::make_pair(path, node));

        if(node->parent != NULL)
            node->parent->children.insert(std::make_pair(std::make_pair(node->row, node->column), node));

    }


    QDirIterator it(path, QDir::Files | QDir::AllDirs);

    while(it.hasNext()) {

        it.next();

        QModelIndex baseNode = sourceModel()->index(it.filePath());

        tagDataNode * n = new tagDataNode();
        n->row = sourceModel()->index(it.filePath()).row();
        n->column = 0;
        n->parent = node;

        std::pair<int, int> coords(n->row, n->column);
        if(dirs.count(dirName) > 0)
            dirs.at(dirName)->children.insert(std::make_pair(coords, n));

        //n->baseinternalPointer = sourceModel()->index(n->path).internalPointer();

        if(it.fileName().contains(".ogg"))
            populateSong(n, it.filePath());
    }

    return;
}


bool oggTagProxy::setData(const QModelIndex &index, const QVariant &value, int role)
{
    std::cout << "setdata" << std::endl;
    return QSortFilterProxyModel::setData(index, value, role);
}


oggTagProxy::oggTagProxy() {}


oggTagProxy::oggTagProxy(int extra, std::set<int> valid, QString root) : extraColumns(extra), QSortFilterProxyModel(),
                                                                             rootPath(root)
{
    validColumns = valid;
    tagDataRoot.row = 0;
    tagDataRoot.column = 0;
    tagDataRoot.data = NULL;
    tagDataRoot.parent = NULL;

    dirs.insert(std::make_pair(rootPath, &tagDataRoot));
}


QFileSystemModel* oggTagProxy::sourceModel() const
{
    QAbstractItemModel *m = QSortFilterProxyModel::sourceModel();
    if(m != NULL)
        return dynamic_cast<QFileSystemModel*>(m);
    return NULL;
}


QModelIndex oggTagProxy::getParentFromNode(void* ptr) const
{
    if(ptr == NULL) {
        QModelIndex i = QModelIndex();
        std::cout << "get parent rtns " << i.row() << " " << i.column() << std::endl;
        return i;
    }

    tagDataNode* n = (tagDataNode*) ptr;
    //std::cout << "get parent rtns " << n->parent->row << " " << n->parent->column << std::endl;

    return createIndex(n->parent->row, n->parent->column, n->parent);

}


QModelIndex oggTagProxy::parent(const QModelIndex &child) const
{
   /* QFileSystemModel* m = sourceModel();
    if(child.column() < m->columnCount())
        return QSortFilterProxyModel::parent(child);

    QModelIndex p = QSortFilterProxyModel::parent(QSortFilterProxyModel::index(child.row(), 0));

    //std::cout << "index is " << child.row() << " " << child.column() << "      parent of col 0 is " << p.data().toString().toStdString() << std::endl;
    return QSortFilterProxyModel::parent(index(child.row(), 1, p));*/

    return getParentFromNode(child.internalPointer());
}



QModelIndex oggTagProxy::sibling(int row, int column, const QModelIndex &idx) const
{
    QModelIndex i = QModelIndex();
    std::cout << row << " " << column << i.row() << " " << i.column() << std::endl << std::flush;
    return i;
}


QModelIndex oggTagProxy::index(tagDataNode * node) const
{
    if(node->parent != NULL)
        return index(node->row, node->column, index(node->parent));
    return index(node->row, node->column, QModelIndex());
}


QModelIndex oggTagProxy::index(int row, int column, const QModelIndex &parent) const
{
    //std::cout << row << " " << column << std::endl;

    /*if(column < sourceModel()->columnCount())
        return QSortFilterProxyModel::index(row, column, parent);

    //std::cout << "parent is \'" << parent.data().toString().toStdString() << "\' and is valid? " << parent.isValid()
      //        << "          row is " << row << " and col is " << column << std::endl << std::flush;

    if(parent.isValid() && parent.data().toString() != NULL && dirs.count(parent.data().toString())) {
        tagDataNode* dir = dirs.at(parent.data().toString());

        if(dir->children.size() > row) {
            tagDataNode* dat = (dir->children)[row];
            return createIndex(row, column, dat);
        }
    }*/


    if(row >= 0 && row < rowCount(parent) && column >= 0 && column < columnCount(parent)) {
        tagDataNode parentNode;

        if(!parent.isValid())
            parentNode = tagDataRoot;
        else
            parentNode = *((tagDataNode*) parent.internalPointer());

        tagDataNode* child = (tagDataNode*) parent.internalPointer();

        return createIndex(row, column, child);
    }

    return QModelIndex();
}


int oggTagProxy::columnCount(const QModelIndex &parent) const
{
    return QSortFilterProxyModel::columnCount() + extraColumns;
}


QModelIndex oggTagProxy::mapToSource(const QModelIndex &proxyIndex) const
{
    if(proxyIndex.column() > sourceModel()->columnCount() || proxyIndex.column() < 0
            || proxyIndex.row() < 0 || proxyIndex.internalPointer() == NULL)
        return QModelIndex();

    QString fileName;

    if(proxyIndex.column() == 0)
        fileName = *((QString*) proxyIndex.internalPointer());
    else
        fileName = *((QString*) proxyIndex.parent().internalPointer());


    QModelIndex sourceFileIndex = sourceModel()->index(fileName);
    return sourceModel()->index(sourceFileIndex.row(), sourceFileIndex.column(),
                                sourceFileIndex.parent());
}


QModelIndex oggTagProxy::mapFromSource(const QModelIndex &sourceIndex) const
{
    if(sourceIndex.isValid()) {
        QString parentPath = sourceIndex.parent().data().toString();
        if(dirs.count(parentPath) > 0) {
            tagDataNode* parent = dirs.at(parentPath);
            if(parent->children.count(std::make_pair(sourceIndex.row(),
                                                     sourceIndex.column())) > 0) {
                tagDataNode* node = parent->children.at(std::make_pair(sourceIndex.row(),
                                                                   sourceIndex.column()));
                return index(node);
            }
        }
    }

    return QModelIndex();
}


QVariant oggTagProxy::data(const QModelIndex &index, int role) const
{
    if(validColumns.count(index.column()) <= 0)
        return QVariant();

    if(index.internalPointer() == NULL)
        return QVariant();

    tagDataNode * node = (tagDataNode *) index.internalPointer();

    tagDataMap * tags = (tagDataMap *) node->data;

    if(tags->count(role) > 0)
        return tags->at(role);

    return QVariant();
}


Qt::ItemFlags oggTagProxy::flags(const QModelIndex &index) const
{
    if(index.column() < sourceModel()->columnCount())
        return QSortFilterProxyModel::flags(index);

    return(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable);
}


void oggTagProxy::setSource(QFileSystemModel *sourceModel)
{
    QSortFilterProxyModel::setSourceModel(sourceModel);
    QObject::connect(sourceModel, &QFileSystemModel::directoryLoaded,
                     this, &oggTagProxy::fill);
    //tagDataRoot.name = sourceModel->rootPath().split("/").takeLast();
}


void oggTagProxy::populateSong(tagDataNode * leftmostNode, QString path)
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

   leftmostNode->children.insert(std::make_pair(std::make_pair(leftmostNode->row, int(Tag::title)), node));

   node->data = &titleInfo;

}
