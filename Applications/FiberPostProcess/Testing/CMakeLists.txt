# project FiberPostProcess

# Add standard remote object stores to user's
# configuration.
list(APPEND ExternalData_URL_TEMPLATES
  "http://slicer.kitware.com/midas3/api/rest?method=midas.bitstream.download&checksum=%(hash)"
  )
set(TEMP_DIR ${FiberPostProcess_BINARY_DIR}/Testing/Temporary )
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Data )
set(Output ${TEMP_DIR}/output.vtk)
set(Output1 ${TEMP_DIR}/output1.vtk)
set(Output2 ${TEMP_DIR}/output2.vtk)
set(OutputCleaned ${TEMP_DIR}/output-cleaned.vtk)
set(VoidMask ${SOURCE_DIR}/inputnull.nrrd)

add_executable(FiberProcessingTest vtkTest.cxx)
target_link_libraries(FiberProcessingTest FiberPostProcessLib)

# Attribute at 0 Mask at 0 Crop at 0 Clean at 0 -> checkNan
ExternalData_Add_Test(FiberPostProcessData NAME FiberPostProcessTest0000 COMMAND $<TARGET_FILE:FiberProcessingTest>
  --compare
  DATA{Data/LeftMotor0000.vtk}
  ${Output}
  ModuleEntryPoint
  -i DATA{Data/LeftMotorInput.vtk}
  -o ${Output}
)

# Attribute at 0 Mask at 0 Crop at 0 Clean at 0 -> checkNan + noNan (remove fibers containing NaN values)
ExternalData_Add_Test(FiberPostProcessData NAME FiberPostProcessTestNoNan COMMAND $<TARGET_FILE:FiberProcessingTest>
  --compare
  DATA{Data/CorpusCallosumOccipitalSpleniumNoNan.vtk}
  ${Output}
  ModuleEntryPoint
  -i DATA{Data/CorpusCallosumOccipitalSpleniumInput.vtk}
  -o ${Output}
  --noNan
)

