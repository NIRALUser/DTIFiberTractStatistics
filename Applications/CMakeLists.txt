OPTION(COMPILE_DTITRACTSTAT "Compile dtitractstat." ON)
IF(COMPILE_DTITRACTSTAT)
  add_subdirectory( dtitractstat )
ENDIF(COMPILE_DTITRACTSTAT)


OPTION(COMPILE_DTIATLASFIBERANALYZER "Compile DTIAtlasFiberAnalyzer." ON)
IF(COMPILE_DTIATLASFIBERANALYZER)
  add_subdirectory( DTIAtlasFiberAnalyzer )
ENDIF(COMPILE_DTIATLASFIBERANALYZER)

OPTION(COMPILE_MERGERSTATWITHFIBER "Compile MergeStatWithFiber." ON)
IF(COMPILE_MERGERSTATWITHFIBER)
  add_subdirectory( MergeStatWithFiber )
ENDIF(COMPILE_MERGERSTATWITHFIBER)


OPTION(COMPILE_EXTERNAL_DTIPROCESS "Compile external DTIProcess package (for fiberprocess application)." ON)
IF(COMPILE_EXTERNAL_DTIPROCESS)
  if( NOT DEFINED ITKV3_COMPATIBILITY OR NOT ${ITKV3_COMPATIBILITY}  )
    message( WARNING "Choose ITKv4 compiled with ITKV3_COMPATIBILITY set to ON (or GenerateCLP compiled against such an ITK version). If not, you may have compilation errors" )
  endif()
  if( EXTENSION )
    set(DTIProcessVarExt 
        -DDTIProcess_BUILD_SLICER_EXTENSION:BOOL=ON
        -DSlicer_DIR:PATH=${Slicer_DIR}
        -DSubversion_SVN_EXECUTABLE:FILEPATH=${Subversion_SVN_EXECUTABLE}
        -DGIT_EXECUTABLE:FILEPATH=${GIT_EXECUTABLE}
        -DUSE_GIT_PROTOCOL:BOOL=${USE_GIT_PROTOCOL}
       )
  endif()
  set(proj DTIProcess)
  ExternalProject_Add(${proj}
    SVN_REPOSITORY "https://www.nitrc.org/svn/dtiprocess/trunk"
    SVN_USERNAME slicerbot
    SVN_PASSWORD slicer
    SVN_REVISION -r 221
    SOURCE_DIR ${proj}
    BINARY_DIR ${proj}-build
    DEPENDS ${ITK_DEPEND} ${VTK_DEPEND} ${SlicerExecutionModel_DEPEND}
    CMAKE_GENERATOR ${gen}
    CMAKE_ARGS
      -DBUILD_dwiAtlas:BOOL=OFF
      -DBUILD_TESTING:BOOL=OFF
      ${CMAKE_OSX_EXTERNAL_PROJECT_ARGS}
      ${COMMON_EXTERNAL_PROJECT_ARGS}
      -DEXECUTABLES_ONLY:BOOL=${EXECUTABLES_ONLY}
      -DDTIProcess_SUPERBUILD:BOOL=FALSE
      -DITK_DIR:PATH=${ITK_DIR}
      -DVTK_DIR:PATH=${VTK_DIR}
      -DVTK_VERSION_MAJOR:STRING=${VTK_VERSION_MAJOR}
      -DSlicerExecutionModel_DIR:PATH=${SlicerExecutionModel_DIR}
      ${DTIProcessVarExt}
      -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
      -DINSTALL_RUNTIME_DESTINATION:STRING=${install_dir}
      -DINSTALL_LIBRARY_DESTINATION:STRING=${install_dir}
  )
ENDIF(COMPILE_EXTERNAL_DTIPROCESS)

OPTION(COMPILE_FIBERCOMPARE " Compile FiberCompare." OFF)
IF(COMPILE_FIBERCOMPARE)
  add_subdirectory( FiberCompare )
ENDIF(COMPILE_FIBERCOMPARE)
