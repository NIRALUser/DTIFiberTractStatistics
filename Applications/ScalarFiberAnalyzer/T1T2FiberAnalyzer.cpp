#include <iostream>
#include <utility>
#include <functional>
#include <typeinfo>
#include "T1T2FiberAnalyzer.h"

T1T2FiberAnalyzer::T1T2FiberAnalyzer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    para_File = QTGUI_XML_NAME;
    conf_File = QTGUI_CONF_XML_NAME;
    isSync = false;
    isSync_conf = false;
    isHField = false;
    ui->setupUi(this);
    InitializeState();        
}


T1T2FiberAnalyzer::~T1T2FiberAnalyzer()
{
    writer->close();
}

void T1T2FiberAnalyzer::initializeConfPath()
{
    // write to version.py
    // tool::writePyVersion()
    writer = ScriptWriter::getInstance("tool.py","pipeline.py");
    writer->writePreliminary();

    std::string pypath;

    if(std::getenv("TFA_PYTHON"))
        pypath = std::string((std::getenv("TFA_PYTHON")));
    else
        pypath = "";

    if (ui->conf_pypath->text() != ""){
        pypath = ui->conf_pypath->text().toStdString();
    }

    if(pypath == "")
    {
        char* path = std::getenv("PATH");
        std::vector<std::string> checkpath;
        tool::tokenize(path,":",checkpath);
        if(checkpath.empty())
            ErrorReporter::fire("Failed to locate a python compiler in $PATH! Please configure it manually.");
        else
        {
            for(std::vector<std::string>::iterator pit = checkpath.begin(); pit != checkpath.end(); ++pit){
                QDirIterator dirit(QString::fromStdString(*pit));
                while(dirit.hasNext()){
                    QString fn = dirit.next();
                    if(tool::checkExecutable(fn.toStdString()) && fn.toStdString().find("python") != std::string::npos){
                        //qDebug() << fn;
                        if(checkPyVersion(fn.toStdString())){
                            ui->conf_pypath->setText(fn);
                            return;
                        }
                    }
                }
            }

            // failed to find python compiler
            ErrorReporter::fire("Failed to locate a python compiler in $PATH! Please configure it manually.");
            return;
        }

    }
    else if(tool::checkExecutable(pypath)){
        QString str_pypath = QString::fromStdString(pypath);
        // to-do: codes need cleanup here. Python version should be checked first before populating it to gui
        ui->conf_pypath->setText(str_pypath);
    }
    else{
        ErrorReporter::fire("Given python path is not executable!");
        return;
    }

    std::string vc = ui->conf_pypath->text().toStdString();
    tool::checkNewLine(vc);
    if(!checkPyVersion(vc)){
        ErrorReporter::fire("Given executable is unsupported, or python version is below minimum requirement (2.5.0)!");
        ui->conf_pypath->clear();
    }


    if(ui->conf_FiberProcessPath->text() != "")
    {
       if(!tool::checkExecutable(ui->conf_FiberProcessPath->text().toStdString()))
       {
        ErrorReporter::fire("Given FiberProcess path is not executable!");
       }

    }else{
        char* fp_path = std::getenv("TFA_FIBERPROCESS");
        if(fp_path && !(tool::checkExecutable(fp_path))){
            ErrorReporter::fire("Given FiberProcess path is not executable!");
        }
    }
}

// private methods

