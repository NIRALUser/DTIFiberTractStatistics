/**
  DTI Atlas Fiber Analyser

  Command Line : ./DTIAtlasFiberAnalyser --nogui --datafile "datafilename" --analysisfile "analysisfilename"


  Gui window : ./DTIAtlasFiberAnalyser
**/

#include "DTIAtlasFiberAnalyzerCLP.h"
#include "GlobalFunction.h"
#include <QApplication>


int main(int argc, char *argv[])
{
	PARSE_ARGS;
	
	if(!nogui)
	{
		if(debug)
			std::cout<<"DTI Atlas Fiber Analyser WITH GUI"<<std::endl;
		QApplication app(argc, argv);
		/* Set and show the window */
		DTIAtlasFiberAnalyzerguiwindow DTIAFAwindow(debug);
		DTIAFAwindow.show();
        DTIAFAwindow.raise();
		return app.exec();
	}
	else
	{
		if(debug)
			std::cout<<"DTI Atlas Fiber Analyser WITHOUT GUI"<<std::endl;
		
		if(!CommandLine(loadCSV, datafile, analysisfile, debug))
			std::cout<<"Stop DTIAtlasFiberAnalyzer ..."<<std::endl;
		
		return 0;
	}
	
	if(debug)
		std::cout<<"END OF DTI Atlas Fiber Analyser"<<std::endl;
}

