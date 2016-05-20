#include "csvbrowsemodel.h"
#include <iostream>

CSVBrowseModel::CSVBrowseModel (QObject *parent,  vector<vector<string>> csv_results, vector<string> h)
    :QAbstractTableModel(parent){
    headers = h;
    table =csv_results;
}

int CSVBrowseModel::rowCount(const QModelIndex &parent) const{
    return table.size();
}
int CSVBrowseModel::columnCount(const QModelIndex &parent) const{
    return headers.size();
}

QVariant CSVBrowseModel::data(const QModelIndex &index, int role) const{
    int row = index.row();
    int col = index.column();
    std::string s = table[row][col];
    if (role == Qt::DisplayRole){
        return QString::fromStdString(table[row][col]);
    }

    return QVariant();
}

QVariant CSVBrowseModel::headerData(int section, Qt::Orientation orientation, int role) const{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal){
        return QString::fromStdString(headers[section]);
    }

    return QVariant();
}
