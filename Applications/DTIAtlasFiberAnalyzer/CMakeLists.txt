Project(DTIAtlasFiberAnalyzer)

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
if( NOT QWT_LIBRARY OR QWT_LIBRARY_PATH )
  if( EXTENSION OR SUPERBUILD_NOT_EXTENSION )
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

Include_directories( ${QWT_INCLUDE_DIR} )

QT4_WRAP_CPP(MOC_FILES DTIAtlasFiberAnalyzerguiwindow.h DTIPlotWindow.h)
QT4_WRAP_UI(UI_FILES DTIAtlasFiberAnalyzerguiwindow.ui)

set(DTIAtlasFiberAnalyzer_source DTIAtlasFiberAnalyzerguiwindow.cxx CSVClass.cxx GlobalFunction.cxx DTIPlotWindow.cxx ${MOC_FILES} ${UI_FILES})

set(DTIAtlasFiberAnalyzer_lib  ${ITK_LIBRARIES} vtkHybrid ${QT_LIBRARIES} ${QWT_LIBRARIES} )

if( EXTENSION )
  generateclp(Launcher_CLP DTIAtlasFiberAnalyzer.xml )
  add_executable( DTIAtlasFiberAnalyzerLauncher Launcher.cxx ${Launcher_CLP} )
  install(TARGETS DTIAtlasFiberAnalyzerLauncher
    RUNTIME DESTINATION ${SlicerExecutionModel_DEFAULT_CLI_INSTALL_RUNTIME_DESTINATION} COMPONENT RuntimeLibraries
    )
  set( SETRUNTIMEDESTINATION INSTALL_RUNTIME_DESTINATION ${SlicerExecutionModel_DEFAULT_CLI_INSTALL_RUNTIME_DESTINATION}/../hidden-cli-modules )
else()
  set( SETRUNTIMEDESTINATION INSTALL_RUNTIME_DESTINATION ${RUNTIME_DESTINATION} )
endif()


SEMMacroBuildCLI(
    NAME DTIAtlasFiberAnalyzer
    ADDITIONAL_SRCS ${DTIAtlasFiberAnalyzer_source}
    TARGET_LIBRARIES ${DTIAtlasFiberAnalyzer_lib}
    ${STATIC}
    INSTALL_ARCHIVE_DESTINATION ${ARCHIVE_DESTINATION}
    INSTALL_LIBRARY_DESTINATION ${LIBRARY_DESTINATION}
    ${SETRUNTIMEDESTINATION}
    )

