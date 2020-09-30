#include "xml_file_reader.h"

#include <QFile>
#include <QXmlStreamReader>

XmlFileReader::XmlFileReader(const QString &file_name, QObject *parent) :
    QObject(parent),
    file_name_(file_name)
{

}

void XmlFileReader::readFile()
{
    QFile xml_file(file_name_);
    if (!xml_file.open(QFile::ReadOnly | QFile::Text))
    {
        emit statusSignal(false, file_name_,
                          "Failed to open XML file");
        return;
    }

    global_def::ColumnValues column_values;

    QXmlStreamReader xml_reader;
    xml_reader.setDevice(&xml_file);
    if (xml_reader.readNextStartElement())
    {
        if (xml_reader.name() != "root")
        {
            emit statusSignal(false, file_name_,
                              "No root element");
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
        {
            emit statusSignal(false, file_name_,
                              xml_reader.errorString());
            return;
        }
    }

    emit valuesSignal(column_values);
    emit statusSignal(true, file_name_);
}
