#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkCell.h>
#include <vtkCellData.h>
#include <vtkCellArray.h>
#include <iostream>
#include <cstring>
#include "processing.h"

#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itksys/SystemTools.hxx>

#include <vtkImageWriter.h>
#include <itkPoint.h>
#include <itkLinearInterpolateImageFunction.h>

#include <vtkCellArray.h>
#include <vtkDoubleArray.h>

#include <vtkPointData.h>

#include <vtkPolyLine.h>

#include <fstream>
#include <iomanip>
#include <sstream>

#include "csv.h"
#include "utils.h"

#include <vtkMath.h>
#include <vtkCellLocator.h>

#include <vtkZLibDataCompressor.h>

/* allocate memory for an nrow x ncol matrix */
template< class TReal>
TReal **create_matrix( long nrow, long ncol )
{
    typedef TReal* TRealPointer ;
    TReal **m = new TRealPointer[ nrow ] ;
    TReal* block = ( TReal* ) calloc( nrow*ncol , sizeof( TReal ) ) ;
    m[ 0 ] = block ;
    for( int row = 1 ; row < nrow ; ++row )
    {
        m[ row ] = &block[ row * ncol ] ;
    }
    return m ;
}

/* free a TReal matric allocated with matrix () */
template< class TReal >
void free_matrix( TReal **m )
{
    free( m[0] ) ;
    delete[] m ;
}

processing::processing()
{
}

void processing::SetInputFileName( std::string inputFiberFileName )
{
    InputFileName = inputFiberFileName ;
}
void processing::SetOutputFileName( std::string outputFiberFileName )
{
    OutputFileName = outputFiberFileName ;
}
void processing::SetVisualisation( int visualisationFlag )
{
    FlagVisualize = visualisationFlag ;
}
void processing::SetAttributeFileName( std::string attributeFileName )
{
    AttributeFileName = attributeFileName ;
}
void processing::SetThreshold( float threshold )
{
    Threshold = threshold ;
}
void processing::processing::SetThresholdFlag( int thresholdFlag )
{
    FlagThreshold = thresholdFlag ;
}

void processing::SetAttributeFlag( int attributeFlag )
{
    FlagAttribute = attributeFlag ;
}

void processing::SetMaskFlag( int maskFlag )
{
    FlagMask = maskFlag ;
}

void processing::SetCropFlag( int cropFlag )
{
    FlagCrop = cropFlag ;
}
void processing::SetCleanFlag( int cleanFlag )
{
    FlagClean = cleanFlag ;
}
void processing::SetThresholdMode( std::string thresholdMode )
{
    ThresholdMode = thresholdMode ;
}
void processing::SetNoNanFlag( int noNanFlag )
{
    FlagNoNan = noNanFlag ;
}
void processing::SetLengthMatchFlag( int lengthMatchFlag )
{
    FlagLengthMatch = lengthMatchFlag ;
}
void processing::SetLengthMatchFiber(std::string lengthMatchFiber )
{
    LengthMatchFiber = lengthMatchFiber ;
}
void processing::WriteLogFile( processing::fileNameStruct fileName , std::vector< std::vector< float> > vecPointData ,
                               vtkSmartPointer< vtkPolyData > fiberFile , std::vector< float > cumul , std::vector< float > average )
{
    std::vector< std::vector < std::string > > data = ConvertArray( vecPointData ) ;
    csv csvFile ;
    std::vector< std::vector < std::string > > headerData ;
    std::vector< std::string > buff ;
    char logFileName[] = "log.csv" ;
    buff.push_back( "Fiber file input" ) ;
    buff.push_back( fileName.input ) ;
    headerData.push_back( buff ) ;
    buff.clear() ;
    buff.push_back( "Mask input" ) ;
    buff.push_back( fileName.mask ) ;
    headerData.push_back( buff ) ;
    buff.clear() ;
    buff.push_back( "Fiber file output" ) ;
    buff.push_back( fileName.output ) ;
    headerData.push_back( buff ) ;
    buff.clear() ;
    buff.push_back( "Visualizable fiber file" ) ;
    buff.push_back( fileName.visu ) ;
    headerData.push_back( buff ) ;
    buff.clear() ;
    buff.push_back( "Number of fibers" ) ;
    buff.push_back( Convert( vecPointData.size() ) ) ;
    headerData.push_back( buff ) ;
    buff.clear() ;
    if( FlagThreshold == true )
    {   buff.push_back( "Threshold = " ) ;
        buff.push_back( Convert( Threshold ) ) ;
        headerData.push_back( buff ) ;
        buff.clear() ;
    }
    for( int i = 0 ; i < vecPointData.size() ; i++ )
    {
        data[ i ].insert( data[ i ].begin() , Convert( cumul[ i ] ) ) ;
    }
    for( int i = 0 ; i < vecPointData.size() ; i++ )
    {
        data[ i ].insert( data[ i ].begin() , Convert( average[ i ] ) ) ;
    }
    csvFile.initHeader( headerData ) ;
    csvFile.initData( data ) ;
    std::vector< std::string > rowsId , colsId ;
    for( int i = 0 ; i < vecPointData.size() ; i++ )
    {
        rowsId.push_back( "FIBER " + Convert( i ) ) ;
    }
    int max = FindMaxNbOfCols( data ) ;
    colsId.push_back( "FIBER ID" ) ;
    colsId.push_back( "AVERAGE VALUE" ) ;
    colsId.push_back( "CUMUL VALUE" ) ;
    for( int i = 0 ; i < max ; i++ )
    {
        colsId.push_back( "POINT " + Convert( i ) ) ;
    }
    csvFile.initRowsId( rowsId ) ;
    csvFile.initColsId( colsId ) ;
    csvFile.write( logFileName ) ;
}

