#include "DTIPlotWindow.h"

/********************************************************************************
 *Constructor
 ********************************************************************************/

PlotWindow::PlotWindow(QVector<qv3double> casedata, QVector<qv3double> atlasdata, QVector<qv3double> statdata, std::string parameters, DTIAtlasFiberAnalyzerguiwindow* parent) : QWidget(0)
{
	QFont Font=font();
	Font.setPointSize(7);
	setFont(Font);
	m_Error=false;
	m_parent=parent;
	m_SpecialCurves=4;
	setStyleSheet("QListWidget::indicator, QRadioButton::indicator{width:17px;height:17px;}");
	
	//Fill the m_Parameters, m_Cases, m_Fibers and m_Corr vectors
	LineInVector(parameters, m_Parameters);
	m_Cases=m_parent->getCases();
	m_Fibers=m_parent->getFibers();
	ComputeCorr(casedata, statdata);
	if(m_Corr.size()==0)
	{
		close();
		m_Error=true;
		return;
	}
	
	//Fill style vectors vector
	CreateCaseStyle();
	CreateAtlasStyle();
	CreateStatStyle();
	
	InitWidget();
	
	//Data calculation to plot curves
	Plotting(casedata, "Case");
	Plotting(atlasdata, "Atlas");
	Plotting(statdata, "Stat");
	m_Plot->setAxisTitle(QwtPlot::xBottom,"Arc_Lengh");
	m_Plot->setAxisTitle(QwtPlot::yRight,"Nb_of_fiber_points");
	m_Plot->setAxisTitle(QwtPlot::yLeft,m_Parameters[0].c_str());
	InitAxisInterval();
	show();
}

/********************************************************************************
 *InitWidget: Initialisation of all widget and alignment settings.
 ********************************************************************************/

