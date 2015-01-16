/*source: https://github.com/NIRALUser/NeosegPipeline/XMLReader.h*/

#ifndef DEF_XmlReader
#define DEF_XmlReader

#include <QXmlStreamReader>

#include "ExecutablePaths.h"

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

