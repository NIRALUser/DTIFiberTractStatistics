#include "DTIAtlasFiberAnalyzerguiwindow.h"
#include "GlobalFunction.h"
#include <sstream>

/********************************************************************************* 
 * Constructor
 ********************************************************************************/
DTIAtlasFiberAnalyzerguiwindow::DTIAtlasFiberAnalyzerguiwindow(bool debug, QWidget * parent , Qt::WFlags f  ): QMainWindow(parent, f)
{
	setupUi(this);
	
	/* Parameters */
	m_debug = debug;
	m_numberstep = 0;
	m_CSV = NULL;
	m_DataCol = -1;
	m_DeformationCol = -1;
	m_NameCol = -1;
	m_CSVcreated = false;
	m_nextStep = false;
	m_currentRow = -1;
	m_currentColumn = -1;
	m_transposeColRow = false;
	
	this->setMouseTracking(true);
	
	/* set the main layout */
	/* ScrollArea */
	QScrollArea *const scroll(new QScrollArea);
	centralwidget->setLayout(PrincipalLayout);
	scroll->setWidget(centralwidget);
	setCentralWidget(scroll);
	
	//Design
	CsvTitle->setFont(QFont("Courrier", 20,4));
	Fiberprocesstitle->setFont(QFont("Courrier", 20,4));
	Propertiestitle->setFont(QFont("Courrier", 20,4));
	
	//Enable every tab except the first one
	for(int i=1; i<3; i++)
		PrincipalWidget->setTabEnabled (m_numberstep + i,false);
	
	/* Signaux and slot */
	/* Tab 1 */
	// next step / last step 
	connect(this->nextstep, SIGNAL(clicked()), this, SLOT(NextStep()));
	connect(this->previousstep, SIGNAL(clicked()), this, SLOT(PreviousStep()));
	//load csv
	connect(this->browsercsvfilename, SIGNAL(clicked()), this, SLOT(BrowserCSVFilename()));
	connect(this->csvfilename, SIGNAL(editingFinished()), this, SLOT(EnteronCSVFilename()));
	//new csv
	connect(this->apply, SIGNAL(clicked()), this, SLOT(applyNewCSV()));
	//Delete/Save CSV
	connect(this->deletecsv, SIGNAL(clicked()), this, SLOT(DeleteCSV()));
	connect(this->savecsv, SIGNAL(clicked()), this, SLOT(BrotherSaveCSV()));
	//Column
	connect(this->addcolumn, SIGNAL(clicked()), this, SLOT(AddC()));
	connect(this->CSVtable, SIGNAL(currentItemChanged(QTableWidgetItem *, QTableWidgetItem *)), 
		this, SLOT(SetCurrentColumn()));
	connect(this->deletecolumn, SIGNAL(clicked()), this, SLOT(DeleteC()));
	//Row
	connect(this->addrow, SIGNAL(clicked()), this, SLOT(AddR()));
	connect(this->CSVtable, SIGNAL(currentItemChanged(QTableWidgetItem *, QTableWidgetItem *)), 
		this, SLOT(SetCurrentRow()));
	connect(this->deleterow, SIGNAL(clicked()), this, SLOT(DeleteR()));
	//Column for data/deformation
	connect(this->ColumnData, SIGNAL(editingFinished()), this, SLOT(SetDataColumn()));
	connect(this->ColumnDeformation, SIGNAL(editingFinished()), this, SLOT(SetDeformationColumn()));
	connect(this->ColumnNameCases, SIGNAL(editingFinished()), this, SLOT(SetNameColumn()));
	connect(this->CSVtable, SIGNAL(cellDoubleClicked( int, int)),this, SLOT(CellAsBrowser(int,int)));
	connect(this->CSVtable->horizontalHeader(), SIGNAL(sectionDoubleClicked(int)), this,
		SLOT(ChangeHeader(int)));
	connect(this->browserOutputFolder, SIGNAL(clicked()), this, SLOT(BrowserOutputFolder()));
	connect(this->outputfolder, SIGNAL(editingFinished()), this, SLOT(EnterOutputFolder()));

	
	/* Tab 2 */
	connect(this->browserAtlasFiberFolder, SIGNAL(clicked()), this, SLOT(BrowserAtlasFiberFolder()));
	connect(this->AtlasFiberFolder, SIGNAL(editingFinished()), this, SLOT(EnteronAtlasFiberFolder()));
	connect(this->fiberprocessapply, SIGNAL(clicked()), this, SLOT(Computefiberprocess()));
	connect(this->AddFiber, SIGNAL(clicked()), this, SLOT(AddFiberInList()));
	connect(this->AddAllFiber, SIGNAL(clicked()), this, SLOT(SelectAllFiber()));
	connect(this->RemoveFiber, SIGNAL(clicked()), this, SLOT(RemoveFiberInList()));
	connect(this->Removeallfiber, SIGNAL(clicked()), this, SLOT(RemoveAllFiber()));
	
	/* Tab 3 */
	connect(this->ApplyDTI_tract_stat, SIGNAL(clicked()), this, SLOT(Computedti_tract_stat()));
	
	//"File" action
	connect(this->actionSave_parameters, SIGNAL(triggered()), SLOT(saveparamaction()));
	connect(this->actionOpen_parameters, SIGNAL(triggered()), SLOT(openparam()));
}

/* Slots */
/********************************************************************************* 
 * When the user click on next step, enable next tab, disenable previous tab
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::NextStep()
{
	//If the user click on an other tab without using previous step, he is still able to clik on next
	if(PrincipalWidget->currentIndex()!=m_numberstep)
	{
		m_nextStep = true;
		m_numberstep = m_numberstep -1;
	}
	//Go to next step if we are able
	if(m_nextStep)
	{
		if(outputfolder->text().compare("")!=0)
		{
			if(m_numberstep==0 && csvfilename->text().compare("")==0)
			{
				int number=0;
				bool nofile = false;
				std::stringstream SNumber;
				SNumber << number;
				m_csvfilename =  m_OutputFolder + "/Database_" + SNumber.str() + ".csv";
				//check if there is already a file with the default name
				while(!nofile)
				{
					if(!FileExisted(m_csvfilename))
						nofile = true;
					else
					{
						std::stringstream SNumber2;
						number = number + 1;
						SNumber2 << number;
						m_csvfilename =  m_OutputFolder + "/Database_" + 
								SNumber2.str() + ".csv";
					}
				}
				m_CSV->SetFilename(m_csvfilename);
				SaveCSV();
			}
			if(m_numberstep < 3)
			{
				/* Enable the next tab */
				PrincipalWidget->setTabEnabled (m_numberstep + 1,true);
				/* move to the next tab */
				PrincipalWidget->setCurrentWidget (stepwidget(m_numberstep + 1));
				/* Increment numberstep */
				m_numberstep = m_numberstep + 1;
				m_nextStep = false;
				previousstep->setEnabled (true);
			}
		}
		else 
			QMessageBox::information(this, "Warning", "No Output folder Specified!");
	}
	else if(m_numberstep==0)
		QMessageBox::information(this, "Warning", "Load or create a CSV File, first!");
	else if(m_numberstep==1)
		QMessageBox::information(this, "Warning", "Compute to attribute fibers, first!");
	else if(m_numberstep==2)
		QMessageBox::information(this, "Warning", "Compute to calculate properties, first!");
}