int processing::FindMaxNbOfPoints( vtkSmartPointer< vtkPolyData > polyData )
{
    polyData->GetLines()->InitTraversal() ;
    vtkSmartPointer< vtkIdList > idList = vtkSmartPointer< vtkIdList >::New() ;
    int max = 0 ;
    while( polyData->GetLines()->GetNextCell( idList ) )
    {
        if( idList->GetNumberOfIds() > max )
        {
            max = idList->GetNumberOfIds() ;
        }
    }
    return max ;
}
template <class T>
vtkSmartPointer< vtkPolyData > processing::ReadFiberFile( T reader , std::string fiberFile )
{
    vtkSmartPointer< vtkPolyData > PolyData ;
    reader->SetFileName( fiberFile.c_str() ) ;
    PolyData=reader->GetOutput() ;
    reader->Update() ;
    return PolyData ;
}

int processing::WriteFiberFile( std::string encoding , std::string extension , const char* outputFileName ,
                int compressionLevel , vtkSmartPointer< vtkPolyData > readerPolyData )
{
    if( extension == "vtp" )
        {
            vtkSmartPointer< vtkXMLPolyDataWriter > writer = vtkSmartPointer< vtkXMLPolyDataWriter >::New() ;
            writer->SetFileName( outputFileName ) ;
            writer->SetInputData( readerPolyData ) ;
            vtkZLibDataCompressor *compressor = dynamic_cast< vtkZLibDataCompressor* > ( writer->GetCompressor() ) ;
            if( compressor )
            {
                compressor->SetCompressionLevel( compressionLevel ) ;
            }
            if( encoding == "binary" )
            {
                writer->SetDataModeToBinary() ;
            }
            else if( encoding == "appended" )
            {
                writer->SetDataModeToAppended() ;
            }
            else if( encoding == "ascii" )
            {
                writer->SetDataModeToAscii() ;
            }
            else // should not arrive here. tested in main before already and program should exit at that time.
            {
                return -1 ;
            }
            writer->Update() ;
            return writer->GetErrorCode() ;
        }
        else
        {
            vtkSmartPointer< vtkPolyDataWriter > writer = vtkSmartPointer< vtkPolyDataWriter >::New() ;
            writer->SetFileName( outputFileName ) ;
            writer->SetInputData( readerPolyData ) ;
            if( encoding =="binary" )
            {
                writer->SetFileTypeToBinary();
            }
            else if( encoding == "ascii" )
            {
                writer->SetFileTypeToASCII() ;
            }
            else
            {
                return -1 ;
            }

            writer->Update() ;
            return writer->GetErrorCode() ;
        }
}

/*std::vector< int > processing::CheckNaN( vtkSmartPointer< vtkPolyData > polyData , std::vector< std::vector< float > > vecPointData )
{
    std::vector< int > cellData ;
    vtkCellArray* lines = polyData->GetLines() ;
    vtkIdType* ids ;
    vtkIdType numberOfPoints ;
    lines->InitTraversal() ;
    for( int fiberId = 0 ; lines->GetNextCell( numberOfPoints , ids ) ; fiberId++ )
    {
        int NanFiber = 0 ;
        for( int pointId = 0 ; pointId < numberOfPoints ; pointId ++ )
        {
            if( std::isnan( vecPointData[ fiberId ][ pointId ] ) != 0 )
            {
                NanFiber = 1 ;
            }
        }
        cellData.push_back( NanFiber ) ;
    }
    return cellData ;
}*/

