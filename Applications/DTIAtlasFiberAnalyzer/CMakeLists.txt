Project(DTIAtlasFiberAnalyzer)
set(LOCAL_PROJECT_NAME DTIAtlasFiberAnalyzer)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

FIND_PACKAGE(ITK REQUIRED)
IF (ITK_FOUND)
   INCLUDE(${USE_ITK_FILE})
ELSE(ITK_FOUND)
   MESSAGE(FATAL_ERROR, "ITK not found. Please set ITK_DIR.")
ENDIF (ITK_FOUND)

FIND_PACKAGE(VTK REQUIRED)
IF (VTK_FOUND)
  INCLUDE(${VTK_USE_FILE})
ELSE(VTK_FOUND)
   MESSAGE(FATAL_ERROR, "VTK not found. Please set VTK_DIR.")
ENDIF (VTK_FOUND)

FIND_PACKAGE(GenerateCLP REQUIRED)
IF (GenerateCLP_FOUND)
  INCLUDE(${GenerateCLP_USE_FILE})
ELSE(GenerateCLP_FOUND)
   MESSAGE(FATAL_ERROR, "GenerateCLP not found. Please set GenerateCLP_DIR.")
ENDIF (GenerateCLP_FOUND)

FIND_PACKAGE(Qt4 REQUIRED)
IF(QT_USE_FILE)
  INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR})
  INCLUDE(${QT_USE_FILE})
ELSE(QT_USE_FILE)
   MESSAGE(FATAL_ERROR, "QT not found. Please set QT_DIR.")
ENDIF(QT_USE_FILE)

# Qwt include directory
set(QWT_INCLUDE_DIR CACHE PATH "Qwt include directory")
if(NOT QWT_INCLUDE_DIR)
  message(FATAL_ERROR "QWT_INCLUDE_DIR should be set to a path like '/home/jchris/Projects/qwt-6.0.1-svn/include'")
endif()
include_directories(${QWT_INCLUDE_DIR})

# Qwt libraries
set(QWT_LIBRARY CACHE FILEPATH "Qwt library")
if(NOT QWT_LIBRARY)
  message(FATAL_ERROR "QWT_LIBRARY should be set to a filepath like '/home/jchris/Projects/qwt-6.0.1-svn/lib/libqwt.so'")
endif()
set(QWT_MATHML_LIBRARY CACHE FILEPATH "Qwt library")
if(NOT QWT_MATHML_LIBRARY)
  message(FATAL_ERROR "QWT_MATHML_LIBRARY should be set to a filepath like '/home/jchris/Projects/qwt-6.0.1-svn/lib/libqwtmathml.so'")
endif()
set(qwt_libraries ${QWT_LIBRARY} ${QWT_MATHML_LIBRARY})

INCLUDE_DIRECTORIES(
  ${QT_INCLUDE_DIR}
  ${DTIAtlasFiberAnalyser_BINARY_DIR}
  ${DTIAtlasFiberAnalyser_SOURCE_DIR}
)

QT4_WRAP_CPP(MOC_FILES DTIAtlasFiberAnalyzerguiwindow.h DTIPlotWindow.h)
QT4_WRAP_UI(UI_FILES DTIAtlasFiberAnalyzerguiwindow.ui)

SET(DTIAtlasFiberAnalyzer_source DTIAtlasFiberAnalyzer.cxx DTIAtlasFiberAnalyzerguiwindow.cxx CSVClass.cxx GlobalFunction.cxx DTIPlotWindow.cxx ${MOC_FILES} ${UI_FILES})

GENERATECLP(DTIAtlasFiberAnalyzer.cxx DTIAtlasFiberAnalyzer.xml)
add_executable( DTIAtlasFiberAnalyzer ${DTIAtlasFiberAnalyzer_source})

TARGET_LINK_LIBRARIES(DTIAtlasFiberAnalyzer
  ${ITK_LIBRARIES}
  vtkHybrid
  ${QT_LIBRARIES}
  ${qwt_libraries}
)
