#ifndef _GLOBALFUNCTION_H_
#define _GLOBALFUNCTION_H_

#include <QWidget>
#include <QVector>
#include <QThread>
/* Specific librairies */
#include "CSVClass.h"

/* STL */
#include <queue>
#include <vector>

typedef QVector<QVector<double> > qv2double;
typedef QVector<qv2double> qv3double;

/* Functions defined in, exported from Globalfunc.cxx */
/* Get the XML file containing paths to each executable used in the program
void setExecutablesConfigFile() ;*/
/* Call when there is nogui */
bool CommandLine( std::string pathToExecutable , std::string CSVFilename, std::string datafile, std::string analysisfile, int bandWidth, 
					bool debug , double sampling , bool rodent , bool removeCleanFibers, bool removeNanFibers , std::string configFile, int numThreads=1);

/* Create a Directory */
bool CreateDirectoryForData(std::string outputfolder, std::string name);


/* Tab 2 Thread Related */


// Fiber process
class CFPParams{ // Parameter class for Calldti_tract_stat
  public:
    CFPParams(std::string _pathFiberProcess,
              std::string _AtlasFolder,
              std::string _outputname,
              std::string _Data,
              std::string _DeformationField,
              bool _FieldType,
              std::string _Fiber,
              bool _isFinalOutput,
              std::string _name_of_fiber,
              int _col,
              int _row,
              int _DataCol, 
              int _NameCol, 
              std::string _OutputFolder){
      
      pathFiberProcess=_pathFiberProcess;
      AtlasFolder=_AtlasFolder;
      outputname=_outputname;
      Data=_Data;
      DeformationField=_DeformationField;
      FieldType=_FieldType;
      Fiber=_Fiber;    

      /* For threading*/
      isFinalOutput=_isFinalOutput;
      name_of_fiber=_name_of_fiber;
      col=_col;
      row=_row;
      DataCol=_DataCol;
      NameCol=_NameCol;
      OutputFolder=_OutputFolder;

    };
    CFPParams(const CFPParams& params){
      init(params.pathFiberProcess,
           params.AtlasFolder,
           params.outputname,
           params.Data,
           params.DeformationField,
           params.FieldType,
           params.Fiber,
           params.isFinalOutput,
           params.name_of_fiber,
           params.col,
           params.row,
           params.DataCol,
           params.NameCol,
           params.OutputFolder
           );
    };
    ~CFPParams(){};

    void init(std::string _pathFiberProcess,
              std::string _AtlasFolder,
              std::string _outputname,
              std::string _Data,
              std::string _DeformationField,
              bool _FieldType,
              std::string _Fiber,
              bool _isFinalOutput,
              std::string _name_of_fiber,
              int _col,
              int _row,
              int _DataCol, 
              int _NameCol, 
              std::string _OutputFolder){

      pathFiberProcess=_pathFiberProcess;
      AtlasFolder=_AtlasFolder;
      outputname=_outputname;
      Data=_Data;
      DeformationField=_DeformationField;
      FieldType=_FieldType;
      Fiber=_Fiber;

      //For Threading
      isFinalOutput=_isFinalOutput;
      name_of_fiber=_name_of_fiber;
      col=_col;
      row=_row;
      DataCol=_DataCol;
      NameCol=_NameCol;
      OutputFolder=_OutputFolder;
    };

    std::string pathFiberProcess;
    std::string AtlasFolder;
    std::string outputname;
    std::string Data;
    std::string DeformationField;
    bool FieldType;
    std::string Fiber;
    /* For Threading */
      
    bool isFinalOutput;
    std::string name_of_fiber;
    int col;
    int row;
    int DataCol;
    int NameCol;
    std::string OutputFolder;

    void showParams();
};

class FiberWorker: public QObject{
  Q_OBJECT
  public:
    FiberWorker(QObject* _parent,CFPParams* p,int _index,CSVClass* _CSV){
      params=p;
      index=_index;
      parent=_parent;
      CSV=_CSV;
    };
    ~FiberWorker(){};
  public slots:
    void process();
    void done();
  signals:
    void finished();
    void error(QString err);
  private:
    CFPParams* params;
    QObject* parent;
    int index;
    CSVClass* CSV;
};