vtkSmartPointer< vtkPolyData > processing::CheckNaN( vtkSmartPointer< vtkPolyData > polyData ) // compute the fa on the tensors
{
    vtkSmartPointer< vtkPolyData > cleanedPolyData = polyData ;
    vtkSmartPointer< vtkDoubleArray > cellData = vtkSmartPointer< vtkDoubleArray >::New() ;
    cellData->SetNumberOfComponents( 1 ) ;
    cellData->SetName( "NaNCases" ) ;
    int nbArrays = polyData->GetPointData()->GetNumberOfArrays() ;
    std::vector< int > NanFiberId ;//std::cout<<"nbArrays = "<<nbArrays<<std::endl ;
    /*for( int i = 0 ; i < nbArrays ; i++ )
     {*/
    /*if( strcmp( polyData->GetPointData()->GetArray( i )->GetName() , "tensors" ) == 0 )
         {*/
    /*int nbComponents, nbTuples ;
             nbComponents = polyData->GetPointData()->GetArray( i )->GetNumberOfComponents() ;
             nbTuples = polyData->GetPointData()->GetArray( i )->GetNumberOfTuples() ;*/
    //for( int j = 0 ; j < polyData->GetNumberOfPoints() ; j++ )
    int nbLines = polyData->GetNumberOfLines() ;
    int tmpPoint = 0;
    for( int i = 0 ; i < nbLines ; i++ )
    {
        int nbPoints = polyData->GetCell( i )->GetNumberOfPoints() ;
        for( int j = 0 ; j < nbPoints ; j++ )
        {
            //std::cout<<j<<std::endl;
            double eigenValues[ 3 ] ;
            double **eigenVectors = create_matrix< double >( 3, 3 ) ;
            double **tensors = create_matrix< double >( 3, 3 ) ;
            tensors[ 0 ][ 0 ] = polyData->GetPointData()->GetTensors()->GetComponent( tmpPoint , 0 ) ;
            tensors[ 0 ][ 1 ] = polyData->GetPointData()->GetTensors()->GetComponent( tmpPoint , 1 ) ;
            tensors[ 0 ][ 2 ] = polyData->GetPointData()->GetTensors()->GetComponent( tmpPoint , 2 ) ;

            tensors[ 1 ][ 0 ] = polyData->GetPointData()->GetTensors()->GetComponent( tmpPoint , 3 ) ;
            tensors[ 1 ][ 1 ] = polyData->GetPointData()->GetTensors()->GetComponent( tmpPoint , 4 ) ;
            tensors[ 1 ][ 2 ] = polyData->GetPointData()->GetTensors()->GetComponent( tmpPoint , 5 ) ;

            tensors[ 2 ][ 0 ] = polyData->GetPointData()->GetTensors()->GetComponent( tmpPoint , 6 ) ;
            tensors[ 2 ][ 1 ] = polyData->GetPointData()->GetTensors()->GetComponent( tmpPoint , 7 ) ;
            tensors[ 2 ][ 2 ] = polyData->GetPointData()->GetTensors()->GetComponent( tmpPoint , 8 ) ;
            vtkMath::Jacobi( tensors , eigenValues , eigenVectors ) ;
            for( int  k = 0 ; k < 3 ; k ++ )
            {
                //std::cout<<"eigenValues["<<k<<"]="<<eigenValues [ k ]<<" ";
                if( eigenValues [ k ] <= 0 )
                {//std::cout<<j<<std::endl;
                    NanFiberId.push_back( i ) ;
                }
            }//std::cout<<std::endl;
            free_matrix ( tensors ) ;
            //}
            //}
            /* vtkSmartPointer< vtkDataArray > dataArray = polyData->GetPointData()->GetArray( i ) ;
         std::vector< std::vector< float > > dataVector = convertDataToVector( dataArray ) ;
         CheckNaN( cleanedPolyData , dataVector ) ;*/
            tmpPoint ++ ;
        }
    }
    vtkCellArray* lines = polyData->GetLines() ;
    vtkIdType numberOfPoints ;
    vtkIdType* ids ;
    lines->InitTraversal() ;
    //std::vector < int > NanFiberId ;
    //for(int i = 0 ; i < NanFiberPointId.size() ; i++ )
    //{
     //   std::cout<<locateFiber->FindCell( polyData->GetPoint())<<std::endl;
        /*NanFiberId.push_back(locateFiber->FindCell( polyData->GetPoint(NanFiberId[i])));*/
    //}
    for( int i = 0 ; lines->GetNextCell( numberOfPoints , ids ) ; i++ )
    {
        if( std::find(NanFiberId.begin(), NanFiberId.end(), i ) != NanFiberId.end() )
         {
             cellData->InsertNextValue( 1 ) ;
         }
         else
         {
             cellData->InsertNextValue( 0 ) ;
         }
    }
    cleanedPolyData->GetCellData()->AddArray( cellData ) ;
    return cleanedPolyData ;
}



std::vector< std::vector< float > > processing::convertDataToVector(vtkSmartPointer<vtkDataArray> array )
{
    std::vector< std::vector< float > > dataVector ;
    int nbComponents = array->GetNumberOfComponents() ;
    int nbTuples = array->GetNumberOfTuples() ;
    for( int i = 0 ; i < nbTuples ; i++ )
    {
        std::vector< float > tmp ;
        for( int j = 0 ; j < nbComponents ; j++ )
        {
            tmp.push_back( array->GetComponent( i , j ) ) ;
        }
        dataVector.push_back( tmp ) ;
    }
    return dataVector ;
}

std::vector< std::vector< float > > processing::ApplyMaskToFiber(vtkSmartPointer< vtkPolyData > polyData )
{
    typedef itk::Image< float , 3 > ImageType ;
    typedef itk::ImageFileReader< ImageType > ReaderType ;
    ReaderType::Pointer readerMask = ReaderType::New() ;
    readerMask->SetFileName( AttributeFileName ) ;
    readerMask->Update() ;
    ImageType::Pointer maskImage = ImageType::New() ;
    maskImage = readerMask->GetOutput() ;
    typedef itk::Index< 3 > IndexType ;
    ImageType::IndexType index ;
    typedef itk::LinearInterpolateImageFunction< ImageType , double > InterpolationType ;
    InterpolationType::Pointer interp = InterpolationType::New() ;
    interp->SetInputImage( maskImage ) ;
    itk::Point< double , 3 > p ;
    const int nfib = polyData->GetNumberOfCells() ;
    std::vector< std::vector< float > > pointData ;
    for( int i = 0 ; i < nfib ; ++i )
    {
        std::vector < float > fib ;
        vtkSmartPointer< vtkCell > fiber = polyData->GetCell( i ) ;
        vtkSmartPointer< vtkPoints > fiberPoints = fiber->GetPoints() ;
        for( int j = 0 ; j < fiberPoints->GetNumberOfPoints() ; ++j )
        {
            double* coordinates = fiberPoints->GetPoint( j ) ;
            /* Flip the coordinates */
            coordinates[0] = - coordinates[0] ;
            coordinates[1] = - coordinates[1] ;
            p[0] = coordinates[0] ;
            p[1] = coordinates[1] ;
            p[2] = coordinates[2] ;
            maskImage->TransformPhysicalPointToIndex( p , index ) ;
            //Checks if the index is inside the image boundaries
            if( !interp->IsInsideBuffer( index ) )
            {
                std::cout << "Index out of bounds" << std::endl ;
            }
            else
            {
            }
            ImageType::PixelType pixel = maskImage->GetPixel( index ) ;
            fib.push_back( pixel ) ;
        }
        pointData.push_back( fib ) ;
    }
    return pointData ;
}