void PlotWindow::InitWidget()
{
	//Widgets definitions
	m_MainLayout=new QHBoxLayout;
	m_VLayout=new QVBoxLayout;
	m_InfoLayout=new QVBoxLayout;
	m_OptionLayout=new QHBoxLayout;
	m_ParameterLayout=new QVBoxLayout;
	m_CheckBoxLayout=new QVBoxLayout;
	m_CaseLayout=new QGridLayout;
	m_AtlasLayout=new QGridLayout;
	m_StatLayout=new QGridLayout;
	m_FiberLayout=new QVBoxLayout;
	m_PixelGridLayout=new QGridLayout;
	m_CorrLayout=new QGridLayout;
	m_AxesLayout=new QGridLayout;
	m_Option2Layout=new QHBoxLayout;
	
	m_CaseBoxes=new QListWidget(this);
	m_CaseBoxes->setSpacing(-2);
	m_CaseBoxes->setMinimumSize(150,40);
	m_CaseBoxes->setMaximumSize(235,300);
	
	m_SAllReg=new QPushButton("Show All Average", this);
	m_SAllReg->setMinimumHeight(20); //Maximum?
	m_SAllStd=new QPushButton("Show All Std", this);
	m_SAllStd->setMinimumHeight(20);
	m_DAllReg=new QPushButton("Hide All Average", this);
	m_DAllReg->setMinimumHeight(20);
	m_DAllReg->hide();
	m_DAllStd=new QPushButton("Hide All Std", this);
	m_DAllStd->setMinimumHeight(20);
	m_DAllStd->hide();
	
	m_AtlasBoxes=new QListWidget(this);
	m_AtlasBoxes->setSpacing(-2);
	m_AtlasBoxes->setMinimumSize(150,40);
	m_AtlasBoxes->setMaximumSize(235,300);
	
	m_StatBoxes=new QListWidget(this);
	m_StatBoxes->setSpacing(-2);
	m_StatBoxes->setMinimumSize(150,40);
	m_StatBoxes->setMaximumSize(235,300);
	
	m_CaseLcd=new QLCDNumber(this);
	m_CaseSlider=new QSlider(Qt::Horizontal, this);
	m_AtlasLcd=new QLCDNumber(this);
	m_AtlasSlider=new QSlider(Qt::Horizontal, this);
	m_StatLcd=new QLCDNumber(this);
	m_StatSlider=new QSlider(Qt::Horizontal, this);	
	m_CasePxSize=new QLabel("Case width", this);
	m_AtlasPxSize=new QLabel("Atlas width", this);
	m_StatPxSize=new QLabel("Stat width", this);
	
	m_ComputeCorr=new QPushButton("Color correlation", this);
	m_ComputeCorr->setMinimumSize(150, 20);
	m_DecomputeCorr=new QPushButton("Decolor correlation", this);
	m_DecomputeCorr->setMinimumSize(150, 20);
	m_DecomputeCorr->setVisible(false);
	m_SaveCleanCSV=new QPushButton("Save Clean CSV", this);
	m_SaveCleanCSV->setMinimumSize(150,20);
	m_ThSlider=new QSlider(Qt::Horizontal, this);
	m_ThLcd=new QLCDNumber(this);
	m_ThLabel=new QLabel("Threshold", this);
	m_CorrText=new QTextEdit("", this);
	m_CorrText->setMaximumHeight(45);
	m_L_CorrColorMap=new QLabel(this);
	m_L_CorrColorMap->setFixedSize(245,20);
	m_L_CorrColorMap->setPixmap(GeneratePixmap());
	m_L_CorrColorMap->hide();
	QFont Font=font();
	Font.setBold(true);
	m_L_Min=new QLabel("<FONT COLOR=#000000>0.8</FONT>",this);
	m_L_Min->setFont(Font);
	m_L_Min->hide();
	m_L_Max=new QLabel("<FONT COLOR=#000000>1</FONT>",this);
	m_L_Max->setFont(Font);
	m_L_Max->hide();
	
	m_Min=new QLabel("Min", this);
	m_Max=new QLabel("Max", this);
	m_X=new QLabel("X", this);
	m_Y=new QLabel("Y", this);
	m_XMin=new QLineEdit;
	m_XMin->setFixedSize(40,18);
	m_XMax=new QLineEdit;
	m_XMax->setFixedSize(40,18);
	m_YMin=new QLineEdit;
	m_YMin->setFixedSize(40,18);
	m_YMax=new QLineEdit;
	m_YMax->setFixedSize(40,18);
	m_AutoScale=new QPushButton("Auto Scale", this);
	m_AutoScale->setMinimumHeight(20);
	
	m_Plot=new QwtPlot;
	m_Plot->setMinimumSize(600,600);
	
	InitCorrText();
	
	QGroupBox* DetailledInfo=new QGroupBox("Detailled Information");
	QGroupBox* CurveDisplay=new QGroupBox("Choose Curve to diplay");
	QGroupBox* GroupParameters=new QGroupBox("Parameters");
	QGroupBox* GroupCases=new QGroupBox("Cases");
	QGroupBox* GroupAtlas=new QGroupBox("Atlas");
	QGroupBox* GroupStat=new QGroupBox("Stat");
	QGroupBox* GroupFibers=new QGroupBox("Fibers");
	QGroupBox* GroupPxSize=new QGroupBox("Pixels size");
	QGroupBox* GroupCorr=new QGroupBox("Correlation");
	QGroupBox* GroupAxes=new QGroupBox("Axes Configuration");
	
	setSliderLcd();
	
	//Set alignment for each BoxLayout
	m_VLayout->setAlignment(Qt::AlignTop);
	m_InfoLayout->setAlignment(Qt::AlignTop);
	m_OptionLayout->setAlignment(Qt::AlignLeft);
	m_ParameterLayout->setAlignment(Qt::AlignTop);
	m_CheckBoxLayout->setAlignment(Qt::AlignTop);
	m_CaseLayout->setAlignment(Qt::AlignTop);
	m_AtlasLayout->setAlignment(Qt::AlignTop);
	m_StatLayout->setAlignment(Qt::AlignTop);
	m_FiberLayout->setAlignment(Qt::AlignTop);
	m_PixelGridLayout->setVerticalSpacing(0);
	GroupPxSize->setAlignment(Qt::AlignLeft);
	
	 
	//Fill each Layout
	DetailledInfo->setLayout(m_InfoLayout);
	
	GroupParameters->setLayout(m_ParameterLayout);
	
	m_CaseLayout->addWidget(m_CaseBoxes,0,0,1,2);
	m_CaseLayout->addWidget(m_SAllReg,1,0);
	m_CaseLayout->addWidget(m_SAllStd,1,1);
	m_CaseLayout->addWidget(m_DAllReg,1,0);
	m_CaseLayout->addWidget(m_DAllStd,1,1);
	m_CaseLayout->setRowStretch(2,1);
	m_CaseLayout->setColumnStretch(3,1);
	GroupCases->setLayout(m_CaseLayout);
	
	m_AtlasLayout->addWidget(m_AtlasBoxes,0,0);
	m_AtlasLayout->setRowStretch(1,1);
	m_AtlasLayout->setColumnStretch(1,1);
	GroupAtlas->setLayout(m_AtlasLayout);
	
	m_StatLayout->addWidget(m_StatBoxes,0,0);
	m_StatLayout->setRowStretch(1,1);
	m_StatLayout->setColumnStretch(1,1);
	GroupStat->setLayout(m_StatLayout);
	
	GroupFibers->setLayout(m_FiberLayout);
	
	m_CheckBoxLayout->addWidget(GroupCases);
	m_CheckBoxLayout->addWidget(GroupAtlas);
	m_CheckBoxLayout->addWidget(GroupStat);
	
	m_OptionLayout->addWidget(GroupParameters);
	m_OptionLayout->addLayout(m_CheckBoxLayout);
	m_OptionLayout->addWidget(GroupFibers);
	m_OptionLayout->addStretch(2);
	CurveDisplay->setLayout(m_OptionLayout);
	
	m_PixelGridLayout->addWidget(m_CaseLcd, 0, 0);
	m_PixelGridLayout->addWidget(m_CaseSlider, 0, 1);
	m_PixelGridLayout->addWidget(m_CasePxSize, 0, 2);
	m_PixelGridLayout->addWidget(m_AtlasLcd, 1, 0);
	m_PixelGridLayout->addWidget(m_AtlasSlider, 1, 1);
	m_PixelGridLayout->addWidget(m_AtlasPxSize, 1, 2);
	m_PixelGridLayout->addWidget(m_StatLcd, 2, 0);
	m_PixelGridLayout->addWidget(m_StatSlider, 2, 1);
	m_PixelGridLayout->addWidget(m_StatPxSize, 2, 2);
	m_PixelGridLayout->setHorizontalSpacing(1);
	m_PixelGridLayout->setColumnStretch(3,1);
	m_PixelGridLayout->setRowStretch(3,1);
	GroupPxSize->setLayout(m_PixelGridLayout);
	
	m_CorrLayout->addWidget(m_ComputeCorr,0,3);
	m_CorrLayout->addWidget(m_DecomputeCorr,0,3);
	m_CorrLayout->addWidget(m_ThLcd,0,0);
	m_CorrLayout->addWidget(m_ThSlider,0,1);
	m_CorrLayout->addWidget(m_ThLabel,0,2);
	m_CorrLayout->addWidget(m_CorrText,1,2,3,2);
	m_CorrLayout->addWidget(m_L_CorrColorMap,1,0,1,2);
	m_CorrLayout->addWidget(m_L_Min,1,0);
	m_CorrLayout->addWidget(m_L_Max,1,1, Qt::AlignRight);
	m_CorrLayout->addWidget(m_SaveCleanCSV,2,0,1,2);
	m_CorrLayout->setColumnStretch(4,2);
	m_CorrLayout->setRowStretch(4,2);
	GroupCorr->setLayout(m_CorrLayout);
	
	m_AxesLayout->addWidget(m_Min,0,1);
	m_AxesLayout->addWidget(m_Max,0,2);
	m_AxesLayout->addWidget(m_X,1,0,Qt::AlignRight);
	m_AxesLayout->addWidget(m_XMin,1,1);
	m_AxesLayout->addWidget(m_XMax,1,2);
	m_AxesLayout->addWidget(m_Y,2,0,Qt::AlignRight);
	m_AxesLayout->addWidget(m_YMin,2,1);
	m_AxesLayout->addWidget(m_YMax,2,2);
	m_AxesLayout->addWidget(m_AutoScale,3,0,1,2);
	m_AxesLayout->setColumnStretch(3,1);
	GroupAxes->setLayout(m_AxesLayout);
	
	m_VLayout->addWidget(DetailledInfo);
	m_VLayout->addWidget(CurveDisplay);
	
	m_Option2Layout->addWidget(GroupAxes);
	m_Option2Layout->addWidget(GroupPxSize);
	m_Option2Layout->addStretch(2);
	
	m_VLayout->addLayout(m_Option2Layout);
	m_VLayout->addWidget(GroupCorr);
	
	m_MainLayout->addWidget(m_Plot,1);
	m_MainLayout->addLayout(m_VLayout);
	setLayout(m_MainLayout);
	
	CreateParameterButtons();
	CreateCaseBoxes();
	CreateAtlasBoxes();
	CreateStatBoxes();
	CreateFiberButtons();
	CreateInfoLabel();
	CreateCurves();
	
	connect(m_CaseSlider, SIGNAL(valueChanged(int)), this, SLOT(setPenWidth(int)));
	connect(m_AtlasSlider, SIGNAL(valueChanged(int)), this, SLOT(setPenWidth(int)));
	connect(m_StatSlider, SIGNAL(valueChanged(int)), this, SLOT(setPenWidth(int)));
	connect(m_ComputeCorr, SIGNAL(clicked()), this, SLOT(ColorCorr()));
	connect(m_DecomputeCorr, SIGNAL(clicked()), this, SLOT(DecolorCorr()));
	connect(m_ThSlider, SIGNAL(valueChanged(int)), this, SLOT(ApplyTh(int)));
	connect(m_XMin, SIGNAL(editingFinished()), this, SLOT(UpdateAxis()));
	connect(m_XMax, SIGNAL(editingFinished()), this, SLOT(UpdateAxis()));
	connect(m_YMin, SIGNAL(editingFinished()), this, SLOT(UpdateAxis()));
	connect(m_YMax, SIGNAL(editingFinished()), this, SLOT(UpdateAxis()));
	connect(m_AutoScale, SIGNAL(clicked()), this, SLOT(AutoScale()));
	connect(m_CaseBoxes, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(setCurveVisible()));
	connect(m_AtlasBoxes, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(setCurveVisible()));
	connect(m_StatBoxes, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(setCurveVisible()));
	connect(m_SAllReg, SIGNAL(clicked()), this, SLOT(SelectAllRegular()));
	connect(m_DAllReg, SIGNAL(clicked()), this, SLOT(DeselectAllRegular()));
	connect(m_SAllStd, SIGNAL(clicked()), this, SLOT(SelectAllStd()));
	connect(m_DAllStd, SIGNAL(clicked()), this, SLOT(DeselectAllStd()));
	connect(m_SaveCleanCSV, SIGNAL(clicked()), this, SLOT(WriteCleanCSV()));
	
}