class FiberProcessor : public QObject{
  Q_OBJECT
  public:
   FiberProcessor(std::queue<CFPParams>,int,CSVClass*);
    ~FiberProcessor(){};
    void run();
  public slots:
    void childFinished();
  private:
    std::queue<CFPParams> paramsQueue;
    int numThreads;
    int currentThreads;
    CSVClass* CSV;
};

// Fiber Post Process

class FiberPostWorker: public QObject{
  Q_OBJECT
  public:
    FiberPostWorker(QObject* _parent,CFPParams* p,int _index,CSVClass* _CSV){
      params=p;
      index=_index;
      parent=_parent;
      CSV=_CSV;
    };
    ~FiberPostWorker(){};
  public slots:
    void process();
    void done();
  signals:
    void finished();
    void error(QString err);
  private:
    CFPParams* params;
    QObject* parent;
    int index;
    CSVClass* CSV;
};

class FiberPostProcessor : public QObject{
  Q_OBJECT
  public:
   FiberPostProcessor(std::queue<CFPParams>,int,CSVClass*);
    ~FiberPostProcessor(){};
    void run();
  public slots:
    void childFinished();
  private:
    std::queue<CFPParams> paramsQueue;
    int numThreads;
    int currentThreads;
    CSVClass* CSV;
};

/* Compute fiber process : take the parameters and call the software for every fiber/data */

bool Applyfiberprocess_mt(CSVClass* CSV, std::string pathFiberProcess, std::string AtlasFiberDir,
               std::string OutputFolder, int DataCol, int DefCol, bool FieldType, int NameCol, vstring fibers,
           bool nogui, int numThreads,QWidget* parent=NULL);

/* Compute FiberPostProcess : take the parameters and call the software for every fiber/data */

bool ApplyFiberPostProcess_mt(CSVClass* CSV, std::string pathFiberProcess, std::string AtlasFiberDir,
               std::string OutputFolder, int DataCol, int DefCol, bool FieldType, int NameCol, vstring fibers,
           bool nogui, int numThreads, QWidget* parent=NULL);

/* Check if the header existed */
int HeaderExisted(CSVClass* CSV, std::string header);

/* Check if there is a data */
bool DataExistedInFiberColumn(CSVClass* CSV, int row, int column, std::string outputname);

/* Check if the file existed */
bool FileExisted(std::string Filename);

/* Give the name of the case */
std::string NameOfCase(CSVClass* CSV, int row, int NameCol, int DataCol);

/* Give the choice of the user about skip the computation */
std::vector<bool> MessageExistedFile(bool nogui, std::string nameoffile, QWidget*parent=NULL);

/* Call fiberprocess */
int CallFiberProcess(std::string pathFiberProcess, std::string AtlasFolder, std::string outputname,
             std::string Data, std::string DeformationField, bool FieldType, std::string Fiber);

/* Call fiberPostProcess */
int CallFiberPostProcess(std::string pathFiberProcess, std::string inputname, std::string outputname,
             std::string Data);

/* Read the files in the folder and keep the one with the extension ".vtk" */
void ReadFiberNameInAtlasDirectory(vstring &fibers, vstring &fibersplane, std::string AtlasFiberDir);

/* Take off the extension of the filename */
std::string takeoffExtension(std::string filename);

/* Take off the path of the filename */
std::string takeoffPath(std::string filename);

/* Keep just the extension of the filename */
std::string ExtensionofFile(std::string filename);

/* Thread class (QT) */

