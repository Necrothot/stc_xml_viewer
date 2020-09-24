#include "text_editor_model.h"

#include <QDebug>
#include <QDir>

const QVector<QString> header_column = {
    "Text Editor",
    "File Formats",
    "Encoding",
    "Has Intellisense",
    "Has Plugins",
    "Can Compile"
};

TextEditorModel::TextEditorModel(QObject *parent) :
    QAbstractTableModel(parent)
{

}

/**
 * @brief Read all XML files from given directory
 *
 * @param[in] path Path to directory
 */
void TextEditorModel::readFilesFromDir(const QString &path)
{
    try
    {
        QDir dir(path);

        QFileInfoList info_list = dir.entryInfoList();
        for (auto i = info_list.begin(); i != info_list.end(); ++i)
        {
            if (i->completeSuffix() == "xml")
            {
                TextEditorInfo text_editor_info(i->canonicalFilePath());
                text_editor_info_.push_back(text_editor_info);
            }
        }

    }
    catch (const std::runtime_error &e)
    {
        qCritical("TextEditorInfo error: %s\n", e.what());
    }
}

int TextEditorModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return text_editor_info_.size();
}

int TextEditorModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return header_column.size();
}

QVariant TextEditorModel::headerData(int section,
                                     Qt::Orientation orientation,
                                     int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            if (section < header_column.size())
                return header_column[section];
        }
        else if (orientation == Qt::Vertical)
        {
            return section;
        }
    }

    return QVariant();
}

QVariant TextEditorModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        switch (index.column())
        {
        case 0:
            return text_editor_info_[index.row()].textEditor();
        case 1:
            return text_editor_info_[index.row()].fileFormats();
        case 2:
            return text_editor_info_[index.row()].encoding();
        case 3:
            return text_editor_info_[index.row()].hasIntellisense();
        case 4:
            return text_editor_info_[index.row()].hasPlugins();
        case 5:
            return text_editor_info_[index.row()].canCompile();
        }
    }

    return QVariant();
}
