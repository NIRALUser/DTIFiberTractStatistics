
# Make sure this file is included only once
get_filename_component(CMAKE_CURRENT_LIST_FILENAME ${CMAKE_CURRENT_LIST_FILE} NAME_WE)
if(${CMAKE_CURRENT_LIST_FILENAME}_FILE_INCLUDED)
  return()
endif()
set(${CMAKE_CURRENT_LIST_FILENAME}_FILE_INCLUDED 1)

## External_${extProjName}.cmake files can be recurisvely included,
## and cmake variables are global, so when including sub projects it
## is important make the extProjName and proj variables
## appear to stay constant in one of these files.
## Store global variables before overwriting (then restore at end of this file.)
ProjectDependancyPush(CACHED_extProjName ${extProjName})
ProjectDependancyPush(CACHED_proj ${proj})

# Include dependent projects if any
set(extProjName QWT) #The find_package known name
set(proj ${extProjName})      #This local name

# Set dependency list
set(${proj}_DEPENDENCIES "" )

SlicerMacroCheckExternalProjectDependency(${proj})

if(NOT DEFINED ${extProjName}_DIR AND NOT ${USE_SYSTEM_${extProjName}})
  ExternalProject_add(QWT
    SVN_REPOSITORY http://svn.code.sf.net/p/qwt/code/tags/qwt-6.0.2/
    SOURCE_DIR QWT
    BINARY_DIR QWT-build
    SVN_REVISION -r 1666
    "${cmakeversion_external_update}"
    PATCH_COMMAND ${CMAKE_COMMAND} -E
    copy ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt.qwt
    <SOURCE_DIR>/CMakeLists.txt
    CMAKE_ARGS ${COMMON_EXTERNAL_PROJECT_ARGS}
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}
    INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}
  )
  set(QWT_LIBRARY_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib)
  set(QWT_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/QWT/src)
else()
  if(${USE_SYSTEM_${extProjName}})
    find_package(${extProjName} REQUIRED)
    if(NOT ${extProjName}_DIR)
      message(FATAL_ERROR "To use the system ${extProjName}, set ${extProjName}_DIR")
    endif()
    message("USING the system ${extProjName}, set ${extProjName}_DIR=${${extProjName}_DIR}")
  endif()
  # The project is provided using ${extProjName}_DIR, nevertheless since other
  # project may depend on ${extProjName}v4, let's add an 'empty' one
  SlicerMacroEmptyExternalProject(${proj} "${${proj}_DEPENDENCIES}")
endif()

list(APPEND ${CMAKE_PROJECT_NAME}_SUPERBUILD_EP_VARS ${extProjName}_DIR:PATH)

ProjectDependancyPop(CACHED_extProjName extProjName)
ProjectDependancyPop(CACHED_proj proj)

#list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_BINARY_DIR} )
