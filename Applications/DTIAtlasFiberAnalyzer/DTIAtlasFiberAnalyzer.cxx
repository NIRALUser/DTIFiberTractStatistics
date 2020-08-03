/**
  DTI Atlas Fiber Analyser

  Command Line : ./DTIAtlasFiberAnalyser --nogui --datafile "datafilename" --analysisfile "analysisfilename"


  Gui window : ./DTIAtlasFiberAnalyser
**/

#include "DTIAtlasFiberAnalyzerCLP.h"
#include "GlobalFunction.h"
#include "DTIAtlasFiberAnalyzerguiwindow.h"
#include <QApplication>
#include <itksys/SystemTools.hxx>
#include <itksys/Directory.hxx>



int main(int argc, char *argv[])
{
    PARSE_ARGS;
    //Verify that arg0 contains a '/' or a '\'. Otherwise find executable in PATH
    std::string path = argv[ 0 ] ;
    std::string pathToExecutable ;
    size_t found = path.find_last_of("/\\") ;
    if( found != std::string::npos )
    {
        pathToExecutable = itksys::SystemTools::CollapseFullPath( argv[ 0 ] ) ;
    }
    else
    {
        pathToExecutable = itksys::SystemTools::FindProgram( argv[ 0 ] ) ;
    }
    found = pathToExecutable.find_last_of("/\\");
    pathToExecutable = pathToExecutable.substr( 0 , found ) ;
    //setExecutablesConfigFile() ;

    if(!nogui)
    {
        if(debug)
            std::cout<<"DTI Atlas Fiber Analyser WITH GUI"<<std::endl;
        QApplication app(argc, argv);
        /* Set and show the window */
        DTIAtlasFiberAnalyzerguiwindow DTIAFAwindow( pathToExecutable , configFile, debug ) ;
        DTIAFAwindow.show();
        DTIAFAwindow.raise();
        return app.exec();
    }
    else
    {
        if(debug)
            std::cout<<"DTI Atlas Fiber Analyser WITHOUT GUI"<<std::endl;

        if(!CommandLine( pathToExecutable , loadCSV, datafile, analysisfile, bandwidth , debug , sampling , rodent , removeCleanFibers , removeNanFibers , configFile ))
            std::cout<<"Stop DTIAtlasFiberAnalyzer ..."<<std::endl;

        return 0;
    }

    if(debug)
        std::cout<<"END OF DTI Atlas Fiber Analyser"<<std::endl;
}