vtkSmartPointer< vtkPolyData > processing::CropFiber( vtkSmartPointer< vtkPolyData > polyData , std::vector< std::vector< float > > & vecPointData )
{
    float epsilon = 0.000001f ;
    vtkSmartPointer< vtkDoubleArray > newTensors = vtkSmartPointer< vtkDoubleArray >::New() ;
    newTensors->SetName( "tensors" ) ;
    newTensors->SetNumberOfComponents( 9 ) ;
    vtkSmartPointer< vtkDataArray > tensors = polyData->GetPointData()->GetTensors() ;
    vtkSmartPointer< vtkPolyData > cropFiber = vtkSmartPointer< vtkPolyData >::New() ;
    vtkSmartPointer<vtkPoints> NewPoints = vtkSmartPointer<vtkPoints>::New() ;
    std::vector< std::vector< float > > pointData ;
    vtkSmartPointer<vtkCellArray> NewLines = vtkSmartPointer<vtkCellArray>::New() ;
    vtkPoints* Points = polyData->GetPoints() ;
    vtkCellArray* Lines = polyData->GetLines() ;
    vtkIdType* Ids ;
    vtkIdType NumberOfPoints ;
    vtkIdType NewId = 0 ;
    Lines->InitTraversal() ;
    const int nfib = polyData->GetNumberOfCells() ;
    for( int fiberId = 0 ; Lines->GetNextCell( NumberOfPoints , Ids ) ; fiberId++ )
    {
        int endOfFiber = 0 ;
        std::vector< float > pointDataPerFiber ;
        vtkSmartPointer< vtkPolyLine > NewLine = vtkSmartPointer< vtkPolyLine >::New() ;
        int pointId = 0 ;
        while( vecPointData[ fiberId ][ pointId ] == 0 )
        {
            pointId++ ;
        }
        if( pointId >= NumberOfPoints )
        {
            continue ;
        }
        else
        {
            NumberOfPoints = NumberOfPoints - pointId ;
        }
        while( vecPointData[ fiberId ][ vecPointData[ fiberId ].size() - endOfFiber -1 ] < epsilon && endOfFiber < vecPointData[ fiberId ].size() ) //TODO: add to the XML
        {
            endOfFiber++ ;
        }
        NewLine->GetPointIds()->SetNumberOfIds( NumberOfPoints - endOfFiber ) ;
        int location = 0 ;
        int compteur = 0 ;
        while( pointId < vecPointData[ fiberId ].size() - endOfFiber )
        {
            newTensors->InsertNextTuple( tensors->GetTuple9( GetPointId( fiberId , pointId , polyData ) ) ) ;
            pointDataPerFiber.push_back( vecPointData[ fiberId ][ pointId ] ) ;
            NewPoints->InsertNextPoint( Points->GetPoint( Ids[ pointId ] ) ) ;
            NewLine->GetPointIds()->SetId( location , NewId ) ;
            NewId ++ ;
            pointId ++ ;
            location++ ;
            compteur++;
        }
        pointData.push_back( pointDataPerFiber ) ;
        NewLines->InsertNextCell( NewLine ) ;
    }
    cropFiber->SetPoints( NewPoints ) ;
    cropFiber->SetLines( NewLines ) ;
    for( int i = 0 ; i < polyData->GetCellData()->GetNumberOfArrays() ; i++ )
    {
        cropFiber->GetCellData()->AddArray( polyData->GetCellData()->GetAbstractArray( i ) ) ;
    }
    cropFiber->GetPointData()->AddArray( CreatePointData( pointData , "InsideMask" ) ) ;
    cropFiber->GetPointData()->SetTensors( newTensors ) ;
    return cropFiber ;
}

void processing::deletePoint( vtkSmartPointer< vtkPoints > fiberPoints , int pointId )
{
    vtkSmartPointer<vtkPoints> newPoints = vtkSmartPointer<vtkPoints>::New() ;
    for(vtkIdType i = 0 ; i < fiberPoints->GetNumberOfPoints() ; i++ )
    {
        if( i != pointId )
        {
            double p[3];
            fiberPoints->GetPoint( i , p ) ;
            newPoints->InsertNextPoint( p ) ;
        }
    }
    fiberPoints->ShallowCopy( newPoints ) ;
}

vtkSmartPointer< vtkDoubleArray > processing::CreatePointData( std::vector< std::vector< float> > vecPointData , const char* fieldName )
{
    vtkSmartPointer< vtkDoubleArray > pointData = vtkSmartPointer< vtkDoubleArray >::New() ;
    for( int i = 0 ; i < vecPointData.size() ; i++ )
    {
        for( int j = 0 ; j < vecPointData[i].size() ; j++ )
        {
            pointData->SetNumberOfComponents( 1 ) ;
            pointData->SetName( fieldName ) ;
            pointData->InsertNextValue( vecPointData[ i ][ j ] ) ;
        }
    }
    return pointData ;
}