void T1T2FiberAnalyzer::InitializeState()
{
    SetEventTriggers();

    ui->tabWidget->setCurrentIndex(0);
    ui->para_T12MapInputText->setReadOnly(true);
    ui->para_DTIdefInputText->setReadOnly(true);

    m_gui = new para_Model_T1T2FiberAnalyzer();
    s_gui = new para_Save_T1T2FiberAnalyzer();
    l_gui = new para_Load_T1T2FiberAnalyzer();
    m_gui_conf = new conf_Model_T1T2FiberAnalyzer();
    s_gui_conf = new conf_Save_T1T2FiberAnalyzer();
    l_gui_conf = new conf_Load_T1T2FiberAnalyzer();

    atlas = NULL;
    tracts = NULL;

    // default path of file dialog
    if(tool::checkDirExist(DEFAULT_DIR))
        DEFAULT_PATH = new QString(DEFAULT_DIR);
    else
        DEFAULT_PATH = new QString("./");
    QString app_dir(QCoreApplication::applicationDirPath() + '/' + QTGUI_XML_NAME);
    QString app_dir_conf(QCoreApplication::applicationDirPath() + '/' + QTGUI_CONF_XML_NAME);
    // load value from QtGUI xml if data was previously stored
    QFileInfo checkXML(app_dir);
    QFileInfo checkXML_CONF(app_dir_conf);

    if(checkXML.exists() && checkXML.isFile()){
        isSync = true;
        l_gui->load(*m_gui,app_dir.toStdString());
        SyncToUI();
    }

    if(checkXML_CONF.exists() && checkXML_CONF.isFile()){
        isSync_conf = true;
        l_gui_conf->load(*m_gui_conf,app_dir_conf.toStdString());
        SyncToUI_Conf();
    }


}

void T1T2FiberAnalyzer::SetEventTriggers(){
   // set/check header selection
    QObject::connect(ui->para_T12MapInputText,SIGNAL(textChanged(QString)),this,SLOT(T12extractHeaders()));
    QObject::connect(ui->para_DTIdefInputText,SIGNAL(textChanged(QString)),this,SLOT(DTIextractHeaders()));
    QObject::connect(ui->para_DTIComboPath,SIGNAL(currentIndexChanged(int)),this,SLOT(checkHeaderSelection()));
    QObject::connect(ui->para_DTIComboSID,SIGNAL(currentIndexChanged(int)),this,SLOT(checkHeaderSelection()));
    QObject::connect(ui->para_T12ComboPath,SIGNAL(currentIndexChanged(int)),this,SLOT(checkHeaderSelection()));
    QObject::connect(ui->para_T12ComboSID,SIGNAL(currentIndexChanged(int)),this,SLOT(checkHeaderSelection()));

    // menu actions
    QObject::connect(ui->actionExit,SIGNAL(triggered()),this,SLOT(close()));
    QObject::connect(ui->actionSave,SIGNAL(triggered()),this,SLOT(savePara()));
    QObject::connect(ui->actionLoad,SIGNAL(triggered()),this,SLOT(loadPara()));
    QObject::connect(ui->actionSave_Configuration,SIGNAL(triggered()),this,SLOT(saveConf()));
    QObject::connect(ui->actionLoad_Configuration,SIGNAL(triggered()),this,SLOT(loadConf()));

}

bool T1T2FiberAnalyzer::checkPyVersion(std::string path)
{
    std::string cmd = path+" "+ writer->getToolScriptName().toStdString();
    std::string result = tool::syscall(cmd.c_str());
    // removing newline character
    tool::checkNewLine(result);

    if(result == "true")
        return true;
    else
        return false;
}

bool T1T2FiberAnalyzer::checkRunCondition()
{
    if(!ui->RunBtn->isEnabled() && atlas && tracts && ui->para_output_dir->text() != ""){
        ui->RunBtn->setEnabled(true);
        return true;
    }
    return false;
}

void T1T2FiberAnalyzer::SyncToModel()
{
    m_gui->setpara_T12MapInputText(ui->para_T12MapInputText->text());
    m_gui->setpara_T12ComboPath(ui->para_T12ComboPath->currentText());
    m_gui->setpara_T12ComboSID(ui->para_T12ComboSID->currentText());
    m_gui->setpara_DTIdefInputText(ui->para_DTIdefInputText->text());
    m_gui->setpara_DTIComboPath(ui->para_DTIComboPath->currentText());
    m_gui->setpara_DTIComboSID(ui->para_DTIComboSID->currentText());
    m_gui->setpara_DTIFiber_Path(ui->para_DTIFiber_Path->text());
    m_gui->setpara_output_dir(ui->para_output_dir->text());
    m_gui->setpara_scalarname(ui->para_scalarname->text());


    if(atlas)
    {
        m_gui->setpara_CSVMatchTable(SyncFromAtlasTableView());
    }

    if(tracts)
    {
        m_gui->setpara_Fiber_Tracts_Table(SyncFromTractsTableView());
    }


}

