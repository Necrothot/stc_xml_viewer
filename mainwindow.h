#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QTableView>
#include <QMenu>

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

    QMenu *table_context_menu_;
    QModelIndex context_menu_index_;

    FileLoadDialog *file_load_dialog_;

    void openFiles();
    void clearDb();
    void quitApp();

    void tableContextMenu(QPoint pos);

    void showMessageBox(const QString &title,
                        const QString &msg);

};

#endif // MAINWINDOW_H