/********************************************************************************* 
 * When the user click on last step, enable last tab, disenable next tab
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::PreviousStep()
{
	if(m_numberstep > 0)
	{
		int Previousstep = m_numberstep - 1;
		/* Enable the previous tab */
		PrincipalWidget->setTabEnabled (Previousstep,true);
		/* move to the previous tab */
		PrincipalWidget->setCurrentWidget (stepwidget(Previousstep));
		/* Disenable the former tab */
		PrincipalWidget->setTabEnabled (m_numberstep,false);
		/* decrement numberstep */
		m_numberstep = Previousstep;
		m_nextStep = true;
		nextstep->setEnabled (true);
		if(m_numberstep==0)
			previousstep->setEnabled (false);
	}
}


/********************************************************************************* 
 * Widget associates to numberstep
 ********************************************************************************/
QWidget* DTIAtlasFiberAnalyzerguiwindow::stepwidget(int numberstep)
{
	switch(numberstep)
	{
		case 0:
			return Step1;
		case 1:
			return Step2;
		case 2:
			return Step3;
	}
	return NULL;
}


/* CSV Load */
/********************************************************************************* 
 * Open a dialog to search the name of the vector's path and write it in the GUI
 * Widget
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::BrowserCSVFilename()
{
	/* Mouse Event when running */
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	
	//Clear the number of column
	m_DataCol = -1;
	m_DeformationCol = -1;
	m_NameCol = -1;
	ColumnData->clear();
	ColumnDeformation->clear();
	ColumnNameCases->clear();
	
	QString filename,type;
	filename = QFileDialog::getOpenFileName(this, "Open CSV File", "/", 
			"Text (*.csv *.txt)",&type);
	
	if(m_debug)
		std::cout<<"Filename : "<< (filename.toStdString()).c_str() <<std::endl;
	
	/* Keep the value if cancel */
	if(filename !=NULL)
	{
		std::cout<<std::endl;
		std::cout<<"---- New Data ----"<<std::endl;
		
		/* Clear CSV */
		if(m_CSV !=NULL)
		{
			clearCSV();
		//clear the lineEdit
			csvfilename->clear();
		}
		
		csvfilename->setText(filename);
		m_csvfilename = filename.toStdString();
		/* load the csv */
		if(!m_CSVcreated)
		{
			m_CSV = new CSVClass(m_csvfilename,m_debug);
			m_CSVcreated = true;
		}
		if(m_CSV->loadCSVFile(m_csvfilename))
		{
			FillCSVFileOnQTable();
			/* Enable the frame to add column and row */
			addCRframe->setEnabled (true);
			saveframe->setEnabled (true);
			ColumnFrame->setEnabled (true);
			m_nextStep = true;
		}
	}
	else if(m_debug)
		std::cout<<"Error : filename is NULL."<<std::endl;
	
	/* Restore the mouse */
	QApplication::restoreOverrideCursor();
}
/********************************************************************************* 
 * When press enter in the QLineEdit in the load frame, load the csv file
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::EnteronCSVFilename()
{
	/* Mouse Event when running */
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	
	/* Clear the CSV */
	if(m_CSV !=NULL)
		clearCSV();
	
	//Clear the number of column
	m_DataCol = -1;
	m_DeformationCol = -1;
	m_NameCol = -1;
	ColumnData->clear();
	ColumnDeformation->clear();
	ColumnNameCases->clear();
	
	if((csvfilename->text().toStdString())!="")
	{
		std::cout<<std::endl;
		std::cout<<"---- New Data ----"<<std::endl;
		
		m_csvfilename = csvfilename->text().toStdString();
		if(!m_CSVcreated)
		{
			m_CSV = new CSVClass(m_csvfilename,m_debug);
			m_CSVcreated = true;
		}
		if(m_CSV->loadCSVFile(m_csvfilename))
		{
			FillCSVFileOnQTable();
			/* Enable the frame to add column and row */
			addCRframe->setEnabled (true);
			saveframe->setEnabled (true);
			ColumnFrame->setEnabled (true);
			m_nextStep = true;
		}
	}
	
	/* Restore the mouse */
	QApplication::restoreOverrideCursor();
}
void DTIAtlasFiberAnalyzerguiwindow::FillCSVFileOnQTable()
{
	// Create the table with the right number of columns and rows
	CSVtable->setRowCount (m_CSV->getRowSize()-1); //minus 1 for the header
	CSVtable->setColumnCount (m_CSV->getColSize(0));
	//Write the name of the header/first line of the csv
	for(unsigned int j = 0; j<m_CSV->getColSize(0);j++)
	{
		//create an item for the QTable
		QTableWidgetItem* header = new QTableWidgetItem;
		QString qs ( ((*m_CSV->getData())[0][j]).c_str() ); //convert the data(string) in QString
		header->setData( 0, qs );
		CSVtable->setHorizontalHeaderItem(j,header);
		//Color the header
		if(j==static_cast<unsigned int>(m_DataCol))
			ColorCol(1);
		if(j==static_cast<unsigned int>(m_DeformationCol))
			ColorCol(2);
		if(j==static_cast<unsigned int>(m_NameCol))
			ColorCol(3);
	}
	
	//fill the tab
	for(unsigned int i = 1; i<m_CSV->getRowSize();i++)
	{
		for(unsigned int j = 0; j<m_CSV->getColSize(i);j++)
		{
			//create an item for the QTable
			QTableWidgetItem* item = new QTableWidgetItem;
			QString qs ( ((*m_CSV->getData())[i][j]).c_str() ); //convert the data(string) in QString
			item->setData( 0, qs );
			CSVtable->setItem (i-1,j,item);
		}
		
	}
}


/* CSV NEW */
/********************************************************************************* 
 * Apply in CSV frame : create a new csv
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::applyNewCSV()
{
	//Clear the number of column
	m_DataCol = -1;
	m_DeformationCol = -1;
	m_NameCol = -1;
	ColumnData->clear();
	ColumnDeformation->clear();
	ColumnNameCases->clear();
	/* Clear the CSV */
	if(m_CSV !=NULL)
		clearCSV();
	//clear the lineEdit
	csvfilename->clear();
	
	std::cout<<std::endl;
	std::cout<<"---- New Data ----"<<std::endl;
	//add the number of columns
	CSVtable->setColumnCount (numbercolumn->value());
	//add the number of rows
	CSVtable->setRowCount (numberdata->value());
	
	/* Create the CSV class object */
	if(!m_CSVcreated)
	{
		m_CSV = new CSVClass("",m_debug);
		m_CSVcreated = true;
	}
	
	/* Enable the frame to add column and row */
	addCRframe->setEnabled (true);
	saveframe->setEnabled (true);
	ColumnFrame->setEnabled (true);
	m_nextStep = true;
	
	//Fill the Table with empty items
	for(int i=0;i<CSVtable->rowCount();i++)
	{
		for(int j=0;j<CSVtable->columnCount();j++)
		{
			QTableWidgetItem* item = new QTableWidgetItem;
			QString qs (""); //convert the word(string) in QString
			item->setData( 0, qs);
			CSVtable->setItem (i,j,item);
		}
	}
}


