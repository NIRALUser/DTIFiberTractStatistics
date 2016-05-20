#ifndef CSVBROWSEMODEL_H
#define CSVBROWSEMODEL_H


#include <QAbstractTableModel>
#include "tfatool.h"
using namespace std;

class CSVBrowseModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit CSVBrowseModel (QObject *parent = 0,  vector<vector<string>> csv_results = vector<vector<string>>(), vector<string> h = vector<string>());
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
private:
    vector<vector<string>> table;
    vector<string> headers;
signals:

public slots:

};

#endif // CSVBROWSEMODEL_H
