PROJECT(DTIFiberTractStat)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

FIND_PACKAGE(ITK)
IF(ITK_FOUND)
INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
MESSAGE(FATAL_ERROR
"ITK not found. Please set ITK_DIR.")
ENDIF(ITK_FOUND)

FIND_PACKAGE(VTK)
IF(VTK_FOUND)
  INCLUDE(${VTK_USE_FILE})
ELSE(VTK_FOUND)
  MESSAGE(FATAL_ERROR
  "VTK not found.  Please set VTK_DIR.")
ENDIF(VTK_FOUND)

find_package(GenerateCLP REQUIRED)
include(${GenerateCLP_USE_FILE})

find_package(Slicer3 REQUIRED)
include(${Slicer3_USE_FILE})

SET(CXX_FILES
dtitractstat.cxx
fiberprocessing.cxx
regression.cxx
)

SET(H_FILES
dtitypes.h
argio.h
fiberprocessing.h
regression.h
)

set(dtitractstat_SOURCE ${CXX_FILES})
GENERATECLP(dtitractstat_SOURCE dtitractstat.xml)

ADD_EXECUTABLE(dtitractstatCLP ${CXX_FILES} ${H_FILES})
TARGET_LINK_LIBRARIES(dtitractstatCLP ITKCommon ITKIO  vtkIO)

slicer3_set_plugins_output_path(dtitractstatCLP)
slicer3_install_plugins(dtitractstatCLP)
