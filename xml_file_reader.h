#ifndef XML_FILE_READER_H
#define XML_FILE_READER_H

#include <QObject>
#include <QMap>
#include <QFileInfo>

#include "global_definitions.h"

class XmlFileReader : public QObject
{
public:
    explicit XmlFileReader(const QFileInfo &file_info, QObject *parent = nullptr);

    void readFile();

private:
    Q_OBJECT

    QFileInfo file_info_;

signals:
    void valuesSignal(const global_def::ColumnValues &values);
    void statusSignal(bool success,
                      const QString &name,
                      const QString &status = QString());

};

#endif // XML_FILE_READER_H
