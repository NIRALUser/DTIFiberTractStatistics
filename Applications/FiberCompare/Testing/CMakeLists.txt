#
# Test executable
#


add_test(NAME FiberCompareTest COMMAND ${Slicer_LAUNCH_COMMAND} $<TARGET_FILE:FiberCompare> --help )

