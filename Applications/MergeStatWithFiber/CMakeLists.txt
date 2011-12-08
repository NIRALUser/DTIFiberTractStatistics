Project(MergeStatWithFiber)
set(LOCAL_PROJECT_NAME dtiparametrization)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

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

INCLUDE_DIRECTORIES(
  ${MergeStatWithFiber_BINARY_DIR}
  ${MergeStatWithFiber_SOURCE_DIR}
)

SET(MergeStatWithFiber_source MergeStatWithFiber.cxx processing.cxx)
SET(MergeStatWithFiber_header processing.h)

GENERATECLP(MergeStatWithFiber.cxx MergeStatWithFiber.xml)
add_executable( MergeStatWithFiber ${MergeStatWithFiber_source} ${MergeStatWithFiber_header})

TARGET_LINK_LIBRARIES(MergeStatWithFiber
  vtkHybrid
)