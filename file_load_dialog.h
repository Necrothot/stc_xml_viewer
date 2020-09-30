#ifndef FILE_LOAD_DIALOG_H
#define FILE_LOAD_DIALOG_H

#include <QDialog>
#include <QProgressBar>
#include <QTextEdit>

class FileLoadDialog : public QDialog
{
public:
    FileLoadDialog(QWidget *parent = nullptr);

    void reset();

    void setFilesCount(int count);
    void setProgress(int value);
    void incProgress();

    void addErrorString(const QString &error);

private:
    int current_progress;
    bool showing_errors_flag;

    QProgressBar *progress_bar;
    QTextEdit *text_edit;

};

#endif // FILE_LOAD_DIALOG_H
