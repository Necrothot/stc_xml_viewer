#ifndef XML_FILE_READER_H
#define XML_FILE_READER_H

#include <QObject>
#include <QString>

#include "global_definitions.h"

class XmlFileReader : public QObject
{
public:
    explicit XmlFileReader(const QString &file_name,
                           QObject *parent = nullptr);

    void readFile();

private:
    Q_OBJECT

    QString file_name_;

signals:
    void valuesSignal(const global_def::ColumnValues &values);
    void statusSignal(bool success,
                      const QString &name,
                      const QString &status = QString());

};

#endif // XML_FILE_READER_H