# Attribute at 0 Mask at 0 Crop at 0 Clean at 1 -> should return an error message
ExternalData_Add_Test(FiberPostProcessData NAME FiberPostProcessTest0001 COMMAND $<TARGET_FILE:FiberProcessingTest>
  --compare
  DATA{Data/LeftMotorInput.vtk}
  ${Output}
  ModuleEntryPoint
  -i DATA{Data/LeftMotorInput.vtk}
  -o ${Output}
  --clean
)
set_tests_properties( FiberPostProcessTest0001 PROPERTIES WILL_FAIL true )
# Attribute at 0 Mask at 0 Crop at 1 Clean at 0 -> should return an error message
ExternalData_Add_Test(FiberPostProcessData NAME FiberPostProcessTest0010 COMMAND $<TARGET_FILE:FiberProcessingTest>
  --compare
  DATA{Data/LeftMotorInput.vtk}
  ${Output}
  ModuleEntryPoint
  -i DATA{Data/LeftMotorInput.vtk}
  -o ${Output}
  --crop
)
set_tests_properties( FiberPostProcessTest0010 PROPERTIES WILL_FAIL true )
# Attribute at 0 Mask at 0 Crop at 1 Clean at 1 -> should return an error message
ExternalData_Add_Test(FiberPostProcessData NAME FiberPostProcessTest0011 COMMAND $<TARGET_FILE:FiberProcessingTest>
  --compare
  DATA{Data/LeftMotorInput.vtk}
  ${Output}
  ModuleEntryPoint
  -i DATA{Data/LeftMotorInput.vtk}
  -o ${Output}
  --crop
  --clean
)
set_tests_properties( FiberPostProcessTest0011 PROPERTIES WILL_FAIL true )
# Attribute at 0 Mask at 1 Crop at 0 Clean at 0 -> should return an error message
ExternalData_Add_Test(FiberPostProcessData NAME FiberPostProcessTest0100 COMMAND $<TARGET_FILE:FiberProcessingTest>
  --compare
  DATA{Data/LeftMotorInput.vtk}
  ${Output}
  ModuleEntryPoint
  -i DATA{Data/LeftMotorInput.vtk}
  -o ${Output}
  --mask
)
set_tests_properties( FiberPostProcessTest0100 PROPERTIES WILL_FAIL true )
# Attribute at 0 Mask at 1 Crop at 0 Clean at 1 -> should return an error message
ExternalData_Add_Test(FiberPostProcessData NAME FiberPostProcessTest0101 COMMAND $<TARGET_FILE:FiberProcessingTest>
  --compare
  DATA{Data/LeftMotorInput.vtk}
  ${Output}
  ModuleEntryPoint
  -i DATA{Data/LeftMotorInput.vtk}
  -o ${Output}
  --mask
  --clean
)
set_tests_properties( FiberPostProcessTest0101 PROPERTIES WILL_FAIL true )
# Attribute at 0 Mask at 1 Crop at 1 Clean at 0 -> should return an error message
ExternalData_Add_Test(FiberPostProcessData NAME FiberPostProcessTest0110 COMMAND $<TARGET_FILE:FiberProcessingTest>
  --compare
  DATA{Data/LeftMotorInput.vtk}
  ${Output}
  ModuleEntryPoint
  -i DATA{Data/LeftMotorInput.vtk}
  -o ${Output}
  --mask
  --clean
)
set_tests_properties( FiberPostProcessTest0110 PROPERTIES WILL_FAIL true )
# Attribute at 0 Mask at 1 Crop at 1 Clean at 1 -> should return an error message
ExternalData_Add_Test(FiberPostProcessData NAME FiberPostProcessTest0111 COMMAND $<TARGET_FILE:FiberProcessingTest>
  --compare
  DATA{Data/LeftMotorInput.vtk}
  ${Output}
  ModuleEntryPoint
  -i DATA{Data/LeftMotorInput.vtk}
  -o ${Output}
  --mask
  --crop
  --clean
)
set_tests_properties( FiberPostProcessTest0111 PROPERTIES WILL_FAIL true )
# Attribute at 1 Mask at 0 Crop at 0 Clean at 0 -> should return an error message
ExternalData_Add_Test(FiberPostProcessData NAME FiberPostProcessTest1000 COMMAND $<TARGET_FILE:FiberProcessingTest>
  --compare
  DATA{Data/LeftMotorInput.vtk}
  ${Output}
  ModuleEntryPoint
  -i DATA{Data/LeftMotorInput.vtk}
  -o ${Output}
  -m DATA{Data/LeftMotorMask.nrrd}
)
set_tests_properties( FiberPostProcessTest1000 PROPERTIES WILL_FAIL true )
# Attribute at 1 Mask at 0 Crop at 0 Clean at 1 -> should return an error message
ExternalData_Add_Test(FiberPostProcessData NAME FiberPostProcessTest1001 COMMAND $<TARGET_FILE:FiberProcessingTest>
  --compare
  DATA{Data/LeftMotorInput.vtk}
  ${Output}
  ModuleEntryPoint
  -i DATA{Data/LeftMotorInput.vtk}
  -o ${Output}
  -m DATA{Data/LeftMotorMask.nrrd}
  --clean
)
set_tests_properties( FiberPostProcessTest1001 PROPERTIES WILL_FAIL true )
# Attribute at 1 Mask at 0 Crop at 1 Clean at 0 -> crop functionality
ExternalData_Add_Test(FiberPostProcessData NAME FiberPostProcessTest1010 COMMAND $<TARGET_FILE:FiberProcessingTest>
  --compare
  DATA{Data/LeftMotor1010.vtk}
  ${Output}
  ModuleEntryPoint
  -i DATA{Data/LeftMotorInput.vtk}
  -o ${Output}
  -m DATA{Data/LeftMotorMask.nrrd}
  --crop
)
# Attribute at 1 Mask at 0 Crop at 1 Clean at 1 -> should return an error message
ExternalData_Add_Test(FiberPostProcessData NAME FiberPostProcessTest1011 COMMAND $<TARGET_FILE:FiberProcessingTest>
  --compare
  DATA{Data/LeftMotorInput.vtk}
  ${Output}
  ModuleEntryPoint
  -i DATA{Data/LeftMotorInput.vtk}
  -o ${Output}
  -m DATA{Data/LeftMotorMask.nrrd}
  --crop
  --clean
)
set_tests_properties( FiberPostProcessTest1011 PROPERTIES WILL_FAIL true )
# Attribute at 1 Mask at 1 Crop at 0 Clean at 0 -> mask functionality (only adds a data field, does not remove fibers)
ExternalData_Add_Test(FiberPostProcessData NAME FiberPostProcessTest1100 COMMAND $<TARGET_FILE:FiberProcessingTest>
  --compare
  DATA{Data/LeftMotor1100.vtk}
  ${Output}
  ModuleEntryPoint
  -i DATA{Data/LeftMotorInput.vtk}
  -o ${Output}
  -m DATA{Data/LeftMotorMask.nrrd}
  -t 0.5
  --thresholdMode above
  --mask
)
# Attribute at 1 Mask at 1 Crop at 0 Clean at 1 -> clean functionality (mask then take out the fibers that go too many times in the gray matter)
ExternalData_Add_Test(FiberPostProcessData NAME FiberPostProcessTest1101 COMMAND $<TARGET_FILE:FiberProcessingTest>
  --compare
  DATA{Data/LeftMotor1101.vtk}
  ${Output}
  ModuleEntryPoint
  -i DATA{Data/LeftMotorInput.vtk}
  -o ${Output}
  -m DATA{Data/LeftMotorMask.nrrd}
  -t 0.5
  --thresholdMode below
  --mask
  --clean
)
# Attribute at 1 Mask at 1 Crop at 1 Clean at 0 -> mask and crop
ExternalData_Add_Test(FiberPostProcessData NAME FiberPostProcessTest1110 COMMAND $<TARGET_FILE:FiberProcessingTest>
  --compare
  DATA{Data/LeftMotor1110.vtk}
  ${Output}
  ModuleEntryPoint
  -i DATA{Data/LeftMotorInput.vtk}
  -o ${Output}
  -m DATA{Data/LeftMotorMask.nrrd}
  -t 0.5
  --thresholdMode below
  --mask
  --crop
)
# Attribute at 1 Mask at 1 Crop at 1 Clean at 1 -> mask (take out fibers) and crop
ExternalData_Add_Test(FiberPostProcessData NAME FiberPostProcessTest1111 COMMAND $<TARGET_FILE:FiberProcessingTest>
  --compare
  DATA{Data/LeftMotor1111.vtk}
  ${Output}
  ModuleEntryPoint
  -i DATA{Data/LeftMotorInput.vtk}
  -o ${Output}
  -m DATA{Data/LeftMotorMask.nrrd}
  -t 0.5
  --thresholdMode below
  --mask
  --crop
  --clean
)