void PlotWindow::SelectAllRegular()
{
	for(unsigned int CaseId=0; CaseId<m_Cases.size(); CaseId++)
		m_CaseBoxes->item(CaseId*2)->setCheckState(Qt::Checked);
	m_SAllReg->hide();
	m_DAllReg->show();
}

void PlotWindow::DeselectAllRegular()
{
	for(unsigned int CaseId=0; CaseId<m_Cases.size(); CaseId++)
		m_CaseBoxes->item(CaseId*2)->setCheckState(Qt::Unchecked);
	m_SAllReg->show();
	m_DAllReg->hide();
}

void PlotWindow::SelectAllStd()
{
	for(unsigned int CaseId=0; CaseId<m_Cases.size(); CaseId++)
		m_CaseBoxes->item(CaseId*2+1)->setCheckState(Qt::Checked);
	m_SAllStd->hide();
	m_DAllStd->show();
}

void PlotWindow::DeselectAllStd()
{
	for(unsigned int CaseId=0; CaseId<m_Cases.size(); CaseId++)
		m_CaseBoxes->item(CaseId*2+1)->setCheckState(Qt::Unchecked);
	m_SAllStd->show();
	m_DAllStd->hide();
}
/********************************************************************************
 *setSliderLcd: Geometry settings of Lcd and Slider widgets
 ********************************************************************************/

