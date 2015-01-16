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

/*void XmlWriter::writeExecutables(QXmlStreamWriter* stream)
{
   ExecutablePaths* executablePaths = m_parameters->getExecutablePaths();
   stream->writeStartElement("Tools-Paths");
   stream->writeStartElement("Executables");
   writeElement(stream, "python", "path", executablePaths->getExecutablePath("python"));
   writeElement(stream, "SegPostProcessCLP", "path", executablePaths->getExecutablePath("SegPostProcessCLP"));
   writeElement(stream, "N4ITKBiasFieldCorrection", "path", executablePaths->getExecutablePath("N4ITKBiasFieldCorrection"));
   writeElement(stream, "ANTS", "path", executablePaths->getExecutablePath("ANTS"));
   writeElement(stream, "ITKTransformTools", "path", executablePaths->getExecutablePath("ITKTransformTools"));
   writeElement(stream, "dtiestim", "path", executablePaths->getExecutablePath("dtiestim"));
   writeElement(stream, "dtiprocess", "path", executablePaths->getExecutablePath("dtiprocess"));
   writeElement(stream, "bet2", "path", executablePaths->getExecutablePath("bet2"));
   writeElement(stream, "SpreadFA", "path", executablePaths->getExecutablePath("SpreadFA"));
   writeElement(stream, "unu", "path", executablePaths->getExecutablePath("unu"));
   writeElement(stream, "ResampleScalarVectorDWIVolume", "path", executablePaths->getExecutablePath("ResampleScalarVectorDWIVolume"));
   writeElement(stream, "ImageMath", "path", executablePaths->getExecutablePath("ImageMath"));
   writeElement(stream, "WeightedLabelsAverage", "path", executablePaths->getExecutablePath("WeightedLabelsAverage"));
   writeElement(stream, "neoseg", "path", executablePaths->getExecutablePath("neoseg"));
   writeElement(stream, "ReassignWhiteMatter", "path", executablePaths->getExecutablePath("ReassignWhiteMatter"));
   writeElement(stream, "InsightSNAP", "path", executablePaths->getExecutablePath("InsightSNAP"));
   stream->writeEndElement();

   LibraryPaths* libraryPaths = m_parameters->getLibraryPaths();

   stream->writeStartElement("Library-directories");
   writeElement(stream, "FSL", "path", libraryPaths->getLibraryPath("FSL"));
   stream->writeEndElement();
   stream->writeEndElement();
}*/

/*void XmlWriter::writeExecutablesConfiguration(QString file_path)
{
   QFile* file = new::QFile(file_path);
   file->open(QIODevice::WriteOnly);

   QXmlStreamWriter* stream = new::QXmlStreamWriter(file);
   stream->setAutoFormatting(true);

   stream->writeStartDocument();
   stream->writeDTD("<!DOCTYPE Neoseg-pipeline-executables>");

   writeExecutables(stream);

   stream->writeEndDocument();

   file->close();
}*/




