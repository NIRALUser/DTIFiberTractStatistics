#include "GlobalFunction.h"

/* QT librairies */
#include <QProcess>

/********************************************************************************* 
 * Command Line function
 ********************************************************************************/
bool CommandLine(std::string CSVFilename, std::string parametersfile, bool debug)
{
	//variables
	std::string pathFiberProcess, pathdti_tract_stat, OutputFolder, AtlasFiberDir, parameters, csvfile;
	vstring fibers, SelectedFibers, fibersplane, Selectedfibersplane;
	int DataCol, DefCol, NameCol,NumberOfParameters;
	DataCol = -1;
	DefCol = -1;
	NameCol = -1;
	bool transposeColRow;
	
	//read the parameters from the file
	if(!ReadParametersFromFile(parametersfile,csvfile,AtlasFiberDir,OutputFolder,parameters,DataCol, DefCol,
		 NameCol,SelectedFibers, transposeColRow))
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
		std::cout<<"NameCol :"<<NameCol<<std::endl;
		std::cout<<"Selected Fibers :"<<std::endl;
		for(unsigned int i=0;i<SelectedFibers.size();i++)
			std::cout<<SelectedFibers[i]<<std::endl;
		std::cout<<"Transpose col and row :"<<transposeColRow<<std::endl;
	}
	
	//create the CSV file
	if(CSVFilename.compare("no file")!=0)
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
	
	// Read atlas directory
	ReadFiberNameInAtlasDirectory(fibers,fibersplane, AtlasFiberDir);
	
	int plane = -1;
	//Select the plane of the selected fiber
	for(unsigned int i=0;i<SelectedFibers.size();i++)
	{
		/* Check the plane and add/erase it */
		plane = PlaneAssociatedToFiber(SelectedFibers[i],1,fibersplane,Selectedfibersplane);
							
		if(plane!=-1)
			Selectedfibersplane.push_back(fibersplane[plane]);
	}
	
	//if in the parameter file there is a selected fibers, compute with it, else compute with all the fibers
	if(!SelectedFibers.empty())
	{
		//Find path for fiberprocess
		pathFiberProcess= itksys::SystemTools::FindProgram("fiberprocess");
		// enter the path for fiberprocess if it didn't find it
		if(pathFiberProcess.empty()==true)
		{
			std::cout<<"Give the path for fiberprocess : "<<std::endl;
			std::cin>>pathFiberProcess;
		}
		//Call fiberprocess
		Applyfiberprocess(CSVFile, pathFiberProcess, AtlasFiberDir, OutputFolder, DataCol, DefCol,
								NameCol, SelectedFibers,parameters,transposeColRow,true);
		
		/* Looking for dti_tract_stat */
		pathdti_tract_stat= itksys::SystemTools::FindProgram("dtitractstatCLP");
		// enter the path for fiberprocess if it didn't find it
		if(pathdti_tract_stat.empty()==true)
		{
			std::cout<<"Give the path for dtitractstat : "<<std::endl;
			std::cin>>pathdti_tract_stat;
		}
		//Call dti_tract_stat
		Applydti_tract_stat(CSVFile, pathdti_tract_stat, AtlasFiberDir, OutputFolder, SelectedFibers,
								  Selectedfibersplane,parameters, DataCol, DefCol, NameCol, transposeColRow,
								  true);
	}
	else
	{
		//Find path for fiberprocess
		pathFiberProcess= itksys::SystemTools::FindProgram("fiberprocess");
		// enter the path for fiberprocess if it didn't find it
		if(pathFiberProcess.empty()==true)
		{
			std::cout<<"Give the path for fiberprocess : "<<std::endl;
			std::cin>>pathFiberProcess;
		}
		//Call fiberprocess
		Applyfiberprocess(CSVFile, pathFiberProcess, AtlasFiberDir, OutputFolder, DataCol, DefCol,
								NameCol, fibers,parameters,transposeColRow,true);
		
		/* Looking for dti_tract_stat */
		pathdti_tract_stat= itksys::SystemTools::FindProgram("dtitractstatCLP");
		// enter the path for fiberprocess if it didn't find it
		if(pathdti_tract_stat.empty()==true)
		{
			std::cout<<"Give the path for dtitractstat : "<<std::endl;
			std::cin>>pathdti_tract_stat;
		}
		//Call dti_tract_stat
		Applydti_tract_stat(CSVFile, pathdti_tract_stat, AtlasFiberDir, OutputFolder, fibers,
								  fibersplane,parameters, DataCol, DefCol, NameCol,transposeColRow,true);
	}
	
	//calcul the number of parameters
	NumberOfParameters = CalculNumberOfProfileParam(parameters);
	//Gather fibers profile information 
	GatheringFiberProfile(CSVFile, OutputFolder, NumberOfParameters, DataCol, NameCol,transposeColRow);
	
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
 * Call fiberprocess for every data/deformation and for every fiber in the atlas
 ********************************************************************************/