void PlotWindow::setSliderLcd()
{
	m_CaseLcd->setSegmentStyle(QLCDNumber::Flat);
	m_CaseLcd->display(1);
	m_CaseLcd->setMaximumSize(40,20);
	m_AtlasLcd->setSegmentStyle(QLCDNumber::Flat);
	m_AtlasLcd->display(1);
	m_AtlasLcd->setMaximumSize(40,20);
	m_StatLcd->setSegmentStyle(QLCDNumber::Flat);
	m_StatLcd->display(1);
	m_StatLcd->setMaximumSize(40,20);
	m_ThLcd->setSegmentStyle(QLCDNumber::Flat);
	m_ThLcd->display(0.8);
	m_ThLcd->setMaximumSize(40,20);
	
	m_CaseSlider->setRange(1,10);
	m_CaseSlider->setFixedSize(150,20);
	m_AtlasSlider->setRange(1,10);
	m_AtlasSlider->setFixedSize(150,20);
	m_StatSlider->setRange(1,10);
	m_StatSlider->setFixedSize(150,20);
	m_ThSlider->setRange(0,100);
	m_ThSlider->setValue(80);
	m_ThSlider->setFixedSize(200,20);
}

void PlotWindow::InitCorrText()
{
	std::ostringstream corrtext;
	m_CorrText->setVisible(false);
	m_CorrText->setReadOnly(true);
	for(unsigned int i=0; i<m_Cases.size(); i++)
	{
		corrtext<<m_Cases[i]<<" : "<<m_Corr[0][i][0]<<" ";
		if(i%5==0&&i!=0)
			corrtext<<std::endl;
	}
	m_CorrText->setText(corrtext.str().c_str());
}

void PlotWindow::UpdateAxis()
{
	m_Plot->setAxisAutoScale(QwtPlot::xBottom, false);
	m_Plot->setAxisAutoScale(QwtPlot::yLeft, false);
	m_Plot->setAxisScale(QwtPlot::xBottom, atof(m_XMin->text().toStdString().c_str()), atof(m_XMax->text().toStdString().c_str()));
	m_Plot->setAxisScale(QwtPlot::yLeft, atof(m_YMin->text().toStdString().c_str()), atof(m_YMax->text().toStdString().c_str()));
	m_Plot->replot();
}

void PlotWindow::AutoScale()
{
	m_Plot->setAxisAutoScale(QwtPlot::xBottom, true);
	m_Plot->setAxisAutoScale(QwtPlot::yLeft, true);
	m_Plot->replot();
	InitAxisInterval();
}

void PlotWindow::InitAxisInterval()
{
	QwtInterval Interval;
	std::ostringstream text;
	Interval=m_Plot->axisInterval(QwtPlot::xBottom);
	text<<Interval.minValue();
	m_XMin->setText(text.str().c_str());
	text.str("");
	text<<Interval.maxValue();
	m_XMax->setText(text.str().c_str());
	text.str("");
	
	Interval=m_Plot->axisInterval(QwtPlot::yLeft);
	text<<Interval.minValue();
	m_YMin->setText(text.str().c_str());
	text.str("");
	text<<Interval.maxValue();
	m_YMax->setText(text.str().c_str());
	text.str("");
}

/**********************************************************************************************
 *CreateStyle : Fill the style vector of each type of curves with different pens.
 **********************************************************************************************/
 
void PlotWindow::CreateCaseStyle()
{	
	QColor color;
	QPen pen;
	double coef;
	QVector <qreal> dashes;
	dashes<<10<<5;
	
	
	for(unsigned int i=0; i<m_Cases.size()*3; i++)
	{
		if(i%3==0)
		{
			if(m_Cases.size()==1)
				coef=0;
			else
				coef=i/(double)((m_Cases.size()-1)*3);
			//Generate random RGB color based on red
			if(coef*242<=116)
				color.setRgb((int)(139+coef*242), 0, 0);
			else
				color.setRgb(255, (int)(coef*242-116), (int)(coef*242-116));
			pen.setStyle(Qt::SolidLine);
		}
		else
			pen.setDashPattern(dashes);
		pen.setColor(color);
		m_CaseStyle.push_back(pen);
	}
}

void PlotWindow::CreateAtlasStyle()
{
	QVector <qreal> dashes;
	QPen pen;
	
	pen.setColor(Qt::blue);
	m_AtlasStyle.push_back(pen);
	
	dashes<<10<<5;
	pen.setDashPattern(dashes);
	pen.setColor(Qt::darkBlue);
	m_AtlasStyle.push_back(pen);
	m_AtlasStyle.push_back(pen);
	
	pen.setStyle(Qt::SolidLine);
	pen.setColor(Qt::gray);
	m_AtlasStyle.push_back(pen);
}

void PlotWindow::CreateStatStyle()
{
	QVector <qreal> dashes;
	QPen pen;
	QColor color;
	color.setRgb(204,153,0);
	
	pen.setColor(color);
	m_StatStyle.push_back(pen);
	
	dashes<<10<<5;
	pen.setDashPattern(dashes);
	color.setRgb(204,102,0);
	pen.setColor(color);
	m_StatStyle.push_back(pen);
	m_StatStyle.push_back(pen);
}

/********************************************************************************
 *setPenWidth: Slot bind to sliders to set pen width at the slider's value
 ********************************************************************************/