vtkSmartPointer< vtkDoubleArray > processing::CreateCellData(std::vector< float > vecCellData , const char *fieldName )
{
    vtkSmartPointer< vtkDoubleArray > cellData = vtkSmartPointer< vtkDoubleArray >::New() ;
    for( int i = 0 ; i < vecCellData.size() ; i++ )
    {
        cellData->SetNumberOfComponents( 1 ) ;
        cellData->SetName( fieldName ) ;
        cellData->InsertNextValue( vecCellData[ i ] ) ;
    }
    return cellData ;
}

std::vector< float > processing::CumulValuePerFiber( std::vector< std::vector< float> > pointData )
{
    std::vector< float > cumul ;
    for(int fiberId = 0 ; fiberId < pointData.size() ; fiberId++ )
    {
        float tmp = 0;
        for( int pointId = 0 ; pointId < pointData[ fiberId ].size() ; pointId++ )
        {
            tmp = tmp + pointData[ fiberId ][ pointId ] ;
        }
        cumul.push_back( tmp ) ;
    }
    return cumul ;
}

std::vector< float > processing::AverageValuePerFiber( std::vector< std::vector< float> > pointData )
{
    std::vector< float > cumul ;
    for(int fiberId = 0 ; fiberId < pointData.size() ; fiberId++ )
    {
        float tmp = 0;
        for( int pointId = 0 ; pointId < pointData[ fiberId ].size() ; pointId++ )
        {
            tmp = tmp + pointData[ fiberId ][ pointId ] ;
        }
        cumul.push_back( tmp/pointData[ fiberId ].size() ) ;
    }
    return cumul ;
}

vtkSmartPointer< vtkPolyData > processing::AddPointData( vtkSmartPointer< vtkPolyData > polyData )
{
    const int nfib = polyData->GetNumberOfCells() ;
    vtkSmartPointer< vtkDoubleArray > pointData = vtkSmartPointer< vtkDoubleArray >::New() ;
    for( int i = 0 ; i < nfib ; ++i )
    {
        vtkSmartPointer< vtkCell > fiber = polyData->GetCell( i ) ;
        vtkSmartPointer< vtkPoints > fiberPoints = fiber->GetPoints() ;
        for( int j = 0 ; j < fiberPoints->GetNumberOfPoints() ; ++j )
        {
            pointData->SetNumberOfComponents( 1 ) ;
            pointData->SetName( "InsideMask" ) ;
            pointData->InsertNextValue( 0 ) ;
        }

    }
    polyData->GetPointData()->AddArray( pointData ) ;
    return polyData;
}

vtkSmartPointer< vtkPolyData > processing::CleanFiber( vtkSmartPointer< vtkPolyData > polyData , float threshold )
{
    vtkSmartPointer< vtkDoubleArray > newTensors = vtkSmartPointer< vtkDoubleArray >::New() ;
    newTensors->SetName( "tensors" ) ;
    newTensors->SetNumberOfComponents( 9 ) ;
    vtkSmartPointer< vtkDataArray > tensors = polyData->GetPointData()->GetArray( "tensors" ) ;
    vtkSmartPointer< vtkPolyData > newPolyData = vtkSmartPointer< vtkPolyData >::New() ;
    vtkSmartPointer<vtkPoints> newPoints = vtkSmartPointer<vtkPoints>::New() ;
    vtkSmartPointer<vtkCellArray> newLines = vtkSmartPointer<vtkCellArray>::New() ;
    vtkPoints* points = polyData->GetPoints() ;
    vtkCellArray* lines = polyData->GetLines() ;
    vtkIdType* ids ;
    int newId = 0 ;
    vtkSmartPointer< vtkDoubleArray > fiberReadArray = vtkSmartPointer< vtkDoubleArray >::New() ;
    fiberReadArray = vtkDoubleArray::SafeDownCast( polyData->GetCellData()->GetArray( "AverageValue" ) ) ;
    vtkSmartPointer< vtkDoubleArray > cellData = vtkSmartPointer< vtkDoubleArray >::New() ;
    vtkSmartPointer< vtkDoubleArray > removeFiber = vtkSmartPointer< vtkDoubleArray >::New() ;
    removeFiber->SetName( "removeFiber" ) ;
    cellData = vtkDoubleArray::SafeDownCast( polyData->GetCellData()->GetArray( "NanCases" ) ) ;
    lines->InitTraversal() ;
    int nbLines = polyData->GetNumberOfLines() ;
    for( int i = 0 ; i < nbLines ; i++ )
    {
        int nbPoints = polyData->GetCell( i )->GetNumberOfPoints() ;
        for( int j = 0 ; j < nbPoints ; j++ )
        {
            newTensors->InsertNextTuple( tensors->GetTuple9( GetPointId( i , j , polyData ) ) ) ;
        }
        vtkSmartPointer< vtkPolyLine > newLine = vtkSmartPointer< vtkPolyLine >::New() ;
        newLine->GetPointIds()->SetNumberOfIds( nbPoints ) ;
        if( ThresholdMode == "above" )
        {
            if( fiberReadArray->GetValue( i ) >= threshold /*&& cellData->GetValue( i ) == 0*/ )
            {
                removeFiber->InsertNextValue( 1 ) ;
            }
            else
            {
                for( int j = 0 ; j < nbPoints ; j ++ )
                {
                    newPoints->InsertNextPoint( points->GetPoint( polyData->GetCell( i )->GetPointId( j ) ) ) ;
                    newLine->GetPointIds()->SetId( j , newId ) ;
                    newId++ ;
                }
                newLines->InsertNextCell( newLine ) ;
                removeFiber->InsertNextValue( 0 ) ;
            }
        }
        else
        {
            if( fiberReadArray->GetValue( i ) <= threshold /*&& cellData->GetValue( i ) == 0*/ )
            {
                removeFiber->InsertNextValue( 1 ) ;
            }
            else
            {
                for( int j = 0 ; j < nbPoints ; j ++ )
                {
                    newPoints->InsertNextPoint( points->GetPoint( polyData->GetCell( i )->GetPointId( j ) ) ) ;
                    newLine->GetPointIds()->SetId( j , newId ) ;
                    newId++ ;
                }
                newLines->InsertNextCell( newLine ) ;
                removeFiber->InsertNextValue( 0 ) ;
            }
        }
    }
    newPolyData->SetPoints( newPoints ) ;
    newPolyData->SetLines( newLines ) ;
    newPolyData->GetPointData()->SetTensors( newTensors ) ;
    polyData->GetCellData()->AddArray( removeFiber ) ;
    if( FlagClean == true )
    {
        return newPolyData ;
    }
    return polyData ;
}

