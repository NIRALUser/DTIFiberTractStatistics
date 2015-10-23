#
# Test executable
#


if( COMPILE_EXTERNAL_DTIPROCESS )
  add_test(NAME fiberprocessTest COMMAND ${Slicer_LAUNCH_COMMAND} ${CMAKE_BINARY_DIR}/DTIProcess-install/${install_dir}/fiberprocess${fileextension} --help )
endif()
