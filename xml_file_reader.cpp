#include "xml_file_reader.h"

#include <QFile>
#include <QXmlStreamReader>

XmlFileReader::XmlFileReader(const QFileInfo &file_info, QObject *parent) :
    QObject(parent),
    file_info_(file_info)
{

}

void XmlFileReader::readFile()
{
    QFile xml_file(file_info_.canonicalFilePath());
    if (!xml_file.open(QFile::ReadOnly | QFile::Text))
    {
        emit errorSignal(file_info_.fileName() + ": Failed to open XML file");
        emit progressSignal();
        return;
    }

    global_def::ColumnValues column_values;

    QXmlStreamReader xml_reader;
    xml_reader.setDevice(&xml_file);
    if (xml_reader.readNextStartElement())
    {
        if (xml_reader.name() != "root")
        {
            emit errorSignal(file_info_.fileName() + ": No root element");
            emit progressSignal();
            return;
        }

        while (xml_reader.readNextStartElement())
        {
            for (auto &cn: global_def::column_names)
            {
                if (xml_reader.name() == cn)
                {
                    column_values[cn] = xml_reader.readElementText();
                    break;
                }
            }
        }

        if (xml_reader.hasError())
            emit errorSignal(file_info_.fileName() + ": " +
                             xml_reader.errorString());
    }

    emit valuesSignal(column_values);
    emit progressSignal();
}
