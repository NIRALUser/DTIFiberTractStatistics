/*source: https://github.com/NIRALUser/NeosegPipeline/XMLWriter.cxx*/
#include "XmlWriter.h"

XmlWriter::XmlWriter()
{

}


void XmlWriter::writeElement(QXmlStreamWriter* stream, QString tag, QString name, QString value)
{
    stream->writeStartElement(tag);
    stream->writeAttribute(name, value);
    stream->writeEndElement();
}

void XmlWriter::writeElement(QXmlStreamWriter* stream, QString tag, QString name1, QString value1, QString name2, QString value2)
{
    stream->writeStartElement(tag);
    stream->writeAttribute(name1, value1);
    stream->writeAttribute(name2, value2);
    stream->writeEndElement();
}

void XmlWriter::writeElement(QXmlStreamWriter* stream, QString tag, QString name1, QString value1, QString name2, QString value2, QString name3, QString value3)
{
    stream->writeStartElement(tag);
    stream->writeAttribute(name1, value1);
    stream->writeAttribute(name2, value2);
    stream->writeAttribute(name3, value3);
    stream->writeEndElement();
}

void XmlWriter::writeElement(QXmlStreamWriter* stream, QString tag, QString name1, QString value1, QString name2, QString value2, QString name3, QString value3, QString name4, QString value4)
{
    stream->writeStartElement(tag);
    stream->writeAttribute(name1, value1);
    stream->writeAttribute(name2, value2);
    stream->writeAttribute(name3, value3);
    stream->writeAttribute(name4, value4);
    stream->writeEndElement();
}

void XmlWriter::writeElement(QXmlStreamWriter* stream, QString tag, QString name1, QString value1, QString name2, QString value2, QString name3, QString value3, QString name4, QString value4, QString name5, QString value5)
{
    stream->writeStartElement(tag);
    stream->writeAttribute(name1, value1);
    stream->writeAttribute(name2, value2);
    stream->writeAttribute(name3, value3);
    stream->writeAttribute(name4, value4);
    stream->writeAttribute(name5, value5);
    stream->writeEndElement();
}

void XmlWriter::writeExecutables(QXmlStreamWriter* stream)
{
    stream->writeStartElement("Executables");
    QMapIterator<QString, QString> iterator(ExecutablePathMap);
    while( iterator.hasNext() )
    {
        iterator.next() ;
        writeElement(stream, iterator.key() , "path" , iterator.value() ) ;
    }
    stream->writeEndElement();
}

void XmlWriter::writeExecutablesConfiguration(QString file_path)
{
    QFile* file = new::QFile(file_path);
    file->open(QIODevice::WriteOnly);

    QXmlStreamWriter* stream = new::QXmlStreamWriter(file);
    stream->setAutoFormatting(true);

    stream->writeStartDocument();
    stream->writeDTD("<!DOCTYPE FiberPostProcess-executables>");

    writeExecutables(stream);

    stream->writeEndDocument();

    file->close();
}