bool Applyfiberprocess(CSVClass* CSV,
							  std::string pathFiberProcess,
							  std::string AtlasFiberDir,
							  std::string OutputFolder,
							  int DataCol,
							  int DefCol,
							  int NameCol,
							  vstring fibers,
							  std::string parameters,
							  bool transposeColRow,
							  bool nogui,
							  QWidget* parent)
{
	int col=-1;
	std::vector<bool> skipdata; //0 is skip, 1 alldata, initializate to false,2  cancel
	skipdata.push_back(false);
	skipdata.push_back(false);
	skipdata.push_back(false);
	bool ExistedFile,DataAdded;
	std::string outputname, name_of_fiber,header, namecase, nameoffile,outputcasefolder;
	
	/* create the output folder where there will be all of the case output fibers and informations */
	outputcasefolder = "Cases";
	CreateDirectoryForData(OutputFolder,outputcasefolder);
	
	/* Loop for all the fibers */
	for(unsigned int j=0;j<fibers.size();j++)
	{
		DataAdded = false;
		name_of_fiber = takeoffExtension(fibers[j]);
		//Add the name of the fiber at the place 0 for the header
		header = name_of_fiber + ".vtk";
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
							if(CallFiberProcess(pathFiberProcess, AtlasFiberDir,
								outputname, 
								(*CSV->getData())[row][DataCol],
								(*CSV->getData())[row][DefCol],fibers[j],
								namecase)==0)
							{
								//Check if the output exist
								if(FileExisted(outputname))
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
								(*CSV->getData())[row][DataCol],"no",fibers[j],
								namecase)==0)
							{
								//Check if the output exist
								if(FileExisted(outputname))
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
	
	/* save the parameters by default */
	filename = OutputFolder + "/" + takeoffExtension(takeoffPath(CSV->getFilename()))
			+"_parameters.txt";
	saveparam(filename, CSV->getFilename(), DataCol, DefCol, NameCol, OutputFolder,AtlasFiberDir, fibers,
				 parameters, transposeColRow);
	
	return true;
}
/* Return the column with the header name or -1 if there is no column*/
int HeaderExisted(CSVClass* CSV, std::string header)
{
	//Find the fibername colomn if exist just in the first row = header
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
/* Return true if there filename exists */
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
		Box.setInformativeText("Do you want to skip the computation?");
		Box.setStandardButtons(QMessageBox::Yes | QMessageBox::No |
				QMessageBox::YesToAll | QMessageBox::NoToAll | QMessageBox::Cancel);
		Box.setDefaultButton(QMessageBox::Yes);
		int ret = Box.exec();
		switch (ret) {
			case QMessageBox::Yes:
				skip = true;
				break;
			case QMessageBox::No:
				skip = false;
				break;
			case QMessageBox::YesToAll:
				alldata = true;
				skip = true;
				break;
			case QMessageBox::NoToAll:
				alldata = true;
				skip = false;
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
		std::cout<<"The file "<< (nameoffile).c_str()<<" already exists. Skip computation? Enter 'yes' or 'no' or  'yes_all' for yes to all or 'no_all' for no to all."<<std::endl;
		std::cin>>answer;
		
		if(answer.compare("yes")==0)
			skip = true;
		else if(answer.compare("no")==0)
			skip = false;
		else if(answer.compare("yes_all")==0)
		{
			alldata = true;
			skip = true;
		}
		else if(answer.compare("no_all")==0)
		{
			alldata = true;
			skip = false;
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
							std::string Fiber,
							std::string nameofcase)
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
		arguments.append("--fiber_file " +qs );
		//Data
		qs =  Data.c_str();
		arguments.append("--tensor_volume " +qs);
		//Deformation field
		if(DeformationField.compare("no")!=0 && DeformationField.compare("no deformation")!=0)
		{
			qs =  DeformationField.c_str();
			arguments.append("--h_field " +qs);
		}
		//output
		qs = outputname.c_str();
		arguments.append("--fiber_output " + qs);
		//verbose
		arguments.append("--verbose");
		std::cout<<"Command Line :  "<< (arguments.join(" ")).toStdString() <<std::endl;
		state = process->execute( pathFiberProcess.c_str(), arguments);
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
	bool noplane;
	itksys::Directory AtlasDirectory;
	AtlasDirectory.Load(AtlasFiberDir.c_str());
	unsigned long NumberOfFiles = AtlasDirectory.GetNumberOfFilesInDirectory(AtlasFiberDir.c_str());
	std::string planename,fibername, extensionfiber, extensionfiberplane;
	
	//save the name of the Fibers Files in an vstring
	for( unsigned long i=0;i<NumberOfFiles;i++)
	{
		noplane = false;
		extensionfiber = ExtensionofFile(AtlasDirectory.GetFile(i));
		if(extensionfiber.compare("vtk")==0 || extensionfiber.compare("fib")==0)
		{
			fibers.push_back(AtlasDirectory.GetFile(i));
			fibername = takeoffExtension(AtlasDirectory.GetFile(i));
			//Looking if there is a plane file with the fibers
			for(unsigned long j=0;j<NumberOfFiles;j++)
			{
				planename = takeoffExtension(AtlasDirectory.GetFile(j));
				if(planename.compare(fibername)==0)
				{
					extensionfiberplane = ExtensionofFile(AtlasDirectory.GetFile(j));
					if(extensionfiberplane.compare("fvb")==0 ||
									 extensionfiberplane.compare("fvp")==0)
					{
						fibersplane.push_back(AtlasDirectory.GetFile(j));
						noplane = true;
					}
				}
			}
			if(!noplane)
				fibersplane.push_back(fibername + "_auto");
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
	inputfile = filename.substr(0,filename.find_first_of("."));
	
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
 * Call fiberprocess for every data/deformation and for every fiber in the atlas
 ********************************************************************************/
bool Applydti_tract_stat(CSVClass* CSV, std::string pathdti_tract_stat, std::string AtlasDirectory,
								 std::string OutputFolder, vstring fibers, vstring fibersplane, std::string parameters,
								 int DataCol, int DefCol, int NameCol, bool transposeColRow, bool nogui, QWidget *parent)
{
	int col=-1;
	bool ExistedFile,DataAdded;
	std::vector<bool> skipdata; //0 is skip, 1 alldata, initializate to false, 2  cancel
	skipdata.push_back(false);
	skipdata.push_back(false);
	std::string outputname, name_of_fiber, header, namecase, nameoffile;
	CreateDirectoryForData(OutputFolder,"Fibers");
	
	/* Loop for all the fibers */
	for(unsigned int j=0;j<fibers.size();j++)
	{
		DataAdded = false;
		name_of_fiber = takeoffExtension(fibers[j]);
		//Column where there is the name of the fiber
		//header for the new column
		header = name_of_fiber + ".fvp";
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
					"_" + name_of_fiber + ".fvp";
			nameoffile = namecase + "_" + name_of_fiber + ".fvp";
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
				if(!DataExistedInFiberColumn(CSV,row,col,outputname))
				{
					CSV->AddData(outputname,row,col);
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
						std::string input_fiber = (*CSV->getData())[row][fibercol];
						/* If dti_tract_stat worked */
						if(Calldti_tract_stat(pathdti_tract_stat, AtlasDirectory,
							input_fiber, outputname, fibersplane[j],
							parameters)==0)
						{
							if(FileExisted(outputname))
							{
								CSV->AddData(outputname,row,col);
							}
						}
						else
							std::cout<<"Fail during dti_tract_stat!"<< std::endl;
					}
				}
				else
					CSV->AddData("no",row,col);
			}
		}
		
		std::string inputname=AtlasDirectory+"/"+fibers[j];
		outputname=OutputFolder+"/Fibers/"+header;
		if(FileExisted(outputname))
		{
			ExistedFile = true;
			if(!skipdata[1])
				skipdata = MessageExistedFile(nogui, header, parent);
		}
		if(skipdata[2] == true)
			return false;
		else if(!skipdata[0] || !ExistedFile)
		{
			if(Calldti_tract_stat(pathdti_tract_stat, AtlasDirectory, inputname, outputname, fibersplane[j], parameters)!=0)
				std::cout<<"Fail during dti_tract_stat!"<<std::endl;
		}
		
	}
	
	//save
	std::string filename;
	filename = takeoffPath(CSV->getFilename());
	filename = takeoffExtension(filename);
	filename = OutputFolder+ "/" + filename + "_computed.csv";
	CSV->SaveFile(filename);
	
	/* save the parameters */
	filename = OutputFolder + "/" + takeoffExtension(takeoffPath(CSV->getFilename()))
			+"_parameters.txt";
	saveparam(filename, CSV->getFilename(), DataCol, DefCol, NameCol, OutputFolder, AtlasDirectory, fibers,
				 parameters, transposeColRow);
	
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
							  std::string parameter)
{
	int state=0;
	QProcess *process= new QProcess();
	QStringList arguments;
	
	std::cout<<"Compute dti_tract_stat..."<< std::endl;
	
	if(Input_fiber_file.compare("no fiber")!=0 && Input_fiber_file.compare("no")!=0)
	{
		//Input fiber file
		QString qs = Input_fiber_file.c_str();
		arguments.append(QString("--input_fiber_file ") + qs);
		//Output fiber file
		qs =  Output_fiber_file.c_str();
		arguments.append(QString("--ouput_stats_file ") + qs);
		//Plane
		if((plane.substr(plane.find_last_of("_")+1,plane.size()-plane.find_last_of("_")+1))
				  .compare("auto")!=0)
		{
			qs = AtlasDirectory.c_str();
			qs.append("/");
			qs.append(plane.c_str());
			arguments.append(QString("--plane_file ") + qs);
		}
		//parameter
		if(parameter.compare("")!=0)
		{
			qs = parameter.c_str();
			arguments.append(QString("--parameter_list ") + qs);
		}
		std::cout<<"Command Line :  "<< (arguments.join(" ")).toStdString() <<std::endl;
		
		state = process->execute( pathdti_tract_stat.c_str(), arguments);
	}
	else
		std::cout<<"Warning: No input fiber!"<<std::endl;
	
	return state;
}


/********************************************************************************* 
 * Gather the fiber profile information from .fvp file :
 * It stocks the data in an vector(case) of vector(line) of vstring (data)
 ********************************************************************************/
void GatheringFiberProfile(CSVClass* CSV, std::string OutputFolder, int NumberOfParameters, int DataCol, int NameCol,
									bool transposeColRow)
{
	// variables
	std::string fibername,filename,outputprofilefolder;
	std::vector<v2string> FiberProfiles;
	v2string profiles;
	
	/* create the output folder where there will be all of the gathering data */
	outputprofilefolder = "FiberProfiles";
	CreateDirectoryForData(OutputFolder,outputprofilefolder);
	
	OutputFolder = OutputFolder + "/" + outputprofilefolder;
	
	/* look for the header .fvp and create the folder for each fiber.fvp with the name of the fiber */
	for(unsigned int col=0;col<CSV->getColSize(0);col++)
	{
		FiberProfiles.clear();
		if(ExtensionofFile((*CSV->getData())[0][col]).compare("fvp")==0)
		{
			fibername = takeoffExtension((*CSV->getData())[0][col]);
			if(CreateDirectoryForData(OutputFolder,fibername))
			{
				/* read the data of every case and put it in the vector */
				for(unsigned int row=1;row<CSV->getRowSize();row++)
				{
					profiles.clear();
					if(ReadProfileInformation(fibername,(*CSV->getData())[row][col],profiles))
					{
						//read the profile information from the file 
						//and put it for the fiber and data
						FiberProfiles.push_back(profiles);
					}
				}
				
				if(!FiberProfiles.empty())
				{
					/* write the information in a file for each parameters */
					for(int i=0;i<NumberOfParameters;i++)
					{
						if(FiberProfiles[0][0].size()> static_cast<unsigned int>(i+2))
						{
							//the two first columns are Arclenght and fiberpoints
							filename = OutputFolder + "/" + fibername + "/" 
									+ FiberProfiles[0][0][i+2]
									+ "_" + fibername + ".csv";
							WriteProfile(CSV, filename,FiberProfiles,DataCol,
											 NameCol,i+2,transposeColRow);
						}
					}
				}
				else
					std::cout<<"Read information from the fvp files failed for"<<
							" the fiber "<<fibername<<"!"<<std::endl;
			}
		}
	}
}


/********************************************************************************* 
 * Read profile information from .fvp file
 ********************************************************************************/
bool ReadProfileInformation(std::string fibername,std::string filename, v2string &Profiles)
{
	bool firstword;
	std::ifstream file(filename.c_str(), std::ios::in); //open the file in reading
	
	if(file)
	{
		std::string line;
		//read the data only at the seventh line
		for(int i=0;i<6;i++)
		{
			getline(file, line);
		}
		
		while(getline(file, line))
		{
			firstword =true;
			//create vstring
			vstring column;
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
					line = line.substr(line.find_first_of(",")+1,
											 line.size()-line.find_first_of(",")+1);
				}
				else
				{
					column.push_back(line);
					it = line.end();
				}
			}
			Profiles.push_back(column);
		}
		file.close();
		
		return true;
	}
	else
		std::cout<<"Error opening the file from fiber : "<<fibername<<" "<<filename<<"!"<<std::endl;
	
	return false;
}

