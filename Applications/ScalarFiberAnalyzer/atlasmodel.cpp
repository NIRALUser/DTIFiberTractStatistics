#include "atlasmodel.h"
#include <iostream>
#include <unordered_set>
#include <errorreporter.h>


// AtlasModel is for the csv match table between csv of T1/T2 images and csv of DTI deformation fields
AtlasModel::AtlasModel(QObject *parent, std::map<std::string,tool::TractData> db1,
                       std::map<std::string,tool::TractData> db2) :
    QAbstractTableModel(parent)
{
    tract_db.clear();
    unordered_set<std::string> found;
    bool wflag = true;

    // store the union of db1 and db2 into vector
    for(std::map<std::string,tool::TractData>::iterator it = db1.begin(); it!= db1.end(); ++it){
        std::string key = it->first;

        // iteratively search data in DTI csv to match an DTI csv file id with an T1/T2 map csv file id.
        // matching conditions: s1 equal s2, or s1 contains s2, or s2 contains s1

        for(std::map<std::string,tool::TractData>::iterator itt = db2.begin(); itt!= db2.end(); ++itt){
            std::string mkey = itt->first;

            // one id is supposed to be matched only once
            if(!found.empty() && found.find(mkey) != found.end()){
                if(wflag &&(key == mkey || mkey.find(key) != std::string::npos || key.find(mkey) != std::string::npos)){
                    wflag = false;
                    ErrorReporter::fire("Duplicate matches were found and eliminated!");
                }
                continue;
            }

            if(key == mkey){
                tool::MapData mapd = {
                    it->second.file_path,
                    itt->second.file_path,
                    QString::fromStdString(key),
                    QString::fromStdString(mkey)
                };
                tract_db.push_back(mapd);
                found.insert(mkey);
                break;
            }
            else if(key.length() <= mkey.length() && mkey.find(key) != std::string::npos){
                tool::MapData mapd = {
                    it->second.file_path,
                    itt->second.file_path,
                    QString::fromStdString(key),
                    QString::fromStdString(mkey)
                };
                tract_db.push_back(mapd);
                found.insert(mkey);
                break;
            }
            else if(mkey.length() < key.length() && key.find(mkey) != std::string::npos){
                tool::MapData mapd = {
                    it->second.file_path,
                    itt->second.file_path,
                    QString::fromStdString(key),
                    QString::fromStdString(mkey)
                };
                tract_db.push_back(mapd);
                found.insert(mkey);
                break;
            }

        }
    }
    checkedState = new Qt::CheckState[tract_db.size()]();

    for(unsigned int i =0; i< tract_db.size(); i++){
        checkedState[i] = Qt::Checked;
    }
}

AtlasModel::~AtlasModel(){
    delete checkedState;
}

int AtlasModel::rowCount(const QModelIndex &parent) const{
    return tract_db.size();
}

int AtlasModel::columnCount(const QModelIndex &parent) const{
    return 1 + TRACT_CSV_COL_NUM;
}


QVariant AtlasModel::data(const QModelIndex &index, int role) const{
    int row = index.row();
    int col = index.column();

    // issue: no way that the entire path can be seen when it is too long
    if (role == Qt::DisplayRole){
        switch(col){
        case 1:
            return tract_db[row].subjectID;
        case 2:
            return tract_db[row].mapID;
        case 3:
            return tract_db[row].t12_path;
        case 4:
            return tract_db[row].def_path;
        }
    }
    else if (role == Qt::CheckStateRole){
        if(col == 0){
            return checkedState[row];
        }
    }

    return QVariant();

}

Qt::ItemFlags AtlasModel::flags(const QModelIndex &index) const{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    if (index.column() == 0){
        flags |= Qt::ItemIsUserCheckable;
    }
    return flags;
}


bool AtlasModel::setData(const QModelIndex & index, const QVariant & value, int role){
    if(role == Qt::CheckStateRole && index.column() == 0){
        Qt::CheckState state = (Qt::CheckState)this->data(index,Qt::CheckStateRole).toInt();
        if(state == Qt::Checked)
            checkedState[index.row()] = Qt::Unchecked;
        else
            checkedState[index.row()] = Qt::Checked;
        return true;

    }
    return false;
}


QVariant AtlasModel::headerData(int section, Qt::Orientation orientation, int role) const{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal){
        switch (section){
            case 0:
                return "";
            case 1:
                return "Subject ID";
            case 2:
                return "Subject ID 2";
            case 3:
                return "T1/T2 Image Path";
            case 4:
                return "Deformation Field Path";
        }
    }

    return QVariant();

}

tool::MapData AtlasModel::getData(int i){
    return tract_db[i];
}

unsigned int AtlasModel::getDataSize(){
    return tract_db.size();
}

//O(n) run time. Could we optimize it?
int AtlasModel::findData(QString str){
    unsigned int i = 0;
    for(std::vector<tool::MapData>::iterator it = tract_db.begin(); it != tract_db.end(); ++it) {
        if(it->subjectID == str)
            return i;
        i++;
    }

    return -1;
}

bool AtlasModel::getCheckState(int i){
    return checkedState[i] == Qt::Checked? true:false;
}

void AtlasModel::resetModel(Qt::CheckState state, int index){
    this->beginResetModel();
    if(index < 0){
        for(unsigned int i =0; i< tract_db.size(); i++){
            checkedState[i] = state;
        }
    }else
        checkedState[index] = state;
    this->endResetModel();
}
