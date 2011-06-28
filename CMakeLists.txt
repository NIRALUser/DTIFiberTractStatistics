Project(DTIFiberTractMain)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
CMAKE_POLICY(VERSION 2.8)

IF(${CMAKE_SOURCE_DIR} STREQUAL ${DTIFiberTractMain_SOURCE_DIR})
	set(LIBRARY_OUTPUT_PATH ${DTIFiberTractMain_BINARY_DIR}/lib CACHE PATH "Single output directory for building all libraries.")
	set(EXECUTABLE_OUTPUT_PATH ${DTIFiberTractMain_BINARY_DIR}/bin CACHE PATH "Single output directory for building all executables.")
ENDIF(${CMAKE_SOURCE_DIR} STREQUAL ${DTIFiberTractMain_SOURCE_DIR})


#------Compile DTIFiberTractStatMain as standalone package------
OPTION(COMPILE_EXTERNAL_PROJECTS " Compile External Projects." OFF)
IF(COMPILE_EXTERNAL_PROJECTS)
	
	FIND_PACKAGE(Slicer3 QUIET NO_DEFAULT_PATH)
	IF(Slicer3_FOUND)
		INCLUDE(${Slicer3_USE_FILE})
		slicer3_set_default_install_prefix_for_external_projects()
	ELSE (Slicer3_FOUND)
		FIND_PACKAGE(ITK REQUIRED)
		IF(ITK_FOUND)
			INCLUDE (${USE_ITK_FILE})
		ELSE(ITK_FOUND)
			MESSAGE(FATAL_ERROR, "ITK not found. Please set ITK_DIR.")
		ENDIF(ITK_FOUND)
		
		FIND_PACKAGE(GenerateCLP REQUIRED)
		IF(GenerateCLP_FOUND)
			INCLUDE (${GenerateCLP_USE_FILE})
		ENDIF(GenerateCLP_FOUND)
	ENDIF(Slicer3_FOUND)
	
	#External Projects
	include(ExternalProject)
	if(CMAKE_EXTRA_GENERATOR)
		set(gen "${CMAKE_EXTRA_GENERATOR} - ${CMAKE_GENERATOR}")
	else ()
		set(gen "${CMAKE_GENERATOR}")
	endif()
	
# 	OPTION(COMPILE_EXTERNAL_DTIPROCESS "Compile External DTIProcess." ON)
# 	IF(COMPILE_EXTERNAL_DTIPROCESS)
# 	# Build dtiprocess https://www.nitrc.org/svn/dtiprocess/trunk/Applications
# 		set(proj dtiprocess)
# 		IF (Slicer3_FOUND)
# 			ExternalProject_Add(${proj}
# 			SVN_REPOSITORY "https://www.nitrc.org/svn/dtiprocess/trunk/Applications"
# 			SVN_USERNAME slicerbot
# 			SVN_PASSWORD slicer
# 			SOURCE_DIR ${proj}
# 			BINARY_DIR ${proj}-build
# 			DEPENDS  ${ITK_DEPEND}
# 			CMAKE_GENERATOR ${gen}
# 			CMAKE_ARGS
# 			${LOCAL_CMAKE_BUILD_OPTIONS}
# 			-DSlicer3_DIR:PATH=${Slicer3_DIR}
# 			INSTALL_COMMAND ""
# 			)
# 		ELSE (Slicer3_FOUND)
# 			ExternalProject_Add(${proj}
# 			SVN_REPOSITORY "https://www.nitrc.org/svn/dtiprocess/trunk/Applications"
# 			SVN_USERNAME slicerbot
# 			SVN_PASSWORD slicer
# 			SOURCE_DIR ${proj}
# 			BINARY_DIR ${proj}-build
# 			DEPENDS  ${ITK_DEPEND}
# 			CMAKE_GENERATOR ${gen}
# 			CMAKE_ARGS
# 			${LOCAL_CMAKE_BUILD_OPTIONS}
# 			-DGenerateCLP_DIR:PATH=${GenerateCLP_DIR}
# 			-DITK_DIR:PATH=${ITK_DIR}
# 			INSTALL_COMMAND ""
# 			)
# 		ENDIF (Slicer3_FOUND)
# 	ENDIF(COMPILE_EXTERNAL_DTIPROCESS)
	
ENDIF(COMPILE_EXTERNAL_PROJECTS)
#Add the path to the CMakeLists.txt for the two applications
SUBDIRS(Applications)
