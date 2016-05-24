/*source: https://github.com/NIRALUser/NeosegPipeline/XMLWriter.h*/

#ifndef DEF_XmlWriter
#define DEF_XmlWriter

#include <iostream>

//#include <QtXml>
#include <QXmlStreamWriter>
#include <QString>
#include <QDir>
#include <QFile>
#include <QFileInfo>
//#include <QFileInfoListIterator>
#include <QProcessEnvironment>
#include <QMap>
#include <QtGlobal>
#include <QMapIterator>


class XmlWriter
{

public:

    XmlWriter();

    void writeElement(QXmlStreamWriter* stream, QString tag, QString name, QString value);
    void writeElement(QXmlStreamWriter* stream, QString tag, QString name1, QString value1, QString name2, QString value2);
    void writeElement(QXmlStreamWriter* stream, QString tag, QString name1, QString value1, QString name2, QString value2, QString name3, QString value3);
    void writeElement(QXmlStreamWriter* stream, QString tag, QString name1, QString value1, QString name2, QString value2, QString name3, QString value3, QString name4, QString value4);
    void writeElement(QXmlStreamWriter* stream, QString tag, QString name1, QString value1, QString name2, QString value2, QString name3, QString value3, QString name4, QString value4, QString name5, QString value5);
    void writeExecutables(QXmlStreamWriter* stream);
    void writeExecutablesConfiguration(QString file_path);

    QMap< QString , QString > ExecutablePathMap ;

private:

};

#endif