/* Action on data and QTable */
/********************************************************************************* 
 * Transform the Hearder's names on a std::string
 ********************************************************************************/
std::string DTIAtlasFiberAnalyzerguiwindow::HeaderLine()
{
	QString qs;
	std::string line;
	/* Copy each item in the std::string separate by a coma */
	for(int i=0;i<CSVtable->columnCount();i++)
	{
		if(i!= CSVtable->columnCount() -1)
		{
			if(CSVtable->horizontalHeaderItem(i)!= 0)
			{
				qs = (CSVtable->horizontalHeaderItem(i))->text();
				line+= qs.toStdString();
			}
			line+= ",";
		}
		else
		{
			if(CSVtable->horizontalHeaderItem(i)!= 0)
			{
				qs = (CSVtable->horizontalHeaderItem(i))->text();
				line+= qs.toStdString();
			}
		}
	}
	return line;
}
/********************************************************************************* 
 * Transform the QTable item from a row on a std::string
 ********************************************************************************/
std::string DTIAtlasFiberAnalyzerguiwindow::LineItemsToString(int row)
{
	QString qs;
	std::string line;
	/* Copy each item in the std::string separate by a coma */
	for(int i=0;i<CSVtable->columnCount();i++)
	{
		if(i!= CSVtable->columnCount() -1)
		{
			if(CSVtable->item(row,i)!= 0)
			{
				qs = (CSVtable->item(row,i))->text();
				line+= qs.toStdString();
			}
			else
				line+= "no";
			// Put the coma
			line+= ",";
		}
		else
		{
			if(CSVtable->item(row,i)!= 0)
			{
				qs = (CSVtable->item(row,i))->text();
				line+= qs.toStdString();
			}
			else
				line+= "no";
		}
	}
	return line;
}
/********************************************************************************* 
 * Put the word in the cell (row,column)
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::PutDataonQTable(int row, int column, std::string word)
{
	//create an item for the QTable
	QTableWidgetItem* item = new QTableWidgetItem;
	QString qs ( word.c_str() ); //convert the word(string) in QString
	item->setData( 0, qs );
	CSVtable->setItem (row,column,item);
}


/********************************************************************************* 
 * Delete / Save a CSV file
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::DeleteCSV()
{
	//Delete the CSVClass
	m_CSV = NULL;
	m_CSVcreated = false;
	//clear table
	CSVtable->clear ();
	CSVtable->setColumnCount (1);
	CSVtable->setRowCount (1);
	//clear the lineEdit
	csvfilename->clear();
	/* Enable the frame to add column and row */
	addCRframe->setEnabled (false);
	saveframe->setEnabled (false);
	ColumnFrame->setEnabled (false);
	//Unable next step
	m_nextStep = false;
	nextstep->setEnabled (true);
	previousstep->setEnabled (false);
	//Reinitialize the current cell to nothing
	m_currentRow = -1;
	m_currentColumn = -1;
	m_numberstep = 0;
	//Enable every tab except the first one
	for(int i=1; i<3; i++)
		PrincipalWidget->setTabEnabled (m_numberstep + i,false);
	//Clear the vector of fiber
	AtlasFiberFolder->clear();
	FiberInAtlasList->clear();
	FiberSelectedList->clear();
	FiberSelected2List->clear();
	FiberPlaneFile->clear();
	//Clear the number of column
	m_DataCol = -1;
	m_DeformationCol = -1;
	m_NameCol = -1;
	ColumnData->clear();
	ColumnDeformation->clear();
	ColumnNameCases->clear();
	//default value
	ArcOnCase->setChecked(true);
	FABox->setCheckState(Qt::Checked);
	MDBox->setCheckState(Qt::Checked);
	FroBox->setCheckState(Qt::Checked);
	l1Box->setCheckState(Qt::Checked);
	l2Box->setCheckState(Qt::Checked);
	l3Box->setCheckState(Qt::Checked);
	RDBox->setCheckState(Qt::Checked);
	GABox->setCheckState(Qt::Checked);
	
}
/* type = anything, save with filename as the name, type = 2, save with an extension of the filename */
void DTIAtlasFiberAnalyzerguiwindow::SaveCSV()
{
	AddDataFromTableToCSV();
	m_CSV->SaveFile(m_csvfilename);
}
void DTIAtlasFiberAnalyzerguiwindow::AutoSaveCSV()
{
	std::string filename;
	filename = takeoffPath(m_csvfilename);
	filename = takeoffExtension(filename);
	filename = m_OutputFolder+ "/" + filename + "_computed.csv";
	AddDataFromTableToCSV();
	m_CSV->SaveFile(filename);
}
void DTIAtlasFiberAnalyzerguiwindow::AddDataFromTableToCSV()
{
	//clear the vector in CSVClass
	m_CSV->clearData();
	//add Header's names
	m_CSV->LineInVector(HeaderLine());
	for(int i=0;i<CSVtable->rowCount();i++)
	{
		//add every line to the vector
		m_CSV->LineInVector(LineItemsToString(i));
	}
}
void DTIAtlasFiberAnalyzerguiwindow::BrotherSaveCSV()
{
	QString file = QFileDialog::getSaveFileName(this, "Save a file", QString(), "Text (*.csv)");
	
	if(file!=NULL)
	{
		m_csvfilename = file.toStdString();
		m_CSV->SetFilename(m_csvfilename);
		SaveCSV();
	}
	else
		std::cout<<"ERROR: File name null!"<<std::endl;
}
void DTIAtlasFiberAnalyzerguiwindow::clearCSV()
{
	/* Clear Data in the CSV object */
	m_CSV->clearData();
	//clear table
	CSVtable->clear();
	CSVtable->setColumnCount (1);
	CSVtable->setRowCount (1);
}

/* Row/Column */
/********************************************************************************* 
 * Add collumn
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::AddC()
{
	int columns = CSVtable->columnCount ();
	CSVtable->setColumnCount (columns + 1);
	//add the header
	QTableWidgetItem *header=  new QTableWidgetItem;
	header->setData( 0, headername->text() );
	CSVtable->setHorizontalHeaderItem(columns,header);
	//Clear the Line edit
	headername->clear();
}
void DTIAtlasFiberAnalyzerguiwindow::AddColumnWithHearderName(std::string header)
{
	int columns = CSVtable->columnCount ();
	CSVtable->setColumnCount (columns + 1);
	//add the header
	QTableWidgetItem *title=  new QTableWidgetItem;
	title->setData( 0, header.c_str() );
	CSVtable->setHorizontalHeaderItem(columns,title);
}
/********************************************************************************* 
 * Delete current column
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::DeleteC()
{
	if(m_currentColumn != -1)
	{
		/* Delete the column */
		CSVtable->removeColumn (m_currentColumn);
		m_currentRow = -1;
		m_currentColumn = -1;
	}
	else
		QMessageBox::information(this, "Warning", "Select a column!");
}

