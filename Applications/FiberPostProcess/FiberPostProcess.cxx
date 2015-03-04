#include <vtkSmoothPolyDataFilter.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <iostream>
#include <cstring>
#include <string>
#include <string.h>
#include "processing.h"
#include "FiberPostProcessCLP.h"
#include "utils.h"

int main( int argc , char* argv[] )
{
  PARSE_ARGS ;
  bool attributeFlag = false ;
  bool thresholdFlag = false ;
  bool lengthMatchFlag = false ;
  if( !attributeFileName.empty() )
  {
      attributeFlag = true ;
  }
  for( int i = 0 ; i < argc ; i++ )
  {
      if( !strcmp( argv[ i ] , "-t" ) || !strcmp( argv[ i ] , "--threshold" ) )
      {
          thresholdFlag = true ;
      }
  }
  for( int i = 0 ; i < argc ; i++ )
  {
      if( !strcmp( argv[ i ] , "--lengthMatch" ) || !strcmp( argv[ i ] , "-l" )  )
      {
          lengthMatchFlag = true ;
      }
  }
  processing FiberProcessing ;
  if( attributeFlag == false )
  {
      if( crop == true || clean == true || mask == true )
      {
          std::cerr << "No attribute file detected. --crop, --clean, --mask requires an attribute file to be used. Please use --attributeFile or -m to input an attribute file or consult the help for more details." << std::endl ;
          return EXIT_FAILURE ;
      }
  }
  else if( mask == false )
  {
          if( clean == true )
          {
              std::cerr << "--clean requires --mask to be used. Please consult the help for more details." << std::endl ;
              return EXIT_FAILURE ;
          }
  }
  FiberProcessing.SetInputFileName( inputFiberFileName ) ;
  FiberProcessing.SetOutputFileName( outputFiberFileName ) ;
  FiberProcessing.SetVisualisation( visualisationFlag ) ;
  FiberProcessing.SetAttributeFileName( attributeFileName ) ;
  FiberProcessing.SetThreshold( threshold ) ;
  FiberProcessing.SetThresholdFlag( thresholdFlag ) ;
  FiberProcessing.SetAttributeFlag( attributeFlag ) ;
  FiberProcessing.SetMaskFlag( mask ) ;
  FiberProcessing.SetCropFlag( crop ) ;
  FiberProcessing.SetCleanFlag( clean ) ;
  FiberProcessing.SetThresholdMode ( thresholdMode ) ;
  FiberProcessing.SetNoNanFlag( noNan ) ;
  FiberProcessing.SetLengthMatchFlag( lengthMatchFlag ) ;
  FiberProcessing.SetLengthMatchFiber( lengthMatch ) ;
  FiberProcessing.run() ;
  return EXIT_SUCCESS ;
}
