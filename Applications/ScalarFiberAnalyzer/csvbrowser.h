#ifndef CSVBROWSER_H
#define CSVBROWSER_H

#include <QDialog>
#include <iostream>



namespace Ui {
class CSVBrowser;
}

class CSVBrowser : public QDialog
{
    Q_OBJECT
public:
    explicit CSVBrowser(QWidget *parent = 0);
    void loadTable(std::vector<std::vector<std::string> > csv_results, std::vector<std::string> headers);
    ~CSVBrowser();

private slots:
    void on_CSVbrwOK_clicked();

private:
    Ui::CSVBrowser *ui;
};

#endif // CSVBROWSER_H
