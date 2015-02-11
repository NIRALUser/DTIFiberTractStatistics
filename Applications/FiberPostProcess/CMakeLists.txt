# This is the root ITK CMakeList file:
cmake_minimum_required(VERSION 2.8)

# This project is designed to be built outside the Insight source tree.
project(FiberPostProcess)


include(ExternalData)
set(CXX_FILES
FiberPostProcess.cxx
processing.cxx
utils.cxx
)

set(H_FILES
processing.h
utils.h
csv.h
)

SEMMacroBuildCLI(
  NAME FiberPostProcess
  EXECUTABLE_ONLY
  ADDITIONAL_SRCS ${CXX_FILES} ${H_FILES}
  TARGET_LIBRARIES ${ITK_LIBRARIES} ${VTK_LIBRARIES}
  LINK_DIRECTORIES ${ITK_LIBRARY_DIRS} ${VTK_LIBRARY_DIRS}
  INCLUDE_DIRECTORIES ${ITK_INCLUDE_DIR} ${VTK_INCLUDE_DIR}
  INSTALL_ARCHIVE_DESTINATION ${ARCHIVE_DESTINATION}
  INSTALL_RUNTIME_DESTINATION ${RUNTIME_DESTINATION}
  INSTALL_LIBRARY_DESTINATION ${LIBRARY_DESTINATION}
  )
IF(BUILD_TESTING)
  ADD_SUBDIRECTORY(Testing)
ENDIF(BUILD_TESTING)
