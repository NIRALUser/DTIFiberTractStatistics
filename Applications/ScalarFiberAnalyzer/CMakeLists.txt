add_definitions(-std=c++0x)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
#set(CMAKE_AUTOMOC ON)

set(srcheaders
scriptwriter.h
csvparser.h
atlasmodel.h
csvbrowsemodel.h
fibertractmodel.h
csvbrowser.h
T1T2FiberAnalyzer.h
errorreporter.h
)

set(srcfiles
tfatool.cpp
errorreporter.cpp
T1T2FiberAnalyzer.cpp
csvbrowser.cpp
csvbrowsemodel.cpp
fibertractmodel.cpp
atlasmodel.cpp
scriptwriter.cpp
csvparser.cpp
)

set(srcUI
csvbrowser.ui
mainwindow.ui
)

if(USE_QT4)
  find_package(Qt4 COMPONENTS QtXml)
  include(${QT_USE_FILE})
  include_directories(${QT_INCLUDES})
  QT4_WRAP_CPP(QT_HEADERS_MOC ${srcheaders})
  QT4_WRAP_UI(UI_FILES ${srcUI})
else()
  find_package(Qt5Widgets REQUIRED)
  set(QT_LIBRARIES ${Qt5Widgets_LIBRARIES})
  set(QT_INCLUDE_DIRS ${Qt5Widgets_INCLUDE_DIRS})
  qt5_wrap_cpp(QT_HEADERS_MOC ${srcheaders})
  qt5_wrap_ui(UI_FILES ${srcUI})
endif()



find_package(SlicerExecutionModel REQUIRED)
include(${SlicerExecutionModel_USE_FILE})
include(${GenerateCLP_USE_FILE})

find_package(QtToCppXML REQUIRED)
include(${QtToCppXML_USE_FILE})



QtToCppXML( SOURCES srcfiles NAME T1T2FiberAnalyzer UIXML_FILE mainwindow.ui PREFIXES para_ conf_ )

list( APPEND
srcfiles
${QT_HEADERS_MOC}
${UI_FILES}
)


SEMMacroBuildCLI(
  NAME ScalarFiberAnalyzer
  EXECUTABLE_ONLY
  ADDITIONAL_SRCS ${srcfiles}
  TARGET_LIBRARIES ${QT_LIBRARIES} ${ITK_LIBRARIES}
  INCLUDE_DIRECTORIES ${QT_INCLUDE_DIRS}
  INSTALL_RUNTIME_DESTINATION ${INSTALL_RUNTIME_DESTINATION}
  INSTALL_LIBRARY_DESTINATION ${INSTALL_LIBRARY_DESTINATION}
  INSTALL_ARCHIVE_DESTINATION ${INSTALL_ARCHIVE_DESTINATION} 
  
)

#add_executable(T1T2FiberAnalyzer ${srcfiles})
#target_link_libraries(T1T2FiberAnalyzer Qt5::Widgets)