int processing::GetPointId( int fiberId, int pointId , vtkSmartPointer< vtkPolyData > polyData )
{
    return polyData->GetCell( fiberId )->GetPointId( pointId ) ;
}

std::vector< std::string > processing::ThresholdPolyData(vtkSmartPointer< vtkPolyData > polyData )
{
    std::vector< std::string > fiberStatus ;
    vtkCellArray* lines = polyData->GetLines() ;
    vtkIdType numberOfPoints ;
    vtkIdType* ids ;
    vtkSmartPointer< vtkDoubleArray > fiberReadArray = vtkSmartPointer< vtkDoubleArray >::New() ;
    fiberReadArray = vtkDoubleArray::SafeDownCast( polyData->GetCellData()->GetArray( "CumulativeValue" ) ) ;
    lines->InitTraversal() ;
    for( int i = 0 ; lines->GetNextCell( numberOfPoints , ids ) ; i++ )
    {
        if( fiberReadArray->GetValue( i ) <= Threshold )
        {
            fiberStatus.push_back( "0" ) ;
        }
        else
        {
            fiberStatus.push_back( "1" ) ;
        }
    }
    return fiberStatus ;
}

vtkSmartPointer< vtkPolyData > processing::CreateVisuFiber(vtkSmartPointer< vtkPolyData > polyData )
{
    vtkSmartPointer< vtkPolyData > NewPolyData = vtkSmartPointer< vtkPolyData >::New() ;
    vtkSmartPointer<vtkPoints> NewPoints = vtkSmartPointer<vtkPoints>::New() ;
    vtkSmartPointer<vtkCellArray> NewLines = vtkSmartPointer<vtkCellArray>::New() ;
    vtkSmartPointer< vtkDoubleArray > pointData = vtkSmartPointer< vtkDoubleArray >::New() ;
    vtkPoints* Points = polyData->GetPoints() ;
    vtkCellArray* Lines = polyData->GetLines() ;
    vtkIdType* Ids ;
    vtkIdType NumberOfPoints ;
    int NewId = 0 ;
    Lines->InitTraversal() ;
    const int nfib = polyData->GetNumberOfCells() ;
    for( int i = 0 ; Lines->GetNextCell( NumberOfPoints , Ids ) ; i++ )
    {
        vtkSmartPointer< vtkPolyLine > NewLine = vtkSmartPointer< vtkPolyLine >::New() ;
        NewLine->GetPointIds()->SetNumberOfIds( NumberOfPoints ) ;
        for( int j = 0 ; j < NumberOfPoints ; j ++ )
        {
            NewPoints->InsertNextPoint( Points->GetPoint( Ids[ j ] ) ) ;
            NewLine->GetPointIds()->SetId( j , NewId ) ;
            NewId++ ;
        }
        NewLines->InsertNextCell( NewLine ) ;
    }
    NewPolyData->SetPoints( NewPoints ) ;
    NewPolyData->SetLines( NewLines ) ;
    NewPolyData->GetCellData()->AddArray( polyData->GetCellData()->GetAbstractArray( "CumulativeValue") ) ;
    NewPolyData->GetCellData()->AddArray( polyData->GetCellData()->GetAbstractArray( "AverageValue") ) ;
    NewPolyData->GetPointData()->AddArray( polyData->GetPointData()->GetAbstractArray( "InsideMask" ) ) ;
    return NewPolyData ;
}