/********************************************************************************* 
 * Add row
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::AddR()
{
	int rows = CSVtable->rowCount ();
	CSVtable->setRowCount (rows + 1);
	//fill the data column with no data and deformation field with no deformation
	if(m_DataCol !=-1)
	{
		QTableWidgetItem *item=  new QTableWidgetItem;
		QString str("no data");
		item->setData( 0, str);
		CSVtable->setItem(CSVtable->rowCount()-1,m_DataCol,item);
	}
	if(m_DeformationCol !=-1)
	{
		QTableWidgetItem *item=  new QTableWidgetItem;
		QString str("no deformation");
		item->setData( 0, str);
		CSVtable->setItem(CSVtable->rowCount()-1,m_DeformationCol,item);
	}
	if(m_NameCol !=-1)
	{
		QTableWidgetItem *item=  new QTableWidgetItem;
		QString str("no name");
		item->setData( 0, str);
		CSVtable->setItem(CSVtable->rowCount()-1,m_NameCol,item);
	}
}
/********************************************************************************* 
 * Delete current row
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::DeleteR()
{
	if(m_currentRow != -1)
	{
		/* Delete the row */
		CSVtable->removeRow (m_currentRow);
		m_currentRow = -1;
		m_currentColumn = -1;
	}
	else
		QMessageBox::information(this, "Warning", "Select a line!");
}

/* Set Data column number */
void DTIAtlasFiberAnalyzerguiwindow::SetDataColumn()
{
	if(ColumnData->text().toInt() - 1<CSVtable->columnCount() && ColumnData->text().toInt()!=0
		  && ColumnData->text().toInt()>0)
	{
		// decolor the last column
		DecolorCol(1);
		m_DataCol = ColumnData->text().toInt() -1;
		//check if it was an other column with this number
		if(m_DataCol==m_DeformationCol)
		{
			m_DeformationCol = -1;
			ColumnDeformation->setText("");
			DecolorCol(2);
		}
		else if(m_DataCol==m_NameCol)
		{
			m_NameCol = -1;
			ColumnNameCases->setText("");
			DecolorCol(3);
		}
		ColorCol(1);
		//Fill it with "no data" if there is no data
		for(int i=0;i<CSVtable->rowCount();i++)
		{
			if((CSVtable->item(i,m_DataCol))->text().compare("")==0 ||
						 (CSVtable->item(i,m_DataCol))->text().compare("no")==0)
			{
				QTableWidgetItem *item=  new QTableWidgetItem;
				QString str("no data");
				item->setData( 0, str);
				CSVtable->setItem(i,m_DataCol,item);
			}
		}
	}
	else if((ColumnData->text()).compare("")==0)
	{
		DecolorCol(1);
		m_DataCol = -1;
	}
	else
		ColumnData->setText("");
}

/* Set Deformation column number */
void DTIAtlasFiberAnalyzerguiwindow::SetDeformationColumn()
{
	if(ColumnDeformation->text().toInt() - 1<CSVtable->columnCount()&& ColumnDeformation->text().toInt()!=0
		  && ColumnDeformation->text().toInt()>0)
	{
		// decolor the last column
		DecolorCol(2);
		m_DeformationCol = ColumnDeformation->text().toInt() -1;
		//check if it was an other column with this number
		if(m_DeformationCol==m_DataCol)
		{
			m_DataCol = -1;
			ColumnData->setText("");
			DecolorCol(1);
		}
		else if(m_DeformationCol==m_NameCol)
		{
			m_NameCol = -1;
			ColumnNameCases->setText("");
			DecolorCol(3);
		}
		ColorCol(2);
		//Fill it with "no deformation" if there is no data
		for(int i=0;i<CSVtable->rowCount();i++)
		{
			if((CSVtable->item(i,m_DeformationCol))->text().compare("")==0 ||
						 (CSVtable->item(i,m_DeformationCol))->text().compare("no")==0)
			{
				QTableWidgetItem *item=  new QTableWidgetItem;
				QString str("no deformation");
				item->setData( 0, str);
				CSVtable->setItem(i,m_DeformationCol,item);
			}
		}
	}
	else if((ColumnDeformation->text()).compare("")==0)
	{
		DecolorCol(2);
		m_DeformationCol = -1;
	}
	else
		ColumnDeformation->setText("");
}

/* Set Name for cases column number */
void DTIAtlasFiberAnalyzerguiwindow::SetNameColumn()
{
	if(ColumnNameCases->text().toInt() - 1 < CSVtable->columnCount() && ColumnNameCases->text().toInt()!=0
		   && ColumnNameCases->text().toInt()>0)
	{
		// decolor the last column
		DecolorCol(3);
		m_NameCol = ColumnNameCases->text().toInt() -1;
		//check if it was an other column with this number
		if(m_NameCol==m_DeformationCol)
		{
			m_DeformationCol = -1;
			ColumnDeformation->setText("");
			DecolorCol(2);
		}
		else if(m_NameCol==m_DataCol)
		{
			m_DataCol = -1;
			ColumnData->setText("");
			DecolorCol(1);
		}
		ColorCol(3);
		//Fill it with "no deformation" if there is no data
		for(int i=0;i<CSVtable->rowCount();i++)
		{
			if((CSVtable->item(i,m_NameCol))->text().compare("")==0 ||
						 (CSVtable->item(i,m_NameCol))->text().compare("no")==0)
			{
				QTableWidgetItem *item=  new QTableWidgetItem;
				QString str("no name");
				item->setData( 0, str);
				CSVtable->setItem(i,m_NameCol,item);
			}
		}
	}
	else if((ColumnNameCases->text()).compare("")==0)
	{
		DecolorCol(3);
		m_NameCol = -1;
	}
	else
		ColumnNameCases->setText("");
}

/* Color the column : 1->Data(green),2->Def(blue),3->Name(orange) */
void DTIAtlasFiberAnalyzerguiwindow::ColorCol(int type)
{
	switch(type)
	{
		case 1:
			if(m_DataCol>=0 && m_DataCol < CSVtable->columnCount())
				paintForeGround(0,147,0,m_DataCol,1);
			break;
		case 2:
			if(m_DeformationCol>=0 && m_DeformationCol < CSVtable->columnCount())
				paintForeGround(85,170,255,m_DeformationCol,2);
			break;
		case 3:
			if(m_NameCol>=0 && m_NameCol < CSVtable->columnCount())
				paintForeGround(255,170,0,m_NameCol,3);
			break;
	}
}