class CTSParams{ // Parameter class for Calldti_tract_stat
  public:
    CTSParams(std::string _pathdti_tract_stat, std::string _AtlasDirectory, std::string _Input_fiber_file, 
      std::string _Output_fiber_file, std::string _plane, std::string _parameter, double _bandWidth,
      bool _CoG, double _sampling , bool _rodent , bool _clean , bool _noNan, bool _useBandWidth, bool _Parametrized,
      bool _isFinalOutput,std::string _name_of_fiber,int _col, int _row, int _DataCol,int _NameCol,std::string _OutputFolder){
    
      pathdti_tract_stat=_pathdti_tract_stat;
      AtlasDirectory=_AtlasDirectory;
      Input_fiber_file=_Input_fiber_file;
      Output_fiber_file=_Output_fiber_file;
      plane=_plane;
      parameter=_parameter;
      bandWidth=_bandWidth;
      CoG=_CoG;
      sampling=_sampling;
      rodent=_rodent;
      clean=_clean;
      noNan=_noNan;
      useBandWidth=_useBandWidth;
      Parametrized=_Parametrized;

      isFinalOutput=_isFinalOutput;
      name_of_fiber=_name_of_fiber;
      col=_col;
      row=_row;
      DataCol=_DataCol;
      NameCol=_NameCol;
      OutputFolder=_OutputFolder;

    };
    CTSParams(const CTSParams& params){
      init(params.pathdti_tract_stat,
                           params.AtlasDirectory,
                           params.Input_fiber_file,
                           params.Output_fiber_file,
                           params.plane,
                           params.parameter,
                           params.bandWidth,
                           params.CoG,
                           params.sampling,
                           params.rodent,
                           params.clean,
                           params.noNan,
                           params.useBandWidth,
                           params.Parametrized,
                           params.isFinalOutput,
                           params.name_of_fiber,
                           params.col,
                           params.row,
                           params.DataCol,
                           params.NameCol,
                           params.OutputFolder);
    };
    ~CTSParams(){};

    void init(std::string _pathdti_tract_stat, std::string _AtlasDirectory, std::string _Input_fiber_file, 
      std::string _Output_fiber_file, std::string _plane, std::string _parameter, double _bandWidth,
      bool _CoG, double _sampling , bool _rodent , bool _clean , bool _noNan, bool _useBandWidth, bool _Parametrized,
      bool _isFinalOutput,std::string _name_of_fiber,int _col,int _row,int _DataCol, int _NameCol, std::string _OutputFolder){

            pathdti_tract_stat=_pathdti_tract_stat;
            AtlasDirectory=_AtlasDirectory;
            Input_fiber_file=_Input_fiber_file;
            Output_fiber_file=_Output_fiber_file;
            plane=_plane;
            parameter=_parameter;
            bandWidth=_bandWidth;
            CoG=_CoG;
            sampling=_sampling;
            rodent=_rodent;
            clean=_clean;
            noNan=_noNan;
            useBandWidth=_useBandWidth;
            Parametrized=_Parametrized;

            isFinalOutput=_isFinalOutput;
            name_of_fiber=_name_of_fiber;
            col=_col;
            row=_row;
            DataCol=_DataCol;
            NameCol=_NameCol;
            OutputFolder=_OutputFolder;
    }
    std::string pathdti_tract_stat;
    std::string AtlasDirectory;
    std::string Input_fiber_file;
    std::string Output_fiber_file;
    std::string plane;
    std::string parameter;
    double bandWidth;
    bool CoG;
    double sampling;
    bool rodent;
    bool clean;
    bool noNan;
    bool useBandWidth;
    bool Parametrized;

    /*apply dti params  */
    bool isFinalOutput;
    std::string name_of_fiber;
    int col;
    int row;
    int DataCol;
    int NameCol;
    std::string OutputFolder;

    void showParams();
};

class Worker: public QObject{
  Q_OBJECT
  public:
    Worker(QObject* _parent,CTSParams* p,int _index,CSVClass* _CSV){
      params=p;
      index=_index;
      parent=_parent;
      CSV=_CSV;
    };
    ~Worker(){};
  public slots:
    void process();
    void done();
  signals:
    void finished();
    void error(QString err);
  private:
    CTSParams* params;
    QObject* parent;
    int index;
    CSVClass* CSV;
};

class DTITractWorker : public QObject{
  Q_OBJECT
  public:
    DTITractWorker(std::queue<CTSParams>,int,CSVClass* _CSV);
    ~DTITractWorker(){};
    void run();
  public slots:
    void childFinished();
  private:
    std::queue<CTSParams> paramsQueue;
    int numThreads;
    int currentThreads;
    CSVClass* CSV;
};



/* Compute dti_tract_stat : take the parameters and call the software for every fiber selected with its plane */

