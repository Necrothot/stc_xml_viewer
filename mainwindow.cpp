#include "mainwindow.h"

#include <QCoreApplication>
#include <QWidget>
#include <QLayout>
#include <QPushButton>
#include <QTableView>
#include <QDir>
#include <QFileInfoList>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QPushButton *open_button = new QPushButton("Open", this);
    open_button->setFixedSize(70, 25);

    QPushButton *clear_db_button = new QPushButton("Clear DB", this);
    clear_db_button->setFixedSize(70, 25);

    QPushButton *quit_button = new QPushButton("Quit", this);
    quit_button->setFixedSize(70, 25);

    QHBoxLayout *button_layout = new QHBoxLayout;
    button_layout->addWidget(open_button);
    button_layout->addWidget(clear_db_button);
    button_layout->addWidget(quit_button);
    button_layout->addStretch(1);

    text_editor_model = new TextEditorModel(this);

    table_view = new QTableView(this);
    table_view->setModel(text_editor_model);

    QVBoxLayout *main_layout = new QVBoxLayout;
    main_layout->addLayout(button_layout);
    main_layout->addWidget(table_view);

    QWidget *main_widget = new QWidget(this);
    main_widget->setLayout(main_layout);
    setCentralWidget(main_widget);

    file_load_dialog = new FileLoadDialog(this);

    QObject::connect(open_button, &QPushButton::clicked,
                     this, &MainWindow::openFiles);
    QObject::connect(clear_db_button, &QPushButton::clicked,
                     this, &MainWindow::clearDb);
    QObject::connect(quit_button, &QPushButton::clicked,
                     this, &MainWindow::quitApp);

    QObject::connect(text_editor_model, &TextEditorModel::fileReadSignal,
                     file_load_dialog, &FileLoadDialog::addErrorString);
    QObject::connect(text_editor_model, &TextEditorModel::fileProgressSignal,
                     file_load_dialog, &FileLoadDialog::incProgress);
}

void MainWindow::openFiles()
{
    // TODO: let user select desired folder
    QString path = QCoreApplication::applicationDirPath() + "/TestFolder";
    QDir dir(path);

    QFileInfoList file_list = dir.entryInfoList();

    // Get XML files list
    QFileInfoList xml_file_list;
    for (auto &f: file_list)
    {
        if (f.completeSuffix() == "xml")
            xml_file_list.push_back(f);
    }

    file_load_dialog->reset();
    file_load_dialog->setFilesCount(xml_file_list.size());
    file_load_dialog->show();

    for (auto &f: xml_file_list)
        text_editor_model->readFileIntoDb(f.canonicalFilePath());
}

void MainWindow::clearDb()
{
    text_editor_model->clearDb();
}

void MainWindow::quitApp()
{
    QCoreApplication::quit();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Escape:
        quitApp();
        break;
    }
}
