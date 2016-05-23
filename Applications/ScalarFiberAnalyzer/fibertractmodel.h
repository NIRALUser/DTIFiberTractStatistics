#ifndef FIBERTRACTMODEL_H
#define FIBERTRACTMODEL_H
#define FIBER_CSV_COL_NUM 1

#include <QAbstractTableModel>
#include "tfatool.h"

class FiberTractModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit FiberTractModel (QObject *parent = 0, std::vector<tool::TractData> db = std::vector<tool::TractData>());
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void resetModel(Qt::CheckState state, int index = -1);
    tool::TractData getData(int i);
    bool getCheckState(int i);
    unsigned int getDataSize();
    int findData(QString str);

private:
    Qt::CheckState* checkedState;
    std::vector<tool::TractData> tract_db;
    static bool Tract_Comparator(tool::TractData a, tool::TractData b);

};

#endif // FIBERTRACTMODEL_H