bool Applydti_tract_stat_mt(CSVClass* CSV, std::string pathdti_tract_stat, std::string AtlasDirectory,
             std::string OutputFolder, vstring fibers, vstring fibersplane, std::string parameters, double bandWidth,
             int DataCol, int NameCol, bool nogui, bool CoG, double sampling , bool rodent , 
             bool removeCleanFibers , bool removeNanFibers, bool useBandWidth, bool isFiberPostProcessed,
             int numThread=1,QWidget *parent=NULL);


/* Call dti_tract_stat */
int Calldti_tract_stat(std::string pathdti_tract_stat, std::string AtlasDirectory, std::string Input_fiber_file, 
      std::string Output_fiber_file, std::string plane, std::string parameter, double bandWidth,
			bool CoG, double sampling , bool rodent , bool clean , bool noNan, bool useBandWidth, bool Parametrized=true);

/* Gather the fiber profile in different output file */
std::vector<std::vector<v2string> > GatheringFiberProfile(CSVClass* CSV, std::string OutputFolder, int DataCol, int NameCol, bool transposeColRow, vstring fibers, bool& success);

/* Read the fiber profile information from .fvp file */
bool ReadProfileInformation(std::string globalFile, v2string& finaldata, vstring parameters);

bool ReadFiberPtInformation(std::string globalFile, v2string& finaldata, vstring parameters);

/* Write Profile information in an csv file */
void WriteProfile(CSVClass* CSV, std::string filename,std::vector< v2string > FiberProfiles,int DataCol,int NameCol,int ParamCol, bool transposeColRow);

void SaveData(std::string filename,std::string CSVFilename, int DataCol, int DefCol, bool FieldType, int NameCol, std::string OutputFolder);

void SaveAnalysis(std::string filename, std::string AtlasFiberFolder, vstring FiberSelectedname, vstring RelevantPlane, std::string parameters, bool transposeColRow);

/* Read the parameters from a file */
bool ReadParametersFromFiles(std::string datafile, std::string analysisfile, std::string &CSVfilename, std::string &AtlasFiberDir, std::string &OutputFolder, std::string &parameters, int &DataCol, int &DefCol, bool &FieldType, int &NameCol, vstring &SelectedFibers, vstring &SelectedPlanes, bool &transposeColRow, bool &CoG);

/* Calcul the number of profiles parameters : FA,MD ... */
int CalculNumberOfProfileParam(std::string parameters);

/* Finds path on system to a program, looking first in the current executable directory as well as in "current executable/../ExternalBin" and then in the system PATH*/
void FindExecutable( const char* name , std::string pathToCurrentExecutable , std::string &pathToExecutable , bool interactive = true  ) ;

/* look for a plane associated to fiber */
std::vector<int> PlaneAssociatedToFiber(std::string fibername, vstring fibersplane);

void convert(QVector <std::string> line, QVector < QVector <double> >& data);

void LineInVector(std::string line, QVector < QVector <double> >& data);

void LineInVector(std::string line, std::vector <std::string>& v_string);

std::vector <std::string> getparameterslines(std::ifstream& fvpfile);

int getnb_of_samples(std::vector <std::string> parameterslines);

QVector < QVector <double> > getdata(std::ifstream& fvpfile, int nb_of_samples);

v2string getdata2(std::ifstream& fvpfile, int nb_of_samples);

QVector <double> GetColumn(int column, QVector < QVector <double> > data);

vstring GetColumn(int column, v2string data);

void ReadDataFilesNameInCaseDirectory(vstring &datafiles, std::string CaseDir);

double getMean(QVector< double > data);

long double getStd(QVector <double> data, double mean);

QVector <QVector <double> > getStdData(QVector <QVector <double> > data, int factor);

QVector<QVector<double> > getCrossMeanData(qv3double data);

QVector<QVector<double> > getCrossStdData(qv3double data, QVector<QVector<double> > crossmean, int factor);

QVector<double> getCorr(QVector<QVector<double> > data1, QVector<QVector<double> > data2);

vstring getparameterslines(std::ifstream& fvpfile);

std::string getParamFromFile(std::string filepath);
std::string getParamFromDirectory(std::string directory, std::string fibername);

bool IsFile(std::string);

int CallMergeStatWithFiber(std::string, std::string, std::string, std::string, std::string, std::string , std::string ) ;

bool IsFloat(std::string);

#endif
