/*source: https://github.com/NIRALUser/NeosegPipeline/XMLReader.h*/

#ifndef DEF_XmlReader
#define DEF_XmlReader

#include <QXmlStreamReader>
#include <QString>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoListIterator>
#include <QProcessEnvironment>
#include <QMap>
#include <QtGlobal>

class XmlReader
{

public:
    XmlReader();
    bool isBoolean(int value);
    QString readExecutablesConfigurationFile(QString file_path);
    QString pathToFiberProcess;
    QString pathToFiberPostProcess;
    QString pathToDtiTractstat;
private:

};



#endif

