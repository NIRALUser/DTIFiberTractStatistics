#ifndef _DTIATLASFIBERANALYZERGUIWINDOW_H_
#define _DTIATLASFIBERANALYZERGUIWINDOW_H_


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

/* itksys */
#include <itksys/Process.h>
#include <itksys/SystemTools.hxx>
#include <itksys/Directory.hxx>

//Specific librairies
#include "ui_DTIAtlasFiberAnalyzerguiwindow.h"
#include "CSVClass.h"
#include "DTIPlotWindow.h"

class PlotWindow;
typedef std::vector<std::string> vstring;
typedef QVector<QVector<QVector<double> > > qv3double;

class DTIAtlasFiberAnalyzerguiwindow : public QMainWindow, public Ui::MainWindow 
{
	Q_OBJECT
	public:
		DTIAtlasFiberAnalyzerguiwindow(bool debug=false, QWidget * parent = 0, Qt::WFlags f = 0 );
		vstring getCases(){return m_Cases;}
		vstring getFibers(){return m_Fibers;}
		vstring getFiberInformations(std::string fiber);
		
		
	private slots:
		void NextStep();
		void PreviousStep();
		void BrowserCSVFilename();
		void applyNewCSV();
		void BrotherSaveCSV();
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
		bool Computedti_tract_stat();
		bool OpenPlotWindow();
		void OpenDataFile();
		void OpenAnalysisFile();
		void SaveDataAction();
		void SaveAnalysisAction();
		void ClearDataInformation();
		void CheckNextStep();
		void EnterCsvFileName();
		void ApplySlot();
		void TabChanged(int);
		
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
		void setCases();
		void setFibers();
		void FillDataFilesList();
		void ReadDataFilesNameInDirectory(vstring &datafiles, std::string Dir);
		QVector <QVector <double> > getdatatable(std::string filepath);
		QVector <QVector <double> > getfiberpoints(std::string filepath);
		void CompleteWithMeanData();
		void CompleteWithCrossStdData();
		QVector< QVector<double> > TableConversion(v2string table);
		void LoadAnalysisFile(std::string filename);
		

		
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
		vstring m_FibersplaneSelected;
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
};

#endif