/********************************************************************************* 
 * Write profile information
 ********************************************************************************/
void WriteProfile(CSVClass* CSV, std::string filename, std::vector<v2string> FiberProfiles, int DataCol, int NameCol, 
						int ParamCol,bool transposeColRow)
{
	/* Open a file in writing */
	std::ofstream bfile(filename.c_str(), std::ios::out);
	
	if(bfile)
	{
		if(!transposeColRow)
		{
			//first line header : arc lenght
			bfile << "Data vs Arc_Length,";
			for(unsigned int i=1;i<FiberProfiles[0].size();i++)
			{
				if(i!=(FiberProfiles[0].size()-1))
					bfile << FiberProfiles[0][i][0] << "," ;
				else
					bfile << FiberProfiles[0][i][0];
			}
			bfile << std::endl;
			
			//Take the fibers profiles information (FA for example) for each data; one data = one line 
			//in the csv
			for(unsigned int data=0;data<FiberProfiles.size();data++)
			{
				//give the name of the case
				if(NameCol == -1)
					bfile<<takeoffPath(takeoffExtension((*CSV->getData())[data+1][DataCol]))<<",";
				else if((*CSV->getData())[data+1][NameCol].compare("no name")==0)
					bfile<<takeoffPath(takeoffExtension((*CSV->getData())[data+1][DataCol]))<<",";
				else
					bfile << (*CSV->getData())[data+1][NameCol]<< "," ;
					
				for(unsigned int line=1;line<FiberProfiles[data].size();line++)
				{
					/* Write line per line from the fiberprofiles vector */
					if((unsigned int)ParamCol<FiberProfiles[data][line].size())
					{
						if(line!=(FiberProfiles[data].size()-1))
						{
							bfile << FiberProfiles[data][line][ParamCol] << "," ;
						}
						else
						{
							bfile << FiberProfiles[data][line][ParamCol];
						}
					}
				}
				bfile << std::endl;
			}
		}
		else
		{
			//first line header : case name
			bfile << "Arc_Length vs Data,";
			for(unsigned int i=0;i<FiberProfiles.size();i++)
			{
				//give the name of the case
				if(NameCol == -1)
					bfile<<takeoffPath(takeoffExtension((*CSV->getData())[i+1][DataCol]))<<",";
				else if((*CSV->getData())[i+1][NameCol].compare("no name")==0)
					bfile<<takeoffPath(takeoffExtension((*CSV->getData())[i+1][DataCol]))<<",";
				else
					bfile << (*CSV->getData())[i+1][NameCol]<< "," ;
			}
			bfile << std::endl;
			
			//Take the fibers profiles information (FA for example) for each data; one arc lenght
			//value = one line in the csv
			for(unsigned int line=1;line<FiberProfiles[0].size();line++)
			{
				//valeur de l'arc lenght
				bfile << FiberProfiles[0][line][0]<< "," ;
				
				for(unsigned int data=0;data<FiberProfiles.size();data++)
				{
					/* Write line per line from the fiberprofiles vector */
					if(data!=(FiberProfiles.size()-1))
						bfile << FiberProfiles[data][line][ParamCol] << "," ;
					else
						bfile << FiberProfiles[data][line][ParamCol];
				}
				bfile << std::endl;
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
void saveparam(std::string filename,std::string CSVFilename, int DataCol, int DefCol, int NameCol,
					std::string OutputFolder, std::string AtlasFiberFolder, vstring FiberSelectedname, 
					std::string parameters, bool transposeColRow)
{
	std::string ListOfFiber = "";
	
	std::ofstream savefile(filename.c_str(), std::ios::out);
	if(savefile)
	{
		savefile << "Parameters for DTIAtlasFiberAnalyzer : " <<std::endl;
		
		savefile << "CSVFile : " << CSVFilename <<std::endl;
		savefile << "#Data/Case/Deformation Column --> index starts at 1" <<std::endl;
		savefile << "Data Column : " << DataCol +1 <<std::endl;
		if(DefCol!=-1)
			savefile << "Defomation Field Column : " << DefCol +1 <<std::endl;
		if(NameCol!=-1)
			savefile << "Case Column : " << NameCol +1 <<std::endl;
		savefile << "Output Folder : " << OutputFolder <<std::endl;
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
		if(parameters.compare("")!=0)
		{
			savefile << "#Profile parameter --> choice between fa,md,fro,ad,rd,l2,l3,ga "<<std::endl;
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
bool ReadParametersFromFile(std::string filename, std::string &CSVfilename, std::string &AtlasFiberDir,
									 std::string &OutputFolder, std::string &parameters, int &DataCol, int &DefCol,
									 int &NameCol, vstring &SelectedFibers, bool &transposeColRow)
{
	//variables
	vstring fibers;
	std::string ListOfFiber;
	
	if(filename.compare("")!=0)
	{
		std::ifstream file(filename.c_str() , std::ios::in);  // open in reading
		std::string str,buf1,buf2;
			
		if(file)  // if open
		{
				//the first line
			getline(file, buf1);
			if(buf1.compare(0,39,"Parameters for DTIAtlasFiberAnalyzer : ")==0)
			{
				/* Loop for reading the file and setting the parameters values */
				while(!file.eof())
				{
					getline(file, buf1);
					if(buf1.compare(0,10,"CSVFile : ")==0)
						CSVfilename = buf1.substr(10,buf1.size()-10);
					
					else if(buf1.compare(0,14,"Data Column : ")==0)
					{
						str = buf1.substr(14,buf1.size()-14);
						DataCol = atoi(str.c_str())-1;
					}
						
					else if(buf1.compare(0,26,"Defomation Field Column : ")==0)
					{
						str = buf1.substr(26,buf1.size()-26);
						DefCol = atoi(str.c_str())-1;
					}
						
					else if(buf1.compare(0,14,"Case Column : ")==0)
					{
						str = buf1.substr(14,buf1.size()-14);
						NameCol = atoi(str.c_str())-1;
					}
						
					else if(buf1.compare(0,16,"Output Folder : ")==0)
						OutputFolder = buf1.substr(16,buf1.size()-16);
						
					else if(buf1.compare(0,21,"Atlas Fiber Folder : ")==0)
						AtlasFiberDir = buf1.substr(21,buf1.size()-21);
						
					else if(buf1.compare(0,18,"Selected Fibers : ")==0)
					{
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
								SelectedFibers.push_back(word);
								it = ListOfFiber.begin();
								ListOfFiber = ListOfFiber.substr(
										ListOfFiber.find_first_of(",")+1,
								ListOfFiber.size() -
										ListOfFiber.find_first_of(",")+1);
							}
							else
							{
								SelectedFibers.push_back(ListOfFiber);
								it = ListOfFiber.end();
							}
						}
					}
					
					else if(buf1.compare(0,20,"Profile parameter : ")==0)
						parameters = buf1.substr(20,buf1.size()-20);
					
					else if(buf1.compare(0,14,"Col and Row : ")==0)
					{
						if(buf1.substr(14,buf1.size()-14).compare("Case in column")==0 ||
											buf1.substr(14,buf1.size()-14).compare("Case in col")==0 ||
											buf1.substr(14,buf1.size()-14).compare("Arc lenght in row")==0)
							transposeColRow = true;
						else
							transposeColRow = false;
					}
				}
				
				std::cout<<std::endl;
				std::cout<<"Parameters file loaded : New Data"<<std::endl;
			}
			else
			{
				std::cout<<"ERROR: Wrong file for parameters"<<std::endl;
				return false;
			}
			
			file.close();
		}
		else 
		{
			std::cout << "ERROR: No parameters file found" << std::endl;
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
int PlaneAssociatedToFiber(std::string fibername,int type, vstring fibersplane, vstring Selectedfibersplane)
{
	if(type ==1)
	{
		for(unsigned int j=0;j<fibersplane.size();j++)
		{
			if(fibersplane[j].compare(takeoffExtension(fibername)+".fvp")==0 ||
						fibersplane[j].compare(takeoffExtension(fibername)+".fvb")==0)
				return j;
			if(fibersplane[j].compare(takeoffExtension(fibername)+"_auto")==0)
				return j;
		}
	}
	else
	{
		for(unsigned int j=0;j<Selectedfibersplane.size();j++)
		{
			if(Selectedfibersplane[j].compare(takeoffExtension(fibername)+".fvp")==0 ||
						Selectedfibersplane[j].compare(takeoffExtension(fibername)+".fvb")==0)
				return j;
			if(Selectedfibersplane[j].compare(takeoffExtension(fibername)+"_auto")==0)
				return j;
		}
	}
	return -1;
}

 /************************************************************************************
 * convert: conversion of each line of read file into 2D double QVector.
 ************************************************************************************/
 
 void convert(QVector <std::string> line, QVector < QVector <double> >& data)
{
	QVector <double> buffer;
	int i=0;
	for(i=0; line[i]!=line[line.size()-1]; i++)
	{
		buffer.append(atof(line[i].c_str()));
	}
	buffer.append(atof(line[i].c_str()));
	data.append(buffer);
}

 /************************************************************************************
 * LineInVector : Like previous definition of LineInVector, take line with data 
 * 	separated by ',' and put it in a 2D double vector.
 ************************************************************************************/
void LineInVector(std::string line, QVector < QVector <double> >& data)
{
	QVector <std::string> column;
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
			column.append(word);
			it = line.begin();
			//erase the first word in the line
			line = line.substr(line.find_first_of(",")+1,line.size()-line.find_first_of(",")+1);
		}
		else
		{
			column.append(line);
			it = line.end();
		}
	}
	convert(column, data);
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
			//if(line.compare(0,2,"  ")!=0)
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
 * getdata : put every data lines read from the file in a 2D double vector
 ************************************************************************************/

QVector < QVector <double> > getdata(std::ifstream& fvpfile, int nb_of_samples)
{
	std::string buffer;
	QVector < QVector <double> > data;
	for(int i=0; i<nb_of_samples; i++)
	{
		getline(fvpfile, buffer);
		LineInVector(buffer, data);
	}
	return data;	
}

/************************************************************************************
 * GetColumn : As the data is stored by line, this function allow users to get a
 * 	column of data for plot.
 ************************************************************************************/

QVector <double> GetColumn(int column, QVector < QVector <double> > data)
{
	QVector <double> QV_column;
	for(int i=0; i<data.size(); i++)
	{
		QV_column.append(data[i][column]);
	}
	return QV_column;
}

/************************************************************************************
 * getFiberIndex : return the index associated to the name of a fiber. This index
 * 	correspond to index of fibermean vector.
 ************************************************************************************/

int getFiberIndex(std::string filename)
{
	std::string fibername;
// 	if(filename.find_last_of("_")!=std::string::npos)
	fibername=filename.substr(filename.find_last_of("_")+1,filename.size()-filename.find_last_of("_")+1);
	fibername=takeoffExtension(fibername);
	if(fibername=="Genu")
		return 0;
	else if(fibername=="ILF-Left")
		return 1;
	else if(fibername=="ILF-Right")
		return 2;
	else if(fibername=="Splenium")
		return 3;
	else if(fibername=="Uncinate-Left")
		return 4;
	else if(fibername=="Uncinate-Right")
		return 5;
	else
		return -1;
}

double getMean(QVector< double > data)
{
	double mean=0;
	for(int i=0; i<data.size(); i++)
		mean+=data[i];
	mean/=data.size();
	return mean;
}

double getStd(QVector <double> data, double mean)
{
	double std=0;
	for(int i=0; i<data.size(); i++)
		std+=pow(data[i]-mean,2);
	std/=data.size();
	std=sqrt(std);
	return std;
}

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
			for(unsigned int l=0; l<data.size(); l++)
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
			for(unsigned int l=0; l<data.size(); l++)
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



