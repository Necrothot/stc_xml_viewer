#include "text_editor_model.h"

#include <QDebug>
#include <QVector>
#include <QDir>
#include <QFile>
#include <QXmlStreamReader>
#include <QSqlQuery>
#include <QSqlError>

const QVector<QString> column_names = {
    "texteditor",
    "fileformats",
    "encoding",
    "hasintellisense",
    "hasplugins",
    "cancompile"
};

TextEditorModel::TextEditorModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    initDb();
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
        for (auto &i : info_list)
        {
            if (i.completeSuffix() == "xml")
                insertIntoDb(readXmlFile(i.canonicalFilePath()));
        }

    }
    catch (const std::runtime_error &e)
    {
        qWarning() << "TextEditorModel::readFilesFromDir error - " <<
                      e.what();
    }
}

int TextEditorModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    QSqlQuery query("SELECT id FROM text_editors");
    int count = 0;
    if (query.exec())
    {
        while (query.next())
            count++;
    }

    return count;
}

int TextEditorModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return column_names.size();
}

QVariant TextEditorModel::headerData(int section,
                                     Qt::Orientation orientation,
                                     int role) const
{
    const QVector<QString> header_column = {
        "Text Editor",
        "File Formats",
        "Encoding",
        "Has Intellisense",
        "Has Plugins",
        "Can Compile"
    };

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
    QVariant value;

    if (role == Qt::DisplayRole &&
        index.column() < column_names.size())
    {
        QSqlQuery query("SELECT " +
                        column_names[index.column()] + " " +
                        "FROM text_editors "
                        "WHERE id=" +
                        QString::number(index.row() + 1));

        if (query.exec())
        {
            query.first();
            value = query.value(0).toString();
        }
    }

    return value;
}

bool TextEditorModel::setData(const QModelIndex &index,
                              const QVariant &value,
                              int role)
{
    if (role == Qt::EditRole)
    {
        if (!checkIndex(index))
            return false;

        QString query_str = "UPDATE text_editors SET " +
                            column_names[index.column()] + "='" +
                            value.toString() + "' "
                            "WHERE id=" +
                            QString::number(index.row() + 1);

        QSqlQuery query(query_str);
        if (!query.exec())
        {
            qWarning() << "TextEditorModel::setData error - " <<
                          query.lastError();
            return false;
        }

        return true;
    }

    return false;
}

bool TextEditorModel::insertRows(int row, int count,
                                 const QModelIndex &parent)
{
    if (count < 1)
        return false;

    beginInsertRows(parent, row, row + count - 1);
    endInsertRows();

    return true;
}

bool TextEditorModel::removeRows(int row, int count,
                                 const QModelIndex &parent)
{
    if (count < 1)
        return false;

    beginRemoveRows(parent, row, row + count - 1);
    endRemoveRows();

    return true;
}

Qt::ItemFlags TextEditorModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

/**
 * @brief Read XML file
 *
 * @param[in] name Name of XML file
 * @exception std::runtime_error Failed to open XML file
 */
TextEditorModel::ColumnValues TextEditorModel::readXmlFile(const QString &name) const
{
    QFile xml_file(name);
    if (!xml_file.open(QFile::ReadOnly | QFile::Text))
        throw std::runtime_error("Failed to open XML file");

    ColumnValues column_values;

    QXmlStreamReader xml_reader;
    xml_reader.setDevice(&xml_file);
    if (xml_reader.readNextStartElement())
    {
        if (xml_reader.name() != "root")
            throw std::runtime_error("No root element");

        while (xml_reader.readNextStartElement())
        {
            for (auto &cn: column_names)
            {
                if (xml_reader.name() == cn)
                {
                    column_values[cn] = xml_reader.readElementText();
                    break;
                }
            }
        }

        if (xml_reader.hasError())
        {
            qDebug() << "TextEditorModel::readXmlFile error - " <<
                        xml_reader.errorString();
        }
    }

    return column_values;
}

bool TextEditorModel::initDb()
{
    const QString db_driver = "QSQLITE";

    // Check SQLite driver
    if (!QSqlDatabase::isDriverAvailable(db_driver))
    {
        qCritical() << "TextEditorModel::initDb error -"
                       "No SQLite driver";
        return false;
    }

    db_ = QSqlDatabase::addDatabase(db_driver);
//    db_.setDatabaseName(":memory:");
    db_.setDatabaseName("text_editors.db");
    if (!db_.open())
    {
//        throw std::runtime_error("Failed to open database");
        qCritical() << "TextEditorModel::initDb error -"
                       "Failed to open DB file";
        return false;
    }

    QString query_str = "CREATE TABLE IF NOT EXISTS text_editors"
                        "("
                        "id INTEGER PRIMARY KEY";
    for (auto &cn: column_names)
        query_str +=    "," + cn + " TEXT";
    query_str +=        ")";

    QSqlQuery query(query_str);
    if (!query.exec())
    {
        qWarning() << "TextEditorModel::initDb error - " <<
                      query.lastError();
        return false;
    }

    return true;
}

bool TextEditorModel::clearDb()
{
    // Remove all rows
    if (!removeRows(0, rowCount(), QModelIndex()))
        return false;

    QSqlQuery query("DELETE FROM text_editors");
    if (!query.exec())
    {
        qWarning() << "TextEditorModel::clearDb error - " <<
                      query.lastError();
        return false;
    }

    return true;
}

bool TextEditorModel::insertIntoDb(const ColumnValues &columns)
{
    QString query_str = "INSERT INTO text_editors"
                        "(";
    for (auto &cn: column_names)
        query_str +=    cn + ",";
    query_str.chop(1);  // Remove last comma
    query_str +=        ")"
                        "VALUES(";
    for (int i = 0; i < column_names.size(); i++)
        query_str +=    "?,";
    query_str.chop(1);  // Remove last comma
    query_str +=        ")";

    QSqlQuery query(query_str);

    for (auto &cn: column_names)
        query.addBindValue(columns[cn]);

    if (!query.exec())
    {
        qWarning() << "TextEditorModel::insertIntoDb error - " <<
                      query.lastError().text();
        return false;
    }

    // Insert one row
    if (!insertRows(0, 1, QModelIndex()))
        return false;

    return true;
}
