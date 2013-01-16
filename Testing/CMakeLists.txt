#
# Test executable
#



if( COMPILE_DTIATLASFIBERANALYZER )
  add_test(NAME DTIAtlasFiberAnalyzerTest COMMAND ${DTIAtlasFiberAnalyzer_BUILD_DIR}/DTIAtlasFiberAnalyzer --help )
endif()

if( COMPILE_FIBERCOMPARE )
  add_test(NAME FiberCompareTest COMMAND ${FiberCompare_BUILD_DIR}/FiberCompare --help )
endif()
if( COMPILE_MERGERSTATWITHFIBER )
  add_test(NAME MergeStatWithFiberTest COMMAND ${MergeStatWithFiber_BUILD_DIR}/MergeStatWithFiber --help )
endif()
if( COMPILE_DTITRACTSTAT )
  add_test(NAME dtitractstatTest COMMAND ${dtitractstat_BUILD_DIR}/dtitractstat --help )
endif()
if( COMPILE_EXTERNAL_DTIPROCESS )
  add_test(NAME dtiprocessTest COMMAND ${dtiprocess_BUILD_DIR}/dtiprocess --help )
endif()