void PlotWindow::setPenWidth(int width)
{
	QPen pen;
	if(sender()==m_CaseSlider)
	{
		for(unsigned int i=0; i<m_CaseStyle.size(); i++)
			m_CaseStyle[i].setWidth(width);
		m_CaseLcd->display(width);
		for(unsigned int i=0; i<m_Fibers.size(); i++)
		{
			for(unsigned int j=0; j<m_CaseStyle.size(); j++)
			{
				for(unsigned int k=0; k<m_Parameters.size(); k++)
				{
					pen=m_CaseCurves[i][j][k]->pen();
					pen.setWidth(width);
					m_CaseCurves[i][j][k]->setPen(pen);
				}
			}
		}
	}
	else if(sender()==m_AtlasSlider)
	{
		for(unsigned int i=0; i<m_AtlasStyle.size(); i++)
			m_AtlasStyle[i].setWidth(width);
		m_AtlasLcd->display(width);
		for(unsigned int i=0; i<m_Fibers.size(); i++)
		{
			for(unsigned int j=0; j<m_AtlasStyle.size(); j++)
			{
				for(unsigned int k=0; k<m_Parameters.size(); k++)
						m_AtlasCurves[i][j][k]->setPen(m_AtlasStyle[j]);
			}
		}
	}
	else if(sender()==m_StatSlider)
	{
		for(unsigned int i=0; i<m_StatStyle.size(); i++)
			m_StatStyle[i].setWidth(width);
		m_StatLcd->display(width);
		for(unsigned int i=0; i<m_Fibers.size(); i++)
		{
			for(unsigned int j=0; j<m_StatStyle.size(); j++)
			{
				for(unsigned int k=0; k<m_Parameters.size(); k++)
						m_StatCurves[i][j][k]->setPen(m_StatStyle[j]);
			}
		}
	}
	
	m_Plot->replot();
}

/**********************************************************************************************
 *setCurveVisible : bind checkboxes and curves with member setVisible of QwtPlotCurve class.
 *		An expression using modulo operand is required to match curve's indexes and box's indexes.
 **********************************************************************************************/

void PlotWindow::setCurveVisible()
{
	int paramindex, fiberindex, boxindex;
	std::vector <int> caseindex, atlasindex, statindex;
	std::ostringstream corrtext;
	vstring parameterslines;
	paramindex=GetCheckedParameter();
	fiberindex=GetCheckedFiber();
	caseindex=GetCheckedCase();
	atlasindex=GetCheckedAtlas();
	statindex=GetCheckedStat();

	
	for(int i=0; i<(int)m_CaseCurves.size(); i++)
	{
		for(int k=0; k<(int)m_CaseCurves[0][0].size(); k++)
		{
			boxindex=-1;
			for(int j=0; j<(int)m_CaseCurves[0].size(); j++)
			{
				if(j%3!=2)
					boxindex++;
				if(i==fiberindex && std::find(caseindex.begin(), caseindex.end(),boxindex )!=caseindex.end() && k==paramindex)
					m_CaseCurves[i][j][k]->setVisible(true);
				else
					m_CaseCurves[i][j][k]->setVisible(false);
			}
			boxindex=-1;
			for(int j=0; j<(int)m_AtlasCurves[0].size(); j++)
			{
				if(j%3!=2)
					boxindex++;
				if(i==fiberindex && std::find(atlasindex.begin(), atlasindex.end(),boxindex )!=atlasindex.end() && k==paramindex)
				{
					if(j==3)
						m_Plot->enableAxis(QwtPlot::yRight, true);
					m_AtlasCurves[i][j][k]->setVisible(true);
				}
				else
				{
					if(j==3 && std::find(atlasindex.begin(), atlasindex.end(),2 )==atlasindex.end())
						m_Plot->enableAxis(QwtPlot::yRight, false);
					m_AtlasCurves[i][j][k]->setVisible(false);
				}
			}
			boxindex=-1;
			for(int j=0; j<(int)m_StatCurves[0].size(); j++)
			{
				if(j%3!=2)
					boxindex++;
				if(i==fiberindex && std::find(statindex.begin(), statindex.end(),boxindex )!=statindex.end() && k==paramindex)
					m_StatCurves[i][j][k]->setVisible(true);
				else
					m_StatCurves[i][j][k]->setVisible(false);
			}
			if(k==paramindex)
				m_Plot->setAxisTitle(QwtPlot::yLeft,m_Parameters[k].c_str());
		}
		//Change the information label for informations of selected fiber
		if(i==fiberindex)
		{
			parameterslines=m_parent->getFiberInformations(m_Fibers[i].c_str());
			ChangeInfoLabel(parameterslines);
		}
	}
	m_Plot->replot();
	if(fiberindex>=0 && paramindex>=0)
	{
		ApplyTh(m_ThSlider->value());
		for(unsigned int i=0; i<m_Cases.size(); i++)
		{
			corrtext<<m_Cases[i]<<" : "<<m_Corr[fiberindex][i][paramindex]<<" ";
			if(i%5==0&&i!=0)
				corrtext<<std::endl;
		}
		m_CorrText->setText(corrtext.str().c_str());
	}
}		


/**********************************************************************************************
 *Color & Decolor : Use color (or not) using correlation to display case's curves
 **********************************************************************************************/

void PlotWindow::ColorCorr()
{
	m_ComputeCorr->setVisible(false);
	m_DecomputeCorr->setVisible(true);
	m_CorrText->setVisible(true);
	m_L_CorrColorMap->show();
	m_L_Min->show();
	m_L_Max->show();
	ApplyTh(m_ThSlider->value());
}

void PlotWindow::DecolorCorr()
{
	m_ComputeCorr->setVisible(true);
	m_DecomputeCorr->setVisible(false);
	m_CorrText->setVisible(false);
	m_L_CorrColorMap->hide();
	m_L_Min->hide();
	m_L_Max->hide();
	ApplyTh(m_ThSlider->value());
}

