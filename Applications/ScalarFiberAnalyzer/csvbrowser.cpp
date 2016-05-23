#include "csvbrowser.h"
#include "ui_csvbrowser.h"
#include "csvbrowsemodel.h"


CSVBrowser::CSVBrowser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CSVBrowser)
{
    ui->setupUi(this);
}

void CSVBrowser::loadTable(std::vector<std::vector<std::string>> csv_results, std::vector<std::string> headers){
    CSVBrowseModel *mm = new CSVBrowseModel(0,csv_results,headers);
    ui->BrowseTable->setModel(mm);
    ui->BrowseTable->horizontalHeader()->setStretchLastSection(true);
}

CSVBrowser::~CSVBrowser()
{
    delete ui;
}

void CSVBrowser::on_CSVbrwOK_clicked()
{
    delete this;
}

