#include "GlobalFunction.h"
#include "DTIAtlasFiberAnalyzerguiwindow.h"
#include "XmlReader.h"

/* itksys */
#include <itksys/Process.h>
#include <itksys/SystemTools.hxx>
#include <itksys/Directory.hxx>

/* QT librairies */
#include <QProcess>
#include <sstream>

/* STL */
#include <exception>


void FindExecutable( const char* name , std::string pathToCurrentExecutable , std::string &pathToExecutable , bool interactive )
{
    // If path already set, we just do nothing.
    // This allows us to leave most of the source code mostly unmodified after adding an option
    // to load software path from an XML file.
    if( !pathToExecutable.empty() )
    {
        return ;
    }
    //Find path for executable
    std::vector< std::string > listDir ;
    listDir.push_back( pathToCurrentExecutable ) ;
#ifdef SlicerExtension
    listDir.push_back( pathToCurrentExecutable + "/../ExternalBin" ) ;
    listDir.push_back( pathToCurrentExecutable + "/../cli-modules" ) ;
#endif
    pathToExecutable = itksys::SystemTools::FindProgram( name , listDir , true ) ;
    if( pathToExecutable.empty() )
    {
        pathToExecutable= itksys::SystemTools::FindProgram( name ) ;
    }
    //if path not found
    if( interactive && pathToExecutable.empty() == true )
    {
        std::cout << "Give the path for " << pathToExecutable << " : " << std::endl ;
        std::cin >> pathToExecutable ;
    }
}

/*********************************************************************************
 * Command Line function
 ********************************************************************************/
bool CommandLine( std::string pathCurrentToExecutable ,
                  std::string CSVFilename ,
                  std::string datafile ,
                  std::string analysisfile ,
                  int bandWidth,
                  bool debug ,
                  double sampling ,
                  bool rodent ,
                  bool removeCleanFibers ,
                  bool removeNanFibers ,
                  std::string configFile,
                  int numThreads
                  )
{
    //variables
    std::string pathFiberProcess, pathdti_tract_stat, OutputFolder, AtlasFiberDir, parameters, csvfile, pathFiberPostProcess;
    vstring fibers, SelectedFibers, SelectedPlanes, fibersplane, Selectedfibersplane;
    int DataCol, DefCol, NameCol,NumberOfParameters;
    DataCol = -1;
    DefCol = -1;
    NameCol = -1;
    bool transposeColRow, success=false, CoG=false, FieldType=true; //FieldType=true -> H-Field false -> D-Field
    bool useBandWidth=true;
    if (bandWidth == -1)
    {
        useBandWidth=true;
    }

    //read the parameters from the file
    if(!ReadParametersFromFiles(datafile, analysisfile, csvfile,AtlasFiberDir,OutputFolder,parameters,DataCol, DefCol, FieldType, NameCol,SelectedFibers, SelectedPlanes, transposeColRow, CoG))
        return false;

    if(debug)
    {
        //print parameters
        std::cout<<"Readed parameters :"<<std::endl;
        std::cout<<"CSV filename : "<< csvfile <<std::endl;
        std::cout<<"AtlasFiberDir : "<<AtlasFiberDir<<std::endl;
        std::cout<<"OutputFolder : "<<OutputFolder<<std::endl;
        std::cout<<"DataCols :"<<DataCol<<std::endl;
        std::cout<<"DefCol :"<<DefCol<<std::endl;
        std::cout<<"Field Type :"<<FieldType<<std::endl;
        std::cout<<"NameCol :"<<NameCol<<std::endl;
        std::cout<<"Selected Fibers :"<<std::endl;
        for(unsigned int i=0;i<SelectedFibers.size();i++)
            std::cout<<SelectedFibers[i]<<std::endl;
        std::cout<<"Selected Planes :"<<std::endl;
        for(unsigned int i=0; i<SelectedPlanes.size(); i++)
            std::cout<<SelectedPlanes[i]<<std::endl;
        std::cout<<"Transpose col and row :"<<transposeColRow<<std::endl;
        std::cout<<"Auto Plane Option :"<<CoG<<std::endl;
    }

    //create the CSV file
    if(!CSVFilename.empty())
        csvfile = CSVFilename;
    CSVClass* CSVFile =  new CSVClass(csvfile,debug);

    //check the extension of the csvfile
    if(ExtensionofFile(csvfile).compare("csv")!=0)
    {
        std::cout<<"File's name : "<<csvfile<<std::endl;
        std::cout<<"Extension of the file : "<<ExtensionofFile(csvfile)<<std::endl;
        std::cout<<"The extension of the file is not .csv!"<<std::endl;
        return false;
    }
    //Load the CSV
    if(!CSVFile->loadCSVFile(csvfile))
        return false;
    // If config file given or environment variable defined, read config XML file
   if( configFile.empty() )
   {
       configFile = itksys::SystemTools::GetEnv( "DTIAtlasFibExecConfig" ) ;
   }
   if( !configFile.empty() )
   {
        XmlReader xmlReader ;
        QString errors = xmlReader.readExecutablesConfigurationFile( configFile.c_str() ) ;
        if( !errors.isEmpty() )
        {
            std::cerr << "Errors detected while trying to read XML configuration file:" << std::endl ;
            std::cerr << errors.toStdString() << std::endl ;
            return false;
        }
        pathFiberProcess = xmlReader.ExecutablePathMap["fiberprocess"].toStdString() ;
        pathFiberPostProcess = xmlReader.ExecutablePathMap["FiberPostProcess"].toStdString() ;
        pathdti_tract_stat = xmlReader.ExecutablePathMap["dtitractstat"].toStdString() ;
    }
    // Read atlas directory
    ReadFiberNameInAtlasDirectory(fibers,fibersplane, AtlasFiberDir);

    std::vector<int> plane;
    //Select the plane of the selected fiber
    for(unsigned int i=0;i<SelectedFibers.size();i++)
    {
        /* Check the plane and add/erase it */
        plane = PlaneAssociatedToFiber(SelectedFibers[i], SelectedPlanes);
        if(plane.size()!=0)
        {
            for(unsigned int k=0; k<plane.size(); k++)
                Selectedfibersplane.push_back(SelectedPlanes[plane[k]]);
        }
        else
            Selectedfibersplane.push_back("");
    }

    //if in the parameter file there is a selected fibers, compute with it, else compute with all the fibers
    if(!SelectedFibers.empty())
    {
        //Find path for fiberprocess
        FindExecutable( "fiberprocess" , pathCurrentToExecutable , pathFiberProcess ) ;
        //Call fiberprocess
        Applyfiberprocess_mt(CSVFile, pathFiberProcess, AtlasFiberDir, 
                            OutputFolder, DataCol, DefCol, FieldType,NameCol, 
                            SelectedFibers,true , numThreads);
        if( removeNanFibers == true )
        {
            //Find path for fiberpostprocess
            FindExecutable( "FiberPostProcess" , pathCurrentToExecutable , pathFiberPostProcess ) ;
            //Call fiberpostrocess
            ApplyFiberPostProcess_mt(CSVFile, pathFiberPostProcess, AtlasFiberDir, 
                             OutputFolder, DataCol, DefCol, FieldType,NameCol, 
                             SelectedFibers,true, numThreads);
        }
        /* Looking for dti_tract_stat */
        FindExecutable( "dtitractstat" , pathCurrentToExecutable , pathdti_tract_stat ) ;
        //Call dti_tract_stat
        Applydti_tract_stat_mt(CSVFile ,
                            pathdti_tract_stat ,
                            AtlasFiberDir ,
                            OutputFolder ,
                            SelectedFibers ,
                            Selectedfibersplane ,
                            parameters ,
                            bandWidth,
                            DataCol ,
                            NameCol ,
                            true ,
                            CoG ,
                            sampling ,
                            rodent ,
                            removeCleanFibers ,
                            removeNanFibers,
                            useBandWidth,
                            numThreads
                            ) ;
    }
    else
    {
        //Find path for fiberprocess
        FindExecutable( "fiberprocess" , pathCurrentToExecutable , pathFiberProcess ) ;
        //Call fiberprocess
        Applyfiberprocess_mt(CSVFile, pathFiberProcess, AtlasFiberDir, 
                            OutputFolder, DataCol, DefCol, FieldType,NameCol, 
                            fibers,true,numThreads);
        if( removeNanFibers == true )
        {
            //Find path for fiberpostprocess
            FindExecutable( "FiberPostProcess" , pathCurrentToExecutable , pathFiberPostProcess ) ;
            //Call fiberpostrocess
            ApplyFiberPostProcess_mt(CSVFile, pathFiberPostProcess, AtlasFiberDir, 
                                OutputFolder, DataCol, DefCol, FieldType,NameCol, 
                                SelectedFibers,true,numThreads);
        }
        /* Looking for dti_tract_stat */
        FindExecutable( "dtitractstat" , pathCurrentToExecutable , pathdti_tract_stat ) ;
        //Call dti_tract_stat
        Applydti_tract_stat_mt(CSVFile ,
                            pathdti_tract_stat ,
                            AtlasFiberDir ,
                            OutputFolder ,
                            fibers ,
                            fibersplane ,
                            parameters ,
                            bandWidth,
                            DataCol ,
                            NameCol ,
                            true ,
                            CoG ,
                            sampling ,
                            rodent ,
                            removeCleanFibers,
                            removeNanFibers,
                            useBandWidth,
                            numThreads
                            ) ;
    }

    //calcul the number of parameters
    NumberOfParameters = CalculNumberOfProfileParam(parameters);
    //Gather fibers profile information
    GatheringFiberProfile(CSVFile, OutputFolder, DataCol, NameCol,transposeColRow, SelectedFibers, success);
    if(success)
        std::cout<<"CSV files were written successfully"<<std::endl;
    else
        std::cout<<"Error gathering fiber profiles : CSV files were not written."<<std::endl;

    return true;
}



/*********************************************************************************
 * Create a folder in the directory 'outputfolder' with the name 'name'
 ********************************************************************************/
bool CreateDirectoryForData(std::string outputfolder, std::string name)
{
    /* create a output directory for the data with the name of the case */
    std::string directory;
    if(name.compare("no")!=0)
    {
        directory = outputfolder + "/" + name;
        return itksys::SystemTools::MakeDirectory(directory.c_str());
    }
    return false;
}

/*********************************************************************************
 * Thread classes
 ********************************************************************************/

void CFPParams::showParams(){
    std::cout << "------------------------------" << std::endl;
    std::cout << "pathFiberProces : "<< pathFiberProcess << std::endl;
    std::cout << "AtlasFolder : "<< AtlasFolder << std::endl;
    std::cout << "outputname : "<< outputname << std::endl;
    std::cout << "Data : "<< Data << std::endl;
    std::cout << "DeformationField : "<< DeformationField << std::endl;
    std::cout << "FieldType : "<< FieldType << std::endl;
    std::cout << "Fiber : "<< Fiber << std::endl;
    std::cout << "------------------------------" << std::endl;
}

//Fiber Post Process
void FiberPostWorker::process(){
    try{

                        //Check if the output exist
        std::string outputcasefolder = "Cases";
        std::string outputCase = params->OutputFolder + "/" + outputcasefolder + "/" ;
        std::string name_of_fiber=params->name_of_fiber;
        std::string namecase = NameOfCase(CSV,params->row,params->NameCol,params->DataCol);
        std::string nameoffile= namecase + "_" + name_of_fiber +"_processed.vtk" ;
                namecase + "_" + name_of_fiber + "_processed.vtk";
        std::string inputName = outputCase + namecase + "/" +
                namecase + "_" + name_of_fiber + ".vtk" ;
        std::string gzoutputname = params->outputname + ".gz";

        if(CallFiberPostProcess(params->pathFiberProcess,
                         inputName,
                         params->outputname,
                         (*CSV->getData())[params->row][params->DataCol])==0)
        {

            if(FileExisted(params->outputname) || FileExisted(gzoutputname))
                CSV->AddData(params->outputname,params->row,params->col);
        }
        else
            std::cout<<"Fail during fiberprocess!"<< std::endl;


    }catch(int e){
        std::cout << "Exception : " << e << std::endl;
        emit error("Exception at Fiberworker::process");
    }

    emit finished();
}

void FiberPostWorker::done(){
    std::cout << "Work done : " << index << std::endl;
    delete params;
    ((FiberPostProcessor*)parent)->childFinished();
}

FiberPostProcessor::FiberPostProcessor(std::queue<CFPParams> q,int _numThreads,CSVClass* _CSV){
  paramsQueue=q;
  numThreads=_numThreads;
  currentThreads=0;
  CSV=_CSV;
};

