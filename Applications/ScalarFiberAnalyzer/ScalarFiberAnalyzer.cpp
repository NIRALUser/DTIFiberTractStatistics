#include "ScalarFiberAnalyzerCLP.h"
#include "T1T2FiberAnalyzer.h"
#include <QApplication>

int main(int argc, char **argv)
{
    PARSE_ARGS;

    QApplication a(argc, argv);
    T1T2FiberAnalyzer w;
    w.show();
    w.initializeConfPath();

    return a.exec();
}