# CSV Tests

add_executable(CSVTestInitHeader csvTestInitHeader.cxx)
add_test(NAME FiberPostProcessCSVInitHeader COMMAND $<TARGET_FILE:CSVTestInitHeader>)

add_executable(CSVTestInitData csvTestInitData.cxx)
add_test(NAME FiberPostProcessCSVInitData COMMAND $<TARGET_FILE:CSVTestInitData>)

add_executable(CSVTestInitRowsId csvTestInitRowsId.cxx)
add_test(NAME FiberPostProcessCSVInitRowsId COMMAND $<TARGET_FILE:CSVTestInitRowsId>)

add_executable(CSVTestInitColsId csvTestInitColsId.cxx)
add_test(NAME FiberPostProcessCSVInitColsId COMMAND $<TARGET_FILE:CSVTestInitColsId>)

add_executable(CSVTestInitFull csvTestInitFull.cxx)
add_test(NAME FiberPostProcessCSVInitFull COMMAND $<TARGET_FILE:CSVTestInitFull>)

# NaN Cases Tests
add_executable(NaNFunction TestIsNanFunction.cxx)
add_test(NAME FiberPostProcessNaNFunction COMMAND $<TARGET_FILE:NaNFunction>)

# Add a build target to populate the real data.
ExternalData_Add_Target(FiberPostProcessData)