void FiberPostProcessor::run(){

    // std::vector<Worker*> workers;
    // std::vector<QThread*> threads;
    int index=0;
    while( (!paramsQueue.empty()) || (currentThreads>0)){
        if(currentThreads<numThreads){
            //execute thread
            if(!paramsQueue.empty()){
                CFPParams* params=new CFPParams(paramsQueue.front());
                FiberPostWorker* worker=new FiberPostWorker(this,params,index,CSV);
                QThread* thread=new QThread();
                params->showParams();
                paramsQueue.pop();
                std::cout << "Thread begins ..." << std::endl;
                
                try{
                    worker->moveToThread(thread);
                    connect(thread, SIGNAL (started()), worker, SLOT (process()));
                    connect(worker, SIGNAL (finished()), worker, SLOT (done()));
                    connect(worker, SIGNAL (finished()), thread, SLOT (quit()));
                    connect(worker, SIGNAL (finished()), this, SLOT(childFinished()));
                    connect(worker, SIGNAL (finished()), worker, SLOT (deleteLater()));
                    connect(thread, SIGNAL (finished()), thread, SLOT (deleteLater()));
                    thread->start();
                    currentThreads++;
                    index++;                     
                }catch(int e){
                    std::cout << "Exception in run() : " << e << std::endl;
                }
                std::cout << "Thread sarted" << std::endl;
                QThread::sleep(0.1);
            }else{
                QThread::sleep(1);
            }

        }else{
            //sleep
            //std::cout << "Current number of threads running : " << currentThreads << std::endl;
            QThread::sleep(1);
        }
    }
}

void FiberPostProcessor::childFinished(){
    std::cout <<"Child finished" << std::endl;
    currentThreads--;
}


// Fiber Process
void FiberWorker::process(){
    try{
        if(CallFiberProcess(params->pathFiberProcess,
                         params->AtlasFolder,
                         params->outputname,
                         params->Data,
                         params->DeformationField,
                         params->FieldType,
                         params->Fiber)==0){

                    //Check if the output exist
            std::string outputcasefolder = "Cases";
            std::string name_of_fiber=params->name_of_fiber;
            // std::string header=params->name_of_fiber +".fvp";
            std::string namecase = NameOfCase(CSV,params->row,params->NameCol,params->DataCol);
            std::string nameoffile=namecase + "_" + name_of_fiber + ".vtk";
            std::string outputname = params->OutputFolder + "/" + outputcasefolder + "/" + namecase + "/" +
                    namecase + "_" + name_of_fiber + ".vtk";
            std::string gzoutputname = outputname + ".gz";

            if(FileExisted(outputname) || FileExisted(gzoutputname))
                CSV->AddData(outputname,params->row,params->col);
        }
        else
            std::cout<<"Fail during fiberprocess!"<< std::endl;


    }catch(int e){
        std::cout << "Exception : " << e << std::endl;
        emit error("Exception at Fiberworker::process");
    }

    emit finished();
}

void FiberWorker::done(){
    std::cout << "Work done : " << index << std::endl;
    delete params;
    ((FiberProcessor*)parent)->childFinished();
}

FiberProcessor::FiberProcessor(std::queue<CFPParams> q,int _numThreads,CSVClass* _CSV){
  paramsQueue=q;
  numThreads=_numThreads;
  currentThreads=0;
  CSV=_CSV;
};

void FiberProcessor::run(){

    // std::vector<Worker*> workers;
    // std::vector<QThread*> threads;
    int index=0;
    while( (!paramsQueue.empty()) || (currentThreads>0)){
        if(currentThreads<numThreads){
            //execute thread
            if(!paramsQueue.empty()){
                CFPParams* params=new CFPParams(paramsQueue.front());
                FiberWorker* worker=new FiberWorker(this,params,index,CSV);
                QThread* thread=new QThread();
                params->showParams();
                paramsQueue.pop();
                std::cout << "Thread begins ..." << std::endl;
                
                try{
                    worker->moveToThread(thread);
                    connect(thread, SIGNAL (started()), worker, SLOT (process()));
                    connect(worker, SIGNAL (finished()), worker, SLOT (done()));
                    connect(worker, SIGNAL (finished()), thread, SLOT (quit()));
                    connect(worker, SIGNAL (finished()), this, SLOT(childFinished()));
                    connect(worker, SIGNAL (finished()), worker, SLOT (deleteLater()));
                    connect(thread, SIGNAL (finished()), thread, SLOT (deleteLater()));
                    thread->start();
                    currentThreads++;
                    index++;                     
                }catch(int e){
                    std::cout << "Exception in run() : " << e << std::endl;
                }
                std::cout << "Thread sarted" << std::endl;
                QThread::sleep(0.1);
            }else{
                QThread::sleep(1);
            }

        }else{
            //sleep
            //std::cout << "Current number of threads running : " << currentThreads << std::endl;
            QThread::sleep(1);
        }
    }
}

void FiberProcessor::childFinished(){
    std::cout <<"Child finished" << std::endl;
    currentThreads--;
}


/*********************************************************************************
 * Call fiberprocess for every data/deformation and for every fiber in the atlas
 ********************************************************************************/
bool Applyfiberprocess(CSVClass* CSV,
                       std::string pathFiberProcess,
                       std::string AtlasFiberDir,
                       std::string OutputFolder,
                       int DataCol,
                       int DefCol,
                       bool FieldType,
                       int NameCol,
                       vstring fibers,
                       bool nogui,
                       int numThreads,
                       QWidget* parent)
{
    int col=-1;
    std::vector<bool> skipdata; //0 is skip, 1 alldata, initializate to false,2  cancel
    skipdata.push_back(false);
    skipdata.push_back(false);
    skipdata.push_back(false);
    bool ExistedFile,DataAdded;
    std::string outputname, name_of_fiber,header, namecase, nameoffile, gzoutputname, outputcasefolder;

    /* create the output folder where there will be all of the case output fibers and informations */
    outputcasefolder = "Cases";
    CreateDirectoryForData(OutputFolder,outputcasefolder);

    // Multi Threading qork queue
    std::queue<CFPParams> paramsQueue;

    /* Loop for all the fibers */
    for(unsigned int j=0;j<fibers.size();j++)
    {
        DataAdded = false;
        name_of_fiber=fibers[j];
        header = name_of_fiber;
        name_of_fiber = takeoffExtension(fibers[j]);
        col = HeaderExisted(CSV,header);
        if(col==-1)
        {
            CSV->AddData(header,0,col);
            DataAdded = true;
        }
        /* Loop for all the Data */
        for(unsigned int row=1;row<CSV->getRowSize();row++)
        {
            ExistedFile = false;
            //Set the namecas and the outputname
            namecase = NameOfCase(CSV,row,NameCol,DataCol);
            outputname = OutputFolder + "/" + outputcasefolder + "/" + namecase + "/" +
                    namecase + "_" + name_of_fiber + ".vtk";
            nameoffile = namecase + "_" + name_of_fiber + ".vtk";
            gzoutputname = outputname + ".gz";
            if(FileExisted(outputname) || FileExisted(gzoutputname))
            {
                ExistedFile = true;
                if(!skipdata[1])
                    skipdata = MessageExistedFile(nogui, nameoffile, parent);
            }

            if(skipdata[2] == true)
            {
                if(DataAdded)
                    (*CSV->getData())[0].pop_back();
                return false;
            }
            else if(skipdata[0] && ExistedFile)
            {
                if(!DataExistedInFiberColumn(CSV,row,col,outputname))
                    CSV->AddData(outputname,row,col);
            }
            else
            {
                //if there is a name
                if(namecase.compare("")!=0)
                {
                    /* If the file is created */
                    if(CreateDirectoryForData(OutputFolder+ "/" + outputcasefolder,namecase))
                    {
                        if(DefCol!=-1)
                        {
                            /* If fiberprocess worked */
                            if(CallFiberProcess(pathFiberProcess, 
                                                AtlasFiberDir, 
                                                outputname, 
                                                (*CSV->getData())[row][DataCol], 
                                                (*CSV->getData())[row][DefCol], 
                                                FieldType,fibers[j] ) == 0 )
                            {
                                //Check if the output exist
                                if(FileExisted(outputname) || FileExisted(gzoutputname))
                                    CSV->AddData(outputname,row,col);
                            }
                            else
                                std::cout<<"Fail during fiberprocess!"<< std::endl;
                        }
                        else
                        {
                            /* If fiberprocess worked */
                            if(CallFiberProcess(pathFiberProcess, AtlasFiberDir,
                                                outputname,
                                                (*CSV->getData())[row][DataCol],"no",FieldType,fibers[j] ) == 0 )
                            {
                                //Check if the output exist
                                if(FileExisted(outputname) || FileExisted(gzoutputname))
                                    CSV->AddData(outputname,row,col);
                            }
                            else
                                std::cout<<"Fail during fiberprocess!"<< std::endl;
                        }
                    }
                    else
                        std::cout<<"ERROR : Unable to create the output directory!"
                                <<std::endl;
                }
                else
                    CSV->AddData("no",row,col);
            }
        }
    }

    //save
    std::string filename;
    filename = takeoffPath(CSV->getFilename());
    filename = takeoffExtension(filename);
    filename = OutputFolder+ "/" + filename + "_computed.csv";
    CSV->SaveFile(filename);

    return true;
}

