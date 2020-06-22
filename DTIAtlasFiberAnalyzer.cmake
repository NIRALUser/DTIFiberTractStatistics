project( DTIAtlasFiberAnalyzerProject )

#-----------------------------------------------------------------------------

if(BUILD_TESTING)
  include( CTest )
endif(BUILD_TESTING)

if( DTIAtlasFiberAnalyzer_BUILD_SLICER_EXTENSION )
  #-----------------------------------------------------------------------------
  set(EXTENSION_NAME DTIAtlasFiberAnalyzer)
  set(EXTENSION_HOMEPAGE "http://www.nitrc.org/projects/dti_tract_stat")
  set(EXTENSION_CATEGORY "Diffusion")
  set(EXTENSION_CONTRIBUTORS "Francois Budin (UNC)")
  set(EXTENSION_DESCRIPTION "This extension provides the tool DTIAtlasFiberAnalyzer integrated in Slicer")
  set(EXTENSION_ICONURL "http://www.nitrc.org/project/screenshot.php?group_id=403&screenshot_id=768")
  set(EXTENSION_SCREENSHOTURLS "http://wiki.slicer.org/slicerWiki/images/thumb/2/20/Screenshot-DTI_Atlas_Fiber_Analyser.png/745px-Screenshot-DTI_Atlas_Fiber_Analyser.png http://wiki.slicer.org/slicerWiki/images/thumb/6/6e/DTIAtlasFiberAnalyzerGenu_profile_FA.png/800px-DTIAtlasFiberAnalyzerGenu_profile_FA.png")
  set(EXTENSION_STATUS "")
  set(EXTENSION_DEPENDS "DTIProcess") # Specified as a space separated list or 'NA' if any
  set(EXTENSION_BUILD_SUBDIRECTORY . )
  unsetForSlicer( NAMES QT_QMAKE_EXECUTABLE SlicerExecutionModel_DIR ITK_DIR VTK_DIR CMAKE_C_COMPILER CMAKE_CXX_COMPILER CMAKE_CXX_FLAGS CMAKE_C_FLAGS ITK_LIBRARIES )
  find_package(Slicer REQUIRED)
  include(${Slicer_USE_FILE})
  resetForSlicer( NAMES CMAKE_C_COMPILER CMAKE_CXX_COMPILER CMAKE_CXX_FLAGS CMAKE_C_FLAGS )

  SET(INSTALL_RUNTIME_DESTINATION ${Slicer_INSTALL_CLIMODULES_BIN_DIR})
  SET(INSTALL_LIBRARY_DESTINATION ${Slicer_INSTALL_CLIMODULES_LIB_DIR})
  SET(INSTALL_ARCHIVE_DESTINATION ${Slicer_INSTALL_CLIMODULES_LIB_DIR})

endif()

SETIFEMPTY(INSTALL_RUNTIME_DESTINATION bin)
SETIFEMPTY(INSTALL_LIBRARY_DESTINATION bin)
SETIFEMPTY(INSTALL_ARCHIVE_DESTINATION lib/static)
SETIFEMPTY( ARCHIVE_DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/lib/static )
SETIFEMPTY( LIBRARY_DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/lib )
SETIFEMPTY( RUNTIME_DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin )

set( install_dir ${INSTALL_RUNTIME_DESTINATION} )

find_package(SlicerExecutionModel REQUIRED)
include(${SlicerExecutionModel_USE_FILE})

FIND_PACKAGE(ITK REQUIRED)
include(${ITK_USE_FILE})

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
if( VTK_MAJOR_VERSION VERSION_LESS 6 AND NOT VTK_USE_QT )
  message( FATAL_ERROR "DTIAtlasFiberAnalyzer needs VTK to be built with Qt" )
endif()


if( VTK_MAJOR_VERSION VERSION_GREATER 5 ) 
  if(COMPILE_DTIATLASFIBERANALYZER)
    list( FIND VTK_MODULES_ENABLED "vtkGUISupportQt" vtkQtpos )
    if( vtkQtpos LESS 0 )
      message( FATAL_ERROR "DTIAtlasFiberAnalyzer needs VTK to be built with Qt" )
    endif()
  endif()
endif()

option(USE_QT4 "Build using Qt4" OFF)
if(USE_QT4)
  FIND_PACKAGE(Qt4 REQUIRED)
  IF(QT_USE_FILE)
    INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR})
    INCLUDE(${QT_USE_FILE})
  ELSE(QT_USE_FILE)
    MESSAGE(FATAL_ERROR, "QT not found. Please set QT_DIR.")
  ENDIF(QT_USE_FILE)
