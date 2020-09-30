#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QTableView>

#include "text_editor_model.h"
#include "file_load_dialog.h"

class MainWindow : public QMainWindow
{
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override {}

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    Q_OBJECT

    QTableView *table_view_;
    TextEditorModel *text_editor_model_;

    FileLoadDialog *file_load_dialog_;

    void openFiles();
    void clearDb();
    void quitApp();

};

#endif // MAINWINDOW_H