bool Applyfiberprocess_mt(CSVClass* CSV,
                       std::string pathFiberProcess,
                       std::string AtlasFiberDir,
                       std::string OutputFolder,
                       int DataCol,
                       int DefCol,
                       bool FieldType,
                       int NameCol,
                       vstring fibers,
                       bool nogui,
                       int numThreads,
                       QWidget* parent)
{
    int col=-1;
    std::vector<bool> skipdata; //0 is skip, 1 alldata, initializate to false,2  cancel
    skipdata.push_back(false);
    skipdata.push_back(false);
    skipdata.push_back(false);
    bool ExistedFile,DataAdded;
    std::string outputname, name_of_fiber,header, namecase, nameoffile, gzoutputname, outputcasefolder;

    /* create the output folder where there will be all of the case output fibers and informations */
    outputcasefolder = "Cases";
    CreateDirectoryForData(OutputFolder,outputcasefolder);

    // Multi Threading qork queue
    std::queue<CFPParams> paramsQueue;
    
    /* Loop for all the fibers */
    for(unsigned int j=0;j<fibers.size();j++)
    {
        DataAdded = false;
        name_of_fiber=fibers[j];
        header = name_of_fiber;
        name_of_fiber = takeoffExtension(fibers[j]);
        col = HeaderExisted(CSV,header);
        if(col==-1)
        {
            CSV->AddData(header,0,col);
            DataAdded = true;
        }
        /* Loop for all the Data */
        for(unsigned int row=1;row<CSV->getRowSize();row++)
        {
            ExistedFile = false;
            //Set the namecas and the outputname
            namecase = NameOfCase(CSV,row,NameCol,DataCol);
            outputname = OutputFolder + "/" + outputcasefolder + "/" + namecase + "/" +
                    namecase + "_" + name_of_fiber + ".vtk";
            nameoffile = namecase + "_" + name_of_fiber + ".vtk";
            gzoutputname = outputname + ".gz";
            if(FileExisted(outputname) || FileExisted(gzoutputname))
            {
                ExistedFile = true;
                if(!skipdata[1])
                    skipdata = MessageExistedFile(nogui, nameoffile, parent);
            }

            if(skipdata[2] == true)
            {
                if(DataAdded)
                    (*CSV->getData())[0].pop_back();
                return false;
            }
            else if(skipdata[0] && ExistedFile)
            {
                if(!DataExistedInFiberColumn(CSV,row,col,outputname))
                    CSV->AddData(outputname,row,col);
            }
            else
            {
                //if there is a name
                if(namecase.compare("")!=0)
                {
                    /* If the file is created */
                    if(CreateDirectoryForData(OutputFolder+ "/" + outputcasefolder,namecase))
                    {
                        // if(DefCol!=-1)
                        // {
                        //     /* If fiberprocess worked */
                        //     if(CallFiberProcess(pathFiberProcess, 
                        //                         AtlasFiberDir, 
                        //                         outputname, 
                        //                         (*CSV->getData())[row][DataCol], 
                        //                         (*CSV->getData())[row][DefCol], 
                        //                         FieldType,fibers[j] ) == 0 )
                        //     {
                        //         //Check if the output exist
                        //         if(FileExisted(outputname) || FileExisted(gzoutputname))
                        //             CSV->AddData(outputname,row,col);
                        //     }
                        //     else
                        //         std::cout<<"Fail during fiberprocess!"<< std::endl;
                        // }
                        // else
                        // {
                        //     /* If fiberprocess worked */
                        //     if(CallFiberProcess(pathFiberProcess, AtlasFiberDir,
                        //                         outputname,
                        //                         (*CSV->getData())[row][DataCol],"no",FieldType,fibers[j] ) == 0 )
                        //     {
                        //         //Check if the output exist
                        //         if(FileExisted(outputname) || FileExisted(gzoutputname))
                        //             CSV->AddData(outputname,row,col);
                        //     }
                        //     else
                        //         std::cout<<"Fail during fiberprocess!"<< std::endl;
                        // }
                        paramsQueue.push(CFPParams(pathFiberProcess,
                                                   AtlasFiberDir,
                                                   outputname,
                                                   (*CSV->getData())[row][DataCol],
                                                   (*CSV->getData())[row][DefCol],
                                                   FieldType,
                                                   fibers[j],
                                                   false,name_of_fiber,col,row,DataCol,NameCol,OutputFolder
                                                    ));


                    }
                    else
                        std::cout<<"ERROR : Unable to create the output directory!"
                                <<std::endl;
                }
                else
                    CSV->AddData("no",row,col);
            }
        }
    }

    //run Thread
    std::cout << "Generating thread queue is finished" << std::endl;
    //run queue in multithreaded way
    FiberProcessor threadRunner(paramsQueue,numThreads,CSV);
    threadRunner.run();

    std::cout << "Threads are executed" << std::endl;
    //save
    std::string filename;
    filename = takeoffPath(CSV->getFilename());
    filename = takeoffExtension(filename);
    filename = OutputFolder+ "/" + filename + "_computed.csv";
    CSV->SaveFile(filename);

    return true;
}
bool ApplyFiberPostProcess(CSVClass* CSV,
                           std::string pathFiberPostProcess,
                           std::string AtlasFiberDir,
                           std::string OutputFolder,
                           int DataCol,
                           int DefCol,
                           bool FieldType,
                           int NameCol,
                           vstring fibers,
                           bool nogui,
                           int numThreads,
                           QWidget* parent)
{
    int col=-1;
    std::vector<bool> skipdata; //0 is skip, 1 alldata, initializate to false,2  cancel
    skipdata.push_back(false);
    skipdata.push_back(false);
    skipdata.push_back(false);
    bool ExistedFile,DataAdded;
    std::string outputname, name_of_fiber,header, namecase, nameoffile, gzoutputname, outputcasefolder, outputCase , inputName;
    /* use the output folder where there will be all of the case output fibers and informations */
    outputcasefolder = "Cases";
    outputCase = OutputFolder + "/" + outputcasefolder + "/" ;

    /* Loop for all the fibers */
    for(unsigned int j=0;j<fibers.size();j++)
    {
        DataAdded = false;
        name_of_fiber=fibers[j];
        header = name_of_fiber;
        name_of_fiber = takeoffExtension(fibers[j]);
        col = HeaderExisted(CSV,header);
        if(col==-1)
        {
            CSV->AddData(header,0,col);
            DataAdded = true;
        }
        /* Loop for all the Data */
        for(unsigned int row=1;row<CSV->getRowSize();row++)
        {
            ExistedFile = false;
            //Set the namecas and the outputname
            namecase = NameOfCase(CSV,row,NameCol,DataCol);
            outputname = outputCase + namecase + "/" +
                    namecase + "_" + name_of_fiber + "_processed.vtk";
            nameoffile = namecase + "_" + name_of_fiber +"_processed.vtk" ;
            inputName = outputCase + namecase + "/" +
                    namecase + "_" + name_of_fiber + ".vtk" ;
            gzoutputname = outputname + ".gz";
            if(FileExisted(outputname) || FileExisted(gzoutputname))
            {
                ExistedFile = true;
                if(!skipdata[1])
                    skipdata = MessageExistedFile(nogui, nameoffile, parent);
            }
            if(skipdata[2] == true)
            {
                if(DataAdded)
                    (*CSV->getData())[0].pop_back();
                return false;
            }
            else if(skipdata[0] && ExistedFile)
            {
                if(!DataExistedInFiberColumn(CSV,row,col,outputname))
                    CSV->AddData(outputname,row,col);
            }
            else
            {
                //if there is a name
                if(namecase.compare("")!=0)
                {
                    /* If the file is created */
                    if(CreateDirectoryForData(OutputFolder+ "/" + outputcasefolder,namecase))
                    {
                        if(DefCol!=-1)
                        {
                            /* If fiberpostprocess worked */
                            if(CallFiberPostProcess(pathFiberPostProcess, inputName, outputname, (*CSV->getData())[row][DataCol] ) == 0 )
                            {
                                //Check if the output exist
                                if(FileExisted(outputname) || FileExisted(gzoutputname))
                                    CSV->AddData(outputname,row,col);
                            }
                            else
                                std::cout<<"Fail during FiberPostProcess!"<< std::endl;
                        }
                        else
                        {
                            /* If fiberpostprocess worked */
                            if(CallFiberPostProcess(pathFiberPostProcess, AtlasFiberDir,
                                                    outputname,
                                                    (*CSV->getData())[row][DataCol] ) == 0 )
                            {
                                //Check if the output exist
                                if(FileExisted(outputname) || FileExisted(gzoutputname))
                                    CSV->AddData(outputname,row,col);
                            }
                            else
                                std::cout<<"Fail during fiberprocess!"<< std::endl;
                        }
                    }
                    else
                        std::cout<<"ERROR : Unable to create the output directory!"
                                <<std::endl;
                }
                else
                    CSV->AddData("no",row,col);
            }
        }
    }

    //save
    std::string filename;
    filename = takeoffPath(CSV->getFilename());
    filename = takeoffExtension(filename);
    filename = OutputFolder+ "/" + filename + "_computed.csv";
    CSV->SaveFile(filename);

    return true;
}

bool ApplyFiberPostProcess_mt(CSVClass* CSV,
                           std::string pathFiberPostProcess,
                           std::string AtlasFiberDir,
                           std::string OutputFolder,
                           int DataCol,
                           int DefCol,
                           bool FieldType,
                           int NameCol,
                           vstring fibers,
                           bool nogui,
                           int numThreads,
                           QWidget* parent)
{
    int col=-1;
    std::vector<bool> skipdata; //0 is skip, 1 alldata, initializate to false,2  cancel
    skipdata.push_back(false);
    skipdata.push_back(false);
    skipdata.push_back(false);
    bool ExistedFile,DataAdded;
    std::string outputname, name_of_fiber,header, namecase, nameoffile, gzoutputname, outputcasefolder, outputCase , inputName;
    /* use the output folder where there will be all of the case output fibers and informations */
    outputcasefolder = "Cases";
    outputCase = OutputFolder + "/" + outputcasefolder + "/" ;

    std::queue<CFPParams> paramsQueue;

    /* Loop for all the fibers */
    for(unsigned int j=0;j<fibers.size();j++)
    {
        DataAdded = false;
        name_of_fiber=fibers[j];
        header = name_of_fiber;
        name_of_fiber = takeoffExtension(fibers[j]);
        col = HeaderExisted(CSV,header);
        if(col==-1)
        {
            CSV->AddData(header,0,col);
            DataAdded = true;
        }
        /* Loop for all the Data */
        for(unsigned int row=1;row<CSV->getRowSize();row++)
        {
            ExistedFile = false;
            //Set the namecas and the outputname
            namecase = NameOfCase(CSV,row,NameCol,DataCol);
            outputname = outputCase + namecase + "/" +
                    namecase + "_" + name_of_fiber + "_processed.vtk";
            nameoffile = namecase + "_" + name_of_fiber +"_processed.vtk" ;
            inputName = outputCase + namecase + "/" +
                    namecase + "_" + name_of_fiber + ".vtk" ;
            gzoutputname = outputname + ".gz";
            if(FileExisted(outputname) || FileExisted(gzoutputname))
            {
                ExistedFile = true;
                if(!skipdata[1])
                    skipdata = MessageExistedFile(nogui, nameoffile, parent);
            }
            if(skipdata[2] == true)
            {
                if(DataAdded)
                    (*CSV->getData())[0].pop_back();
                return false;
            }
            else if(skipdata[0] && ExistedFile)
            {
                if(!DataExistedInFiberColumn(CSV,row,col,outputname))
                    CSV->AddData(outputname,row,col);
            }
            else
            {
                //if there is a name
                if(namecase.compare("")!=0)
                {
                    /* If the file is created */
                    if(CreateDirectoryForData(OutputFolder+ "/" + outputcasefolder,namecase))
                    {
                        // if(DefCol!=-1)
                        // {
                        //     /* If fiberpostprocess worked */
                        //     if(CallFiberPostProcess(pathFiberPostProcess, 
                        //                             inputName, 
                        //                             outputname, 
                        //                             (*CSV->getData())[row][DataCol] ) == 0 )
                        //     {
                        //         //Check if the output exist
                        //         if(FileExisted(outputname) || FileExisted(gzoutputname))
                        //             CSV->AddData(outputname,row,col);
                        //     }
                        //     else
                        //         std::cout<<"Fail during FiberPostProcess!"<< std::endl;
                        // }
                        // else
                        // {
                        //     /* If fiberpostprocess worked */
                        //     if(CallFiberPostProcess(pathFiberPostProcess, AtlasFiberDir,
                        //                             outputname,
                        //                             (*CSV->getData())[row][DataCol] ) == 0 )
                        //     {
                        //         //Check if the output exist
                        //         if(FileExisted(outputname) || FileExisted(gzoutputname))
                        //             CSV->AddData(outputname,row,col);
                        //     }
                        //     else
                        //         std::cout<<"Fail during fiberprocess!"<< std::endl;
                        // }
                        paramsQueue.push(CFPParams(pathFiberPostProcess,
                                                   AtlasFiberDir,
                                                   outputname,
                                                   (*CSV->getData())[row][DataCol],
                                                   (*CSV->getData())[row][DefCol],
                                                   FieldType,
                                                   fibers[j],
                                                   false,name_of_fiber,col,row,DataCol,NameCol,OutputFolder
                                                    ));
                    }
                    else
                        std::cout<<"ERROR : Unable to create the output directory!"
                                <<std::endl;
                }
                else
                    CSV->AddData("no",row,col);
            }
        }
    }

    //Thread execution
    std::cout << "Generating thread queue is finished" << std::endl;
    //run queue in multithreaded way
    FiberPostProcessor threadRunner(paramsQueue,numThreads,CSV);
    threadRunner.run();
    std::cout << "Threads are executed" << std::endl;

    //save
    std::string filename;
    filename = takeoffPath(CSV->getFilename());
    filename = takeoffExtension(filename);
    filename = OutputFolder+ "/" + filename + "_computed.csv";
    CSV->SaveFile(filename);

    return true;
}
/* Return the column with the header name or -1 if there is no column*/
int HeaderExisted(CSVClass* CSV, std::string header)
{
    //Find the fibername column if exist just in the first row = header
    for(unsigned int i=0;i<CSV->getColSize(0);i++)
    {
        if((*CSV->getData())[0][i].compare(header)==0)
        {
            return i;
        }
    }

    return -1;
}

/* Return true if there is a data in the colomn with the fiber name at the row given */
bool DataExistedInFiberColumn(CSVClass* CSV, int row, int column,std::string outputname)
{
    if(column >= 0)
    {
        //check there is a data
        if((*CSV->getData())[row][column].compare(outputname)==0)
            return true;
    }

    return false;
}

/* Return true if the filename exists */
bool FileExisted(std::string Filename)
{
    std::ofstream file(Filename.c_str(), std::ios::in | std::ios::out | std::ios::ate);

    if(file)
    {
        file.close();
        return true;
    }

    return false;
}

/* Return the namecase from the input or the namecol*/
std::string NameOfCase(CSVClass* CSV, int row, int NameCol, int DataCol)
{
    std::string namecase;
    /* Filename */
    if(NameCol>=0 && ((*CSV->getData())[row][NameCol].compare("")!=0 &&
                      (*CSV->getData())[row][NameCol].compare("no")!=0 &&
                      (*CSV->getData())[row][NameCol].compare("no name")!=0))
    {
        namecase = (*CSV->getData())[row][NameCol];
    }
    else if(DataCol>=0 && ((*CSV->getData())[row][DataCol].compare("")!=0 &&
                           (*CSV->getData())[row][DataCol].compare("no data")!=0))
    {
        namecase = takeoffPath((*CSV->getData())[row][DataCol]);
        namecase = takeoffExtension(namecase);
    }
    else
    {
        namecase = "";
    }

    return namecase;
}

