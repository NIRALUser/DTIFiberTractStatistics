

#QtXML lib
find_package(Qt5 COMPONENTS Svg)
set(QT_LIBRARIES Qt5::Svg)

#if ( NOT QT_FOUND)
#message(FATAL_ERROR "Package Qt not found!")
#endif()
# if ( NOT QT_QTXML_FOUND )
#  message(FATAL_ERROR " QtXml required, but not found!")
#endif()

#include(${QT_USE_FILE})
#include_directories(${QT_INCLUDES})



QT5_WRAP_CPP(MOC_FILES DTIAtlasFiberAnalyzerguiwindow.h)
QT5_WRAP_UI(UI_FILES DTIAtlasFiberAnalyzerguiwindow.ui)

set(DTIAtlasFiberAnalyzer_source DTIAtlasFiberAnalyzerguiwindow.cxx CSVClass.cxx GlobalFunction.cxx XmlReader.cxx XmlWriter.cxx ${MOC_FILES} ${UI_FILES})

set(DTIAtlasFiberAnalyzer_lib  ${ITK_LIBRARIES} ${VTK_LIBRARIES} ${QT_LIBRARIES} ${QWT_LIBRARIES} )

if( EXTENSION )
  ADD_DEFINITIONS(-DSlicerExtension)
  generateclp(Launcher_CLP DTIAtlasFiberAnalyzer.xml )
  add_executable( DTIAtlasFiberAnalyzerLauncher Launcher.cxx ${Launcher_CLP} )
  install(TARGETS DTIAtlasFiberAnalyzerLauncher
    RUNTIME DESTINATION ${INSTALL_RUNTIME_DESTINATION} COMPONENT RuntimeLibraries
    )
endif()


SEMMacroBuildCLI(
    NAME DTIAtlasFiberAnalyzer
    ADDITIONAL_SRCS ${DTIAtlasFiberAnalyzer_source}
    TARGET_LIBRARIES ${DTIAtlasFiberAnalyzer_lib}
    ${STATIC}
    ARCHIVE_OUTPUT_DIRECTORY ${ARCHIVE_DESTINATION}
    LIBRARY_OUTPUT_DIRECTORY ${LIBRARY_DESTINATION}
    RUNTIME_OUTPUT_DIRECTORY ${RUNTIME_DESTINATION}
    INSTALL_ARCHIVE_DESTINATION ${INSTALL_ARCHIVE_DESTINATION}
    INSTALL_LIBRARY_DESTINATION ${INSTALL_LIBRARY_DESTINATION}
    INSTALL_RUNTIME_DESTINATION ${INSTALL_RUNTIME_DESTINATION}
    )

IF(BUILD_TESTING)
  ADD_SUBDIRECTORY(Testing)
ENDIF(BUILD_TESTING)