else()
  if(UNIX AND NOT APPLE)
    find_package(Qt5 COMPONENTS Core Gui Network Xml Svg X11Extras REQUIRED)
  else()
    find_package(Qt5 COMPONENTS Core Gui Network Xml Svg REQUIRED)
  endif()
endif()

add_definitions(${Qt5Widgets_DEFINITIONS})
set(CMAKE_CXX_FLAGS " -std=c++11 ${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

option(EXECUTABLES_ONLY "Build only executables (CLI)" ON)
if( ${EXECUTABLES_ONLY} )
  set( STATIC "EXECUTABLE_ONLY" )
  set( STATIC_LIB "STATIC" )
else()
  set( STATIC_LIB "SHARED" )
endif()

option(CREATE_BUNDLE "Create bundle" OFF)
if(CREATE_BUNDLE)
  if(APPLE)
    set(OS_BUNDLE MACOSX_BUNDLE)
  elseif(WIN32)
    set(OS_BUNDLE WIN32)
  endif()

  set(version 1.8.0)
  set(bundle_name DTIAtlasFiberAnalyzer)

  #--------------------------------------------------------------------------------
  SET(qtconf_dest_dir bin)
  SET(APPS "\${CMAKE_INSTALL_PREFIX}/bin/${bundle_name}")
  IF(APPLE)
    set(MACOSX_BUNDLE_BUNDLE_NAME "${bundle_name}")
    set(MACOSX_BUNDLE_INFO_STRING "FADTTS: functional analysis of diffusion tensor tract statistics.")
    set(MACOSX_BUNDLE_ICON_FILE "")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "niral.unc.edu.${bundle_name}")
    set(MACOSX_BUNDLE_LONG_VERSION_STRING "DTIAtlasFiberAnalyzer version - ${version}")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${version}")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "${version}")
    set(MACOSX_BUNDLE_COPYRIGHT "Copyright 2016 University of North Carolina , Chapel Hill.")
    SET(qtconf_dest_dir ${bundle_name}.app/Contents/Resources)
    SET(APPS "\${CMAKE_INSTALL_PREFIX}/${bundle_name}.app")
  ENDIF(APPLE)

  IF(WIN32)
    SET(APPS "\${CMAKE_INSTALL_PREFIX}/bin/${bundle_name}.exe")
  ENDIF(WIN32)

  # include_directories(${CMAKE_CURRENT_BINARY_DIR})
  # add_executable(${bundle_name} ${OS_BUNDLE}
  #   ${FADTTS_src}
  # )
  # target_link_libraries(${bundle_name} ${QT_LIBRARIES} ${VTK_LIBRARIES})

  # #--------------------------------------------------------------------------------
  # # Install the QtTest application, on Apple, the bundle is at the root of the
  # # install tree, and on other platforms it'll go into the bin directory.
  # INSTALL(TARGETS ${bundle_name}
  #   DESTINATION . COMPONENT Runtime
  #   RUNTIME DESTINATION bin COMPONENT Runtime
  # )

  macro(install_qt5_plugin _qt_plugin_name _qt_plugins_var)
    get_target_property(_qt_plugin_path "${_qt_plugin_name}" LOCATION)
    if(EXISTS "${_qt_plugin_path}")
      get_filename_component(_qt_plugin_file "${_qt_plugin_path}" NAME)
      get_filename_component(_qt_plugin_type "${_qt_plugin_path}" PATH)
      get_filename_component(_qt_plugin_type "${_qt_plugin_type}" NAME)
      if(APPLE)
        set(_qt_plugin_dest "${bundle_name}.app/Contents/PlugIns/${_qt_plugin_type}")
      elseif(UNIX)
        set(_qt_plugin_dest "lib")
      endif()
      install(FILES "${_qt_plugin_path}"
        DESTINATION "${_qt_plugin_dest}"
        COMPONENT Runtime)
      set(${_qt_plugins_var}
        "${${_qt_plugins_var}};\${CMAKE_INSTALL_PREFIX}/${_qt_plugin_dest}/${_qt_plugin_file}")
    else()
      message(FATAL_ERROR "QT plugin ${_qt_plugin_name} not found")
    endif()
  endmacro()

  if(APPLE)
    install_qt5_plugin("Qt5::QCocoaIntegrationPlugin" QT_PLUGINS)
    set(CPACK_BINARY_DRAGNDROP ON)
    #--------------------------------------------------------------------------------
    # install a qt.conf file
    # this inserts some cmake code into the install script to write the file
    INSTALL(CODE "
        file(WRITE \"\${CMAKE_INSTALL_PREFIX}/${qtconf_dest_dir}/qt.conf\" \"[Paths]\nPlugins = PlugIns\n\")
        " COMPONENT Runtime)
  elseif(UNIX)
    install_qt5_plugin("Qt5::QXcbIntegrationPlugin" QT_PLUGINS)  
  endif()

  # Install the license
  INSTALL(FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/License.txt
    DESTINATION "${CMAKE_INSTALL_PREFIX}/${qtconf_dest_dir}"
    COMPONENT Runtime)

  if(APPLE)
    get_target_property(Qt5_location Qt5::Widgets LOCATION)
    string(FIND ${Qt5_location} "/QtWidgets" length)
    string(SUBSTRING ${Qt5_location} 0 ${length} Qt5_location)    
    #Fix the bundle
    install(CODE "  
      include(BundleUtilities)  
      fixup_bundle(\"${APPS}\" \"${QT_PLUGINS};\" \"${Qt5_location}\")
     "
      COMPONENT Runtime)
  elseif(UNIX)

   # when building, don't use the install RPATH already
   # (but later on when installing)
   SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE) 
   # the RPATH to be used when installing
   SET(CMAKE_INSTALL_RPATH "../lib")

    get_target_property(QtCore_location Qt5::Core LOCATION)
    get_target_property(QtGui_location Qt5::Gui LOCATION)
    get_target_property(QtNetwork_location Qt5::Network LOCATION)
    get_target_property(QtXml_location Qt5::Xml LOCATION)
    get_target_property(QtSql_location Qt5::Sql LOCATION)
    get_target_property(QtSvg_location Qt5::Svg LOCATION)
    get_target_property(QtWidgets_location Qt5::Widgets LOCATION)    

    get_target_property(QtCore_SOFT Qt5::Core IMPORTED_SONAME_RELEASE)
    get_target_property(QtGui_SOFT Qt5::Gui IMPORTED_SONAME_RELEASE)
    get_target_property(QtNetwork_SOFT Qt5::Network IMPORTED_SONAME_RELEASE)
    get_target_property(QtXml_SOFT Qt5::Xml IMPORTED_SONAME_RELEASE)
    get_target_property(QtSql_SOFT Qt5::Sql IMPORTED_SONAME_RELEASE)
    get_target_property(QtWidgets_SOFT Qt5::Widgets IMPORTED_SONAME_RELEASE)
    get_target_property(QtSvg_SOFT Qt5::Svg IMPORTED_SONAME_RELEASE)

    get_target_property(Qt5_location Qt5::Widgets LOCATION)  
    string(FIND ${Qt5_location} "libQt5Widgets" length)
    string(SUBSTRING ${Qt5_location} 0 ${length} Qt5_location)
    
    install(FILES ${QtCore_location} ${QtGui_location} ${QtNetwork_location} ${QtXml_location} ${QtSql_location} ${QtWidgets_location} ${QtSvg_location} ${Qt5_location}/${QtCore_SOFT} ${Qt5_location}/${QtGui_SOFT} ${Qt5_location}/${QtNetwork_SOFT} ${Qt5_location}/${QtXml_SOFT} ${Qt5_location}/${QtSql_SOFT} ${Qt5_location}/${QtWidgets_SOFT} ${Qt5_location}/${QtSvg_SOFT}
        DESTINATION lib
        COMPONENT Runtime)
  endif()

  # To Create a package, one can run "cpack -G DragNDrop CPackConfig.cmake" on Mac OS X
  # where CPackConfig.cmake is created by including CPack
  # And then there's ways to customize this as well  
  include(InstallRequiredSystemLibraries)
  include(CPack)  
endif()

add_subdirectory( Applications )

if( DTIAtlasFiberAnalyzer_BUILD_SLICER_EXTENSION )
  set(CPACK_INSTALL_CMAKE_PROJECTS "${CPACK_INSTALL_CMAKE_PROJECTS};${CMAKE_BINARY_DIR};${EXTENSION_NAME};ALL;/")
  include(${Slicer_EXTENSION_CPACK})
endif()
