#include "text_editor_info.h"

#include <QDebug>
#include <QFile>
#include <QXmlStreamReader>

/**
 * @brief Text editor info
 *
 * @param[in] file_name Name of XML file with editor's info
 * @exception std::runtime_error Failed to open XML file
 */
TextEditorInfo::TextEditorInfo(const QString &file_name)
{
    QFile xml_file(file_name);
    if (!xml_file.open(QIODevice::ReadOnly))
        throw std::runtime_error("Failed to open XML file");

    QXmlStreamReader xml_reader;
    xml_reader.setDevice(&xml_file);
    if (xml_reader.readNextStartElement())
    {
        if (xml_reader.name() == "root")
        {
            while (xml_reader.readNextStartElement())
            {
                if (xml_reader.name() == "texteditor")
                    text_editor_ = xml_reader.readElementText();
                else if (xml_reader.name() == "fileformats")
                    file_formats_ = xml_reader.readElementText();
                else if (xml_reader.name() == "encoding")
                    encoding_ = xml_reader.readElementText();
                else if (xml_reader.name() == "hasintellisense")
                    has_intellisense_ = xml_reader.readElementText();
                else if (xml_reader.name() == "hasplugins")
                    has_plugins_ = xml_reader.readElementText();
                else if (xml_reader.name() == "cancompile")
                    can_compile_ = xml_reader.readElementText();
            }

            if (xml_reader.hasError())
            {
                qDebug() << "TextEditorInfo: XML error " <<
                            xml_reader.errorString();
            }
        }
        else
        {
            xml_reader.raiseError("No root element");
        }
    }
}
