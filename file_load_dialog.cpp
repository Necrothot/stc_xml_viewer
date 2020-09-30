#include "file_load_dialog.h"

#include <QLayout>
#include <QFileInfo>

/**
 * @brief Dialog window for loading file information
 *
 * Displays progress bar, number of loaded files and
 * loaded files status log
 */
FileLoadDialog::FileLoadDialog(QWidget *parent) :
    QDialog(parent),
    files_count_(0),
    files_read_(0),
    current_progress_(0)
{
    setWindowTitle("File load");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setMinimumSize(200, 200);

    progress_bar_ = new QProgressBar(this);
    progress_bar_->setMinimum(0);
    progress_bar_->setMaximum(1);

    status_label_ = new QLabel(this);

    text_edit_ = new QTextEdit(this);
    text_edit_->setReadOnly(true);

    QVBoxLayout *main_layout = new QVBoxLayout;
    main_layout->addWidget(progress_bar_);
    main_layout->addWidget(status_label_);
    main_layout->addWidget(text_edit_);

    setLayout(main_layout);
}

void FileLoadDialog::reset()
{
    files_read_ = 0;
    setFilesCount(0);
    setProgress(0);
    text_edit_->clear();
}

void FileLoadDialog::setFilesCount(int count)
{
    files_count_ = count;

    if (files_count_)
        progress_bar_->setMaximum(files_count_);
    else
        status_label_->setText("XML files not found");
}

void FileLoadDialog::setProgress(int value)
{
    current_progress_ = value;
    progress_bar_->setValue(current_progress_);
}

void FileLoadDialog::incProgress()
{
    setProgress(++current_progress_);
}

void FileLoadDialog::setFileReadStatus(bool success, const QString &name,
                                       const QString &status)
{
    incProgress();

    QFileInfo fi(name);

    if (success)
    {
        files_read_++;
        QString status = "Files read: " + QString::number(files_read_) +
                         "/" + QString::number(files_count_);
        status_label_->setText(status);

        text_edit_->append(fi.fileName() + ": done");
    }
    else
    {
        text_edit_->append(fi.fileName() + ": error - " + status);
    }
}
