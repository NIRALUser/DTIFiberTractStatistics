#ifndef ERRORREPORTER_H
#define ERRORREPORTER_H

#include <QMessageBox>
#include <iostream>

class ErrorReporter : public QMessageBox
{
    Q_OBJECT
public:
    static void fire(QString title, QString str);
    static void fire(std::string str);
    static void friendly_fire(std::string str);
signals:

public slots:

};

#endif // ERRORREPORTER_H
