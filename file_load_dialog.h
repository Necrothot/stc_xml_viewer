#ifndef FILE_LOAD_DIALOG_H
#define FILE_LOAD_DIALOG_H

#include <QDialog>
#include <QProgressBar>
#include <QTextEdit>
#include <QLabel>

class FileLoadDialog : public QDialog
{
public:
    FileLoadDialog(QWidget *parent = nullptr);

    void reset();

    void setFilesCount(int count);
    void setProgress(int value);
    void incProgress();

    void setFileReadStatus(bool success, const QString &name,
                           const QString &status);

private:
    int files_count_;
    int files_read_;
    int current_progress_;

    QProgressBar *progress_bar_;
    QTextEdit *text_edit_;
    QLabel *status_label_;

};

#endif // FILE_LOAD_DIALOG_H
