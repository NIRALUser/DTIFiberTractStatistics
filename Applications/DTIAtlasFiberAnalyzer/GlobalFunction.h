#ifndef _GLOBALFUNCTION_H_
#define _GLOBALFUNCTION_H_

#include <QWidget>
#include <QVector>
/* Specific librairies */
#include "CSVClass.h"

typedef QVector<QVector<double> > qv2double;
typedef QVector<qv2double> qv3double;

/* Functions defined in, exported from Globalfunc.cxx */
/* Get the XML file containing paths to each executable used in the program
void setExecutablesConfigFile() ;*/
/* Call when there is nogui */
bool CommandLine( std::string pathToExecutable , std::string CSVFilename, std::string datafile, std::string analysisfile, bool debug , double sampling , bool rodent , bool removeCleanFibers, bool removeNanFibers );

/* Create a Directory */
bool CreateDirectoryForData(std::string outputfolder, std::string name);

/* Compute fiber process : take the parameters and call the software for every fiber/data */
bool Applyfiberprocess(CSVClass* CSV, std::string pathFiberProcess, std::string AtlasFiberDir,
               std::string OutputFolder, int DataCol, int DefCol, bool FieldType, int NameCol, vstring fibers,
           bool nogui, QWidget* parent=NULL);

/* Compute FiberPostProcess : take the parameters and call the software for every fiber/data */
bool ApplyFiberPostProcess(CSVClass* CSV, std::string pathFiberProcess, std::string AtlasFiberDir,
               std::string OutputFolder, int DataCol, int DefCol, bool FieldType, int NameCol, vstring fibers,
           bool nogui, QWidget* parent=NULL);
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
             std::string Data, std::string DeformationField, bool FieldType, std::string Fiber);

/* Read the files in the folder and keep the one with the extension ".vtk" */
void ReadFiberNameInAtlasDirectory(vstring &fibers, vstring &fibersplane, std::string AtlasFiberDir);

/* Take off the extension of the filename */
std::string takeoffExtension(std::string filename);

/* Take off the path of the filename */
std::string takeoffPath(std::string filename);

/* Keep just the extension of the filename */
std::string ExtensionofFile(std::string filename);

/* Compute dti_tract_stat : take the parameters and call the software for every fiber selected with its plane */
bool Applydti_tract_stat(CSVClass* CSV, std::string pathdti_tract_stat, std::string AtlasDirectory,
             std::string OutputFolder, vstring fibers, vstring fibersplane, std::string parameters,
             int DataCol, int NameCol, bool nogui, bool CoG, double sampling , bool rodent , bool removeCleanFibers , QWidget *parent=NULL);

/* Call dti_tract_stat */
int Calldti_tract_stat(std::string pathdti_tract_stat, std::string AtlasDirectory, std::string Input_fiber_file, std::string Output_fiber_file, std::string plane, std::string parameter, bool CoG, double sampling , bool rodent , bool clean , bool Parametrized=true);

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
void FindExecutable( const char* name , std::string pathToCurrentExecutable , std::string &pathToExecutable ) ;

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