vtkSmartPointer<vtkPolyData> processing::RemoveNanFibers( vtkSmartPointer< vtkPolyData > polyData )
{
    vtkSmartPointer<vtkPolyData> FinalPolyData=vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkFloatArray> NewTensors=vtkSmartPointer<vtkFloatArray>::New();
    vtkSmartPointer<vtkPoints> NewPoints=vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkCellArray> NewLines=vtkSmartPointer<vtkCellArray>::New();
    NewTensors->SetNumberOfComponents(9);
    vtkDataArray* Tensors=polyData->GetPointData()->GetTensors();
    vtkPoints* Points=polyData->GetPoints();
    vtkCellArray* Lines=polyData->GetLines();
    vtkIdType* Ids;
    vtkIdType NumberOfPoints;
    int NewId=0;
    Lines->InitTraversal();
    vtkSmartPointer< vtkDoubleArray > fiberReadArray = vtkSmartPointer< vtkDoubleArray >::New() ;
    fiberReadArray = vtkDoubleArray::SafeDownCast( polyData->GetCellData()->GetArray( "NaNCases" ) ) ;
    for( int i=0; Lines->GetNextCell( NumberOfPoints , Ids ) ; i++ )
    {
        vtkSmartPointer<vtkPolyLine> NewLine=vtkSmartPointer<vtkPolyLine>::New() ;
        NewLine->GetPointIds()->SetNumberOfIds( NumberOfPoints ) ;
        if( !fiberReadArray->GetValue( i ) )
        {
            for( int j=0; j < NumberOfPoints ; j++ )
            {
                NewPoints->InsertNextPoint( Points->GetPoint(Ids[ j ] ) ) ;
                NewLine->GetPointIds()->SetId( j , NewId ) ;
                NewId++ ;
                double tensorValue[9] ;
                for( int k = 0 ; k < 9 ; k++ )
                {
                    tensorValue[ k ] = Tensors->GetComponent( Ids[ j ] , k ) ;
                }
                NewTensors->InsertNextTuple(tensorValue);
            }
            NewLines->InsertNextCell(NewLine);
        }
    }
    FinalPolyData->SetPoints(NewPoints);
    FinalPolyData->GetPointData()->SetTensors(NewTensors);
    FinalPolyData->SetLines(NewLines);
    return FinalPolyData;
}

double processing::ComputeFiberLength( vtkSmartPointer<vtkPolyData> polyData , vtkIdType NumberOfPoints , vtkIdType* Ids )
{
    double fiberLength, step ;
    fiberLength = 0 ;
    vtkPoints* Points = vtkPoints::New() ;
    Points = polyData->GetPoints() ;
    for( unsigned int pointId = 0 ; pointId + 1 < NumberOfPoints ; pointId ++ )
    {
        double Point1[ 3 ] , Point2[ 3 ] , x , y , z ;
        Points->GetPoint( Ids[ pointId ], Point1 ) ;
        Points->GetPoint( Ids[ pointId + 1 ] , Point2 ) ;
        x = Point2[ 0 ]- Point1[ 0 ] ;
        y = Point2[ 1 ]- Point1[ 1 ] ;
        z = Point2[ 2 ]- Point1[ 2 ] ;
        //Distance between two successive points
        step = sqrt( x * x + y * y + z * z ) ;
        //Distance between first point and last calculated one.
        fiberLength += step ;
    }
    return fiberLength ;
}

vtkSmartPointer< vtkPolyData > processing::MatchLength( vtkSmartPointer< vtkPolyData > polyData , std::string MatchLengthFiber )
{
    vtkSmartPointer< vtkPolyData > matchLengthPolyData = vtkSmartPointer<vtkPolyData>::New() ;
    std::string extension = ExtensionOfFile( MatchLengthFiber ) ;
    if( extension.rfind("vtk") != std::string::npos )
    {
        vtkSmartPointer< vtkPolyDataReader > fiberPolyDataReader = vtkSmartPointer< vtkPolyDataReader >::New() ;
        matchLengthPolyData = ReadFiberFile( fiberPolyDataReader, MatchLengthFiber ) ;
    }
    else if( extension.rfind("vtp") != std::string::npos )
    {
        vtkSmartPointer<vtkXMLPolyDataReader> fiberPolyDataReader = vtkSmartPointer< vtkXMLPolyDataReader >::New() ;
        matchLengthPolyData = ReadFiberFile( fiberPolyDataReader, MatchLengthFiber ) ;
    }
    else
    {
        std::cerr << "lengthMatch File could not be read" << std::endl ;
        return polyData ;
    }
    vtkCellArray* Lines = vtkCellArray::New() ;
    Lines = matchLengthPolyData->GetLines() ;
    vtkIdType* Ids ;
    vtkIdType NumberOfPoints ;
    int NewId = 0 ;
    Lines->InitTraversal() ;
    int min , max ;
    int nbLines = matchLengthPolyData->GetNumberOfLines() ;
    double fiberLength, step ;
    for( int i = 0 ; i < nbLines ; i++  )
    {
        Lines->GetNextCell( NumberOfPoints , Ids ) ;
        fiberLength = ComputeFiberLength( matchLengthPolyData , NumberOfPoints , Ids ) ;
        if( i == 0 )
        {
            min = fiberLength ;
            max = fiberLength ;
        }
        else
        {
            if( min > fiberLength )
            {
                min = fiberLength ;
            }
            if(max < fiberLength )
            {
                max = fiberLength ;
            }
        }
    }
    vtkSmartPointer< vtkPolyData > newPolyData = vtkSmartPointer<vtkPolyData>::New() ;
    vtkSmartPointer<vtkFloatArray> NewTensors=vtkSmartPointer<vtkFloatArray>::New() ;
    vtkSmartPointer<vtkPoints> NewPoints=vtkSmartPointer<vtkPoints>::New() ;
    vtkSmartPointer<vtkCellArray> NewLines=vtkSmartPointer<vtkCellArray>::New() ;
    NewTensors->SetNumberOfComponents( 9 ) ;
    vtkDataArray* Tensors = polyData->GetPointData()->GetTensors() ;
    Lines = polyData->GetLines() ;
    Lines->InitTraversal() ;
    vtkPoints* Points = vtkPoints::New() ;
    Points = polyData->GetPoints() ;
    for( int j=0; Lines->GetNextCell( NumberOfPoints , Ids ) ; j++ )
    {
        vtkSmartPointer<vtkPolyLine> NewLine=vtkSmartPointer<vtkPolyLine>::New() ;
        NewLine->GetPointIds()->SetNumberOfIds( NumberOfPoints ) ;
        double tmpFiberLength = ComputeFiberLength( polyData , NumberOfPoints , Ids ) ;
        if( tmpFiberLength > min && tmpFiberLength < max )
        {
            for( int k = 0 ; k < NumberOfPoints ; k++ )
            {
                NewPoints->InsertNextPoint( Points->GetPoint(Ids[ k ] ) ) ;
                NewLine->GetPointIds()->SetId( k , NewId ) ;
                NewId++ ;
                double tensorValue[9] ;
                for( int l = 0 ; l < 9 ; l++ )
                {
                    tensorValue[ l ] = Tensors->GetComponent( Ids[ k ] , l ) ;
                }
                NewTensors->InsertNextTuple(tensorValue);
            }
            NewLines->InsertNextCell(NewLine);
        }
    }
    newPolyData->SetPoints(NewPoints);
    newPolyData->GetPointData()->SetTensors(NewTensors);
    newPolyData->SetLines(NewLines);
    return newPolyData ;
}