/* return the value of skip and alldata*/
std::vector<bool> MessageExistedFile(bool nogui, std::string nameoffile, QWidget*parent)
{
    std::vector<bool> value;
    bool skip = false;
    bool alldata = false;
    bool cancel = false;
    if(!nogui)
    {
        QString File;
        File = "The file ";
        File.append((nameoffile).c_str());
        File.append(" already exists.");
        QCheckBox alldatabutton("Apply on all data");
        QMessageBox Box(parent);
        Box.setWindowTitle("Existed File");
        Box.setIcon(QMessageBox::Question);
        Box.setText(File);
        Box.setInformativeText("Do you want to overwrite the computation?");
        Box.setStandardButtons(QMessageBox::Yes | QMessageBox::No |
                               QMessageBox::YesToAll | QMessageBox::NoToAll | QMessageBox::Cancel);
        Box.setDefaultButton(QMessageBox::Yes);
        int ret = Box.exec();
        switch (ret) {
        case QMessageBox::Yes:
            skip = false;
            break;
        case QMessageBox::No:
            skip = true;
            break;
        case QMessageBox::YesToAll:
            alldata = true;
            skip = false;
            break;
        case QMessageBox::NoToAll:
            alldata = true;
            skip = true;
            break;
        case QMessageBox::Cancel:
            cancel = true;
            alldata = true;
            break;
        default:
            break;
        }
    }
    else
    {
        std::string answer;
        std::cout<<"The file "<< (nameoffile).c_str()<<" already exists. Overwrite computation? Enter 'yes' or 'no' or  'yes_all' for yes to all or 'no_all' for no to all."<<std::endl;
        std::cin>>answer;

        if(answer.compare("yes")==0)
            skip = false;
        else if(answer.compare("no")==0)
            skip = true;
        else if(answer.compare("yes_all")==0)
        {
            alldata = true;
            skip = false;
        }
        else if(answer.compare("no_all")==0)
        {
            alldata = true;
            skip = true;
        }
    }
    value.push_back(skip);
    value.push_back(alldata);
    value.push_back(cancel);
    return value;
}


/*********************************************************************************
 * Call Fiberprocess
 ********************************************************************************/
int CallFiberProcess(std::string pathFiberProcess,
                     std::string AtlasFolder,
                     std::string outputname,
                     std::string Data,
                     std::string DeformationField,
                     bool FieldType,
                     std::string Fiber)
{
    int state=0;
    QProcess *process= new QProcess();
    QStringList arguments;

    if(Data.compare("no data")!=0 && Data.compare("no")!=0)
    {
        std::cout<<"Compute fiberprocess..."<< std::endl;
        /* put arguments for each call of fiberprocess */
        //Fiber
        QString qs = (AtlasFolder + "/" + Fiber).c_str();
        arguments.append("--fiber_file" );
        arguments.append(qs );
        //Data
        qs =  Data.c_str();
        arguments.append("--tensor_volume");
        arguments.append(qs);
        //Deformation field
        if(DeformationField.compare("no")!=0 && DeformationField.compare("no deformation")!=0)
        {
            qs =  DeformationField.c_str();
            if(FieldType)
            {
                arguments.append("--h_field");
            }
            else
            {
                arguments.append("--displacement_field");
            }
            arguments.append(qs);
        }
        arguments.append("--no_warp");
        //output
        qs = outputname.c_str();
        arguments.append("--fiber_output");
        arguments.append(qs);
        //verbose
        arguments.append("--verbose");
        // save fiber properties separately for QC visualizations
        arguments.append("--saveProperties");
        std::cout<<"Command Line :  "<< pathFiberProcess.c_str() << " " << (arguments.join(" ")).toStdString() <<std::endl;
        state = process->execute( pathFiberProcess.c_str(), arguments);
    }
    else
        std::cout<<"Warning: No data to attribute fibers!"<<std::endl;
    return state;
}

/*********************************************************************************
 * Call FiberPostProcess
 ********************************************************************************/
int CallFiberPostProcess(std::string pathFiberPostProcess,
                         std::string inputname,
                         std::string outputname,
                         std::string Data
                         )
{
    int state=0;
    QProcess *process= new QProcess();
    QStringList arguments;

    if(Data.compare("no data")!=0 && Data.compare("no")!=0)
    {
        std::cout<<"Compute FiberPostProcess..."<< std::endl;
        /* put arguments for each call of fiberprocess */
        //Fiber
        QString qs = inputname.c_str() ;
        arguments.append("--inputFiberFile" );
        arguments.append( qs );
        //output
        qs = outputname.c_str();
        arguments.append("--outputFiberFile");
        arguments.append(qs);
        std::cout<<"Command Line :  "<< pathFiberPostProcess.c_str() << " " << (arguments.join(" ")).toStdString() <<std::endl;
        state = process->execute( pathFiberPostProcess.c_str(), arguments);
        std::cout<<"done"<<std::endl;
    }
    else
        std::cout<<"Warning: No data to attribute fibers!"<<std::endl;
    return state;
}
/*********************************************************************************
 * Read all the fiber in the Atlas directory
 ********************************************************************************/
void ReadFiberNameInAtlasDirectory(vstring &fibers, vstring &fibersplane, std::string AtlasFiberDir)
{
    itksys::Directory AtlasDirectory;
    AtlasDirectory.Load(AtlasFiberDir.c_str());
    unsigned long NumberOfFiles = AtlasDirectory.GetNumberOfFilesInDirectory(AtlasFiberDir.c_str());
    std::string planename,fibername, extensionfiber, extensionfiberplane;

    //save the name of the Fibers Files in an vstring
    for( unsigned long i=0;i<NumberOfFiles;i++)
    {
        extensionfiber = ExtensionofFile(AtlasDirectory.GetFile(i));
        if(extensionfiber.compare("vtk")==0 || extensionfiber.compare("fib")==0)
        {
            fibers.push_back(AtlasDirectory.GetFile(i));
            fibername = takeoffExtension(AtlasDirectory.GetFile(i));
            //Looking if there is a plane file with the fibers
            for(unsigned long j=0;j<NumberOfFiles;j++)
            {
                planename = takeoffExtension(AtlasDirectory.GetFile(j));
                if(planename.rfind(fibername)!=std::string::npos)
                {
                    extensionfiberplane = ExtensionofFile(AtlasDirectory.GetFile(j));
                    if(extensionfiberplane.compare("fvp")==0)
                        fibersplane.push_back(AtlasDirectory.GetFile(j));
                }
            }
        }
    }
}

/*********************************************************************************
 * Take off the extension of the name's file
 ********************************************************************************/
std::string takeoffExtension(std::string filename)
{
    std::string inputfile;

    /* take of the extension  */
    inputfile = filename.substr(0,filename.find_last_of("."));

    return inputfile;
}

/*********************************************************************************
 * Take off the extension of the name's file
 ********************************************************************************/
std::string takeoffPath(std::string filename)
{
    std::string inputfile;

    /* take of the extension  */
    inputfile = filename.substr(filename.find_last_of("/")+1,filename.size()-filename.find_last_of("/")+1);

    return inputfile;
}

/*********************************************************************************
 * Return the extension of a file
 ********************************************************************************/
std::string ExtensionofFile(std::string filename)
{
    std::string extension;

    /* take of the extension  */
    extension = filename.substr(filename.find_last_of(".")+1,filename.size()-filename.find_last_of(".")+1);

    return extension;
}


/*********************************************************************************
 * Thread classes (DTITractStat)
 ********************************************************************************/

void CTSParams::showParams(){
    std::cout << "------------------------------" << std::endl;
    std::cout << "pathdti_tract_stat : "<< pathdti_tract_stat << std::endl;
    std::cout << "AtlasDirectory : "<< AtlasDirectory << std::endl;
    std::cout << "Input_fiber_file : "<< Input_fiber_file << std::endl;
    std::cout << "Output_fiber_file : "<< Output_fiber_file << std::endl;
    std::cout << "plane : "<< plane << std::endl;
    std::cout << "parameter : "<< parameter << std::endl;
    std::cout << "bandWidth : "<< bandWidth << std::endl;
    std::cout << "CoG : "<< CoG << std::endl;
    std::cout << "sampling : "<< sampling << std::endl;
    std::cout << "rodent : "<< rodent << std::endl;
    std::cout << "clean : "<< clean << std::endl;
    std::cout << "noNan : "<< noNan << std::endl;
    std::cout << "useBandWidth : "<< useBandWidth << std::endl;
    std::cout << "Parametrized : "<< Parametrized << std::endl;
    std::cout << "------------------------------" << std::endl;
}



void Worker::process(){
    try{
        if(Calldti_tract_stat(params->pathdti_tract_stat,
                           params->AtlasDirectory,
                           params->Input_fiber_file,
                           params->Output_fiber_file,
                           params->plane,
                           params->parameter,
                           params->bandWidth,
                           params->CoG,
                           params->sampling,
                           params->rodent,
                           params->clean,
                           params->noNan,
                           params->useBandWidth,
                           params->Parametrized)){

            std::string name_of_fiber=params->name_of_fiber;
            std::string header=params->name_of_fiber +".fvp";
            std::string namecase = NameOfCase(CSV,params->row,params->NameCol,params->DataCol);
            std::string nameoffile=namecase + "_" + params->name_of_fiber + "_" + params->parameter + ".fvp";;
            std::string outputname = params->OutputFolder + "/Cases/" + namecase + "/" + namecase +
                    "_" + params->name_of_fiber + "_"+params->parameter+".fvp";
            if(FileExisted(outputname) && !params->isFinalOutput)
            {
                if(!DataExistedInFiberColumn(CSV,params->row,params->col,params->Output_fiber_file))
                    CSV->AddData(params->Output_fiber_file,params->row,params->col);
            }else
            {
                std::cout<<"Fail during dti_tract_stat!"<< std::endl;
            }
        }
    }catch(int e){
        std::cout << "Exception : " << e << std::endl;
    }

    emit finished();
}

void Worker::done(){
    std::cout << "Work done : " << index << std::endl;
    delete params;
    ((DTITractWorker*)parent)->childFinished();
}

DTITractWorker::DTITractWorker(std::queue<CTSParams> q,int _numThreads,CSVClass* _CSV){
  CSV=_CSV;
  paramsQueue=q;
  numThreads=_numThreads;
  currentThreads=0;
};

void DTITractWorker::run(){

    // std::vector<Worker*> workers;
    // std::vector<QThread*> threads;
    int index=0;
    while( (!paramsQueue.empty()) || (currentThreads>0)){
        if(currentThreads<numThreads){
            //execute thread
            if(!paramsQueue.empty()){
                CTSParams* params=new CTSParams(paramsQueue.front());
                Worker* worker=new Worker(this,params,index,CSV);
                QThread* thread=new QThread();
                params->showParams();
                paramsQueue.pop();
                std::cout << "Thread begins ..." << std::endl;
                
                try{
                    worker->moveToThread(thread);
                    connect(thread, SIGNAL (started()), worker, SLOT (process()));
                    connect(worker, SIGNAL (finished()), worker, SLOT (done()));
                    connect(worker, SIGNAL (finished()), thread, SLOT (quit()));
                    connect(worker, SIGNAL (finished()), this, SLOT(childFinished()));
                    connect(worker, SIGNAL (finished()), worker, SLOT (deleteLater()));
                    connect(thread, SIGNAL (finished()), thread, SLOT (deleteLater()));
                    thread->start();
                    currentThreads++;
                    index++;                     
                }catch(int e){
                    std::cout << "Exception in run() : " << e << std::endl;
                }
                std::cout << "Thread sarted" << std::endl;
                QThread::sleep(0.1);
            }else{
                QThread::sleep(1);
            }

        }else{
            //sleep
            //std::cout << "Current number of threads running : " << currentThreads << std::endl;
            QThread::sleep(1);
        }
    }
}

void DTITractWorker::childFinished(){
    std::cout <<"Child finished" << std::endl;
    currentThreads--;
}

/*********************************************************************************
 * Call fiberprocess for every data/deformation and for every fiber in the atlas
 ********************************************************************************/
