#ifndef TEXT_EDITOR_MODEL_H
#define TEXT_EDITOR_MODEL_H

#include <QAbstractTableModel>
#include <QSqlDatabase>
#include <QMap>

#include "global_definitions.h"

class TextEditorModel : public QAbstractTableModel
{
public:
    TextEditorModel(QObject *parent = nullptr);

    void readFileIntoDb(const QString &path);
    bool insertIntoDb(const global_def::ColumnValues &columns);
    bool clearDb();
    void saveRowToFile(const QString &file_name, int row);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role) const override;
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index,
                 const QVariant &value,
                 int role = Qt::EditRole) override;
    bool insertRows(int row, int count,
                    const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count,
                    const QModelIndex &parent = QModelIndex()) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    Q_OBJECT

    QSqlDatabase db_;

    bool initDb();

signals:
    void fileReadStatusSignal(bool success, const QString &name,
                              const QString &status);
    void fileWriteStatusSignal(bool success, const QString &name,
                               const QString &status);
    void fileProgressSignal();

};

#endif // TEXT_EDITOR_MODEL_H