void T1T2FiberAnalyzer::SyncToModel_Conf()
{
    m_gui_conf->setconf_pypath(ui->conf_pypath->text());
    m_gui_conf->setconf_FiberProcessPath(ui->conf_FiberProcessPath->text());
    m_gui_conf->setconf_DTIStatPath(ui->conf_DTIStatPath->text());
}

void T1T2FiberAnalyzer::SyncToUI()
{
    ui->para_T12MapInputText->setText(m_gui->getpara_T12MapInputText());
    ui->para_DTIdefInputText->setText(m_gui->getpara_DTIdefInputText());
    ui->para_DTIFiber_Path->setText(m_gui->getpara_DTIFiber_Path());
    ui->para_output_dir->setText(m_gui->getpara_output_dir());
    ui->para_scalarname->setText(m_gui->getpara_scalarname());

    int tid = ui->para_T12ComboPath->findText(m_gui->getpara_T12ComboPath());
    if(tid >= 0){
        ui->para_T12ComboPath->setCurrentIndex(tid);
    }
    int tsid = ui->para_T12ComboSID->findText(m_gui->getpara_T12ComboSID());
    if(tsid >= 0){
        ui->para_T12ComboSID->setCurrentIndex(tsid);
    }
    int did = ui->para_DTIComboPath->findText(m_gui->getpara_DTIComboPath());
    if(did >= 0){
        ui->para_DTIComboPath->setCurrentIndex(did);
    }
    int dsid = ui->para_DTIComboSID->findText(m_gui->getpara_DTIComboSID());
    if(dsid >= 0){
        ui->para_DTIComboSID->setCurrentIndex(dsid);
    }

    if(m_gui->getpara_DTIFiber_Path() != ""){
        PopulateTractsTable(m_gui->getpara_DTIFiber_Path());
    }

    if(checkHeaderSelection()){
        PopulateAtlasTable();
    }

    /* synchronization of table model only occurs after match button is clicked
    */

}

void T1T2FiberAnalyzer::SyncToUI_Conf()
{
    ui->conf_pypath->setText(m_gui_conf->getconf_pypath());
    ui->conf_FiberProcessPath->setText(m_gui_conf->getconf_FiberProcessPath());
    ui->conf_DTIStatPath->setText(m_gui_conf->getconf_DTIStatPath());
}

// issue: loading does not verify if header names and file names are consistent
void T1T2FiberAnalyzer::SyncToAtlasTableView()
{
    std::vector<std::vector<QString> > m = m_gui->getpara_CSVMatchTable();
    for(std::vector<std::vector<QString> >::iterator it1 = m.begin(); it1 != m.end(); ++it1){
        // integrity check
        if (it1->size() != 2)
            continue;
        if(it1->at(0) == QString("false")){
            int found = atlas->findData(it1->at(1));
            if(found >= 0)
                atlas->resetModel(Qt::Unchecked,found);
        }
    }
}

// issue: match subjectID, or match everything?
// currently matches only the subjectID
std::vector<std::vector<QString> > T1T2FiberAnalyzer::SyncFromAtlasTableView()
{
    std::vector<std::vector<QString> > data;
    for(unsigned int i =0; i < atlas->getDataSize(); i++){
        std::vector<QString> row;
        if(atlas->getCheckState(i))
            row.push_back("true");
        else
            row.push_back("false");
        row.push_back(atlas->getData(i).subjectID);
        data.push_back(row);
    }

    return data;
}

void T1T2FiberAnalyzer::SyncToTractsTableView()
{
    std::vector<std::vector<QString> > m = m_gui->getpara_Fiber_Tracts_Table();
    for(std::vector<std::vector<QString> >::iterator it1 = m.begin(); it1 != m.end(); ++it1){
        // integrity check
        if (it1->size() != 2)
            continue;
        if(it1->at(0) == QString("false")){
            int found = tracts->findData(it1->at(1));
            if(found >= 0)
                tracts->resetModel(Qt::Unchecked,found);
        }
    }

    
}

