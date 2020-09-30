#include "file_load_dialog.h"

#include <QLayout>

FileLoadDialog::FileLoadDialog(QWidget *parent) :
    QDialog(parent),
    current_progress(0),
    showing_errors_flag(false)
{
    setWindowTitle("File load");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setMinimumSize(200, 200);

    progress_bar = new QProgressBar(this);
    progress_bar->setMinimum(0);

    text_edit = new QTextEdit(this);
    text_edit->setReadOnly(true);

    QVBoxLayout *main_layout = new QVBoxLayout;
    main_layout->addWidget(progress_bar);
    main_layout->addWidget(text_edit);

    setLayout(main_layout);
}

void FileLoadDialog::reset()
{
    setProgress(0);
    text_edit->clear();
}

void FileLoadDialog::setFilesCount(int count)
{
    progress_bar->setMaximum(count);
}

void FileLoadDialog::setProgress(int value)
{
    current_progress = value;
    progress_bar->setValue(current_progress);
}

void FileLoadDialog::incProgress()
{
    setProgress(++current_progress);
}

void FileLoadDialog::addErrorString(const QString &error)
{
    if (!showing_errors_flag)
    {
        text_edit->append("Errors:");
        showing_errors_flag = true;
    }

    text_edit->append(error);
}