int processing::run()
{
    processing::fileNameStruct fileName ;
    fileName.input = InputFileName ;
    fileName.output = OutputFileName ;
    fileName.mask = AttributeFileName ;
    typedef itk::Image< int , 3 > ImageType ;
    typedef itk::ImageFileReader< ImageType > ReaderType ;
    vtkSmartPointer< vtkPolyData > fiberPolyData ;
    vtkSmartPointer< vtkPolyData > cleanedFiberPolyData ;
    std::string extension = ExtensionOfFile( InputFileName ) ;
    if( extension.rfind("vtk") != std::string::npos )
    {
        vtkSmartPointer< vtkPolyDataReader > fiberPolyDataReader = vtkSmartPointer< vtkPolyDataReader >::New() ;
        fiberPolyData = ReadFiberFile( fiberPolyDataReader, InputFileName ) ;
    }
    else if( extension.rfind("vtp") != std::string::npos )
    {
        vtkSmartPointer<vtkXMLPolyDataReader> fiberPolyDataReader = vtkSmartPointer< vtkXMLPolyDataReader >::New() ;
        fiberPolyData = ReadFiberFile( fiberPolyDataReader, InputFileName ) ;
    }
    else
    {
        std::cout << "File could not be read" << std::endl ;
        return 1 ;
    }
    std::vector< std::vector< float > > vecPointData ;
    std::vector< float > cumul , average ;
    vtkSmartPointer< vtkDoubleArray > pointData ;
    cleanedFiberPolyData = CheckNaN( fiberPolyData ) ;
    if( FlagNoNan == true )
    {
        cleanedFiberPolyData = RemoveNanFibers( cleanedFiberPolyData ) ;
    }
    if( FlagAttribute == true )
    {
        vecPointData = ApplyMaskToFiber( cleanedFiberPolyData ) ;
        cumul = CumulValuePerFiber( vecPointData ) ;
        average = AverageValuePerFiber( vecPointData ) ;
        vtkSmartPointer< vtkDoubleArray > cellData , cellData2 ;
        cellData =  CreateCellData( cumul , "CumulativeValue" ) ;
        cellData2 = CreateCellData( average , "AverageValue" ) ;
        cleanedFiberPolyData->GetCellData()->AddArray( cellData ) ;
        cleanedFiberPolyData->GetCellData()->AddArray( cellData2 ) ;
        pointData =  CreatePointData( vecPointData , "InsideMask" ) ;
        cleanedFiberPolyData->GetPointData()->AddArray( pointData ) ;
    }
    cleanedFiberPolyData = CheckNaN( cleanedFiberPolyData ) ;
    extension = ExtensionOfFile( OutputFileName ) ;
    if( extension.rfind( "vtk" ) != std::string::npos )
    {
        fileName.visu =  ChangeEndOfFileName( OutputFileName , "-visu.vtk" ) ;
    }
    else if( extension.rfind( "vtp" ) != std::string::npos )
    {
        fileName.visu = ChangeEndOfFileName( OutputFileName , "-visu.vtp" ) ;
    }
    std::string encoding = "binary" ;

    int compressionLevel = 1 ;
    if( FlagVisualize )
    {
        WriteFiberFile( encoding , extension , fileName.visu.c_str() , compressionLevel , cleanedFiberPolyData ) ;
    }
    if( FlagAttribute == true )
    {
        if( FlagCrop == true )
        {
            cleanedFiberPolyData = CropFiber( cleanedFiberPolyData , vecPointData ) ;
        }
        else
        {

        }
    }
    if( FlagThreshold == true && FlagMask == true )
    {
        cleanedFiberPolyData = CleanFiber( cleanedFiberPolyData , Threshold ) ;
    }
    if( FlagLengthMatch == true )
    {
        cleanedFiberPolyData = MatchLength( cleanedFiberPolyData , LengthMatchFiber ) ;
    }
    WriteLogFile( fileName , vecPointData , cleanedFiberPolyData , cumul , average ) ;
    WriteFiberFile( encoding , extension , fileName.output.c_str() , compressionLevel , cleanedFiberPolyData ) ;
    return 0 ;
}