std::vector<std::vector<QString> > T1T2FiberAnalyzer::SyncFromTractsTableView()
{
    std::vector<std::vector<QString> > data;
    for(unsigned int i =0; i < tracts->getDataSize(); i++){
        std::vector<QString> row;
        if(tracts->getCheckState(i))
            row.push_back("true");
        else
            row.push_back("false");
        row.push_back(tracts->getData(i).file_path);
        data.push_back(row);
    }

    return data;
}



void T1T2FiberAnalyzer::T12extractHeaders()
{
    QString fileName = ui->para_T12MapInputText->text();
    // parse headers of csv file and populate into combo boxes
    T12headers.clear();
    try{
        tool::parseCSV(fileName.toStdString(),T12headers);
        QComboBox* cb1 = ui->para_T12ComboPath;
        QComboBox* cb2 = ui->para_T12ComboSID;
        cb1->clear();
        cb2->clear();
        for(std::vector<std::string>::iterator it = T12headers.begin(); it != T12headers.end(); ++it){
            QString si = QString::fromStdString(*it);
            cb1->addItem(si);
            cb2->addItem(si);
        }

    }
    catch(csvparser::ReadError e){
            ErrorReporter::fire(string(e.what()));
            return;
        }
}

void T1T2FiberAnalyzer::DTIextractHeaders()
{
    QString fileName = ui->para_DTIdefInputText->text();
    // parse headers of csv file and populate into combo boxes
    DTIheaders.clear();
    try{
        tool::parseCSV(fileName.toStdString(),DTIheaders);
        QComboBox* cb1 = ui->para_DTIComboPath;
        QComboBox* cb2 = ui->para_DTIComboSID;
        cb1->clear();
        cb2->clear();
        for(std::vector<std::string>::iterator it = DTIheaders.begin(); it != DTIheaders.end(); ++it){
            QString si = QString::fromStdString(*it);
            cb1->addItem(si);
            cb2->addItem(si);
        }

    }
    catch(csvparser::ReadError e){
            ErrorReporter::fire(string(e.what()));
            return;
        }
}

QMessageBox::StandardButton T1T2FiberAnalyzer::SaveGuiValue()
{
    SyncToModel();
    QMessageBox::StandardButton rtn = QMessageBox::question(this, "","Save Current Values to Configuration File "+ para_File + " and " + conf_File +"?",
                                                            QMessageBox::Yes|QMessageBox::No| QMessageBox::Cancel);
    if(rtn == QMessageBox::Yes || rtn == QMessageBox::NoButton){
        s_gui->save(*m_gui,para_File.toStdString());
        s_gui_conf->save(*m_gui_conf,conf_File.toStdString());
    }

    return rtn;
}

// protected methods

void T1T2FiberAnalyzer::contextMenuEvent(QContextMenuEvent *event)
{
    // to-do: right-click menu (copy&paste, etc.)
}

void T1T2FiberAnalyzer::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton rtn = SaveGuiValue();

    if(rtn == QMessageBox::Cancel || rtn == QMessageBox::Escape){
        event->ignore();
    }else
        event->accept();
}

// slots

void T1T2FiberAnalyzer::savePara()
{

    SyncToModel();
    QString m_DialogDir = para_File;
    QString filename = QFileDialog::getSaveFileName( this , "Save Changed Value to parameter save File" , m_DialogDir , "XML files (*.xml)" );
    if( filename != "" )
    {
        QFileInfo fi( filename ) ;
        m_DialogDir = fi.dir().absolutePath() ;
        s_gui->save(*m_gui,filename.toStdString());
        para_File = fi.fileName();
    }

}

void T1T2FiberAnalyzer::saveConf(){
    SyncToModel_Conf();
    QString m_DialogDir = conf_File;
    QString filename = QFileDialog::getSaveFileName( this , "Save Changed Value to configuration save File" , m_DialogDir , "XML files (*.xml)" );
    if( filename != "" )
    {
        QFileInfo fi( filename ) ;
        m_DialogDir = fi.dir().absolutePath() ;
        s_gui_conf->save(*m_gui_conf,filename.toStdString());
        conf_File = fi.fileName();
    }
}

