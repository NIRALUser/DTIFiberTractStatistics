#include "errorreporter.h"


void ErrorReporter::fire(QString title, QString str)
{
    ErrorReporter::warning(0,title,str,QMessageBox::Ok);
}

void ErrorReporter::fire(std::string str)
{
    ErrorReporter::warning(0,"Warning",QString::fromStdString(str),QMessageBox::Ok);
}

void ErrorReporter::friendly_fire(std::string str){
    ErrorReporter::information(0,"Info",QString::fromStdString(str),QMessageBox::Ok);
}
