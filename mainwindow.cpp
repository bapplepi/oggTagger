#include "mainwindow.h"

#include <QLabel>

#include <QAbstractItemModelTester>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    //, ui(new Ui::MainWindow)
{
    //ui->setupUi(this);

    setGeometry(200, 200, 1400, 800);
    setWindowTitle("ヤバすぎる痛みを与えてやる");

    QFileSystemModel* fileModel = new QFileSystemModel();
    fileModel->setRootPath("C:/Users/Prime/Music/");
    fileModel->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    fileModel->setReadOnly(false);

    //oggTagProxy* songs = new oggTagProxy(1, valid, "C:/Users/Prime/Music/");
    oggTagProxy* songs = new oggTagProxy(0);
    //songs->setSource(fileModel);
    songs->setSourceModel(fileModel);
    //songs->setHeaderData(1, Qt::Horizontal, QString("Title"), Qt::EditRole);

    oggView* songsView = new oggView(this);

    songsView->setUniformRowHeights(false);

    //QTreeView* songsView = new QTreeView(this);

    //QLabel* l = new QLabel(tr("h,jhkjh,kjhkjhlkjhlkjhlkjhklhjk"));

    setCentralWidget(songsView);
    //setCentralWidget(l);

    songsView->setSortingEnabled(true);
    songsView->setGeometry(0, 50, 1400, 750);

    songsView->setModel(songs);

    songsView->setRootIndex(songs->mapFromSource(fileModel->index("C:/Users/Prime/Music/")));

    QObject::connect(fileModel, &QFileSystemModel::directoryLoaded, songs, &oggTagProxy::fill);

    //songsView->setEditTriggers(QTreeView::NoEditTriggers);

    QHeaderView* h = songsView->header();
    h->setSectionResizeMode(0, QHeaderView::Stretch);

    //new QAbstractItemModelTester(songs, QAbstractItemModelTester::FailureReportingMode::Warning, this);



    //songsView->resizeColumnToContents(1);


    songsView->show();

    //show();
}

MainWindow::~MainWindow()
{

}