void T1T2FiberAnalyzer::loadPara()
{
    // to-do: check for corner cases

    QString m_DialogDir = para_File;
    QString filename = QFileDialog::getOpenFileName(this,"Load parameter save File", m_DialogDir, "XML files (*.xml)");
    if(filename != ""){
        isSync = true;
        l_gui->load(*m_gui,filename.toStdString());
        SyncToUI();
    }
}

void T1T2FiberAnalyzer::loadConf(){
    QString m_DialogDir = conf_File;
    QString filename = QFileDialog::getOpenFileName(this,"Load configuration save File", m_DialogDir, "XML files (*.xml)");
    if(filename != ""){
        isSync_conf = true;
        l_gui_conf->load(*m_gui_conf,filename.toStdString());
        SyncToUI_Conf();
    }
}

bool T1T2FiberAnalyzer::checkHeaderSelection()
{
    QString str1 = ui->para_T12ComboPath->currentText();
    QString str2 = ui->para_T12ComboSID->currentText();
    QString str3 = ui->para_DTIComboPath->currentText();
    QString str4 = ui->para_DTIComboSID->currentText();
    if((!str1.isEmpty()) && (!str2.isEmpty()) && (!str3.isEmpty()) && (!str4.isEmpty())){
        ui->MatchResultBtn->setEnabled(true);
        ui->MatchTableSelectAll->setEnabled(true);
        ui->MatchTableDeselectAll->setEnabled(true);
        return true;
    }
    return false;
}

void T1T2FiberAnalyzer::on_T12MapInputBtn_clicked()
{
    /* setting path in edit line*/
    QString fileName = QFileDialog::getOpenFileName(this,tr("Select File"),*DEFAULT_PATH);
    if(fileName == NULL) return;
    ui->para_T12MapInputText->setText(fileName);
}

void T1T2FiberAnalyzer::on_DTIdefInputBtn_clicked()
{
    /* setting path in edit line*/
    QString fileName = QFileDialog::getOpenFileName(this,tr("Select File"),*DEFAULT_PATH);
    if(fileName == NULL) return;
    ui->para_DTIdefInputText->setText(fileName);
}

void T1T2FiberAnalyzer::on_DTIAtlasPathBtn_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this,tr("Open Directory"),*DEFAULT_PATH,
                                                    QFileDialog::ShowDirsOnly);
    if(dir == NULL) return;
    ui->para_DTIFiber_Path->setText(dir);
    PopulateTractsTable(dir);

}

bool T1T2FiberAnalyzer::PopulateAtlasTable(){
    T12TractData.clear();
    DTITractData.clear();
    std::string str1 = ui->para_T12ComboPath->currentText().toStdString();
    str1 = tool::trim(str1);
    std::string str2 = ui->para_T12ComboSID->currentText().toStdString();
    str2 = tool::trim(str2);
    std::string str3 = ui->para_DTIComboPath->currentText().toStdString();
    str3 = tool::trim(str3);
    std::string str4 = ui->para_DTIComboSID->currentText().toStdString();
    str4 = tool::trim(str4);

    QString T12_csv = ui->para_T12MapInputText->text();
    QString DTI_csv = ui->para_DTIdefInputText->text();

    try{
        tool::parseMapContent(T12_csv,T12TractData,str1,str2);
        tool::parseMapContent(DTI_csv,DTITractData,str3,str4);
        atlas = new AtlasModel(0,T12TractData,DTITractData);
        QItemSelectionModel *m = ui->para_CSVMatchTable->selectionModel();

        // synchronize states if loader exists
        if(isSync)
            SyncToAtlasTableView();

        ui->para_CSVMatchTable->setModel(atlas);
        ui->para_CSVMatchTable->horizontalHeader()->setStretchLastSection(true);
        if(m!=NULL) delete m;

    }catch(csvparser::ReadError e){
        ErrorReporter::fire(string(e.what()));
        return false;
    }

    checkRunCondition();
    return true;
}

