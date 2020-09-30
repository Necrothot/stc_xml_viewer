#ifndef GLOBAL_DEFINITIONS_H
#define GLOBAL_DEFINITIONS_H

#include <QVector>
#include <QString>
#include <QMap>

namespace global_def
{

using ColumnValues = QMap<QString, QString>;

const QVector<QString> column_names = {
    "texteditor",
    "fileformats",
    "encoding",
    "hasintellisense",
    "hasplugins",
    "cancompile"
};

}

#endif // GLOBAL_DEFINITIONS_H
