#include "mainwindow.h"

#include <QDebug>
#include <QCoreApplication>
#include <QWidget>
#include <QLayout>
#include <QTableView>

#include "text_editor_model.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    TextEditorModel *text_editor_model = new TextEditorModel(this);
    text_editor_model->readFilesFromDir(QCoreApplication::applicationDirPath() +
                                        "/TestFolder");

    QTableView *table_view = new QTableView(this);
    table_view->setModel(text_editor_model);


    QVBoxLayout *main_layout = new QVBoxLayout;
    main_layout->addWidget(table_view);

    QWidget *main_widget = new QWidget(this);
    main_widget->setLayout(main_layout);
    setCentralWidget(main_widget);
}

MainWindow::~MainWindow()
{
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Escape:
        QCoreApplication::quit();
        break;
    }
}
