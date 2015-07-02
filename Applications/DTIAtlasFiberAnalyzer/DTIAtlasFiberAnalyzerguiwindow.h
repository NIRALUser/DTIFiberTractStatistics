#ifndef _DTIATLASFIBERANALYZERGUIWINDOW_H_
#define _DTIATLASFIBERANALYZERGUIWINDOW_H_

#include<algorithm>

//QT librairies
#include <QFileDialog>
#include <QScrollArea>
#include <QScrollBar>
#include <QMessageBox>
#include <QInputDialog>
#include <QListWidgetItem>
#include <QBrush>
#include <QTableWidgetItem>
#include <QString>
#include <QList>
#include <QSignalMapper>
#include <QString>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoListIterator>
#include <QProcessEnvironment>
#include <QMap>
#include <QtGlobal>

/* itksys */
#include <itksys/Process.h>
#include <itksys/SystemTools.hxx>
#include <itksys/Directory.hxx>

//Specific librairies
#include "ui_DTIAtlasFiberAnalyzerguiwindow.h"
#include "CSVClass.h"
#include "GlobalFunction.h"
#include "DTIPlotWindow.h"
#include "XmlReader.h"
#include "XmlWriter.h"

class PlotWindow;

class DTIAtlasFiberAnalyzerguiwindow : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT
public:
    DTIAtlasFiberAnalyzerguiwindow( std::string pathToCurrentExecutable , bool debug=false, QWidget * parent = 0, Qt::WFlags f = 0 );
    vstring getCases(){return m_Cases;}
    vstring getFibers(){return m_Fibers;}
    vstring getFiberInformations(std::string fiber, std::string parameter);
    std::string getOutputFolder(){return m_OutputFolder;}


private slots:
    void NextStep();
    void PreviousStep();
    void BrowserCSVFilename();
    void applyNewCSV();
    void BrowserSaveCSV();
    void AddC();
    void DeleteC();
    void AddR();
    void DeleteR();
    void SetDataColumn();
    void SetDeformationColumn();
    void SetNameColumn();
    void SetCurrentRow();
    void SetCurrentColumn();
    void ChangeHeader(int col);
    void CellAsBrowser(int row,int col);
    void BrowserIndividualData(int row);
    void BrowserDeformationField(int row);
    void BrowserAtlasFiberFolder();
    void EnteronAtlasFiberFolder();
    void AddFiberInList();
    void SelectAllFiber();
    void RemoveFiberInList();
    void RemoveAllFiber();
    void BrowserOutputFolder();
    void EnterOutputFolder();
    void AutoCompute();
    bool Computefiberprocess();
    bool ComputeFiberPostProcess();
    bool Computedti_tract_stat();
    bool OpenPlotWindow();
    void OpenDataFile();
    void OpenAnalysisFile();
    void SaveDataAction(std::string="");
    void SaveAnalysisAction(std::string="");
    void ClearDataInformation();
    void CheckNextStep();
    void EnterCsvFileName();
    void ApplySlot();
    void TabChanged(int);
    void BrowserDTIPCsvFilename();
    void BrowserDTIPVtkFilename();
    void BrowserDTIPOutputFilename();
    bool ComputeDTIParametrization();
    void BrowseSoft(int); //in the soft dialog window
    void ResetSoft(int);
    void ConfigDefault();
    void OpenConfigFile( QString filename ) ;
    void SelectConfigFile() ;
    void SaveConfigFile() ;


protected:
    QWidget* stepwidget(int numberstep);
    void FillCSVFileOnQTable();
    std::string HeaderLine();
    std::string LineItemsToString(int row);
    void PutDataonQTable(int row, int column, std::string word);
    void AddColumnWithHearderName(std::string header);
    void setParam();
    void clearCSV();
    void FillFiberFrame();
    void ColorCol(int type);
    void DecolorCol(int type);
    void paintForeGround(int R,int G,int B,int col,int type);
    void SaveCSV();
    void AutoSaveCSV();
    void AddDataFromTableToCSV();
    void checkBoxProfile(std::string parameters);
    void setParamFromFile(std::string filepath);
    void setParamFromDirectory(std::string directory,std::string fibername);
    void setCases();
    void setFibers();
    //relevant to former tab 4, obsolete now
    //void FillDataFilesList();
    void ReadDataFilesNameInDirectory(vstring &datafiles, std::string Dir);
    QVector <QVector <double> > getdatatable(std::string filepath);
    QVector <QVector <double> > getfiberpoints(std::string filepath);
    void CompleteWithMeanData();
    void CompleteWithCrossStdData();
    QVector< QVector<double> > TableConversion(v2string table);
    void LoadDataFile(std::string filename);
    void LoadAnalysisFile(std::string filename);
    void FillSelectedPlane();
    bool GetAutoPlaneOption();
    void SetNewDialogDirFromFileName( QString filename ) ;
    void GUIFindExecutable( std::string soft , QLineEdit* toolQtLine ) ;

private:
    /* debug */
    bool m_debug;

    /* CSVClass item */
    CSVClass* m_CSV;
    bool m_CSVcreated;

    /* Input */
    vstring m_Fibername;
    vstring m_Fibersplane;
    vstring m_FiberSelectedname;
    vstring m_RelevantPlane;
    vstring m_SelectedPlane;
    vstring m_Fibers;
    vstring m_Cases;
    std::vector<std::vector<v2string> > m_FiberProfile;
    std::string m_csvfilename;
    std::string m_OutputFolder;
    std::string m_ParameterFile;
    std::string m_AtlasFiberDir;
    std::string m_parameters;
    int m_DataCol;
    int m_DeformationCol;
    int m_NameCol;
    int m_NumberOfParameters;

    /* step */
    int m_numberstep;
    int m_laststep;
    bool m_nextStep;

    /* current Row/Column */
    int m_currentRow;
    int m_currentColumn;
    int m_transposeColRow; //just for the gathering spreadsheet

    PlotWindow* m_plotwindow;
    QVector<qv3double> m_casedata;
    QVector<qv3double> m_atlasdata;
    QVector<qv3double> m_statdata;
    vstring m_parameterslines;
    bool m_PlotError;
    std::string m_PathToCurrentExecutable ;
    QString m_DialogDir ;

};

#endif