/* Decolor the column : 1->Data(green),2->Def(blue),3->Name(orange) */
void DTIAtlasFiberAnalyzerguiwindow::DecolorCol(int type)
{
	switch(type)
	{
		case 1:
			if(m_DataCol!=-1)
				paintForeGround(0,0,0,m_DataCol,1);
			break;
		case 2:
			if(m_DeformationCol!=-1)
				paintForeGround(0,0,0,m_DeformationCol,2);
			break;
		case 3:
			if(m_NameCol!=-1)
				paintForeGround(0,0,0,m_NameCol,3);
			break;
	}
}

/* Paint the foreground with RGB value */
void DTIAtlasFiberAnalyzerguiwindow::paintForeGround(int R,int G,int B,int col,int type)
{
	QString headername;
	QTableWidgetItem *header=  new QTableWidgetItem;
	if(CSVtable->horizontalHeaderItem(col)!=0)
	{
		headername = CSVtable->horizontalHeaderItem(col)->text();
	}
	else
	{
		switch(type)
		{
			case 1:
				headername = "Data";
				break;
			case 2:
				headername = "Deformation field";
				break;
			case 3:
				headername = "Name";
				break;
		}
	}
	header->setData( 0, headername);
	QBrush brush(QColor(R,G,B,255));
	header->setForeground(brush);
	CSVtable->setHorizontalHeaderItem(col,header);
}


/* Set Current row */
void DTIAtlasFiberAnalyzerguiwindow::SetCurrentRow()
{
	m_currentRow = CSVtable->currentRow();
}

/* Set Current Column */
void DTIAtlasFiberAnalyzerguiwindow::SetCurrentColumn()
{
	m_currentColumn = CSVtable->currentColumn();
}

/* Change the header of a column */
void DTIAtlasFiberAnalyzerguiwindow::ChangeHeader(int col)
{
	bool ok;
	//New header
	QTableWidgetItem *header=  new QTableWidgetItem;
	//ask the name
	QString headername = QInputDialog::getText(this,"header", "name of the column :", QLineEdit::Normal,
					       QString::null, &ok);
	if(ok && !headername.isEmpty())
	{
		header->setData( 0, headername );
		CSVtable->setHorizontalHeaderItem(col,header);
		if(col == m_DataCol)
			ColorCol(1);
		if(col == m_DeformationCol)
			ColorCol(2);
		if(col == m_NameCol)
			ColorCol(3);
	}
}

/* Define the cell in the QTableWidget like a browser */
void DTIAtlasFiberAnalyzerguiwindow::CellAsBrowser(int row,int col)
{
	if(col==m_DataCol)
		BrowserIndividualData(row);
	if(col==m_DeformationCol)
		BrowserDeformationField(row);
}

/* Set Param */
void DTIAtlasFiberAnalyzerguiwindow::setParam()
{
	m_parameters = "";
	m_NumberOfParameters = 0 ;
	//FA
	if(FABox->isChecked())
	{
		m_parameters+="fa,";
		m_NumberOfParameters++;
	}
	//MD
	if(MDBox->isChecked())
	{
		m_parameters+="md,";
		m_NumberOfParameters++;
	}
	//Fro
	if(FroBox->isChecked())
	{
		m_parameters+="fro,";
		m_NumberOfParameters++;
	}
	//lambda 1
	if(l1Box->isChecked())
	{
		m_parameters+="ad,";
		m_NumberOfParameters++;
	}
	//lambda 2
	if(l2Box->isChecked())
	{
		m_parameters+="l2,";
		m_NumberOfParameters++;
	}
	//lambda 3
	if(l3Box->isChecked())
	{
		m_parameters+="l3,";
		m_NumberOfParameters++;
	}
	//RD
	if(RDBox->isChecked())
	{
		m_parameters+="rd,";
		m_NumberOfParameters++;
	}
	//GA
	if(GABox->isChecked())
	{
		m_parameters+="ga,";
		m_NumberOfParameters++;
	}
	
	if(m_parameters.compare("")!=0)
		m_parameters=m_parameters.substr(0,m_parameters.find_last_of(","));
}


/* Load Data/Deformation in the CSV Cell */
/********************************************************************************* 
 * load files for Individual images and put them in the Qtable
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::BrowserIndividualData(int row)
{
	QString filename,type;
	filename = QFileDialog::getOpenFileName(this, "Open Individual Data", "/", 
			"Images (*.gipl *.hdr *.nhdr *.nrrd)",&type);
	
	if(m_debug)
		std::cout<<"Filename : "<< (filename.toStdString()).c_str() <<std::endl;
	
	/* Keep the value if cancel */
	if(filename !=NULL)
	{
		PutDataonQTable(row,m_DataCol,filename.toStdString());
	}
	else if(m_debug)
		std::cout<<"Error : filename is NULL."<<std::endl;
}
/********************************************************************************* 
 * load file for deformation field and put it in the Qtable
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::BrowserDeformationField(int row)
{
	QString filename,type;
	filename = QFileDialog::getOpenFileName(this, "Open Deformation Field", "/");
	
	if(m_debug)
		std::cout<<"Filename : "<< (filename.toStdString()).c_str() <<std::endl;
	
	/* Keep the value if cancel */
	if(filename !=NULL)
	{
		PutDataonQTable(row,m_DeformationCol,filename.toStdString());
	}
	else if(m_debug)
		std::cout<<"Error : filename is NULL."<<std::endl;
}