bool Applydti_tract_stat(CSVClass* CSV, std::string pathdti_tract_stat, std::string AtlasDirectory,
                         std::string OutputFolder, vstring fibers, vstring fibersplane, std::string parameters, double bandWidth,
                         int DataCol, int NameCol , bool nogui, bool CoG, double sampling , bool rodent , bool removeCleanFibers , 
                         bool removeNanFibers, bool useBandWidth , QWidget *parent)
{
    int col=-1;
    bool DataAdded = false;
    bool ExistedFile = false;
    std::vector<bool> skipdata; //0 is skip, 1 alldata, initializate to false, 2  cancel
    skipdata.push_back(false);
    skipdata.push_back(false);
    std::string outputname, name_of_fiber, header, namecase, nameoffile, globalFile;
    CreateDirectoryForData(OutputFolder,"Fibers");
    vstring param;
    LineInVector(parameters, param);
    bool noNan = 1 ;
    for(int i=0; i< ((int) param.size()); i++)
    {
        /* Loop for all the fibers */
        for(unsigned int j=0;j<fibers.size();j++)
        {
            DataAdded = false;
            name_of_fiber = takeoffExtension(fibers[j]);
            //Column where there is the name of the fiber
            //header for the new column
            header = name_of_fiber +".fvp";
            col = HeaderExisted(CSV,header);
            if(col==-1)
            {
                CSV->AddData(header,0,col);
                DataAdded = true;
            }
            /* Loop for all the Data */
            for(unsigned int row=1;row<CSV->getRowSize();row++)
            {
                ExistedFile = false;
                //Set the namecas and the outputname
                namecase = NameOfCase(CSV,row,NameCol,DataCol);
                outputname = OutputFolder + "/Cases/" + namecase + "/" + namecase +
                        "_" + name_of_fiber + "_"+param[i]+".fvp";
                std::cout<<outputname<<std::endl;
                nameoffile = namecase + "_" + name_of_fiber + "_" + param[i] + ".fvp";
                if(FileExisted(outputname))
                {
                    ExistedFile = true;
                    if(!skipdata[1])
                        skipdata = MessageExistedFile(nogui, nameoffile, parent);
                }

                if(skipdata[2] == true)
                {
                    if(DataAdded)
                        (*CSV->getData())[0].pop_back();
                    return false;
                }
                if(skipdata[0] && ExistedFile)
                {
                    globalFile=OutputFolder + "/Cases/" + namecase + "/" + namecase +
                            "_" + name_of_fiber+".fvp";
                    if(!DataExistedInFiberColumn(CSV,row,col,globalFile))
                    {
                        CSV->AddData(globalFile,row,col);
                        //nouveau vtk
                    }
                }
                else
                {
                    //if there is a name
                    if(namecase.compare("")!=0)
                    {
                        int fibercol = HeaderExisted(CSV, fibers[j]);
                        if(fibercol!=-1)
                        {
                            std::string input_fiber=OutputFolder+"/Cases/" + namecase + "/" + namecase +
                                    "_" + name_of_fiber+"_processed.vtk";//std::string input_fiber = (*CSV->getData())[row][fibercol];
                            globalFile=OutputFolder + "/Cases/" + namecase + "/" + namecase +
                                    "_" + name_of_fiber+".fvp";
                            /* If dti_tract_stat worked */
                            if( removeNanFibers == 0 )
                            {
                                noNan = 0 ;
                            }
                            if(Calldti_tract_stat(pathdti_tract_stat, AtlasDirectory,
                                                  input_fiber, globalFile, fibersplane[j],
                                                  param[i], bandWidth, CoG, sampling , rodent , removeCleanFibers , noNan , useBandWidth,false ) == 0 )
                            {
                                if(FileExisted(outputname))
                                {
                                    if(!DataExistedInFiberColumn(CSV,row,col,globalFile))
                                        CSV->AddData(globalFile,row,col);
                                }
                            }
                            else
                            {
                                std::cout<<"Fail during dti_tract_stat!"<< std::endl;
                                return false;
                            }
                        }
                        else
                        {
                            std::cout<<"Header not found in csv file"<<std::endl;
                            return false;
                        }
                    }
                    else
                        CSV->AddData("no",row,col);
                }
            }
            // Add Calldti_tract_stat with parametrized = true , also change output folder that stores the parametrized vtk file NOTE
            // refere the output path in below commented lines of call_dti
            
            std::string inputname=AtlasDirectory+"/"+fibers[j];
            outputname=OutputFolder+"/Fibers/"+header;
            if( Calldti_tract_stat( pathdti_tract_stat , AtlasDirectory ,
                             inputname , 
                             outputname , 
                             fibersplane[ j ] , 
                             param[ i ] , 
                             bandWidth, 
                             CoG , 
                             sampling , 
                             rodent ,
                             removeCleanFibers, 
                             1 ,  //noNan
                             useBandWidth, 
                             true) != 0 ) // parametrized file generation
            {
                std::cout << "Fail during dti_tract_stat!" << std::endl ;
                return false ;
            } // NOTE (outputname )
            if( FileExisted( outputname ) )
            {
                ExistedFile = true ;
                if( !skipdata[ 1 ] )
                {
                    skipdata = MessageExistedFile(nogui, header, parent);
                }

            }
            if( skipdata[ 2 ] == true )
            {
                return false ;
            }
            else if( !skipdata[ 0 ] || !ExistedFile )
            {
                bool removeCleanFibersAtlas = false ;
                if( removeCleanFibers && i != 0 )
                {
                    removeCleanFibersAtlas = true ;
                }
                noNan = 1 ;
                // if( Calldti_tract_stat( pathdti_tract_stat , AtlasDirectory , inputname , outputname , fibersplane[ j ] , param[ i ] , bandWidth, CoG , sampling , rodent ,removeCleanFibers, noNan , useBandWidth, removeCleanFibersAtlas ) != 0 )
                // {
                //     std::cout << "Fail during dti_tract_stat!" << std::endl ;
                //     return false ;
                // } NOTE (outputname )
            }

        }
    }

    //save
    std::string filename;
    filename = takeoffPath(CSV->getFilename());
    filename = takeoffExtension(filename);
    filename = OutputFolder+ "/" + filename + "_computed.csv";
    CSV->SaveFile(filename);

    return true;
}


/*********************************************************************************
 * Call fiberprocess for every data/deformation and for every fiber in the atlas (multi-threaded version)
 ********************************************************************************/
bool Applydti_tract_stat_mt_org(CSVClass* CSV, std::string pathdti_tract_stat, std::string AtlasDirectory,
                         std::string OutputFolder, vstring fibers, vstring fibersplane, std::string parameters, double bandWidth,
                         int DataCol, int NameCol , bool nogui, bool CoG, double sampling , bool rodent , bool removeCleanFibers , 
                         bool removeNanFibers, bool useBandWidth , int numThreads, QWidget *parent)
{
    int col=-1;
    bool DataAdded = false;
    bool ExistedFile = false;
    std::vector<bool> skipdata; //0 is skip, 1 alldata, initializate to false, 2  cancel
    skipdata.push_back(false);
    skipdata.push_back(false);
    std::string outputname, name_of_fiber, header, namecase, nameoffile, globalFile;
    CreateDirectoryForData(OutputFolder,"Fibers");
    vstring param;
    LineInVector(parameters, param);
    bool noNan = 1 ;
    for(int i=0; i< ((int) param.size()); i++)
    {
        /* Loop for all the fibers */
        for(unsigned int j=0;j<fibers.size();j++)
        {
            DataAdded = false;
            name_of_fiber = takeoffExtension(fibers[j]);
            //Column where there is the name of the fiber
            //header for the new column
            header = name_of_fiber +".fvp";
            col = HeaderExisted(CSV,header);
            if(col==-1)
            {
                CSV->AddData(header,0,col);
                DataAdded = true;
            }
            /* Loop for all the Data */
            for(unsigned int row=1;row<CSV->getRowSize();row++)
            {
                ExistedFile = false;
                //Set the namecas and the outputname
                namecase = NameOfCase(CSV,row,NameCol,DataCol);
                outputname = OutputFolder + "/Cases/" + namecase + "/" + namecase +
                        "_" + name_of_fiber + "_"+param[i]+".fvp";
                std::cout<<outputname<<std::endl;
                nameoffile = namecase + "_" + name_of_fiber + "_" + param[i] + ".fvp";
                if(FileExisted(outputname))
                {
                    ExistedFile = true;
                    if(!skipdata[1])
                        skipdata = MessageExistedFile(nogui, nameoffile, parent);
                }

                if(skipdata[2] == true)
                {
                    if(DataAdded)
                        (*CSV->getData())[0].pop_back();
                    return false;
                }
                if(skipdata[0] && ExistedFile)
                {
                    globalFile=OutputFolder + "/Cases/" + namecase + "/" + namecase +
                            "_" + name_of_fiber+".fvp";
                    if(!DataExistedInFiberColumn(CSV,row,col,globalFile))
                    {
                        CSV->AddData(globalFile,row,col);
                        //nouveau vtk
                    }
                }
                else
                {
                    //if there is a name
                    if(namecase.compare("")!=0)
                    {
                        int fibercol = HeaderExisted(CSV, fibers[j]);
                        if(fibercol!=-1)
                        {
                            std::string input_fiber=OutputFolder+"/Cases/" + namecase + "/" + namecase +
                                    "_" + name_of_fiber+"_processed.vtk";//std::string input_fiber = (*CSV->getData())[row][fibercol];
                            globalFile=OutputFolder + "/Cases/" + namecase + "/" + namecase +
                                    "_" + name_of_fiber+".fvp";
                            /* If dti_tract_stat worked */
                            if( removeNanFibers == 0 )
                            {
                                noNan = 0 ;
                            }
                            if(Calldti_tract_stat(pathdti_tract_stat, AtlasDirectory,
                                                  input_fiber, globalFile, fibersplane[j],
                                                  param[i], bandWidth, CoG, sampling , rodent , removeCleanFibers , noNan , useBandWidth,false ) == 0 )
                            {
                                if(FileExisted(outputname))
                                {
                                    if(!DataExistedInFiberColumn(CSV,row,col,globalFile))
                                        CSV->AddData(globalFile,row,col);
                                }
                            }
                            else
                            {
                                std::cout<<"Fail during dti_tract_stat!"<< std::endl;
                                return false;
                            }
                        }
                        else
                        {
                            std::cout<<"Header not found in csv file"<<std::endl;
                            return false;
                        }
                    }
                    else
                        CSV->AddData("no",row,col);
                }
            }
            // Add Calldti_tract_stat with parametrized = true , also change output folder that stores the parametrized vtk file NOTE
            // refere the output path in below commented lines of call_dti
            
            std::string inputname=AtlasDirectory+"/"+fibers[j];
            outputname=OutputFolder+"/Fibers/"+header;
            if( Calldti_tract_stat( pathdti_tract_stat , AtlasDirectory ,
                             inputname , 
                             outputname , 
                             fibersplane[ j ] , 
                             param[ i ] , 
                             bandWidth, 
                             CoG , 
                             sampling , 
                             rodent ,
                             removeCleanFibers, 
                             1 ,  //noNan
                             useBandWidth, 
                             true) != 0 ) // parametrized file generation
            {
                std::cout << "Fail during dti_tract_stat!" << std::endl ;
                return false ;
            } // NOTE (outputname )
            if( FileExisted( outputname ) )
            {
                ExistedFile = true ;
                if( !skipdata[ 1 ] )
                {
                    skipdata = MessageExistedFile(nogui, header, parent);
                }

            }
            if( skipdata[ 2 ] == true )
            {
                return false ;
            }
            else if( !skipdata[ 0 ] || !ExistedFile )
            {
                bool removeCleanFibersAtlas = false ;
                if( removeCleanFibers && i != 0 )
                {
                    removeCleanFibersAtlas = true ;
                }
                noNan = 1 ;
                // if( Calldti_tract_stat( pathdti_tract_stat , AtlasDirectory , inputname , outputname , fibersplane[ j ] , param[ i ] , bandWidth, CoG , sampling , rodent ,removeCleanFibers, noNan , useBandWidth, removeCleanFibersAtlas ) != 0 )
                // {
                //     std::cout << "Fail during dti_tract_stat!" << std::endl ;
                //     return false ;
                // } NOTE (outputname )
            }

        }
    }

    //save
    std::string filename;
    filename = takeoffPath(CSV->getFilename());
    filename = takeoffExtension(filename);
    filename = OutputFolder+ "/" + filename + "_computed.csv";
    CSV->SaveFile(filename);

    return true;
}

