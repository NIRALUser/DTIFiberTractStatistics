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

find_package(SlicerExecutionModel REQUIRED)
include(${SlicerExecutionModel_USE_FILE})

FIND_PACKAGE(Qt4 REQUIRED)
IF(QT_USE_FILE)
  INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR})
  INCLUDE(${QT_USE_FILE})
ELSE(QT_USE_FILE)
   MESSAGE(FATAL_ERROR, "QT not found. Please set QT_DIR.")
ENDIF(QT_USE_FILE)

# Qwt include directory
if( NOT QWT_INCLUDE_DIR )
  FIND_PATH(QWT_INCLUDE_DIR qwt.h
    /usr/local/qwt/include
    /usr/local/include
    /usr/include/qwt
    /usr/include
  )
endif()

if(NOT QWT_INCLUDE_DIR)
  message(FATAL_ERROR "QWT_INCLUDE_DIR not found. It should be set to a path containing qwt.h, like '/Projects/qwt-6.0.1-svn/include'")
endif()

# Qwt libraries
if( NOT QWT_LIBRARY )
  if( EXTENSION_SUPERBUILD_BINARY_DIR )
    set(NO_ENV_PATH "NO_DEFAULT_PATH")
  endif()
  SET(QWT_NAMES ${QWT_NAMES} qwt libqwt)
  FIND_LIBRARY(QWT_LIBRARY
    NAMES ${QWT_NAMES}
    PATHS ${QWT_LIBRARY_PATH} /usr/local/qwt/lib /usr/local/lib /usr/lib
    ${NO_ENV_PATH}
  )
endif()
if(NOT QWT_LIBRARY)
  message(FATAL_ERROR "QWT_LIBRARY not found. It should be set to a filepath like '/Projects/qwt-6.0.1-svn/lib/libqwt.a'")
endif()
set(QWT_LIBRARIES ${QWT_LIBRARY})

Include_directories(
  ${QWT_INCLUDE_DIR}
  ${QT_INCLUDE_DIR}
  ${DTIAtlasFiberAnalyser_BINARY_DIR}
  ${DTIAtlasFiberAnalyser_SOURCE_DIR}
)

QT4_WRAP_CPP(MOC_FILES DTIAtlasFiberAnalyzerguiwindow.h DTIPlotWindow.h)
QT4_WRAP_UI(UI_FILES DTIAtlasFiberAnalyzerguiwindow.ui)

set(DTIAtlasFiberAnalyzer_source DTIAtlasFiberAnalyzerguiwindow.cxx CSVClass.cxx GlobalFunction.cxx DTIPlotWindow.cxx ${MOC_FILES} ${UI_FILES})

set(DTIAtlasFiberAnalyzer_lib  ${ITK_LIBRARIES} vtkHybrid ${QT_LIBRARIES} ${QWT_LIBRARIES} )

SEMMacroBuildCLI(
    NAME DTIAtlasFiberAnalyzer
    ADDITIONAL_SRCS ${DTIAtlasFiberAnalyzer_source}
    TARGET_LIBRARIES ${DTIAtlasFiberAnalyzer_lib}
    EXECUTABLE_ONLY
    INCLUDE_DIRECTORIES ${QT_INCLUDE_DIR} ${DTIAtlasFiberAnalyzer_BINARY_DIR} ${DTIAtlasFiberAnalyzer_SOURCE_DIR} ${QWT_INCLUDE_DIR} ${VTK_INCLUDE_DIRS}
    NO_INSTALL
    )