QPixmap PlotWindow::GeneratePixmap()
{
	QPixmap pixels(250,30);
	pixels.fill(QColor(0,0,0));
	QPainter pen(&pixels);
	for(int i=0; i<156; i++)
	{
		int ActualY=250-i*250/156;
		int NextY=250-(i+1)*250/156;
		pen.setPen(QColor(0,255-i,0));
		pen.setBrush(QColor(0,255-i,0));
		pen.drawRect(ActualY,0,NextY-ActualY,70);
	}
	return pixels;
}
	

/**********************************************************************************************
 *ApplyTh : Color each curves below threshold in red
 **********************************************************************************************/
 
void PlotWindow::ApplyTh(int value)
{
	double thvalue=((double)value)/100.0;
	QBrush brush;
	QColor color;
	m_ThLcd->display(thvalue);
	std::ostringstream min;
	min<<"<FONT COLOR=#000000>"<<thvalue<<"</FONT>";
	m_L_Min->setText(min.str().c_str());
	int FiberId=GetCheckedFiber();
	int ParameterId=GetCheckedParameter();
	for(unsigned int CaseId=0; CaseId<m_Cases.size(); CaseId++)
	{
		QPen pen;
		if(m_Corr[FiberId][CaseId][ParameterId]<thvalue)
		{
			pen.setColor(Qt::red);
			brush.setColor(Qt::red);
		}
		else
		{
			brush.setColor(m_CaseStyle[CaseId*3].color());
			if(m_ComputeCorr->isVisible())
				pen=m_CaseStyle[CaseId*3];
			else
			{
				if(thvalue!=1)
					color.setRgb(0,100+(int)(155*(fabs(m_Corr[FiberId][CaseId][ParameterId])-thvalue)/(1-thvalue)),0);
				else
					color.setRgb(0,255,0);
				brush.setColor(color);
				pen.setColor(color);
			}
		}
		m_CaseBoxes->item(CaseId*2)->setForeground(brush);
		m_CaseBoxes->item(CaseId*2+1)->setForeground(brush);
		m_CaseCurves[FiberId][CaseId*3][ParameterId]->setPen(pen);
		
		QVector <qreal> dashes;
		dashes<<10<<5;
		pen.setDashPattern(dashes);
		m_CaseCurves[FiberId][CaseId*3+1][ParameterId]->setPen(pen);
		m_CaseCurves[FiberId][CaseId*3+2][ParameterId]->setPen(pen);
	}
	m_Plot->replot();
}


/**********************************************************************************************
 *ComputeCorr : Calculate correlation factor for each case's curves
 **********************************************************************************************/

void PlotWindow::ComputeCorr(QVector<qv3double> casedata, QVector<qv3double> statdata)
{
	QVector<double> corrpt;
	QVector<QVector<double> > corr;
	m_Corr.clear();
	for(unsigned int i=0; i<m_Fibers.size(); i++)
	{
		for(unsigned int j=0; j<m_Cases.size(); j++)
		{
			corrpt=getCorr(casedata[i][j*3], statdata[i][0]);
			if(corrpt.size()!=0)
				corr.push_back(corrpt);
			else
			{
				QMessageBox::warning(this, "Warning", "Error during calculation of correlation between cases.\nYou should recomputes data using dtitractstat.");
				return;
			}
		}
			
		m_Corr.push_back(corr);
		corr.clear();
	}
}

/**********************************************************************************************
 *WriteCleanCSV : ReWrite a profile parameter file without cases with a correlation below the 
 *	threshold
 **********************************************************************************************/

void PlotWindow::WriteCleanCSV()
{
	int FiberId=GetCheckedFiber();
	int ParameterId=GetCheckedParameter();
	bool TransposeCol;
	v2string TempTable;
	vstring Line;
	std::string Filename=m_parent->getOutputFolder()+"/FiberProfiles/"+m_Fibers[FiberId]+"/"+m_Parameters[ParameterId]+"_"+m_Fibers[FiberId]+".csv";
	
	std::ifstream File(Filename.c_str(), std::ios::in);
	std::string Buffer;
	
	getline(File, Buffer);
	if(File)
	{
		while(!File.eof())
		{
			LineInVector(Buffer,Line);
			TempTable.push_back(Line);
			getline(File, Buffer);
		}
	}
	else
	{
		std::cout<<"Error reading original fiber profile."<<std::endl;
		return;
	}
	//Check if the table is written by column or by row.
	if(!IsFloat(TempTable[0][1].c_str()))
		TransposeCol=true;
	else if(!IsFloat(TempTable[1][0].c_str()))
		TransposeCol=false;
	else
		std::cout<<"Error finding to transpose csv table or not."<<std::endl;
	
	std::string SaveFilename=takeoffExtension(Filename)+"_Clean.csv";
	std::ofstream SaveFile(SaveFilename.c_str(), std::ios::out);
	if(SaveFile)
	{
		if(!TransposeCol)
		{
			for(int i=0; i<TempTable.size()-1; i++)
			{
				if(i==0 || (i>0 && m_Corr[FiberId][i-1][ParameterId]>=(double)m_ThSlider->value()/100.0))
				{
					for(int j=0; j<TempTable[i].size()-1; j++)
						SaveFile<<TempTable[i][j]<<",";
					SaveFile<<TempTable[i][TempTable[i].size()-1]<<std::endl;
				}
			}
			int Last=TempTable.size()-1;
			for(int j=0; j<TempTable[Last].size()-1; j++)
				SaveFile<<TempTable[Last][j]<<",";
			SaveFile<<TempTable[Last][TempTable[Last].size()-1]<<std::endl;
		}
		else
		{
			for(int i=0; i<TempTable.size(); i++)
			{
				for(int j=0; j<TempTable[i].size()-1; j++)
				{
					if(j==0 || (j>0 && m_Corr[FiberId][j-1][ParameterId]>=(double)m_ThSlider->value()/100.0))
						SaveFile<<TempTable[i][j]<<",";
				}
				SaveFile<<TempTable[i][TempTable[i].size()-1]<<std::endl;
			}
		}
		SaveFile.close();
	}
	else
		std::cout<<"ERROR: Open the file for saving profile info!"<<std::endl;
}

