#include "text_editor_model.h"

#include <QDebug>
#include <QVector>
#include <QDir>
#include <QFile>
#include <QXmlStreamReader>
#include <QSqlQuery>
#include <QSqlError>
#include <QThread>

#include "xml_file_reader.h"
#include "xml_file_writer.h"

TextEditorModel::TextEditorModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    qRegisterMetaType<global_def::ColumnValues>("global_def::ColumnValues");

    initDb();
}

void TextEditorModel::readFileIntoDb(const QString &path)
{
    XmlFileReader *reader = new XmlFileReader(path);

    QThread *thread = new QThread(this);
    reader->moveToThread(thread);

    QObject::connect(thread, &QThread::started,
                     reader, &XmlFileReader::readFile);
    QObject::connect(thread, &QThread::finished,
                     reader, &XmlFileReader::deleteLater);

    QObject::connect(reader, &XmlFileReader::valuesSignal,
                     this, &TextEditorModel::insertIntoDb);

    QObject::connect(reader, &XmlFileReader::statusSignal,
                     this, &TextEditorModel::fileReadStatusSignal);

    QObject::connect(reader, &XmlFileReader::statusSignal,
                     thread, &QThread::quit);

    thread->start();
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
    return global_def::column_names.size();
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
        index.column() < global_def::column_names.size())
    {
        QSqlQuery query("SELECT " +
                        global_def::column_names[index.column()] + " " +
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
                            global_def::column_names[index.column()] + "='" +
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
    if (row < 0 || count < 1)
        return false;

    beginRemoveRows(parent, row, row + count - 1);

    QString query_str = "DELETE FROM text_editors WHERE id='" +
                        QString::number(row + 1) + "'";

    QSqlQuery query(query_str);
    if (!query.exec())
    {
        qWarning() << "TextEditorModel::removeRows error - " <<
                      query.lastError().text();
        return false;
    }


    {
        QString query_str = "UPDATE OR IGNORE text_editors SET id=id-1 WHERE id > '" +
                            QString::number(row + 1) + "'";

        QSqlQuery query(query_str);
        if (!query.exec())
        {
            qWarning() << "TextEditorModel::removeRows error - " <<
                          query.lastError().text();
            return false;
        }
    }

    endRemoveRows();

    return true;
}

Qt::ItemFlags TextEditorModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

bool TextEditorModel::initDb()
{
    const QString db_driver = "QSQLITE";

    // Check SQLite driver
    if (!QSqlDatabase::isDriverAvailable(db_driver))
    {
        qWarning() << "TextEditorModel::initDb error -"
                       "No SQLite driver";
        return false;
    }

    db_ = QSqlDatabase::addDatabase(db_driver);
    db_.setDatabaseName("text_editors.db");
    if (!db_.open())
    {
        qWarning() << "TextEditorModel::initDb error -"
                       "Failed to open DB file";
        return false;
    }

    QString query_str = "CREATE TABLE IF NOT EXISTS text_editors"
                        "("
                        "id INTEGER PRIMARY KEY";
    for (auto &cn: global_def::column_names)
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

void TextEditorModel::saveRowToFile(const QString &file_name, int row)
{
    global_def::ColumnValues column_values;

    for (auto &cn: global_def::column_names)
    {
        QString query_str = "SELECT " +
                            cn +
                            " FROM text_editors WHERE id='" +
                            QString::number(row + 1) + "'";
        QSqlQuery query(query_str);
        if (!query.exec())
        {
            qWarning() << "TextEditorModel::saveRowToFile error - " <<
                          query.lastError();
            return;
        }

        query.first();
        column_values[cn] = query.value(0).toString();
    }

    XmlFileWriter *writer = new XmlFileWriter(file_name);
    writer->setData(column_values);

    QThread *thread = new QThread(this);
    writer->moveToThread(thread);

    QObject::connect(thread, &QThread::started,
                     writer, &XmlFileWriter::writeFile);
    QObject::connect(thread, &QThread::finished,
                     writer, &XmlFileWriter::deleteLater);

    QObject::connect(writer, &XmlFileWriter::statusSignal,
                     this, &TextEditorModel::fileWriteStatusSignal);

    QObject::connect(writer, &XmlFileWriter::statusSignal,
                     thread, &QThread::quit);

    thread->start();
}

bool TextEditorModel::insertIntoDb(const global_def::ColumnValues &columns)
{
    int id = 0;

    // Check if new text editor is already in DB
    {
        QString query_str = "SELECT id FROM text_editors WHERE texteditor='" +
                            columns["texteditor"] + "'";

        QSqlQuery query(query_str);
        if (!query.exec())
        {
            qWarning() << "TextEditorModel::insertIntoDb error - " <<
                          query.lastError().text();
            return false;
        }

        query.first();
        if (query.isValid())
            id = query.value(0).toInt();
    }


    QString query_str;
    if (id)
    {
        query_str =         "UPDATE text_editors SET ";
        for (auto &cn: global_def::column_names)
            query_str +=    cn + "=?,";
        query_str.chop(1);
        query_str +=        " WHERE id='" + QString::number(id) + "'";
    }
    else
    {
        query_str =         "INSERT INTO text_editors"
                            "(";
        for (auto &cn: global_def::column_names)
            query_str +=    cn + ",";
        query_str.chop(1);
        query_str +=        ")"
                            "VALUES(";
        for (int i = 0; i < global_def::column_names.size(); i++)
            query_str +=    "?,";
        query_str.chop(1);
        query_str +=        ")";
    }

    QSqlQuery query(query_str);

    for (auto &cn: global_def::column_names)
        query.addBindValue(columns[cn]);

    if (!query.exec())
    {
        qWarning() << "TextEditorModel::insertIntoDb error - " <<
                      query.lastError().text();
        return false;
    }

    if (id)
    {
        emit dataChanged(QModelIndex(), QModelIndex());
    }
    else
    {
        // If new text editor wasn't found in DB insert new row
        if (!insertRows(0, 1, QModelIndex()))
            return false;
    }

    return true;
}