/* Tab 2 : Fiber Process */
/********************************************************************************* 
 * Find the atlas directory with a browser window
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::BrowserAtlasFiberFolder()
{
	QString path = QFileDialog::getExistingDirectory(this);
	if(path!=NULL)
	{
		//Clear the vectors of Fiber's names and the Lists
		m_Fibername.clear();
		m_FiberSelectedname.clear();
		m_Fibersplane.clear();
		m_FibersplaneSelected.clear();
		//clear the list
		FiberInAtlasList->clear ();
		FiberSelectedList->clear ();
		//Set the path and Fill the list with the fiber
		AtlasFiberFolder->setText(path);
		m_AtlasFiberDir=path.toStdString();
		FillFiberFrame();
	}
}
/********************************************************************************* 
* when you finish editing the atlas directory
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::EnteronAtlasFiberFolder()
{
	if(AtlasFiberFolder->text().compare("")!=0)
	{
		//Clear the vectors of Fiber's names and the Lists
		m_Fibername.clear();
		m_FiberSelectedname.clear();
		m_Fibersplane.clear();
		m_FibersplaneSelected.clear();
		//clear the list
		FiberInAtlasList->clear ();
		FiberSelectedList->clear ();
		FiberSelected2List->clear ();
		FiberPlaneFile->clear ();
		//Set the path and Fill the list with the fiber
		m_AtlasFiberDir=AtlasFiberFolder->text().toStdString();
		FillFiberFrame();
	}
	else
	{
		//Clear the vectors of Fiber's names and the Lists
		m_Fibername.clear();
		m_FiberSelectedname.clear();
		m_Fibersplane.clear();
		m_FibersplaneSelected.clear();
		//clear the list
		FiberInAtlasList->clear ();
		FiberSelectedList->clear ();
		FiberSelected2List->clear ();
		FiberPlaneFile->clear ();
		m_nextStep = false;
	}
}

/********************************************************************************* 
 * Browser the output folder
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::BrowserOutputFolder()
{
	QString path = QFileDialog::getExistingDirectory(this);
	if(path!=NULL)
	{
		outputfolder->setText(path);
		m_OutputFolder=path.toStdString();
	}
}
/********************************************************************************* 
 * change the output folder when enter in the lineEdit
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::EnterOutputFolder()
{
	m_OutputFolder=(outputfolder->text()).toStdString();
}

/********************************************************************************* 
 * Fill the fiber frame with the name of the fiber form the atlas directory
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::FillFiberFrame()
{
	ReadFiberNameInAtlasDirectory(m_Fibername,m_Fibersplane, m_AtlasFiberDir);
	
	//Print the name in the ListWidget
	for( unsigned int j=0;j<m_Fibername.size();j++)
		FiberInAtlasList->addItem(m_Fibername[j].c_str());
}



/********************************************************************************* 
 * Add the fiber selected into the list
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::AddFiberInList()
{
	if(m_debug)
		std::cout<<"Add Fibers"<<std::endl;
	QList<QListWidgetItem *> SelectedFiber;
	SelectedFiber = FiberInAtlasList->selectedItems();
	int plane=-1;
	
	//add this in the FiberSelectedList and in the vector
	for(int i=0;i<SelectedFiber.count();i++)
	{
		//Remove and add the item in the List and vector
		for(int j=0;j<FiberInAtlasList->count();j++)
		{
			if(((FiberInAtlasList->item(j))->text()).compare((SelectedFiber.at(i))->text())==0)
			{
				QListWidgetItem* item = new QListWidgetItem;
				QString qs((SelectedFiber.at(i))->text());
				item->setData( 0, qs );
				FiberSelectedList->addItem(FiberInAtlasList->takeItem(j));
				FiberSelected2List->addItem(item);
				m_FiberSelectedname.push_back(((SelectedFiber.at(i))->text()).toStdString());
				/* Check the plane and add/erase it */
				plane = PlaneAssociatedToFiber(((SelectedFiber.at(i))->text()).toStdString(),1,
						m_Fibersplane, m_FibersplaneSelected);
				if(plane!=-1)
				{
					m_FibersplaneSelected.push_back(m_Fibersplane[plane]);
					QListWidgetItem* itemplane= new QListWidgetItem;
					QString qs(m_Fibersplane[plane].c_str());
					itemplane->setData( 0, qs );
					FiberPlaneFile->addItem(itemplane);
					m_Fibersplane.erase(m_Fibersplane.begin()+plane);
				}
			}
		}
		
		//remove the name in the first vector
		for(unsigned int k=0;k<m_Fibername.size();k++)
		{
			if(m_Fibername[k].compare(((SelectedFiber.at(i))->text()).toStdString())==0)
				m_Fibername.erase(m_Fibername.begin()+k);
		}
	}
}



/********************************************************************************* 
 * Add all fibers into the list
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::SelectAllFiber()
{
	//Read all the fibername data
	for( unsigned int i=0;i<m_Fibername.size();i++)
	{
		for(int j=0;j<FiberInAtlasList->count();j++)
		{
			if(((FiberInAtlasList->item(j))->text()).compare(m_Fibername[i].c_str())==0)
				FiberInAtlasList->takeItem(j);
		}
		
		QListWidgetItem* item1= new QListWidgetItem;
		QString qs(m_Fibername[i].c_str());
		item1->setData( 0, qs );
		FiberSelectedList->addItem(item1);
		QListWidgetItem* item2= new QListWidgetItem;
		qs=m_Fibername[i].c_str();
		item2->setData( 0, qs );
		FiberSelected2List->addItem(item2);
		m_FiberSelectedname.push_back(m_Fibername[i]);
		m_FibersplaneSelected.push_back(m_Fibersplane[i]);
	}
	
	//Read all the fiberplane name data
	for( unsigned int i=0;i<m_Fibersplane.size();i++)
	{
		QListWidgetItem* itemplane= new QListWidgetItem;
		QString qs(m_Fibersplane[i].c_str());
		itemplane->setData( 0, qs );
		FiberPlaneFile->addItem(itemplane);
	}
	
	//Clear the vector with the initial list of fibers' name.
	m_Fibername.clear();
	m_Fibersplane.clear();
}



/********************************************************************************* 
 * Remove the fiber selected from the list
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::RemoveFiberInList()
{
	if(m_debug)
		std::cout<<"Remove Fibers"<<std::endl;
	QList<QListWidgetItem *> SelectedFiber;
	SelectedFiber = FiberSelectedList->selectedItems();
	int plane=-1;
	
	//remove this from the FiberSelectedList and from the vector
	for(int i=0;i<SelectedFiber.count();i++)
	{
		//Remove the item
		for(int j=0;j<FiberSelectedList->count();j++)
		{
			if(((FiberSelectedList->item(j))->text()).compare((SelectedFiber.at(i))->text())==0)
			{
				FiberInAtlasList->addItem(FiberSelectedList->takeItem(j));
				FiberSelected2List->takeItem(j);
				m_Fibername.push_back(((SelectedFiber.at(i))->text()).toStdString());
				
				/* Plane */
				plane = PlaneAssociatedToFiber(((SelectedFiber.at(i))->text()).toStdString(),2,
						m_Fibersplane, m_FibersplaneSelected);
				if(plane!=-1)
				{
					FiberPlaneFile->takeItem(plane);
					m_Fibersplane.push_back(m_FibersplaneSelected[plane]);
					m_FibersplaneSelected.erase(m_FibersplaneSelected.begin()+plane);
				}
			}
		}
		
		//remove the name in the vector
		for(unsigned int k=0;k<m_FiberSelectedname.size();k++)
		{
			if(m_FiberSelectedname[k].compare(((SelectedFiber.at(i))->text()).toStdString())==0)
				m_FiberSelectedname.erase(m_FiberSelectedname.begin()+k);
		}
	}
}