/********************************************************************************
 *GetChecked...: Return indexes of checked buttons and boxes.
 ********************************************************************************/

int PlotWindow::GetCheckedParameter()
{
	for(unsigned int i=0; i<m_ParameterButtons.size(); i++)
	{
		if(m_ParameterButtons[i]->isChecked()==true)
			return i;
	}
	
	return -1;
}

std::vector <int> PlotWindow::GetCheckedCase()
{
	std::vector <int> checked;
	for(int i=0; i<m_CaseBoxes->count(); i++)
	{
		if(m_CaseBoxes->item(i)->checkState()==Qt::Checked)
			checked.push_back(i);
	}
	checked.push_back(-1);
	return checked;
}

std::vector <int> PlotWindow::GetCheckedAtlas()
{
	std::vector <int> checked;
	for(int i=0; i<m_AtlasBoxes->count(); i++)
	{
		if(m_AtlasBoxes->item(i)->checkState()==Qt::Checked)
			checked.push_back(i);
	}
	checked.push_back(-1);
	return checked;
}

std::vector <int> PlotWindow::GetCheckedStat()
{
	std::vector <int> checked;
	for(int i=0; i<m_StatBoxes->count(); i++)
	{
		if(m_StatBoxes->item(i)->checkState()==Qt::Checked)
			checked.push_back(i);
	}
	checked.push_back(-1);
	return checked;
	
}

int PlotWindow::GetCheckedFiber()
{
	for(unsigned int i=0; i<m_FiberButtons.size(); i++)
	{
		if(m_FiberButtons[i]->isChecked()==true)
			return i;
	}
	
	return -1;
}

/**********************************************************************************************
 *CreateBoxes : Dynamic definitions of CheckBoxes and Radio button following previous 
 *		checked parameters, cases and fibers. 
 *		Specific rules apply for CheckBoxes : There are 2 boxes per case, 3 for atlas curves and 
 *			2 for stat curves. 
 **********************************************************************************************/

void PlotWindow::CreateParameterButtons()
{
	//Creation of parameter buttons
	for(unsigned int i=0; i<m_Parameters.size(); i++)
	{
		QRadioButton* button=new QRadioButton(m_Parameters[i].c_str(),this);
		button->setIconSize(QSize(2,2));
		m_ParameterButtons.push_back(button);
		m_ParameterLayout->addWidget(m_ParameterButtons[i]);
		connect(this->m_ParameterButtons[i], SIGNAL(clicked()), this, SLOT(setCurveVisible()));
	}
	m_ParameterButtons[0]->setChecked(true);
}

void PlotWindow::CreateCaseBoxes()
{
	// 	Creation of case checkboxes
	int styleindex=-1;
	for(unsigned int i=0; i<m_Cases.size()*2; i++)
	{
		QListWidgetItem* item=new QListWidgetItem;
		QString label=m_Cases[i/2].c_str();
		QBrush brush;
		styleindex++;
		if(i%2==1)
			styleindex++;
		if(i%2!=0)
			label+=" and Std";
		item->setText(label);
		brush.setColor(m_CaseStyle[styleindex].color());
		item->setForeground(brush);
		if(i==0)
			item->setCheckState(Qt::Checked);
		else
			item->setCheckState(Qt::Unchecked);
		item->setToolTip(label);
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
		m_CaseBoxes->addItem(item);
	}
}

void PlotWindow::CreateAtlasBoxes()
{
	int styleindex=-1;
	//Creation of atlas boxe
	for(unsigned int i=0; i<3; i++)
	{
		QListWidgetItem* item=new QListWidgetItem;
		QString label;
		QBrush brush;
		styleindex++;
		if(i%2==1)
			styleindex++;
		if(i==0)
			label="Atlas value";
		else if(i==1)
			label="Atlas value and std";
		else if(i==2)
			label="Nb of fiber points";
		brush.setColor(m_AtlasStyle[styleindex].color());
		item->setForeground(brush);
		item->setText(label);
		item->setCheckState(Qt::Unchecked);
		item->setToolTip(label);
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
		m_AtlasBoxes->addItem(item);
	}
}

void PlotWindow::CreateStatBoxes()
{
	int styleindex=-1;
	//Creation of stat boxes
	for(unsigned int i=0; i<2; i++)
	{
		QListWidgetItem* item=new QListWidgetItem;
		QString label;
		QBrush brush;
		styleindex++;
		if(i%2==1)
			styleindex++;
		if(i==0)
			label="Mean";
		else if(i==1)
			label="Mean and std";
		brush.setColor(m_StatStyle[styleindex].color());
		item->setText(label);
		item->setForeground(brush);
		item->setCheckState(Qt::Unchecked);
		item->setToolTip(label);
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
		m_StatBoxes->addItem(item);
	}
}

void PlotWindow::CreateFiberButtons()
{
	//Creation of fiber buttons
	for(unsigned int i=0; i<m_Fibers.size(); i++)
	{
		m_FiberButtons.push_back(new QRadioButton(m_Fibers[i].c_str(), this));
		m_FiberLayout->addWidget(m_FiberButtons[i]);
		connect(this->m_FiberButtons[i], SIGNAL(clicked()), this, SLOT(setCurveVisible()));
	}
	m_FiberButtons[0]->setChecked(true);
}

