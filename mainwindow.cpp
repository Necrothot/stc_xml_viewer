#include "mainwindow.h"

#include <QCoreApplication>
#include <QWidget>
#include <QLayout>
#include <QPushButton>
#include <QTableView>
#include <QDir>
#include <QFileInfoList>
#include <QFileDialog>
#include <QMessageBox>

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

    text_editor_model_ = new TextEditorModel(this);

    table_view_ = new QTableView(this);
    table_view_->setModel(text_editor_model_);
    table_view_->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(table_view_, &QTableView::customContextMenuRequested,
                     this, &MainWindow::tableContextMenu);

    table_context_menu_ = new QMenu(this);

    QAction *add_row_action = new QAction("Add row", this);
    table_context_menu_->addAction(add_row_action);
    QObject::connect(add_row_action, &QAction::triggered, this, [=] () {
        text_editor_model_->insertIntoDb(global_def::ColumnValues());
    });

    QAction *remove_row_action = new QAction("Remove row", this);
    table_context_menu_->addAction(remove_row_action);
    QObject::connect(remove_row_action, &QAction::triggered, this, [=] () {
        if (context_menu_index_.isValid())
            text_editor_model_->removeRow(context_menu_index_.row());
    });

    QAction *save_to_file_action = new QAction("Save to file", this);
    table_context_menu_->addAction(save_to_file_action);
    QObject::connect(save_to_file_action, &QAction::triggered, this, [=] () {
        QString name = QFileDialog::getSaveFileName(this,
                                                    "Save File",
                                                    ".",
                                                    "XML file (*.xml)");
        if (name.isEmpty())
            return;

        if (context_menu_index_.isValid())
        {
            text_editor_model_->saveRowToFile(name,
                                              context_menu_index_.row());
        }
    });



    QVBoxLayout *main_layout = new QVBoxLayout;
    main_layout->addLayout(button_layout);
    main_layout->addWidget(table_view_);

    QWidget *main_widget = new QWidget(this);
    main_widget->setLayout(main_layout);
    setCentralWidget(main_widget);

    file_load_dialog_ = new FileLoadDialog(this);

    QObject::connect(open_button, &QPushButton::clicked,
                     this, &MainWindow::openFiles);
    QObject::connect(clear_db_button, &QPushButton::clicked,
                     this, &MainWindow::clearDb);
    QObject::connect(quit_button, &QPushButton::clicked,
                     this, &MainWindow::quitApp);

    QObject::connect(text_editor_model_, &TextEditorModel::fileReadStatusSignal,
                     file_load_dialog_, &FileLoadDialog::setFileReadStatus);

    QObject::connect(text_editor_model_, &TextEditorModel::fileWriteStatusSignal,
                     this, [=] (bool success, const QString &name,
                                const QString &status) {
        QFileInfo fi(name);

        QMessageBox message_box;
        message_box.setWindowTitle("Save file");

        QString msg;
        if (success)
            msg = "Saved to " + fi.fileName();
        else
            msg = "Error (" + fi.fileName() + "): " + status;

        message_box.setText(msg);
        message_box.exec();
    });
}

void MainWindow::openFiles()
{
    QString path = QFileDialog::getExistingDirectory(this,
                                                     "Open XML Directory",
                                                     ".");
    QDir dir(path);

    QFileInfoList file_list = dir.entryInfoList();

    // Get XML files list
    QFileInfoList xml_file_list;
    for (auto &f: file_list)
    {
        if (f.completeSuffix() == "xml")
            xml_file_list.push_back(f);
    }

    file_load_dialog_->reset();
    file_load_dialog_->setFilesCount(xml_file_list.size());
    file_load_dialog_->show();

    for (auto &f: xml_file_list)
        text_editor_model_->readFileIntoDb(f.canonicalFilePath());
}

void MainWindow::clearDb()
{
    text_editor_model_->clearDb();
}

void MainWindow::quitApp()
{
    QCoreApplication::quit();
}

void MainWindow::tableContextMenu(QPoint pos)
{
    context_menu_index_ = table_view_->indexAt(pos);
    table_context_menu_->popup(table_view_->viewport()->mapToGlobal(pos));
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
