#include "xml_file_writer.h"

#include <QFile>
#include <QXmlStreamWriter>

/**
 * @brief Writer of XML files with text editor's properties
 *
 * @param[in] file_name Name of XML file to write
 */
XmlFileWriter::XmlFileWriter(const QString &file_name, QObject *parent) :
    QObject(parent),
    file_name_(file_name)
{
}

void XmlFileWriter::setData(const global_def::ColumnValues &column_values)
{
    column_values_ = column_values;
}

void XmlFileWriter::writeFile()
{
    if (column_values_.size() != global_def::column_names.size())
    {
        emit statusSignal(false, file_name_,
                          "Wrong data");
        return;
    }

    QFile xml_file(file_name_);
    if (!xml_file.open(QFile::WriteOnly | QFile::Text))
    {
        emit statusSignal(false, file_name_,
                          "Failed to open XML file");
        return;
    }

    QXmlStreamWriter xml_writer;
    xml_writer.setDevice(&xml_file);
    xml_writer.setAutoFormatting(true);

    xml_writer.writeStartDocument();
    xml_writer.writeStartElement("root");

    for (auto &cn: global_def::column_names)
        xml_writer.writeTextElement(cn, column_values_[cn]);

    xml_writer.writeEndElement();
    xml_writer.writeEndDocument();

    if (xml_writer.hasError())
    {
        emit statusSignal(false, file_name_,
                          "Write error");
        return;
    }

    emit statusSignal(true, file_name_);
}