/**********************************************************************************************
 *CreateCurves : Dynamic definitions of curves
 **********************************************************************************************/

void PlotWindow::CreateCurves()
{
	std::vector <std::vector <QwtPlotCurve*> > checkcurves;
	std::vector <QwtPlotCurve*> parametercurves;
	
	m_CaseCurves.clear();
	m_AtlasCurves.clear();
	m_StatCurves.clear();
	
	for(unsigned int i=0; i<m_Fibers.size(); i++)
	{
		//Cases, cases and std
		for(unsigned int j=0; j<m_Cases.size()*3; j++)
		{
			for(unsigned int k=0; k<m_Parameters.size(); k++)
			{
					parametercurves.push_back(new QwtPlotCurve);
					if(i!=0 || j!=0 || k!=0)
						parametercurves[k]->setVisible(false);
					parametercurves[k]->setPen(m_CaseStyle[j]);
			}
			checkcurves.push_back(parametercurves);
			parametercurves.clear();
		}
		m_CaseCurves.push_back(checkcurves);
		checkcurves.clear();
		for(unsigned int j=0; j<4; j++)
		{
			for(unsigned int k=0; k<m_Parameters.size(); k++)
			{
				parametercurves.push_back(new QwtPlotCurve);
				parametercurves[k]->setVisible(false);
				parametercurves[k]->setPen(m_AtlasStyle[j]);
			}
			checkcurves.push_back(parametercurves);
			parametercurves.clear();
		}
		m_AtlasCurves.push_back(checkcurves);
		checkcurves.clear();
		for(unsigned int j=0; j<3; j++)
		{
			for(unsigned int k=0; k<m_Parameters.size(); k++)
			{
				parametercurves.push_back(new QwtPlotCurve);
				parametercurves[k]->setVisible(false);
				parametercurves[k]->setPen(m_StatStyle[j]);
			}
			checkcurves.push_back(parametercurves);
			parametercurves.clear();
		}
		m_StatCurves.push_back(checkcurves);
		checkcurves.clear();
	}
}

/**********************************************************************************************
 *FillInfoLabel : Fill the information layout from a string vector containing 6 first lines of .fvp file
 **********************************************************************************************/

void PlotWindow::CreateInfoLabel()
{
	vstring info;
	int labelindex=-1;	
	info=m_parent->getFiberInformations(m_Fibers[0].c_str());
	for(unsigned int i=0; i<info.size(); i++)
	{
		if(i!=4)
		{
			QFont Font=font();
			Font.setPointSize(7);
			labelindex++;
			QLabel* label=new QLabel(info[i].c_str());
			label->setFont(Font);
			m_InfoLabel.push_back(label);
		}
		m_InfoLayout->addWidget(m_InfoLabel[labelindex]);
	}
}

/**********************************************************************************************
 *ChangeInfoLabel : Used to update the information label when checked fiber is changed
 **********************************************************************************************/

void PlotWindow::ChangeInfoLabel(vstring v_string)
{
	unsigned int i, labelindex=-1;
	for(i=0; i<v_string.size(); i++)
	{
		if(i!=4)
		{
			labelindex++;
			if(i<m_InfoLabel.size())
				m_InfoLabel[labelindex]->setText(v_string[i].c_str());
			//If previous label was shorter
			else
			{
				m_InfoLabel.push_back(new QLabel(v_string[i].c_str()));
				m_InfoLayout->addWidget(m_InfoLabel[labelindex]);
			}
		}
	}
	//If previous label was longer
	if(i<m_InfoLabel.size())
	{
		for(unsigned int j=i ; j<m_InfoLabel.size(); j++)
			m_InfoLabel[j]->setText("");
	}
}

/**********************************************************************************************
 *Plotting : Display curve with data samples in 4D vector 'data'. For each fiber you have
 * 	an 3 data tables per cases(Value Value+std Value-std) or 4 data tables in Atlas type 
 * 	(Value Value+std Value-std Nb of Fiber points) or 3 data tables in Stat type(Mean Mean+std Mean-std)
 **********************************************************************************************/

void PlotWindow::Plotting(QVector<qv3double> data, std::string type)
{
	qv3double fiberdata;
	std::vector <std::vector <std::vector <QwtPlotCurve*> > > curves;
	QVector< QVector< double > > casedata;
	QVector<double>x_data, y_data;
	int tablesize=0;
	
	
	//Utilisation pointeur?
	if(type=="Case")
	{
		tablesize=m_Cases.size()*3;
		curves=m_CaseCurves;
	}
	else if(type=="Atlas")
	{
		tablesize=m_AtlasCurves[0].size();
		curves=m_AtlasCurves;
	}
	else if(type=="Stat")
	{
		tablesize=m_StatCurves[0].size();
		curves=m_StatCurves;
	}
	
	
	for(unsigned int i=0; i<m_Fibers.size(); i++)
	{
		fiberdata=data[i];
		for(int j=0; j<tablesize; j++)
		{
			casedata=fiberdata[j];
			x_data=casedata[0];
			for(unsigned int k=0; k<m_Parameters.size(); k++)
			{
				//Column 1 to (number of parameters + 1) up to 9 contains data samples for each parameters
				y_data=casedata[k+1];
				
				curves[i][j][k]->setSamples(x_data.data(), y_data.data(), x_data.size());
				if(type=="Atlas" && j==tablesize-1)
					curves[i][j][k]->setAxes(QwtPlot::xBottom, QwtPlot::yRight);
				curves[i][j][k]->attach(m_Plot);
				
			}
		}
	}
	
	m_Plot->replot();
}