bool Applydti_tract_stat_mt(CSVClass* CSV, std::string pathdti_tract_stat, std::string AtlasDirectory,
                         std::string OutputFolder, vstring fibers, vstring fibersplane, std::string parameters, double bandWidth,
                         int DataCol, int NameCol , bool nogui, bool CoG, double sampling , bool rodent , bool removeCleanFibers , 
                         bool removeNanFibers, bool useBandWidth , int numThreads, QWidget *parent)
{
    int col=-1;
    bool DataAdded = false;
    bool ExistedFile = false;
    std::vector<bool> skipdata; //0 is skip, 1 alldata, initializate to false, 2  cancel
    skipdata.push_back(false);
    skipdata.push_back(false);
    std::string outputname, name_of_fiber, header, namecase, nameoffile, globalFile;
    CreateDirectoryForData(OutputFolder,"Fibers");
    vstring param;
    LineInVector(parameters, param);
    bool noNan = 1 ;

    // Make work queue for threading schedule
    std::queue<CTSParams> paramsQueue;

    for(int i=0; i< ((int) param.size()); i++)
    {
        /* Loop for all the fibers */
        for(unsigned int j=0;j<fibers.size();j++)
        {
            DataAdded = false;
            name_of_fiber = takeoffExtension(fibers[j]);
            //Column where there is the name of the fiber
            //header for the new column
            header = name_of_fiber +".fvp";
            col = HeaderExisted(CSV,header);
            if(col==-1)
            {
                CSV->AddData(header,0,col);
                DataAdded = true;
            }
            /* Loop for all the Data */
            for(unsigned int row=1;row<CSV->getRowSize();row++)
            {
                ExistedFile = false;
                //Set the namecas and the outputname
                namecase = NameOfCase(CSV,row,NameCol,DataCol);
                outputname = OutputFolder + "/Cases/" + namecase + "/" + namecase +
                        "_" + name_of_fiber + "_"+param[i]+".fvp";
                std::cout<<outputname<<std::endl;
                nameoffile = namecase + "_" + name_of_fiber + "_" + param[i] + ".fvp";
                if(FileExisted(outputname))
                {
                    ExistedFile = true;
                    if(!skipdata[1])
                        skipdata = MessageExistedFile(nogui, nameoffile, parent);
                }

                if(skipdata[2] == true)
                {
                    if(DataAdded)
                        (*CSV->getData())[0].pop_back();
                    return false;
                }
                if(skipdata[0] && ExistedFile)
                {
                    globalFile=OutputFolder + "/Cases/" + namecase + "/" + namecase +
                            "_" + name_of_fiber+".fvp";
                    if(!DataExistedInFiberColumn(CSV,row,col,globalFile))
                    {
                        CSV->AddData(globalFile,row,col);
                        //nouveau vtk
                    }
                }
                else
                {
                    //if there is a name
                    if(namecase.compare("")!=0)
                    {
                        int fibercol = HeaderExisted(CSV, fibers[j]);
                        if(fibercol!=-1)
                        {
                            std::string input_fiber=OutputFolder+"/Cases/" + namecase + "/" + namecase +
                                    "_" + name_of_fiber+"_processed.vtk";//std::string input_fiber = (*CSV->getData())[row][fibercol];
                            globalFile=OutputFolder + "/Cases/" + namecase + "/" + namecase +
                                    "_" + name_of_fiber+".fvp";
                            /* If dti_tract_stat worked */
                            if( removeNanFibers == 0 )
                            {
                                noNan = 0 ;
                            }
                            paramsQueue.push(CTSParams(pathdti_tract_stat, AtlasDirectory,
                                                  input_fiber, globalFile, fibersplane[j],
                                                  param[i], bandWidth, CoG, sampling , rodent , 
                                                  removeCleanFibers , noNan , useBandWidth,false,
                                                  false,name_of_fiber,
                                                  col,row,DataCol,NameCol,OutputFolder ));

                        }
                        else
                        {
                            std::cout<<"Header not found in csv file"<<std::endl;
                            return false;
                        }
                    }
                    else
                        CSV->AddData("no",row,col);
                }
            }
            // Add Calldti_tract_stat with parametrized = true , also change output folder that stores the parametrized vtk file NOTE
            // refere the output path in below commented lines of call_dti
            
            std::string inputname=AtlasDirectory+"/"+fibers[j];
            outputname=OutputFolder+"/Fibers/"+header;



            if( FileExisted( outputname ) )
            {
                ExistedFile = true ;
                if( !skipdata[ 1 ] )
                {
                    skipdata = MessageExistedFile(nogui, header, parent);
                }

            }
            if( skipdata[ 2 ] == true )
            {
                return false ;
            }
            else if( !skipdata[ 0 ] || !ExistedFile )
            {
                bool removeCleanFibersAtlas = false ;
                if( removeCleanFibers && i != 0 )
                {
                    removeCleanFibersAtlas = true ;

                }
                noNan = 1 ;
                paramsQueue.push(CTSParams(pathdti_tract_stat , AtlasDirectory ,
                         inputname , 
                         outputname , 
                         fibersplane[ j ] , 
                         param[ i ] , 
                         bandWidth, 
                         CoG , 
                         sampling , 
                         rodent ,
                         removeCleanFibers, 
                         1 ,  //noNan
                         useBandWidth, 
                         true,
                         true,"",-1,-1,-1,-1,"")); // dummies.

            }

        }


    }
    std::cout << "Generating thread queue is finished" << std::endl;
    //run queue in multithreaded way
    DTITractWorker threadRunner(paramsQueue,numThreads,CSV);
    threadRunner.run();

    std::cout << "Threads are executed" << std::endl;
    //save
    std::string filename;
    filename = takeoffPath(CSV->getFilename());
    filename = takeoffExtension(filename);
    filename = OutputFolder+ "/" + filename + "_computed.csv";
    CSV->SaveFile(filename);

    std::cout << "Applydti_tract_stat_mt finished" << std::endl;

    return true;
}



/*********************************************************************************
 * Call dti_tract_stat
 ********************************************************************************/
int Calldti_tract_stat(std::string pathdti_tract_stat,
                       std::string AtlasDirectory,
                       std::string Input_fiber_file,
                       std::string Output_fiber_file,
                       std::string plane,
                       std::string parameter,
                       double bandWidth,
                       bool CoG,
                       double sampling,
                       bool rodent,
                       bool clean ,
                       bool noNan,
                       bool useBandWidth,
                       bool Parametrized // NOTE
                      )
{
    int state=0;
    QProcess* process;
    try{
        process= new QProcess();
    }catch(int e){
        std::cout << "Exception in Calldti_tract_stat : " << e << std::endl;
    }
    
    QStringList arguments;

    std::cout<<"Compute dti_tract_stat..."<< std::endl;
    if(Input_fiber_file.compare("no fiber")!=0 && Input_fiber_file.compare("no")!=0)
    {
        //Input fiber file
        QString qs = Input_fiber_file.c_str();
        arguments.append(QString("--input_fiber_file"));
        arguments.append(qs);
        //Output fiber file
        qs =  (takeoffExtension(Output_fiber_file)+"_"+parameter+".fvp").c_str();
        arguments.append(QString("--ouput_stats_file") );
        arguments.append(qs);

        if(Parametrized)
        {
            qs = (takeoffExtension(Output_fiber_file)+"_parametrized.vtk").c_str();
            //			arguments.append(QString("--output_parametrized_fiber_file ") + qs);
            arguments.append(QString("--output_original_fibers_parametrized"));
            arguments.append(qs);
        }
        if( clean )
        {
            arguments.append(QString("--remove_clean_fiber") ) ;
        }
        if( noNan == 0 )
        {
            arguments.append( QString( "--remove_nan_fibers" ) ) ;
        }
        if (useBandWidth)
        {
            arguments.append( QString( "--bandwidth"));
            arguments.append( QString::number(bandWidth));
        }
        //Plane
        if(plane!="")
        {
            qs = AtlasDirectory.c_str();
            qs.append("/");
            qs.append(plane.c_str());
            arguments.append(QString("--plane_file") );
            arguments.append(qs);
        }
        else
        {
            if(!CoG)
            {
                qs="median";
                arguments.append(QString("--auto_plane_origin"));
                arguments.append(qs);
            }
        }
        //parameter
        if(parameter.compare("")!=0)
        {
            qs = parameter.c_str();
            arguments.append(QString("--parameter_list"));
            arguments.append(qs);
        }
        arguments.append(QString("--step_size") );
        arguments.append(QString::number(sampling) );
        if( rodent )
        {
            arguments.append(QString("--rodent") );
        }
        std::cout<<"Command Line :  "<< pathdti_tract_stat.c_str() << " " << (arguments.join(" ")).toStdString() <<std::endl;
        state = process->execute( pathdti_tract_stat.c_str(), arguments);
        // process->start( pathdti_tract_stat.c_str(), arguments);
        // process->waitForFinished();
        // state=process->state();
        std::cout<<"End of Dti Tract Stat."<<std::endl;
    }
    else
        std::cout<<"Warning: No input fiber!"<<std::endl;

    return state;
}


/*********************************************************************************
 * Gather the fiber profile information from .fvp file :
 * It stocks the data in an vector(case) of vector(line) of vstring (data)
 ********************************************************************************/

std::vector<std::vector<v2string> > GatheringFiberProfile(CSVClass* CSV, 
                                                            std::string OutputFolder, 
                                                            int DataCol, 
                                                            int NameCol, 
                                                            bool transposeColRow, 
                                                            vstring fibers, 
                                                            bool& success)
{

    // variables
    std::string fibername, filename, outputprofilefolder, fiberpath,globalDirectory;
    std::vector<std::vector<v2string> > FiberProfiles;
    std::vector< v2string > v3profiles;
    vstring parameters;
    v2string profiles;


    /* create the output folder where there will be all of the gathering data */
    try{

        outputprofilefolder = "FiberProfiles";
        CreateDirectoryForData(OutputFolder,outputprofilefolder);
        globalDirectory=OutputFolder + "/Cases/" + NameOfCase(CSV, 1, NameCol, DataCol);
        LineInVector(getParamFromDirectory(globalDirectory,takeoffExtension(fibers[0])),parameters);
        fiberpath=OutputFolder + "/Fibers/" + takeoffExtension(fibers[0]) + ".fvp";
        outputprofilefolder = OutputFolder + "/" + outputprofilefolder;

        /* look for the header .fvp and create the folder for each fiber.fvp with the name of the fiber */
        for(unsigned int col=0;col<CSV->getColSize(0);col++)
        {
            if(ExtensionofFile((*CSV->getData())[0][col]).compare("fvp")==0)
            {
                fibername = takeoffExtension((*CSV->getData())[0][col]);
                if(CreateDirectoryForData(outputprofilefolder,fibername))
                {
                    /* read the data of every case and put it in the vector */
                    for(unsigned int row=1;row<CSV->getRowSize();row++)
                    {
                        std::cout <<"GatheringFiberProfile."<<std::endl; // problem
                        //read the profile information from the file and put it for the fiber and data
                        if(ReadProfileInformation((*CSV->getData())[row][col],profiles, parameters))
                            v3profiles.push_back(profiles);
                        profiles.clear();
                    }
                    fiberpath=OutputFolder+"/Fibers/"+(*CSV->getData())[0][col];
                    if(ReadProfileInformation(fiberpath,profiles,parameters))
                    {
                        v3profiles.push_back(profiles);
                        profiles.clear();
                        if(v3profiles.size()!=0)
                            std::cout<<"Informations of "<<fibername<<" read successfully"<<std::endl;
                        else
                        {
                            std::cout<<"Nothing was read."<<std::endl;
                            success=false;
                            return FiberProfiles;
                        }
                    }
                    else
                    {
                        std::cout<<"Read information from the fvp files failed for the fiber "<<fibername<<"!"<<std::endl;
                        success=false;
                        return FiberProfiles;
                    }
                }
                FiberProfiles.push_back(v3profiles);
                for(unsigned int i=0; i<parameters.size(); i++)
                {
                    filename=outputprofilefolder+"/"+fibername+"/"+parameters[i]+"_"+fibername+".csv";
                    WriteProfile(CSV, filename, v3profiles, DataCol, NameCol, i+1, transposeColRow);
                }
                if(ReadFiberPtInformation(fiberpath,profiles,parameters))
                    FiberProfiles[FiberProfiles.size()-1].push_back(profiles);
                profiles.clear();
                v3profiles.clear();
            }
        }
        success=true;
    }catch(std::exception &e){
        std::cout << e.what() << std::endl;
    }
    return FiberProfiles;
}

