#ifndef TEXT_EDITOR_MODEL_H
#define TEXT_EDITOR_MODEL_H

#include <QAbstractTableModel>
#include <QList>

#include "text_editor_info.h"

class TextEditorModel : public QAbstractTableModel
{
public:   
    TextEditorModel(QObject *parent = nullptr);

    void readFilesFromDir(const QString &path);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role) const override;
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const override;

private:
    Q_OBJECT

    QList<TextEditorInfo> text_editor_info_;

};

#endif // TEXT_EDITOR_MODEL_H
