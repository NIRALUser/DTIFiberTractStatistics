# DTI Fiber Tract Statistics

## What is it?

This is a software which allows the user to study the behavior of water diffusion (using DTI data) along the length of the white matter fiber-tracts.
Various tract-oriented scalar diffusion measures obtained from DTI brain images, are treated as a continuous function of white matter fibers' arc-length. To analyze the trend along a given fiber tract, a command line tool performs kernel regression on this data. The idea is to try out different noise models and maximum likelihood estimates within kernel windows (along the tract), such that they best represent the data and are robust to noise and Partial Volume effect.

The package contains several command line based modules and an GUI based tool called DTIAtlasFiberAnalyzer to access most functions.

This package is also available as a 3D Slicer extension (http://www.slicer.org) called DTIAtlasFiberAnalyzer.

Some documentation about the features available in this package are available here: http://www.na-mic.org/Wiki/index.php/Projects:dtistatisticsfibers

## Usage 

#### GUI

```
	$ DTIAtlasFiberAnalyzer
```

#### CLI

Example
```
	$ DTIAtlasFiberAnalyzer --datafile <datafilename> --analysisfile <analysisfilename> --removeCleanFibers --removeNanFibers --numthreads 8 --nogui
```

## Parameter file example

##### Datafile
```
Data parameters for DTIAtlasFiberAnalyzer : 
CSVFile : ../AtlasDIR/DTIAtlas_Result.csv
#Data/Case/Deformation Column --> index starts at 1
Data Column : 2
Deformation Field Column : 3
Field Type : displacement field
Case Column : 1
Output Folder : /outputDIR/
```

##### Analysisfile
```
Analysis parameters for DTIAtlasFiberAnalyzer : 
Atlas Fiber Folder : /FiberDIR/AutoTract/AllFinalCleanedFibers
#Selected Fibers in the Atlas--> give the name of the file without the path
Selected Fibers : ILF_L.vtk
Selected Planes : 
#Profile parameter --> choice between fa,md,fro,ad,l2,l3,ga,rd 
Profile parameter : fa
#What for col and row --> choice between 'Case' or 'Arc lenght' with after 'in row' or 'in column'/'in col'
Col and Row : Case in row
```


## Dockerfile for developers

Use below command in source directory to build docker image (Currently having only CentOS7 image.)

```
$ docker build . -t <image-name>
$ docker run --rm -it -v $PWD/../:/work -w /work <image-name> 
```

## Build comment

- In OSX,  Qt5_DIR environment variable needs to be set (e.g. /usr/local/opt/qt5/lib/cmake/Qt5)

## Change Log:

#### [v1.8.0-beta](http://github.com/NIRALUser/DTIFiberTractStatistics/tree/v1.8.0-beta) (09/17/2020)
- Multithreading in fiberprocessing, dti-tract-stat. 
- Minor bug fixed
- CLI bug fixed


#### [v1.7.3c-beta](http://github.com/NIRALUser/DTIFiberTractStatistics/tree/v1.7.3c-beta) (07/27/2020)
- Merge Tab removed
- Bandwidth parameter can be passed by UI 
- dtitractstat related computation bug fixed
- Minor appearance changed (version, overwriting messages)


#### [v1.7.3b](http://github.com/NIRALUser/DTIFiberTractStatistics/tree/v1.7.3b) (07/27/2020)
- FADTTSter repository chaneged to master
- Build script bug fixed for OSX

#### [v1.7.3a](http://github.com/NIRALUser/DTIFiberTractStatistics/tree/v1.7.3a) (07/13/2020)
- CSV Table is now able to remove a selection of table. (Previously available only line by line)
- Tabs are now all enabled without having processed the previous step.
- Build script fixed , users can build the tool suite with the standard environment described in Dockerfile. (CentOS7 only)


## License

See License.txt

## More information

Find the tool on [NITRC](https://www.nitrc.org/projects/dti_tract_stat)

## Building from source

### Install Qt5

Building using Qt5 is recommended. This version has been build with Qt5.5.1 but it may work with newer version. You can download Qt5 from [here](https://download.qt.io/archive/qt/5.5/5.5.1/)

### Clone this repository and build

```bash
  git clone https://github.com/NIRALUser/DTIFiberTractStatistics.git
  mkdir DTIFiberTractStatistics-build
  cd DTIFiberTractStatistics-build
  cmake -DUSE_QT4:BOOL=OFF -DQt5_DIR:PATH=/your/install/directory/Qt5.5.1/lib/cmake/Qt5 ../DTIFiberTractStatistics
```

If you want to build FADTTS you should use the following settings

```bash
  cmake -DUSE_QT4:BOOL=OFF -DBUILD_FADTTS:BOOL=ON -DQt5_DIR:PATH=/your/install/directory/Qt5.5.1/lib/cmake/Qt5 ../DTIFiberTractStatistics
```
  