/********************************************************************************* 
 * Remove all fibers into the list
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::RemoveAllFiber()
{
	//Read all the fibername data
	for( unsigned int i=0;i<m_FiberSelectedname.size();i++)
	{
		for(int j=0;j<FiberSelectedList->count();j++)
		{
			if(((FiberSelectedList->item(j))->text()).compare(m_FiberSelectedname[i].c_str())==0)
			{
				FiberSelectedList->takeItem(j);
				FiberSelected2List->takeItem(j);
			}
		}
		
		QListWidgetItem* item= new QListWidgetItem;
		QString qs(m_FiberSelectedname[i].c_str());
		item->setData( 0, qs );
		FiberInAtlasList->addItem(item);
		m_Fibername.push_back(m_FiberSelectedname[i]);
	}
	
	//Read all the fiberplanename data
	for( unsigned int i=0;i<m_FibersplaneSelected.size();i++)
	{
		for(int j=0;j<FiberPlaneFile->count();j++)
		{
			if(((FiberPlaneFile->item(j))->text()).compare(m_FibersplaneSelected[i].c_str())==0)
				FiberPlaneFile->takeItem(j);
		}
		
		m_Fibersplane.push_back(m_FibersplaneSelected[i]);
	}
	
	//Clear the vector with the initial list of fibers' name.
	m_FiberSelectedname.clear();
	m_FibersplaneSelected.clear();
}



/*********************************************************************************
 * Compute fiberprocess
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::Computefiberprocess()
{
	int ret=QMessageBox::Cancel;
	/* Mouse Event when running */
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	/* Call of fiberprocess if there is a Data/Deformation columns and at least one fiber */
	if(m_DataCol==-1)
		QMessageBox::information(this,"Warning","No Data column set, you have to set one!");
	else if(m_FiberSelectedname.empty())
		QMessageBox::information(this,"Warning","No fiber selected!");
	else
	{
		std::string pathFiberProcess;
		//Add the data to the CSV
		AddDataFromTableToCSV();
		
		//Find path for fiberprocess
		pathFiberProcess= itksys::SystemTools::FindProgram("fiberprocess");
		
		//if path not found
		if(pathFiberProcess.empty()==true)
		{
			ret = QMessageBox::information(this, "fiberprocess",
					"Select the folder where fiberprocess* is saved .");
			pathFiberProcess = (QFileDialog::getExistingDirectory(this)).toStdString();
			pathFiberProcess = pathFiberProcess +"/fiberprocess";
		}
		
		if(pathFiberProcess.compare("")!=0)
		{
			//Apply fiberprocess on CSV data
			if(!Applyfiberprocess(m_CSV, pathFiberProcess, m_AtlasFiberDir, m_OutputFolder,
			    m_DataCol, m_DeformationCol, m_NameCol, m_FiberSelectedname, m_parameters,
			    m_transposeColRow, false, this))
				std::cout<<"fiberprocess has been cancel!"<<std::endl;
			else
				m_nextStep = true;
			
			//Add the data to the Table
			FillCSVFileOnQTable();
		}
	}
	/* Restore the mouse */
	QApplication::restoreOverrideCursor();
}



/* Tab 3 : DTI_tract_stat */
/*********************************************************************************
 * Compute dti_tract_stat
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::Computedti_tract_stat()
{
	/* Mouse Event when running */
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	/* Call of fiberprocess if there is a Data/Deformation columns and at least one fiber */
	
	std::string pathdti_tract_stat;
	//get the properties
	setParam();
	if(m_parameters.compare("")==0)
		QMessageBox::information(this,"Warning","No properties selected!");
	else
	{
		//set the value of the transposition of row and column
		if(ArcOnCase->isChecked())
			m_transposeColRow = false;
		else if(CaseOnArc->isChecked())
			m_transposeColRow = true;
		
		//Add the data to the CSV
		AddDataFromTableToCSV();
		
		//Find path for fiberprocess
		pathdti_tract_stat= itksys::SystemTools::FindProgram("dtitractstatCLP");
		
		//if path not found
		if(pathdti_tract_stat.empty()==true)
		{
			QMessageBox::information(this, "dtitractstatCLP", 
					"Select the folder where dti_tract_stat is located .");
			pathdti_tract_stat = (QFileDialog::getExistingDirectory(this)).toStdString();
			pathdti_tract_stat = pathdti_tract_stat +"/dtitractstatCLP";
		}
		
		if(pathdti_tract_stat.compare("")!=0)
		{
			//Apply fiberprocess on CSV data
			if(!Applydti_tract_stat(m_CSV, pathdti_tract_stat, m_AtlasFiberDir, m_OutputFolder,
					    m_FiberSelectedname, m_FibersplaneSelected,m_parameters, m_DataCol,
					    m_DeformationCol, m_NameCol, m_transposeColRow, false, this))
				std::cout<<"dtitractstat has been cancel"<<std::endl;
			else
			{
				GatheringFiberProfile(m_CSV,m_OutputFolder,m_NumberOfParameters,m_DataCol,
						m_NameCol, m_transposeColRow);
				m_nextStep = true;
			}
			
			//Add the data to the Table
			FillCSVFileOnQTable();
		}
	}
	/* Restore the mouse */
	QApplication::restoreOverrideCursor();
}


