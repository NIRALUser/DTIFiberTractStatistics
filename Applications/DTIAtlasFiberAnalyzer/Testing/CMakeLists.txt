#
# Test executable
#

add_test(NAME DTIAtlasFiberAnalyzerTest COMMAND ${Slicer_LAUNCH_COMMAND} $<TARGET_FILE:DTIAtlasFiberAnalyzer> --help )