bool T1T2FiberAnalyzer::PopulateTractsTable(QString str){
    QDir dir(str);
    if (!dir.exists())
        return false;
    std::vector<tool::TractData> filelist;
    tool::getvtkfiles(str.toStdString(),filelist);
    tracts = new FiberTractModel(0,filelist);
    if(isSync)
        SyncToTractsTableView();
    QItemSelectionModel *m =ui->para_CSVMatchTable->selectionModel();
    ui->para_Fiber_Tracts_Table->setModel(tracts);
    ui->para_Fiber_Tracts_Table->horizontalHeader()->setStretchLastSection(true);
    ui->FiberTableSelectAll->setEnabled(true);
    ui->FiberTableDeselectAll->setEnabled(true);
    if(m) delete m;

    checkRunCondition();
    return true;
    //m_gui->setpara_Fiber_Tracts_Table(SyncFromTractsTableView());
}

void T1T2FiberAnalyzer::on_T12BrowseBtn_clicked()
{
    QString dir = ui->para_T12MapInputText->text();
    if(dir == NULL) return;
    std::vector<std::vector<std::string> > csv_results;
    std::vector<std::string> headers;
    if(dir != NULL){
        try{
            csv_results = tool::parseCSV(dir.toStdString(),headers);
        }
        catch(csvparser::ReadError e){
                ErrorReporter::fire(string(e.what()));
                return;
            }
    }
    CSVBrowser *bd = new CSVBrowser();
    bd->show();
    bd->loadTable(csv_results,headers);

}

void T1T2FiberAnalyzer::on_DTIBrowseBtn_clicked()
{
    QString dir = ui->para_DTIdefInputText->text();
    std::vector<std::vector<std::string> > csv_results;
    std::vector<std::string> headers;
    if(dir != NULL){
        try{
            csv_results = tool::parseCSV(dir.toStdString(),headers);
        }
        catch(csvparser::ReadError e){
                ErrorReporter::fire(string(e.what()));
                return;
        }
    }
    CSVBrowser *bd = new CSVBrowser();
    bd->show();
    bd->loadTable(csv_results,headers);
}


void T1T2FiberAnalyzer::on_MatchResultBtn_clicked()
{
    PopulateAtlasTable();
}

void T1T2FiberAnalyzer::on_pyPathBtn_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("Select Python Path"), *DEFAULT_PATH);
    if(filename == NULL) return;
    std::string path = filename.toStdString();
    tool::checkNewLine(path);
    if (!tool::checkExecutable(path))
        ErrorReporter::fire("Provided python path is not executable!");
    else if(!checkPyVersion(path)){
        ErrorReporter::fire("Given executable is unsupported, or python version is below minimum requirement (2.5.0)!");
    }else
        ui->conf_pypath->setText(filename);

}


void T1T2FiberAnalyzer::on_conf_pypath_editingFinished()
{
    QString path = ui->conf_pypath->text();
    if(path == "")
        return;
    if (!tool::checkExecutable(path.toStdString())){
        ErrorReporter::fire("Provided python path is not executable!");
        ui->conf_pypath->clear();
    }
    else if(!checkPyVersion(path.toStdString())){
        ErrorReporter::fire("Given executable is unsupported, or python version is below minimum requirement (2.5.0)!");
        ui->conf_pypath->clear();
    }

}

void T1T2FiberAnalyzer::on_FiberProcessBtn_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("Select Fiber Process Path"), *DEFAULT_PATH);
    if(filename == NULL) return;
    std::string path = filename.toStdString();
    tool::checkNewLine(path);
    if (!tool::checkExecutable(path))
        ErrorReporter::fire("Provided path is not executable!");
    else
        ui->conf_FiberProcessPath->setText(filename);

}

void T1T2FiberAnalyzer::on_DTIStatBtn_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("Select DTI Tract Stat Path"), *DEFAULT_PATH);
    if(filename == NULL) return;
    std::string path = filename.toStdString();
    tool::checkNewLine(path);
    if (!tool::checkExecutable(path))
        ErrorReporter::fire("Provided path is not executable!");
    else
        ui->conf_DTIStatPath->setText(filename);

}