/********************************************************************************* 
 * Save the value of the parameters
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::saveparamaction()
{
	QString filename = QFileDialog::getSaveFileName(this, "Save parameters file", QString(), "Text (*.txt)");
	saveparam(filename.toStdString(), m_csvfilename, m_DataCol, m_DeformationCol, m_NameCol, m_OutputFolder,
		  m_AtlasFiberDir, m_FiberSelectedname, m_parameters, m_transposeColRow);
}


/********************************************************************************* 
 * load the parameters 
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::openparam()
{
	bool loaded = false;
	QString filename = QFileDialog::getOpenFileName(this, "Open File", QString(), "Text (*.txt)");
	
	std::string ListOfFiber, str;
	int plane = -1;
	
	//delete the previous CSV
	DeleteCSV();
	
	if(filename!=NULL)
	{
	  std::ifstream file((filename.toStdString()).c_str() , std::ios::in);  // open in reading
		std::string str,buf1,buf2;
		
		if(file)  // if open
		{
			//the first line
			getline(file, buf1);
			if(buf1.compare(0,39,"Parameters for DTIAtlasFiberAnalyzer : ")==0)
			{
				/* Enable the frame to add column and row */
				addCRframe->setEnabled (true);
				saveframe->setEnabled (true);
				ColumnFrame->setEnabled (true);
				/* Loop for reading the file and setting the parameters values */
				while(!file.eof())
				{
					getline(file, buf1);
					if(buf1.compare(0,10,"CSVFile : ")==0)
					{
						m_csvfilename = buf1.substr(10,buf1.size()-10);
						csvfilename->setText(m_csvfilename.c_str());
						if(!m_CSVcreated)
						{
							m_CSV = new CSVClass(m_csvfilename,m_debug);
							m_CSVcreated = true;
						}
						if(m_CSV->loadCSVFile(m_csvfilename))
						{
							FillCSVFileOnQTable();
							loaded = true;
						}
					}
					
					else if(buf1.compare(0,14,"Data Column : ")==0)
					{
						if(loaded)
						{
							str = buf1.substr(14,buf1.size()-14);
							m_DataCol = atoi(str.c_str())-1;
							ColumnData->setText(str.c_str());
							ColorCol(1);
						}
					}
					
					else if(buf1.compare(0,26,"Defomation Field Column : ")==0)
					{
						if(loaded)
						{
							str = buf1.substr(26,buf1.size()-26);
							m_DeformationCol = atoi(str.c_str())-1;
							ColumnDeformation->setText(str.c_str());
							ColorCol(2);
						}
					}
					
					else if(buf1.compare(0,14,"Case Column : ")==0)
					{
						if(loaded)
						{
							str = buf1.substr(14,buf1.size()-14);
							m_NameCol = atoi(str.c_str())-1;
							ColumnNameCases->setText(str.c_str());
							ColorCol(3);
						}
					}
					
					else if(buf1.compare(0,16,"Output Folder : ")==0)
					{
						m_OutputFolder = buf1.substr(16,buf1.size()-16);
						outputfolder->setText(m_OutputFolder.c_str());
					}
					
					else if(buf1.compare(0,21,"Atlas Fiber Folder : ")==0)
					{
						FiberInAtlasList->clear();
						m_AtlasFiberDir = buf1.substr(21,buf1.size()-21);
						AtlasFiberFolder->setText(m_AtlasFiberDir.c_str());
						FillFiberFrame();
					}
					
					else if(buf1.compare(0,18,"Selected Fibers : ")==0)
					{
						FiberInAtlasList->clear();
						FiberSelectedList->clear();
						FiberSelected2List->clear();
						FiberPlaneFile->clear();
						m_Fibername.clear();
						m_FiberSelectedname.clear();
						m_Fibersplane.clear();
						m_FibersplaneSelected.clear();
						//fill the frame
						FillFiberFrame();
						ListOfFiber = buf1.substr(18,buf1.size()-18);
						// take the name of each fiber
						std::string::iterator it;
						std::string word;
						it = ListOfFiber.begin();
						while(it != ListOfFiber.end())
						{
							if(ListOfFiber.find_first_of(",")!=std::string::npos)
							{
								word = ListOfFiber.substr(0,
										ListOfFiber.find_first_of(","));
								m_FiberSelectedname.push_back(word);
								it = ListOfFiber.begin();
								ListOfFiber =
										ListOfFiber.substr(
										ListOfFiber.find_first_of(",")+1,
										ListOfFiber.size() -
										ListOfFiber.find_first_of(",")+1);
							}
							else
							{
								m_FiberSelectedname.push_back(ListOfFiber);
								it = ListOfFiber.end();
							}
						}
						for(unsigned int i=0;i<m_FiberSelectedname.size();i++)
						{
							//add the different fiber from the param
							FiberSelectedList->addItem(m_FiberSelectedname[i].c_str());
							FiberSelected2List->addItem(m_FiberSelectedname[i].c_str());
							
							//remove the name in the first vector and the list
							for(unsigned int k=0;k<m_Fibername.size();k++)
							{
								if(m_Fibername[k].compare(m_FiberSelectedname[i])==0)
								{
									m_Fibername.erase(m_Fibername.begin()+k);
									FiberInAtlasList->takeItem(k);
								}
							}
							
							/* Check the plane and add/erase it */
							plane = PlaneAssociatedToFiber(m_FiberSelectedname[i],1, 
									m_Fibersplane, m_FibersplaneSelected);
							
							if(plane!=-1)
							{
								m_FibersplaneSelected.push_back(m_Fibersplane[plane]);
								QListWidgetItem* itemplane= new QListWidgetItem;
								QString qs(m_Fibersplane[plane].c_str());
								itemplane->setData( 0, qs );
								FiberPlaneFile->addItem(itemplane);
								m_Fibersplane.erase(m_Fibersplane.begin()+plane);
							}
						}
					}
					
					else if(buf1.compare(0,20,"Profile parameter : ")==0)
					{
						m_parameters = buf1.substr(20,buf1.size()-20);
						m_NumberOfParameters = CalculNumberOfProfileParam(m_parameters);
						if(m_parameters.compare("")!=0)
							checkBoxProfile(m_parameters);
					}
					
					else if(buf1.compare(0,14,"Col and Row : ")==0)
					{
						if(buf1.substr(14,buf1.size()-14).compare("Case in column")==0 ||
						   buf1.substr(14,buf1.size()-14).compare("Case in col")==0 ||
						   buf1.substr(14,buf1.size()-14).compare("Arc lenght in row")==0)
						{
							m_transposeColRow = true;
							CaseOnArc->setChecked(true);
						}
						else
						{
							m_transposeColRow = false;
							ArcOnCase->setChecked(true);
						}
					}
				}
				
				//next step true if there is an cvs and an output
				if(loaded && csvfilename->text().compare("")!=0 
							       && outputfolder->text().compare("")!=0 
							       && m_CSVcreated)
					m_nextStep = true;
				
				
				std::cout<<std::endl;
				std::cout<<"Parameters file loaded : New Data"<<std::endl;
			}
			else
				std::cout<<"ERROR: Wrong file for parameters"<<std::endl;
			
			file.close();
		}
		else std::cerr << "ERROR: No parameters file found" << std::endl;
	}
}

/********************************************************************************* 
 * Check the box of profiles with the string parameters (FA, MD, ...)
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::checkBoxProfile(std::string parameters)
{
	//Iterator on the string line
	std::string::iterator it;
	std::string word;
	it = parameters.begin();
	vstring ProfileParam;
	//read the value between two ","
	while(it != parameters.end())
	{
		//if the line is at the last word, set the iterator at the end of the string
		if(parameters.find_first_of(",")!=std::string::npos)
		{
			word = parameters.substr(0, parameters.find_first_of(","));
			ProfileParam.push_back(word);
			it = parameters.begin();
			//erase the first word in the line
			parameters = parameters.substr(parameters.find_first_of(",")+1,parameters.size()
					-parameters.find_first_of(",")+1);
		}
		else
		{
			ProfileParam.push_back(parameters);
			it = parameters.end();
		}
	}
	//Unchecked all the box
	FABox->setCheckState(Qt::Unchecked);
	MDBox->setCheckState(Qt::Unchecked);
	FroBox->setCheckState(Qt::Unchecked);
	l1Box->setCheckState(Qt::Unchecked);
	l2Box->setCheckState(Qt::Unchecked);
	l3Box->setCheckState(Qt::Unchecked);
	RDBox->setCheckState(Qt::Unchecked);
	GABox->setCheckState(Qt::Unchecked);
	
	//Check it if there is the same name in the vstring
	for(unsigned int i=0;i<ProfileParam.size();i++)
	{
		if(ProfileParam[i].compare("fa")==0)
			FABox->setCheckState(Qt::Checked);
		else if(ProfileParam[i].compare("md")==0)
			MDBox->setCheckState(Qt::Checked);
		else if(ProfileParam[i].compare("fro")==0)
			FroBox->setCheckState(Qt::Checked);
		else if(ProfileParam[i].compare("l1")==0)
			l1Box->setCheckState(Qt::Checked);
		else if(ProfileParam[i].compare("l2")==0)
			l2Box->setCheckState(Qt::Checked);
		else if(ProfileParam[i].compare("l3")==0)
			l3Box->setCheckState(Qt::Checked);
		else if(ProfileParam[i].compare("rd")==0)
			RDBox->setCheckState(Qt::Checked);
		else if(ProfileParam[i].compare("ga")==0)
			GABox->setCheckState(Qt::Checked);
	}
}