/*********************************************************************************
 * Read profile information from .fvp file
 ********************************************************************************/

bool ReadProfileInformation(std::string globalFile, v2string& finaldata, vstring parameters)
{
    std::string buffer;
    vstring parameterslines, line;
    v2string data;
    globalFile=takeoffExtension(globalFile);

    for(int i=0; i < ((int) parameters.size()); i++)
    {
        std::string filepath=globalFile+"_"+parameters[i]+".fvp";
        std::ifstream file(filepath.c_str(), std::ios::in); //open the file in reading

        if(file)
        {
            parameterslines=getparameterslines(file);
            getline(file,buffer);
            if(!file.eof())
            {
                data=getdata2(file, getnb_of_samples(parameterslines));
                if(i==0)
                {
                    line=GetColumn(0,data);
                    finaldata.push_back(line);
                }

                line=GetColumn(2,data);
                finaldata.push_back(line);

                file.close();
            }
            else
            {
                std::cout<<"Error reading file: "<<filepath<<" is empty."<<std::endl;
                return false;
            }
        }
        else
        {
            std::cout<<"Error opening the file : "<<filepath<<"!"<<std::endl;
            return false;
        }
    }
    return true;
}

bool ReadFiberPtInformation(std::string globalFile, v2string& fiberptdata, vstring parameters)
{
    std::string buffer;
    vstring parameterslines, line;
    v2string data;
    globalFile=takeoffExtension(globalFile);

    for(int i=0; i< ((int) parameters.size()); i++)
    {
        std::string filepath=globalFile+"_"+parameters[i]+".fvp";
        std::ifstream file(filepath.c_str(), std::ios::in); //open the file in reading
        if(file)
        {
            parameterslines=getparameterslines(file);
            getline(file,buffer);
            data=getdata2(file, getnb_of_samples(parameterslines));
            line=GetColumn(0,data);
            fiberptdata.push_back(line);
            line=GetColumn(1,data);
            fiberptdata.push_back(line);

            file.close();
        }
        else
        {
            std::cout<<"Error opening the file : "<<filepath<<"!"<<std::endl;
            return false;
        }
    }
    return true;
}

/*********************************************************************************
 * Write profile information
 ********************************************************************************/

void WriteProfile(CSVClass* CSV, std::string filename, std::vector<v2string> FiberProfiles, int DataCol, int NameCol, int ParamCol, bool transposeColRow)
{
    /* Open a file in writing */
    std::ofstream bfile(filename.c_str(), std::ios::out);

    if(bfile)
    {
        if(transposeColRow)
        {
            bfile<<"Data vs Arc_length,";
            for(unsigned int i=0; i<FiberProfiles[0][0].size(); i++)
            {
                if(i<(FiberProfiles[0][0].size()-1))
                    bfile << FiberProfiles[0][0][i] << "," ;
                else
                    bfile << FiberProfiles[0][0][i];
            }
            bfile<<std::endl;
            for(unsigned int i=0; i<FiberProfiles.size(); i++)
            {
                if(i<FiberProfiles.size()-1)
                    bfile<<NameOfCase(CSV, i+1, NameCol, DataCol)<<",";
                else
                    bfile<<"Atlas,";
                for(unsigned int j=0; j<FiberProfiles[0][0].size(); j++)
                {
                    if(j<(FiberProfiles[0][0].size()-1))
                        bfile << FiberProfiles[i][ParamCol][j] << "," ;
                    else
                        bfile << FiberProfiles[i][ParamCol][j];
                }
                bfile<<std::endl;
            }
        }
        else
        {
            bfile<<"Arc_length vs Data,";
            for(unsigned int i=1; i<FiberProfiles.size(); i++)
                bfile<<NameOfCase(CSV, i, NameCol, DataCol)<<",";
            bfile<<"Atlas"<<std::endl;
            for(unsigned int j=0; j<FiberProfiles[0][0].size(); j++)
            {
                bfile<<FiberProfiles[0][0][j]<<",";
                for(unsigned int i=0; i<FiberProfiles.size(); i++)
                {
                    if(i<(FiberProfiles.size()-1))
                        bfile << FiberProfiles[i][ParamCol][j] << "," ;
                    else
                        bfile << FiberProfiles[i][ParamCol][j];
                }
                bfile<<std::endl;
            }
        }
        bfile.close();
    }
    else
        std::cout<<"ERROR: Open the file for saving profile info!"<<std::endl;
}

/*********************************************************************************
 * save the parameters for DTIAtlasFiberAnalyzer in a file
 ********************************************************************************/
void SaveData(std::string filename,std::string CSVFilename, int DataCol, int DefCol, bool FieldType, int NameCol, std::string OutputFolder)
{
    std::ofstream savefile(filename.c_str(), std::ios::out);
    if(savefile)
    {
        savefile << "Data parameters for DTIAtlasFiberAnalyzer : " <<std::endl;

        savefile << "CSVFile : " << CSVFilename <<std::endl;
        savefile << "#Data/Case/Deformation Column --> index starts at 1" <<std::endl;
        savefile << "Data Column : " << DataCol +1 <<std::endl;
        if(DefCol!=-1)
            savefile << "Deformation Field Column : " << DefCol +1 <<std::endl;
        if(FieldType==true)
            savefile << "Field Type : h-field"<<std::endl;
        else
            savefile << "Field Type : displacement field"<<std::endl;
        if(NameCol!=-1)
            savefile << "Case Column : " << NameCol +1 <<std::endl;
        savefile << "Output Folder : " << OutputFolder <<std::endl;

        savefile.close();
    }
    else
        std::cout<<"ERROR: Problem to open the file for saving parameters"<<std::endl;
}

void SaveAnalysis(std::string filename, std::string AtlasFiberFolder, vstring FiberSelectedname, vstring RelevantPlane, std::string parameters, bool transposeColRow)
{
    std::string ListOfFiber = "", ListOfPlane = "";
    std::ofstream savefile(filename.c_str(), std::ios::out);
    if(savefile)
    {
        savefile << "Analysis parameters for DTIAtlasFiberAnalyzer : " <<std::endl;

        savefile << "Atlas Fiber Folder : " << AtlasFiberFolder <<std::endl;
        for(unsigned int i=0;i<FiberSelectedname.size();i++)
        {
            if(i!=(FiberSelectedname.size()-1))
                ListOfFiber = ListOfFiber + FiberSelectedname[i] +",";
            else
                ListOfFiber = ListOfFiber + FiberSelectedname[i];
        }
        savefile << "#Selected Fibers in the Atlas--> give the name of the file without the path"<<std::endl;
        savefile << "Selected Fibers : " << ListOfFiber <<std::endl;
        for(unsigned int i=0;i<RelevantPlane.size();i++)
        {
            if(i!=(RelevantPlane.size()-1) && RelevantPlane[i]!="")
                ListOfPlane = ListOfPlane + RelevantPlane[i] +",";
            else
                ListOfPlane = ListOfPlane + RelevantPlane[i];
        }
        savefile << "Selected Planes : " << ListOfPlane <<std::endl;
        if(parameters.compare("")!=0)
        {
            savefile << "#Profile parameter --> choice between fa,md,fro,ad,l2,l3,ga,rd "<<std::endl;
            savefile << "Profile parameter : " << parameters <<std::endl;
        }
        if(transposeColRow)
        {
            savefile << "#What for col and row --> choice between 'Case' or 'Arc lenght' with after 'in row' or 'in column'/'in col'" <<std::endl;
            savefile << "Col and Row : " << "Case in column" <<std::endl;
        }
        else
        {
            savefile << "#What for col and row --> choice between 'Case' or 'Arc lenght' with after 'in row' or 'in column'/'in col'" <<std::endl;
            savefile << "Col and Row : " << "Case in row" <<std::endl;
        }

        savefile.close();
    }
    else
        std::cout<<"ERROR: Problem to open the file for saving parameters"<<std::endl;
}

/*********************************************************************************
 * Read the parameters for DTIAtlasFiberAnalyzer from a file
 ********************************************************************************/
bool ReadParametersFromFiles(std::string datafile, std::string analysisfile, std::string &CSVfilename, std::string &AtlasFiberDir, std::string &OutputFolder, std::string &parameters, int &DataCol, int &DefCol, bool &FieldType, int &NameCol, vstring &SelectedFiber, vstring &SelectedPlanes, bool &transposeColRow, bool &CoG)
{
    //variables
    vstring fibers;
    std::string ListOfFiber, ListOfPlane;

    if(datafile.compare("")!=0)
    {
        std::ifstream file(datafile.c_str() , std::ios::in);  // open in reading
        std::string str,buf1;

        if(file)  // if open
        {
            //the first line
            getline(file, buf1);
            if(buf1.compare(0,44,"Data parameters for DTIAtlasFiberAnalyzer : ")==0)
            {
                /* Loop for reading the file and setting the parameters values */
                while(!file.eof())
                {
                    getline(file, buf1);
                    if(buf1.compare(0,1,"#")!=0)
                    {
                        if(buf1.compare(0,10,"CSVFile : ")==0)
                        {
                            CSVfilename = buf1.substr(10,buf1.size()-10);
                        }
                        else if(buf1.compare(0,14,"Data Column : ")==0)
                        {
                            str = buf1.substr(14,buf1.size()-14);
                            DataCol = atoi(str.c_str())-1;
                        }
                        else if(buf1.compare(0,27,"Deformation Field Column : ")==0)
                        {
                            str = buf1.substr(27,buf1.size()-27);
                            DefCol = atoi(str.c_str())-1;
                        }
                        else if(buf1.compare(0,13,"Field Type : ")==0)
                        {
                            if(buf1.substr(13,buf1.size()-13)=="h-field")
                                FieldType=true;
                            else if(buf1.substr(13,buf1.size()-13)=="displacement field")
                                FieldType=false;
                            else
                                std::cout<<"Warning: Wrong syntax of Field Type. Computing with default h-field."<<std::endl;
                        }
                        else if(buf1.compare(0,14,"Case Column : ")==0)
                        {
                            str = buf1.substr(14,buf1.size()-14);
                            NameCol = atoi(str.c_str())-1;
                        }
                        else if(buf1.compare(0,16,"Output Folder : ")==0)
                            OutputFolder = buf1.substr(16,buf1.size()-16);
                        else if(buf1.size()!=0)
                        {
                            std::cout<<"Syntax error in data file"<<std::endl;
                            std::cout<<"Errot at this line : "<<buf1<<std::endl;
                            return false;
                        }
                    }
                }
                std::cout<<std::endl;
                std::cout<<"Data file loaded : New Data"<<std::endl;
            }
            else
            {
                std::cout<<"ERROR: Wrong file for data parameters"<<std::endl;
                return false;
            }
            file.close();
        }
        else
        {
            std::cout << "ERROR: No data parameters file found" << std::endl;
            return false;
        }
    }

    if(analysisfile.compare("")!=0)
    {
        std::ifstream file(analysisfile.c_str() , std::ios::in);  // open in reading
        std::string str,buf1;

        if(file)  // if open
        {
            //the first line
            getline(file, buf1);
            if(buf1.compare(0,48,"Analysis parameters for DTIAtlasFiberAnalyzer : ")==0)
            {
                /* Loop for reading the file and setting the parameters values */
                while(!file.eof())
                {
                    getline(file, buf1);
                    if(buf1.compare(0,1,"#")!=0)
                    {
                        if(buf1.compare(0,21,"Atlas Fiber Folder : ")==0)
                        {
                            AtlasFiberDir = buf1.substr(21,buf1.size()-21);
                        }
                        else if(buf1.compare(0,18,"Selected Fibers : ")==0)
                        {
                            ListOfFiber = buf1.substr(18,buf1.size()-18);
                            LineInVector(ListOfFiber,SelectedFiber);
                        }
                        else if(buf1.compare(0,18,"Selected Planes : ")==0)
                        {
                            ListOfPlane=buf1.substr(18,buf1.size()-18);
                            LineInVector(ListOfPlane, SelectedPlanes);
                        }
                        else if(buf1.compare(0,20,"Profile parameter : ")==0)
                            parameters = buf1.substr(20,buf1.size()-20);
                        else if(buf1.compare(0,14,"Col and Row : ")==0)
                        {
                            if(buf1.substr(14,buf1.size()-14).compare("Case in column")==0 ||buf1.substr(14,buf1.size()-14).compare("Case in col")==0 ||buf1.substr(14,buf1.size()-14).compare("Arc lenght in row")==0)
                                transposeColRow = true;
                            else
                                transposeColRow = false;
                        }
                        else if(buf1.compare(0,20,"Auto Plane Origin : ")==0)
                        {
                            if(buf1.compare(20, 6, "median")==0)
                                CoG=false;
                            else if(buf1.compare(20,3,"cog")==0)
                                CoG=true;
                        }
                        else if(buf1.size()!=0)
                        {
                            std::cout<<"Syntax error in analysis file"<<std::endl;
                            return false;
                        }
                    }
                }
                std::cout<<std::endl;
                std::cout<<"Analysis file loaded"<<std::endl;
            }
            else
            {
                std::cout<<"ERROR: Wrong file for analysis parameters"<<std::endl;
                return false;
            }
            file.close();
        }
        else
        {
            std::cout << "ERROR: No Analysis parameters file found" << std::endl;
            return false;
        }
    }
    return true;
}