void T1T2FiberAnalyzer::on_MatchTableSelectAll_clicked()
{
    atlas->resetModel(Qt::Checked);
}

void T1T2FiberAnalyzer::on_MatchTableDeselectAll_clicked()
{
    atlas->resetModel(Qt::Unchecked);
}

void T1T2FiberAnalyzer::on_FiberTableSelectAll_clicked()
{
    tracts->resetModel(Qt::Checked);
}

void T1T2FiberAnalyzer::on_FiberTableDeselectAll_clicked()
{
    tracts->resetModel(Qt::Unchecked);
}



void T1T2FiberAnalyzer::on_OutputDirBtn_clicked()
{
    QString filename = QFileDialog::getExistingDirectory(this,tr("Select an output directory"), *DEFAULT_PATH);
    if(filename == NULL) return;
    ui->para_output_dir->setText(filename);
    checkRunCondition();
}

void T1T2FiberAnalyzer::on_RunBtn_clicked()
{
    // first save all parameters and configurations
    s_gui->save(*m_gui,para_File.toStdString());
    s_gui_conf->save(*m_gui_conf,conf_File.toStdString());
    ErrorReporter::friendly_fire("All parameters and configurations are saved to "
                                    + para_File.toStdString() + " and " + conf_File.toStdString());

    std::vector<tool::MapData> data;
    std::vector<tool::TractData> t_data;
    for(unsigned int i =0; i < atlas->getDataSize(); i++){
        if(atlas->getCheckState(i))
            data.push_back(atlas->getData(i));
    }

    for(unsigned int i =0; i < tracts->getDataSize(); i++){
        if(tracts->getCheckState(i))
            t_data.push_back(tracts->getData(i));
    }

    QString abs_out_dir = QFileInfo(ui->para_output_dir->text()).absoluteFilePath();
    QString abs_fiber_dir = QFileInfo(ui->para_DTIFiber_Path->text()).absoluteFilePath();
    QString abs_fiberprocess = QFileInfo(ui->conf_FiberProcessPath->text()).absoluteFilePath();
    QString abs_dtitract = QFileInfo(ui->conf_DTIStatPath->text()).absoluteFilePath();
    bool checkState = false;
    if(ui->TransposeCheckbox->checkState() == Qt::Checked)
        checkState = true;

    if(writer->writeData(abs_out_dir, abs_fiber_dir, abs_fiberprocess, abs_dtitract, isHField,
                         ui->para_scalarname->text(), data, t_data, checkState)){
        // run the script after data has successfully been written
        QProcess p;
        p.setProcessChannelMode(QProcess::MergedChannels);
        QStringList arguments;
        arguments << abs_out_dir+"/pipeline.py";
        p.start(ui->conf_pypath->text(), arguments);
        QMessageBox warning(QMessageBox::Information, "ScalarAnalyzer","Please wait while pipeline is running...");
        warning.setWindowFlags(Qt::FramelessWindowHint);
        warning.setStandardButtons(0);
        warning.open();
        QCoreApplication::processEvents();

        if(!p.waitForFinished(-1))
	  {
            qDebug() << p.errorString();
	    warning.close();
	    ErrorReporter::fire("Process failed to execute with the following error: " + p.errorString().toStdString());
	  }
        else
        {
            QFile file(abs_out_dir + "/log");
	    warning.close();
            if(file.open(QIODevice::WriteOnly)){
                file.write(p.readAll());
                ErrorReporter::friendly_fire("Output is written into " + abs_out_dir.toStdString() + "/log");
                file.close();
            }

            else
            {
                ErrorReporter::fire("failed to write into log file");
            }
        }
    }
    else
    {
        ErrorReporter::fire("Failed to create pipeline script! Maybe you don't have write access at the output path?");
    }

}


void T1T2FiberAnalyzer::on_DFieldRadio_clicked()
{
    isHField = false;
    ui->HFieldRadio->setChecked(false);
}

void T1T2FiberAnalyzer::on_HFieldRadio_clicked()
{
    isHField = true;
    ui->DFieldRadio->setChecked(false);
}
