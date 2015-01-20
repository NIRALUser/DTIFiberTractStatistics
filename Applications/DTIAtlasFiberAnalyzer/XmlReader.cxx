/*source: https://github.com/NIRALUser/NeosegPipeline/XMLReader.cxx*/


#include "XmlReader.h"

XmlReader::XmlReader()
{

}

bool XmlReader::isBoolean(int value)
{
    if(value==0 || value==1)
    {
        return true;
    }
    return false;
}

QString XmlReader::readExecutablesConfigurationFile(QString file_path)
{
    QFile* file = new::QFile(file_path);
    QString currentSection = "" ;
    QStringList levelOne = QStringList() << QString( "Executables" ) ;
    QString errors ;
    if(!file->exists())
    {

        errors = " - " + file_path + " does not exist\n" ;
        return errors ;
    }
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        errors = "Couldn't open xml file \n" ;
        return errors ;
    }
    /* QXmlStreamReader takes any QIODevice. */
    QXmlStreamReader xml( file ) ;
    /* We'll parse the XML until we reach end of it.*/
    while( !xml.atEnd() && !xml.hasError() )
    {
        /* Read next element.*/
        QXmlStreamReader::TokenType token = xml.readNext() ;
        /* If token is just StartDocument, we'll go to next.*/
        if(token == QXmlStreamReader::StartDocument)
            continue;

        /* If token is StartElement, we'll see if we can read it.*/
        if( token == QXmlStreamReader::StartElement )
        {
            QString name = xml.name().toString() ;
            QString path = xml.attributes().value("path").toString();
            if( name == "fiberprocess" )
            {
                if( path.isEmpty() || !QFileInfo(path).isExecutable() )
                {
                    errors = "Path to fiberprocess is not written properly in the configuration file" ;
                    return errors ;
                }
                pathToFiberProcess = path ;
            }
            if( name == "FiberPostProcess" )
            {
                if( path.isEmpty() || !QFileInfo(path).isExecutable() )
                {
                    errors = "Path to FiberPostProcess is not written properly in the configuration file" ;
                    return errors ;
                }
                pathToFiberPostProcess = path ;
            }
            if( name == "dtitractstat" )
            {
                if( path.isEmpty() || !QFileInfo(path).isExecutable() )
                {
                    errors = "Path to dtitractstat is not written properly in the configuration file" ;
                    return errors ;
                }
                pathToDtiTractstat = path ;
            }
            continue;
        }
    }
    /* Error handling. */
    if( xml.hasError() )
    {
        errors = "Couldn't parse properly the xml file \n" ;
        return errors ;
    }
    //resets its internal state to the initial state.
    xml.clear();

    return errors;
}
