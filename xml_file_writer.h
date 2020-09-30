#ifndef XML_FILE_WRITER_H
#define XML_FILE_WRITER_H

#include <QObject>
#include <QString>

#include "global_definitions.h"

class XmlFileWriter : public QObject
{
public:
    explicit XmlFileWriter(const QString &file_name,
                           QObject *parent = nullptr);

    void setData(const global_def::ColumnValues &column_values);
    void writeFile();

private:
    Q_OBJECT

    QString file_name_;
    global_def::ColumnValues column_values_;

signals:
    void statusSignal(bool success,
                      const QString &name,
                      const QString &status = QString());

};

#endif // XML_FILE_WRITER_H
