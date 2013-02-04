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

OPTION(COMPILE_EXTERNAL_DTIPROCESS " Compile external DTIProcess package (for fiberprocess application)." OFF)
IF(COMPILE_EXTERNAL_DTIPROCESS)
  if( NOT DEFINED ITKV3_COMPATIBILITY OR NOT ${ITKV3_COMPATIBILITY}  )
    message( WARNING "Choose ITKv4 compiled with ITKV3_COMPATIBILITY set to ON (or GenerateCLP compiled against such an ITK version). If not, you may have compilation errors" )
  endif()
  set(proj DTIProcess)
  ExternalProject_Add(${proj}
    SVN_REPOSITORY "https://www.nitrc.org/svn/dtiprocess/branches/Slicer4Extension"
    SVN_USERNAME slicerbot
    SVN_PASSWORD slicer
    SVN_REVISION -r 150
    SOURCE_DIR ${proj}
    BINARY_DIR ${proj}-build
    DEPENDS ${ITK_DEPEND} ${VTK_DEPEND} ${SlicerExecutionModel_DEPEND}
    CMAKE_GENERATOR ${gen}
    CMAKE_ARGS
      -DBUILD_dwiAtlas:BOOL=OFF
      ${CMAKE_OSX_EXTERNAL_PROJECT_ARGS}
      ${COMMON_EXTERNAL_PROJECT_ARGS}
    INSTALL_COMMAND ""
  )
ENDIF(COMPILE_EXTERNAL_DTIPROCESS)

OPTION(COMPILE_FIBERCOMPARE " Compile FiberCompare." OFF)
IF(COMPILE_FIBERCOMPARE)
  add_subdirectory( FiberCompare )
ENDIF(COMPILE_FIBERCOMPARE)
