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

    std::set<int> valid = {0, 4};

    //oggTagProxy* songs = new oggTagProxy(1, valid, "C:/Users/Prime/Music/");
    oggTagProxy* songs = new oggTagProxy();
    //songs->setSource(fileModel);
    songs->setSourceModel(fileModel);

    oggView* songsView = new oggView(this);

    //QLabel* l = new QLabel(tr("h,jhkjh,kjhkjhlkjhlkjhlkjhklhjk"));

    setCentralWidget(songsView);
    //setCentralWidget(l);

    songsView->setSortingEnabled(true);
    songsView->setGeometry(0, 40, 1400, 760);

    songsView->setModel(songs);

    songsView->setRootIndex(songs->mapFromSource(fileModel->index("C:/Users/Prime/Music/")));

    songsView->setEditTriggers(QTreeView::NoEditTriggers);

    QHeaderView* h = songsView->header();
    h->setSectionResizeMode(0, QHeaderView::Stretch);

    h->setSectionHidden(1, true);
    h->setSectionHidden(2, true);
    h->setSectionHidden(3, true);

    new QAbstractItemModelTester(songs, QAbstractItemModelTester::FailureReportingMode::Warning, this);



    //songsView->resizeColumnToContents(1);


    songsView->show();

    //show();
}

MainWindow::~MainWindow()
{

}
