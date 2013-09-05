#
# Test executable
#



if( COMPILE_DTIATLASFIBERANALYZER )
  add_test(NAME DTIAtlasFiberAnalyzerTest COMMAND ${Slicer_LAUNCH_COMMAND} $<TARGET_FILE:DTIAtlasFiberAnalyzer> --help )
endif()

if( COMPILE_FIBERCOMPARE )
  add_test(NAME FiberCompareTest COMMAND ${Slicer_LAUNCH_COMMAND} $<TARGET_FILE:FiberCompare> --help )
endif()
if( COMPILE_MERGERSTATWITHFIBER )
  add_test(NAME MergeStatWithFiberTest COMMAND ${Slicer_LAUNCH_COMMAND} $<TARGET_FILE:MergeStatWithFiber> --help )
endif()
if( COMPILE_DTITRACTSTAT )
  add_test(NAME dtitractstatTest COMMAND ${Slicer_LAUNCH_COMMAND} $<TARGET_FILE:dtitractstat> --help )
endif()
if( COMPILE_EXTERNAL_DTIPROCESS )
  add_test(NAME fiberprocessTest COMMAND ${Slicer_LAUNCH_COMMAND} ${CMAKE_CURRENT_BINARY_DIR}/../Applications/DTIProcess-build/bin/fiberprocess --help )
endif()