/*********************************************************************************
 * Calcul of the number of profile parameters
 ********************************************************************************/
int CalculNumberOfProfileParam(std::string parameters)
{
    //Iterator on the string line
    std::string::iterator it;
    it = parameters.begin();
    int NumberOfParameters = 0;

    while(it != parameters.end())
    {
        //if the line is at the last word, set the iterator at the end of the string
        if(parameters.find_first_of(",")!=std::string::npos)
        {
            //look for the first ","
            parameters = parameters.substr(parameters.find_first_of(",")+1,
                                           parameters.size()-parameters.find_first_of(",")+1);
            NumberOfParameters++;
        }
        else
        {
            NumberOfParameters++;
            it = parameters.end();
        }
    }

    return NumberOfParameters;
}


/*********************************************************************************
 * Check if there is file fvp in the atlas with the same name than the fiber
 * return the iterator; type = 1, search in fibersplane, type = 2  search in
 * fibersplaneSelected
 ********************************************************************************/

std::vector<int> PlaneAssociatedToFiber(std::string fibername, vstring fibersplane)
{
    std::vector <int> plane;
    for(unsigned int j=0;j<fibersplane.size();j++)
    {
        if(ExtensionofFile(fibersplane[j])=="fvp")
        {
            if(fibersplane[j].rfind(takeoffExtension(fibername))!=std::string::npos)
                plane.push_back(j);
        }
    }
    return plane;
}

/************************************************************************************
 * LineInVector : Like previous definition of LineInVector, take line with data
 * 	separated by ',' and put it in a string vector.
 ************************************************************************************/

void  LineInVector(std::string line, vstring& v_string)
{
    vstring  column;
    //Iterator on the string line
    std::string::iterator it;
    std::string word;
    it = line.begin();
    //add each word or number between two "," in the vector
    while(it != line.end())
    {
        //if the line is at the last word, set the iterator at the end of the string
        if(line.find_first_of(",")!=std::string::npos)
        {
            word = line.substr(0, line.find_first_of(","));
            column.push_back(word);
            it = line.begin();
            //erase the first word in the line
            line = line.substr(line.find_first_of(",")+1,line.size()-line.find_first_of(",")+1);
        }
        else
        {
            column.push_back(line);
            it = line.end();
        }
    }
    v_string=column;
}

/************************************************************************************
 * getnb_of_samples : return the number of data samples from the 6 informations lines
 ************************************************************************************/

int getnb_of_samples(vstring parameterslines)
{
    int i=0, nb_of_samples;
    std::string buffer;
    buffer=parameterslines[0];
    while( buffer.compare(0,36,"Number of samples along the bundle: ")!=0)
    {
        i++;
        buffer=parameterslines[i];
    }
    nb_of_samples=atoi(buffer.substr(35, buffer.size()-35).c_str());
    return nb_of_samples;
}

/************************************************************************************
 * getdata : put every data lines read from the file in a 2D vstring vector
 ************************************************************************************/

v2string getdata2(std::ifstream& fvpfile, int nb_of_samples)
{
    std::string buffer;
    vstring line;
    v2string data;
    for(int i=0; i<nb_of_samples; i++)
    {
        getline(fvpfile, buffer);
        LineInVector(buffer, line);
        data.push_back(line);
    }
    return data;
}

vstring GetColumn(int column, v2string data)
{
    vstring line;
    for(unsigned int i=0; i<data.size(); i++)
    {
        line.push_back(data[i][column]);
    }
    return line;
}

/************************************************************************************
 * getMean : Calculate mean of a data line
 ************************************************************************************/

double getMean(QVector< double > data)
{
    double mean=0;
    for(int i=0; i<data.size(); i++)
        mean+=data[i];
    mean/=data.size();
    return mean;
}

/************************************************************************************
 * getStd : Calculate std of a data line with mean value
 ************************************************************************************/

long double getStd(QVector <double> data, double mean)
{
    long double std=0;
    for(int i=0; i<data.size(); i++)
        std+= (data[i]-mean) * (data[i]-mean);
    std/=data.size();
    std=sqrtl(std);
    return std;
}

/************************************************************************************
 * getStdData : Make the 2D-vector that contains data+factor*std values for a data table
 ************************************************************************************/

QVector <QVector <double> > getStdData(QVector <QVector <double> > data, int factor)
{
    double mean, std;
    QVector <double> stdlinedata;
    QVector <QVector <double> > stddata;

    stddata.push_back(data[0]);
    for(int i=1; i<data.size(); i++)
    {
        mean=0;
        std=0;
        mean=getMean(data[i]);
        std=getStd(data[i], mean);
        for(int j=0; j<data[i].size(); j++)
            stdlinedata.push_back(data[i][j]+factor*std);
        stddata.push_back(stdlinedata);
        stdlinedata.clear();
    }
    return stddata;
}

/************************************************************************************
 * getCrossMeanData : Make the 2D-vector that contains cross mean data for each case
 * 	of a fiber
 ************************************************************************************/

QVector<QVector<double> > getCrossMeanData(qv3double data)
{
    QVector<QVector<double> > MeanData;
    QVector<double> MeanLine;
    double MeanValue=0;
    MeanData.push_back(data[0][0]);
    for(int j=1; j<data[0].size(); j++)
    {
        for(int k=0; k<data[0][0].size(); k++)
        {
            for(int l=0; l<data.size(); l++)
            {
                if(l%3==0)
                    MeanValue+=data[l][j][k];
            }
            MeanValue/=data.size()/3;
            MeanLine.push_back(MeanValue);
            MeanValue=0;
        }
        MeanData.push_back(MeanLine);
        MeanLine.clear();
    }

    return MeanData;
}

/************************************************************************************
 * getCrossStdData : Make the 2D-vector that contains mean+factor*std between each case
 * 	of a fiber.
 ************************************************************************************/

QVector<QVector<double> > getCrossStdData(qv3double data, QVector<QVector<double> > crossmean, int factor)
{
    QVector<QVector<double> > StdData;
    QVector<double> StdLine;
    long double StdValue=0;
    StdData.push_back(crossmean[0]);
    for(int j=1; j<data[0].size(); j++)
    {
        for(int k=0; k<data[0][0].size(); k++)
        {
            for(int l=0; l<data.size(); l++)
            {
                if(l%3==0)
                {
                    StdValue+=powl((data[l][j][k]-crossmean[j][k]),2);
                }
            }
            StdValue/=data.size()/3;
            StdValue=sqrtl(StdValue);
            StdLine.push_back(crossmean[j][k]+factor*StdValue);
            StdValue=0;

        }
        StdData.push_back(StdLine);
        StdLine.clear();
    }
    return StdData;
}

QVector<double> getCorr(QVector<QVector<double> > data1, QVector<QVector<double> > data2)
{
    long double std1, std2, mean1, mean2, corrvalue=0;
    QVector<double> corrvector;
    if(data1.size()!=data2.size() || data1[0].size()!=data2[0].size())
    {
        std::cout<<"Data sizes between cases need to be equals"<<std::endl;
        return corrvector;
    }
    else
    {
        for(int i=1; i<data1.size(); i++)
        {
            mean1=getMean(data1[i]);
            mean2=getMean(data2[i]);
            std1=getStd(data1[i], mean1);
            std2=getStd(data2[i], mean2);
            for(int j=0; j<data1[i].size(); j++)
                corrvalue+=(data1[i][j]-mean1)*(data2[i][j]-mean2);
            corrvalue/=(std1*std2)*data1[i].size();
            corrvector.push_back(corrvalue);
            corrvalue=0;
        }
    }
    return corrvector;
}

/************************************************************************************
* getparameterslines : read the 6 informations lines of a .fvpfile and put it in a
* 	string vector (one line = one string). The reference allow to continue along the input
* 	stream.
 ************************************************************************************/

vstring getparameterslines(std::ifstream& fvpfile)
{
    vstring parameterslines;
    std::string buffer;

    //Put first line in buffer
    getline(fvpfile, buffer);

    //Stop at last line of information or if it's the end of file
    while(buffer.compare(0,35,"Number of samples along the bundle:")!=0 && !fvpfile.eof())
    {
        parameterslines.push_back(buffer);
        getline(fvpfile,buffer);
    }
    parameterslines.push_back(buffer);
    return parameterslines;
}

std::string getParamFromFile(std::string filepath)
{
    std::string parameters="", buffer;
    std::ifstream file(filepath.c_str(), std::ios::in);
    if(file)
    {
        while(!file.eof())
        {
            getline(file,buffer);
            if(buffer.compare(0, 33, "Parameter chosen for regression: ")==0)
            {
                if(buffer.compare(33,3, "fro")==0)
                    parameters+=buffer.substr(33,3)+",";
                else if(buffer.compare(33,14, "All parameters")==0)
                {
                    parameters="fa,md,fro,ad,l2,l3,rd,ga,";
                    break;
                }
                else
                    parameters+=buffer.substr(33,2)+",";
            }
        }
        //erase last coma
        parameters=parameters.substr(0,parameters.size()-1);
    }
    else
        std::cout<<"Error Opening file to read parameters"<<std::endl;
    return parameters;
}

std::string getParamFromDirectory(std::string directory, std::string fibername)
{
    std::string parameters="";
    itksys::Directory Dir;
    Dir.Load(directory.c_str());
    unsigned long NumberOfFiles = Dir.GetNumberOfFilesInDirectory(directory.c_str());

    for(unsigned long i=0; i<NumberOfFiles; i++)
    {
        std::string filename=Dir.GetFile(i);
        if(filename.rfind("fvp")!=std::string::npos && filename.rfind(fibername)!=std::string::npos)
        {
            std::string parameter=filename.substr(filename.find_last_of("_")+1,filename.find_last_of(".")-filename.find_last_of("_")-1);
            if(parameter=="fa" || parameter=="md" || parameter=="fro" || parameter=="l1"||parameter=="l2"||parameter=="l3"||parameter=="rd"||parameter=="ga" || parameter=="ad")
                parameters+=parameter+",";
            else
                std::cout<<"Couldn't find parameter for : "<<filename<<std::endl;
        }
    }

    return parameters.substr(0,parameters.size()-1);
}

bool IsFile(std::string filename)
{
    if(ExtensionofFile(filename)=="")
        return false;
    else
        return true;
}

int CallMergeStatWithFiber(std::string PathMergeStatWithFiber, std::string CSVFilename, std::string VTKFilename, std::string OutputFilename, std::string Min, std::string Max , std::string pvalue )
{
    int state=0;
    QProcess* process= new QProcess();
    QStringList arguments;

    std::cout<<"Compute MergeStatWithFiber..."<<std::endl;
    QString qs=CSVFilename.c_str();
    arguments.append(QString("-c"));
    arguments.append(qs);
    qs=VTKFilename.c_str();
    arguments.append("-v");
    arguments.append(qs);
    qs=OutputFilename.c_str();
    arguments.append("-o");
    arguments.append(qs);
    if(Min!="")
    {
        qs=Min.c_str();
        arguments.append("-m");
        arguments.append(qs);
    }
    if(Max!="")
    {
        qs=Max.c_str();
        arguments.append("-M");
        arguments.append(qs);
    }
    if( pvalue != "" )
    {
        qs=pvalue.c_str();
        arguments.append("-p");
        arguments.append(qs);
    }
    std::cout<<"Command Line: "<< PathMergeStatWithFiber.c_str() << " " << arguments.join(" ").toStdString()<<std::endl;
    state=process->execute(PathMergeStatWithFiber.c_str(), arguments);
    std::cout<<"End of MergeStatWithFiber."<<std::endl;
    return state;
}

bool IsFloat(std::string Str)
{
    std::istringstream iss( Str );
    float temp;
    return (iss >> temp) && (iss.eof());
}
