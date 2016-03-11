#include <string>
#include <cmath>
#include <memory>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <math.h>

#include <itkSpatialObjectReader.h>
#include <itkSpatialObjectWriter.h>

#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkPolyDataReader.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkSmartPointer.h>
#include <vtkCell.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>
#include <vtkCellData.h>

#include "fiberprocessing.h"

fiberprocessing::fiberprocessing()
{
    plane_origin.Fill(0);plane_normal.Fill(0);
    m_Bandwidth = 0.0 ;
    closest_d = 0 ;
    m_WorldSpace = true ;
}

fiberprocessing::~fiberprocessing()
{}

void fiberprocessing::fiberprocessing_main( std::string& input_file ,
                                            std::string& output_file ,
                                            bool planeautoOn ,
                                            std::string plane_str ,
                                            bool worldspace ,
                                            std::string auto_plane_origin ,
                                            bool useNonCrossingFibers ,
                                            double bandwidth ,
                                            bool removeCleanFibers,
                                            bool removeNanFibers
                                            )
{
    std::string tmpExtension = ExtensionofFile( input_file ) ;
    std::string inputFileNoNan = takeoffExtension( input_file ) + "_noNan." + tmpExtension ;
    m_Bandwidth = 2.0 * bandwidth ;//-1/+1
    m_WorldSpace = worldspace ;
    GroupType::Pointer group = readFiberFile(input_file) ;
    m_Spacing = group->GetSpacing();
    m_Offset = group->GetObjectToParentTransform()->GetOffset();

    if (planeautoOn)
    {
        cout<<"Finding the plane origin and normal automatically\n\n";
        find_plane(group , auto_plane_origin);
    }
    else
    {
        bool plane_defined = read_plane_details(plane_str);
        if (!plane_defined)
        {
            cout<<"Could not read plane file::finding the plane origin and normal automatically\n\n";
            find_plane(group, auto_plane_origin);
        }
        cout<<"return from read_plane_file function :"<<plane_defined<<plane_origin<<plane_normal<<endl;
    }
    if(removeNanFibers == 1)
    {
        vtkSmartPointer<vtkPolyData> PolyData;
        PolyData=RemoveNanFibers(input_file);

        if (input_file.rfind(".vtk") != std::string::npos)
        {
            vtkSmartPointer<vtkPolyDataWriter> fiberwriter = vtkPolyDataWriter::New();
            //fiberwriter->SetFileTypeToBinary();

            fiberwriter->SetFileName(inputFileNoNan.c_str());
#if (VTK_MAJOR_VERSION < 6)
            fiberwriter->SetInput(PolyData);
#else
            fiberwriter->SetInputData(PolyData);
#endif
            fiberwriter->Update();
        }
        else if(input_file.rfind(".vtp") != std::string::npos)
        {
            vtkSmartPointer<vtkXMLPolyDataWriter> fiberwriter = vtkXMLPolyDataWriter::New();
            fiberwriter->SetFileName(inputFileNoNan.c_str());
#if (VTK_MAJOR_VERSION < 6)
            fiberwriter->SetInput(PolyData);
#else
            fiberwriter->SetInputData(PolyData);
#endif
            fiberwriter->Update();
        }
        //We need to read the new file containing only the fibers crossing the plane
        //(since all the filtering was done on the VTK and we work on the spatial object "group")
        /*group = readFiberFile(input_file);
    if( removeCleanFibers )
    {
      remove( input_file.c_str() ) ;
    }*/
    }
    if(!useNonCrossingFibers)
    {

        vtkSmartPointer<vtkPolyData> PolyData;
        if( removeNanFibers == 1 )
        {
            PolyData=RemoveNonCrossingFibers(inputFileNoNan);
            std::string extension=ExtensionofFile(input_file);
            std::string plane_name=takeoffExtension(takeoffPath(plane_str));
            input_file=takeoffExtension(output_file)+"_"+plane_name+"_Clean."+extension;
            if (input_file.rfind(".vtk") != std::string::npos)
            {
                vtkSmartPointer<vtkPolyDataWriter> fiberwriter = vtkPolyDataWriter::New();
                //fiberwriter->SetFileTypeToBinary();
                fiberwriter->SetFileName(input_file.c_str());
    #if (VTK_MAJOR_VERSION < 6)
                fiberwriter->SetInput(PolyData);
    #else
                fiberwriter->SetInputData(PolyData);
    #endif
                fiberwriter->Update();
            }
            else if(input_file.rfind(".vtp") != std::string::npos)
            {
                vtkSmartPointer<vtkXMLPolyDataWriter> fiberwriter = vtkXMLPolyDataWriter::New();
                fiberwriter->SetFileName(input_file.c_str());
    #if (VTK_MAJOR_VERSION < 6)
                fiberwriter->SetInput(PolyData);
    #else
                fiberwriter->SetInputData(PolyData);
    #endif
                fiberwriter->Update();
            }
        }
        else
        {
            PolyData=RemoveNonCrossingFibers(input_file);
            std::string extension=ExtensionofFile(input_file);
            std::string plane_name=takeoffExtension(takeoffPath(plane_str));
            input_file=takeoffExtension(output_file)+"_"+plane_name+"_Clean."+extension;
            if (input_file.rfind(".vtk") != std::string::npos)
            {
                vtkSmartPointer<vtkPolyDataWriter> fiberwriter = vtkPolyDataWriter::New();
                //fiberwriter->SetFileTypeToBinary();
                fiberwriter->SetFileName(input_file.c_str());
    #if (VTK_MAJOR_VERSION < 6)
                fiberwriter->SetInput(PolyData);
    #else
                fiberwriter->SetInputData(PolyData);
    #endif
                fiberwriter->Update();
            }
            else if(input_file.rfind(".vtp") != std::string::npos)
            {
                vtkSmartPointer<vtkXMLPolyDataWriter> fiberwriter = vtkXMLPolyDataWriter::New();
                fiberwriter->SetFileName(input_file.c_str());
    #if (VTK_MAJOR_VERSION < 6)
                fiberwriter->SetInput(PolyData);
    #else
                fiberwriter->SetInputData(PolyData);
    #endif
                fiberwriter->Update();
            }
        }
        //We need to read the new file containing only the fibers crossing the plane
        //(since all the filtering was done on the VTK and we work on the spatial object "group")
        group = readFiberFile(input_file);
        if( removeCleanFibers )
        {
            remove( input_file.c_str() ) ;
        }
    }

    arc_length_parametrization( group ) ;
}

std::string fiberprocessing::takeoffExtension(std::string filename)
{
    std::string inputfile;
    inputfile = filename.substr(0,filename.find_first_of("."));
    return inputfile;
}

std::string fiberprocessing::takeoffPath(std::string filename)
{
    std::string inputfile;
    inputfile = filename.substr(filename.find_last_of("/")+1,filename.size()-filename.find_last_of("/")+1);
    return inputfile;
}

std::string fiberprocessing::ExtensionofFile(std::string filename)
{
    std::string extension;
    extension = filename.substr(filename.find_last_of(".")+1,filename.size()-filename.find_last_of(".")+1);
    return extension;
}

vtkSmartPointer<vtkPolyData> fiberprocessing::RemoveNonCrossingFibers(std::string Filename)
{
    vtkSmartPointer<vtkPolyData> PolyData;
    if (Filename.rfind(".vtk") != std::string::npos)
    {
        vtkSmartPointer<vtkPolyDataReader> reader = vtkPolyDataReader::New();
        reader->SetFileName(Filename.c_str());
        PolyData=reader->GetOutput();
        reader->Update();
    }
    else if (Filename.rfind(".vtp") != std::string::npos)
    {
        vtkSmartPointer<vtkXMLPolyDataReader> reader = vtkXMLPolyDataReader::New();
        reader->SetFileName(Filename.c_str());
        PolyData=reader->GetOutput();
        reader->Update();
    }
    std::cout<<"Total Number of Fibers : "<<PolyData->GetNumberOfCells()<<std::endl;

    vtkSmartPointer<vtkPolyData> FinalPolyData=vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkFloatArray> NewTensors=vtkSmartPointer<vtkFloatArray>::New();
    vtkSmartPointer<vtkPoints> NewPoints=vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkCellArray> NewLines=vtkSmartPointer<vtkCellArray>::New();
    NewTensors->SetNumberOfComponents(9);
    vtkDataArray* Tensors=PolyData->GetPointData()->GetTensors();

    vtkPoints* Points=PolyData->GetPoints();
    vtkCellArray* Lines=PolyData->GetLines();
    vtkIdType* Ids;
    vtkIdType NumberOfPoints;
    int NewId=0;
    Lines->InitTraversal();

    bool Cross;
    for(int i=0; Lines->GetNextCell(NumberOfPoints, Ids); i++)
    {
        Cross=false;
        vtkSmartPointer<vtkPolyLine> NewLine=vtkSmartPointer<vtkPolyLine>::New();
        NewLine->GetPointIds()->SetNumberOfIds(NumberOfPoints);
        for(int j=0; j<NumberOfPoints-1; j++)
        {
            double p1[3], p2[3];
            Points->GetPoint(Ids[j],p1);
            Points->GetPoint(Ids[j+1],p2);

            p1[0]=plane_origin[0]-p1[0];
            p1[1]=plane_origin[1]-p1[1];
            p1[2]=plane_origin[2]-p1[2];

            p2[0]=plane_origin[0]-p2[0];
            p2[1]=plane_origin[1]-p2[1];
            p2[2]=plane_origin[2]-p2[2];

            double ScalarProduct1, ScalarProduct2;
            ScalarProduct1=p1[0]*plane_normal[0]+p1[1]*plane_normal[1]+p1[2]*plane_normal[2];
            ScalarProduct2=p2[0]*plane_normal[0]+p2[1]*plane_normal[1]+p2[2]*plane_normal[2];
            if(ScalarProduct1*ScalarProduct2<0)
            {
                Cross=true;
                break;
            }
        }
        if(Cross)
        {
            for(int j=0; j<NumberOfPoints; j++)
            {
                NewPoints->InsertNextPoint(Points->GetPoint(Ids[j]));
                NewLine->GetPointIds()->SetId(j,NewId);
                NewId++;
                double tensorValue[9];
                for(int k=0; k<9; k++)
                    tensorValue[k]=Tensors->GetComponent(Ids[j],k);
                NewTensors->InsertNextTuple(tensorValue);
            }
            NewLines->InsertNextCell(NewLine);
        }
    }
    FinalPolyData->SetPoints(NewPoints);
    FinalPolyData->GetPointData()->SetTensors(NewTensors);
    FinalPolyData->SetLines(NewLines);
    std::cout<<"Number of Fibers without non crossing fibers : "<<FinalPolyData->GetNumberOfCells()<<std::endl;
    return FinalPolyData;
}

vtkSmartPointer<vtkPolyData> fiberprocessing::RemoveNanFibers(std::string Filename)
{
    vtkSmartPointer<vtkPolyData> PolyData;
    if (Filename.rfind(".vtk") != std::string::npos)
    {
        vtkSmartPointer<vtkPolyDataReader> reader = vtkPolyDataReader::New();
        reader->SetFileName(Filename.c_str());
        PolyData=reader->GetOutput();
        reader->Update();
    }
    else if (Filename.rfind(".vtp") != std::string::npos)
    {
        vtkSmartPointer<vtkXMLPolyDataReader> reader = vtkXMLPolyDataReader::New();
        reader->SetFileName(Filename.c_str());
        PolyData=reader->GetOutput();
        reader->Update();
    }
    std::cout<<"Total Number of Fibers : "<<PolyData->GetNumberOfCells()<<std::endl;
    vtkSmartPointer<vtkPolyData> FinalPolyData=vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkFloatArray> NewTensors=vtkSmartPointer<vtkFloatArray>::New();
    vtkSmartPointer<vtkPoints> NewPoints=vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkCellArray> NewLines=vtkSmartPointer<vtkCellArray>::New();
    NewTensors->SetNumberOfComponents(9);
    vtkDataArray* Tensors=PolyData->GetPointData()->GetTensors();
    vtkPoints* Points=PolyData->GetPoints();
    vtkCellArray* Lines=PolyData->GetLines();
    vtkIdType* Ids;
    vtkIdType NumberOfPoints;
    int NewId=0;
    Lines->InitTraversal();
    vtkSmartPointer< vtkDoubleArray > fiberReadArray = vtkSmartPointer< vtkDoubleArray >::New() ;
    fiberReadArray = vtkDoubleArray::SafeDownCast( PolyData->GetCellData()->GetArray( "NaNCases" ) ) ;    
    
    for( int i=0; i < Lines->GetSize() ; i++ )        
    {
        Lines->GetCell(i, NumberOfPoints , Ids );
        vtkSmartPointer<vtkPolyLine> NewLine=vtkSmartPointer<vtkPolyLine>::New() ;
        NewLine->GetPointIds()->SetNumberOfIds( NumberOfPoints ) ;
        if(fiberReadArray == NULL || (fiberReadArray && !fiberReadArray->GetValue( i ) ))
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
    std::cout<<"Number of Fibers without Nan values : "<<FinalPolyData->GetNumberOfCells()<<std::endl;
    return FinalPolyData;
}
bool fiberprocessing::Siequals(std::string a, std::string b)
{
    unsigned int sz = a.size();
    //std::cout<<"a size is "<<a.size()<<"b size is "<<b.size()<<std::endl;
    if (b.size() != sz)
        return false;
    for (unsigned int i = 0; i < sz; ++i)
        if (tolower(a[i]) != tolower(b[i]))
            return false;
    return true;
}

std::vector< std::vector<double> > fiberprocessing::get_arc_length_parametrized_fiber(std::string param_name)
{
    const char* parameter_std_list[] = {"fa", "md", "l1", "l2" , "l3", "fro" , "rd" ,"ga"};
    int parameter_index = 0; //which parameter was called
    if (param_name == "all")
    {
        cout<<"Total Number of sample points in the fiber bundle: "<<all.size()<<"...getting all parameters..."<<endl;
        return(all);

    }
    else
    {
        for (int a =0;a<8;a++)
        {
            std::string tmp_parameter(parameter_std_list[a]);
            //std::cout<<"param_name is"<<param_name<<"parameter_std_list[a] is "<<tmp_parameter<<" a is "<<a<<" param_name is "<<param_name<<std::endl;
            if (Siequals(tmp_parameter, param_name))
            {
                //std::cout<<"param_name is"<<param_name<<"parameter_std_list[a] is "<<parameter_std_list[a]<<"a is"<<a<<std::endl;
                parameter_index = a;
                break;
            }
        }
    }
    std::vector< std::vector<double> > length_temp;
    for (size_t i =0; i< all.size() ; i++ )
    {
        length_temp.push_back(std::vector<double>());
        length_temp[i].push_back(all[i][0]);
        length_temp[i].push_back(all[i][parameter_index+1]);
    }
    cout<<"Total Number of sample points in the fiber bundle: "<<length_temp.size()<<"...getting parameter "<<parameter_std_list[parameter_index]<<endl;
    return(length_temp);

}

itk::Point< double , 3> fiberprocessing::SpatialPosition( itk::Point<double, 3> position )
{
    typedef DTIPointType::PointType PointType;
    if (m_WorldSpace)
    {
        position[0] = (position[0] * m_Spacing[0]) + m_Offset[0];
        position[1] = (position[1] * m_Spacing[1]) + m_Offset[1];
        position[2] = (position[2] * m_Spacing[2]) + m_Offset[2];
    }
    return position ;
}

double fiberprocessing::DistanceToPlane(itk::Point<double , 3> point)
{
    itk::Vector< double , 3 > vecToPlane ;
    for( int i = 0 ; i < 3 ; i++ )
    {
        vecToPlane[ i ] = point[ i ] - plane_origin[ i ] ;
    }
    vecToPlane.Normalize() ;
    double d = (plane_normal[0] * vecToPlane[0]) + (plane_normal[1] * vecToPlane[1]) + (plane_normal[2] * vecToPlane[2]) ;
    return d ;
}

double fiberprocessing::Find_First_Point( DTIPointListType &pointlist ,
                                          int increment ,
                                          DTIPointListType::iterator &pit_first
                                          )
{
    bool first_point = true ;
    double previous_distance = 0 ;
    for( DTIPointListType::iterator pit = pointlist.begin(); pit != pointlist.end(); pit++ )
    {
        //The norm of the normal vector to the plane has been normalized in arc_length_parametrization()
        itk::Point<double, 3> pos = SpatialPosition( (*pit).GetPosition() );
        double d = DistanceToPlane( pos ) ;
        if( first_point )
        {
            first_point = false ;
        }
        else
        {
            if( d * previous_distance < 0 )//if we have points on 2 sides of the plane, the distance will be of opposite sign
            {
                if( increment < 0 )//this current point is on the good side of the plan
                {
                    pit_first = pit - 1 ;
                    return previous_distance ;
                }
                else//the previous point was on the good side of the plan
                {
                    pit_first = pit ;
                    return d ;
                }
                break ;
            }
        }
        previous_distance = d ;
    }
    return std::numeric_limits<double>::quiet_NaN(); ;
}

void fiberprocessing::AddValueParametrization( DTIPointListType::iterator &pit , itk::Point<double,3> p1 , double distance )
{
    size_t fiber_counter = parametrized_position.size() - 1 ;
    parametrized_distance_struct param_dist ;
    param_dist.dist = distance ;
    param_dist.x = p1[0] ;
    param_dist.y = p1[1] ;
    param_dist.z = p1[2] ;
    parametrized_position[fiber_counter].push_back(param_dist);
    all.push_back(std::vector<double>());
    size_t all_size = all.size() - 1 ;
    all[all_size].push_back( distance );
    all[all_size].push_back((*pit).GetField(DTIPointType::FA));
    all[all_size].push_back((*pit).GetField("MD"));
    all[all_size].push_back((*pit).GetField("l1"));
    all[all_size].push_back((*pit).GetField("l2"));
    all[all_size].push_back((*pit).GetField("l3"));
    all[all_size].push_back((*pit).GetField("FRO"));
    all[all_size].push_back((*pit).GetField("RD"));	//RD
    all[all_size].push_back((*pit).GetField("GA"));
}

void fiberprocessing::ComputeArcLength( DTIPointListType::iterator &beginit ,
                                        DTIPointListType::iterator &endit ,
                                        itk::Point<double , 3 > p2 ,
                                        int increment ,
                                        int displacement ,
                                        double min_distance
                                        )
{
    double cumulative_distance = min_distance ;
    for( DTIPointListType::iterator pit = beginit+increment ; pit != endit ; pit += increment )
    {
        //gives the distance between current and previous sample point
        double current_length ;
        itk::Point<double, 3> p1 = SpatialPosition( (*pit ).GetPosition() ) ;
        current_length = sqrt((p1[0]-p2[0])*(p1[0]-p2[0])+(p1[1]-p2[1])*(p1[1]-p2[1])+(p1[2]-p2[2])*(p1[2]-p2[2]));
        if( current_length > m_Bandwidth )
        {
            std::cout<<" Distance between 2 consecutive points > 2 x bandwidth" <<std::endl ;
            std::cout<<" Distance: "<< current_length <<std::endl ;
            std::cout<<" 2 x Bandwidth: "<< m_Bandwidth <<std::endl ;
        }
        cumulative_distance += displacement * current_length ;
        //multiplying by -1 to make arc length on 1 side of plane as negative
        AddValueParametrization( pit , p1 , cumulative_distance ) ;
        p2 = p1 ;
    }
}

int fiberprocessing::ParametrizesHalfFiber( DTIPointListType &pointlist ,
                                            DTIPointListType::iterator &endit ,
                                            int increment ,
                                            int displacement
                                            )
{
    DTIPointListType::iterator pit_first ;
    double distance_min ;
    distance_min = Find_First_Point( pointlist , increment , pit_first ) ;
#ifdef WIN32
    if( _isnan( distance_min ) )
    {
        return 1 ;
    }
#else
    if( std::isnan( distance_min ) )
    {
        return 1 ;
    }
#endif
    itk::Point<double, 3> p1 = SpatialPosition( (*pit_first).GetPosition() ) ;
    AddValueParametrization( pit_first , p1 , distance_min ) ;//distance_min is a signed distance. No need to multiply it by "displacement"
    ComputeArcLength( pit_first , endit , p1 , increment , displacement , distance_min ) ;
    return 0 ;
}

void fiberprocessing::ParametrizesEntireFiber( DTIPointListType &pointlist , int flag_orientation )
{
    //We start in one direction
    DTIPointListType::iterator pit_begin = pointlist.begin() - 1 ;
    ParametrizesHalfFiber( pointlist , pit_begin , -1 , -1*flag_orientation ) ;
    //Then we do parametrize the other one
    DTIPointListType::iterator pit_end = pointlist.end() ;
    ParametrizesHalfFiber( pointlist , pit_end , 1 , flag_orientation ) ;
}

int fiberprocessing::CheckFiberOrientation( DTIPointListType &pointlist , int &count_opposite )
{
    int flag_orientation ;
    DTIPointListType::iterator pit_tmp ;
    pit_tmp = pointlist.begin();
    itk::Point<double, 3> position_first = SpatialPosition( (*pit_tmp).GetPosition() ) ;
    pit_tmp = pointlist.end() -1 ;
    itk::Point<double, 3> position_last = SpatialPosition( (*pit_tmp).GetPosition() ) ;
    itk::Vector<double, 3> orient_vec = position_last - position_first ;
    //verifies that first and last point are on different sides of the plane

    double distance_first = DistanceToPlane( position_first ) ;
    double distance_last = DistanceToPlane( position_last ) ;
    if( distance_first * distance_last > 0)
    {
        return 0 ;
    }
    //
    double dot_prod = (plane_normal[0]*orient_vec[0] + plane_normal[1]*orient_vec[1] + plane_normal[2]*orient_vec[2] );
    if( dot_prod < 0 )
    {
        //found fiber orientation as opposite
        flag_orientation = -1 ;
        count_opposite += 1 ;
    }
    else
    {
        flag_orientation = 1 ;
    }
    return flag_orientation ;
}

void fiberprocessing::arc_length_parametrization( GroupType::Pointer group )
{
    ChildrenListType* children = group->GetChildren(0);
    ChildrenListType::iterator it;
    //**********************************************************************************************************
    // For each fiber
    int count_opposite = 0;
    float plane_norm = ( sqrt( ( plane_normal[ 0 ] * plane_normal[ 0 ] )
            + ( plane_normal[ 1 ] * plane_normal[ 1 ] )
            + ( plane_normal[ 2 ] * plane_normal[ 2 ] )
            )
            ) ;
    //Normalizing normal
    plane_normal[0] /= plane_norm;
    plane_normal[1] /= plane_norm;
    plane_normal[2] /= plane_norm;
    int ignored_fibers = 0 ;
    for(it = (children->begin()); it != children->end() ; it++)
    {
        if( parametrized_position.empty()
                || (!parametrized_position.empty() && !parametrized_position[parametrized_position.size() - 1 ].empty() )
                )
        {
            parametrized_position.push_back(std::vector<parametrized_distance_struct>());
        }
        DTIPointListType pointlist = dynamic_cast<DTITubeType*>((*it).GetPointer())->GetPoints();
        int flag_orientation = CheckFiberOrientation( pointlist , count_opposite ) ;
        if( flag_orientation )
        {
            ParametrizesEntireFiber( pointlist , flag_orientation ) ;
        }
        else
        {
            ignored_fibers++ ;
        }
    }
    std::cout<<" l_counter: " << all.size() << std::endl;
    cout << "Total # of opposite oriented fibers:" << count_opposite << endl ;
    cout << "Total # of ignored fibers (first and last point on the same side of the plane:" << ignored_fibers << endl ;
}

itk::Vector<double, 3> fiberprocessing::get_plane_origin()
{
    return(plane_origin);
}
itk::Vector<double, 3> fiberprocessing::get_plane_normal()
{
    return(plane_normal);
}


void fiberprocessing::find_plane(GroupType::Pointer group, std::string auto_plane_origin)
{
    ChildrenListType* pchildren = group->GetChildren(0);
    ChildrenListType::iterator it, it_closest;
    DTIPointListType pointlist;
    DTIPointListType::iterator pit, pit_closest;
    double sum_x=0,sum_y=0,sum_z=0;
    int num_points=0;

    if(auto_plane_origin=="cog")
    {
        //Finding Plane origin as the average x,y,z coordinate of the whole fiber bundle
        for(it = (pchildren->begin()); it != pchildren->end() ; it++)
        {
            pointlist = dynamic_cast<DTITubeType*>((*it).GetPointer())->GetPoints();
            for(pit = pointlist.begin(); pit != pointlist.end(); pit++)
            {
                itk::Point<double, 3> position = (*pit).GetPosition();
                num_points++;
                sum_x=sum_x+position[0]; sum_y=sum_y+position[1]; sum_z=sum_z+position[2];
            }
        }
        plane_origin[0]=sum_x/num_points;
        plane_origin[1]=sum_y/num_points;
        plane_origin[2]=sum_z/num_points;
    }
    else if(auto_plane_origin=="median")
    {
        itk::Vector<double, 3> median_point;
        //Finding Plane origin as the average x,y,z of middle points for each fibers
        int median=0;
        double distance_min=10000.0;
        for(it=pchildren->begin(); it!=pchildren->end(); it++)
        {
            pointlist=dynamic_cast<DTITubeType*>((*it).GetPointer())->GetPoints();
            median=ceil((double)pointlist.size()/2.0);
            pit=pointlist.begin();
            pit+=median;
            itk::Point<double,3> position=(*pit).GetPosition();
            num_points++;
            sum_x+=position[0]; sum_y+=position[1]; sum_z+=position[2];
        }
        median_point[0]=sum_x/num_points;
        median_point[1]=sum_y/num_points;
        median_point[2]=sum_z/num_points;
        for(it=pchildren->begin(); it!=pchildren->end(); it++)
        {
            pointlist=dynamic_cast<DTITubeType*>((*it).GetPointer())->GetPoints();
            median=ceil((double)pointlist.size()/2.0);
            pit=pointlist.begin();
            pit+=median;
            itk::Point<double,3> position=(*pit).GetPosition();
            double distance=sqrt(pow(position[0]-median_point[0],2)+pow(position[1]-median_point[1],2)+pow(position[2]-median_point[2],2));
            if(distance<distance_min)
            {
                distance_min=distance;
                plane_origin[0]=position[0];
                plane_origin[1]=position[1];
                plane_origin[2]=position[2];
            }
        }
    }
    cout<<"\nCalculated Plane Origin (avg x,y,z): "<<plane_origin[0]<<","<<plane_origin[1]<<","<<plane_origin[2]<<endl;

    //Leaving a percent of the bundle end points, find the closest point on bundle to the plane origin (to avoid curved bundles getting ends as the closest point)

    int closest_d = 1000.0;
    itk::Point<double, 3> closest_point_coor;
    closest_point_coor.Fill(0.0);
    for(it = (pchildren->begin()); it != pchildren->end() ; it++)
    {
        pointlist = dynamic_cast<DTITubeType*>((*it).GetPointer())->GetPoints();

        int percent_count = 1;
        for(pit = pointlist.begin(); pit != pointlist.end(); pit++)
        {
            //ignoring first 30% and last 30% of points along the fiber(Fiber-ends)
            if (percent_count>floor(pointlist.size() * .3) && percent_count<floor(pointlist.size() * .7))
            {
                itk::Point<double, 3> position = (*pit).GetPosition();
                double dist =  sqrt((position[0]-plane_origin[0])*(position[0]-plane_origin[0])+(position[1]-plane_origin[1])*(position[1]-plane_origin[1])+(position[2]-plane_origin[2])*(position[2]-plane_origin[2]));
                if (dist <= closest_d)
                {
                    closest_d = dist;
                    pit_closest = pit;
                    closest_point_coor[0] = (*pit).GetPosition()[0];
                    closest_point_coor[1] = (*pit).GetPosition()[1];
                    closest_point_coor[2] = (*pit).GetPosition()[2];
                    it_closest  = it;
                }
            }
            percent_count++;
        }
    }

    //Using 3 points to left and right of this closest point, find the plane normal
    itk::Point<double, 3> point_before, point_after;
    point_before.Fill(0);
    point_after.Fill(0);
    for(it = (pchildren->begin()); it != pchildren->end() ; it++)
    {
        if (it==it_closest)
        {
            pointlist = dynamic_cast<DTITubeType*>((*it).GetPointer())->GetPoints();
            for(pit = pointlist.begin(); pit != pointlist.end(); pit++)
            {
                if (pit==pit_closest)
                {
                    point_before = (*(--(--(--pit)))).GetPosition();
                    ++pit;++pit;++pit;
                    point_after = (*(++(++(++pit)))).GetPosition();
                }
            }
        }
    }
    plane_normal=point_after-point_before;
    double plane_norm = (sqrt((plane_normal[0]*plane_normal[0])+(plane_normal[1]*plane_normal[1])+(plane_normal[2]*plane_normal[2])));
    if (plane_norm != 1)
    {
        plane_normal[0] /= plane_norm;
        plane_normal[1] /= plane_norm;
        plane_normal[2] /= plane_norm;
    }
    cout<<"Plane normal:"<<plane_normal[0]<<","<<plane_normal[1]<<","<<plane_normal[2]<<endl;
}


bool fiberprocessing::read_plane_details(std::string plane_str)
{
    char extra[30];
    fstream plane;
    plane.open(plane_str.c_str(),fstream::in);
    if (!plane.is_open())
    {
        return 0;
    }
    cout<<"Plane File successfully opened: "<<plane_str<<endl;
    while (plane.good())
    {
        //reading the plane origin
        for (int i=0;i<3;i++)
        {
            //to discard the words: "Cut Plane Origin:"
            plane.getline(extra, 30, ' ');
        }
        for (int i=0;i<2;i++)
        {
            char value[30];
            plane.getline(value, 30, ' ');
            plane_origin[i] = (atof(value) - m_Offset[i])/ m_Spacing[i];
            if (value[0]=='\0' || value[0]==' ' || value[0]=='\t' || value[0]=='\n')
                i--;
        }
        char value[30];
        plane.getline(value, 30, '\n');
        plane_origin[2] = (atof(value) - m_Offset[2])/ m_Spacing[2];

        //reading the plane normal
        extra[0]='\0';
        for (int i=0;i<3;i++)
        {
            //to discard the words: "Cut Plane normal:"
            plane.getline(extra, 30, ' ');
        }
        for (int i=0;i<2;i++)
        {
            char value[30];
            plane.getline(value, 30, ' ');
            plane_normal[i] = (atof(value) - m_Offset[i])/ m_Spacing[i];
            if (value[0]=='\0' || value[0]==' ' || value[0]=='\t' || value[0]=='\n')
                i--;
        }
        value[0]='\0';
        plane.getline(value, 30, '\n');
        plane_normal[2] = (atof(value) - m_Offset[2])/ m_Spacing[2];
        break;
    }
    plane.close();
    return 1;

}


void fiberprocessing::writeFiberFile(const std::string & filename, GroupType::Pointer fibergroup)
{
    // Make sure origins are updated
    fibergroup->ComputeObjectToWorldTransform();

    // ITK Spatial Object
    if(filename.rfind(".fib") != std::string::npos)
    {
        typedef itk::SpatialObjectWriter<3> WriterType;
        WriterType::Pointer writer  = WriterType::New();
        writer->SetInput(fibergroup);
        writer->SetFileName(filename);
        writer->Update();
    }
    // VTK Poly Data
    else if (filename.rfind(".vt") != std::string::npos)
    {
        // Build VTK data structure
        vtkSmartPointer<vtkPolyData> polydata(vtkPolyData::New());
        vtkSmartPointer<vtkFloatArray> tensorsdata(vtkFloatArray::New());
        vtkSmartPointer<vtkIdList> ids(vtkIdList::New());
        vtkSmartPointer<vtkPoints> pts(vtkPoints::New());

        tensorsdata->SetNumberOfComponents(9);
        polydata->SetPoints (pts);

        ids->SetNumberOfIds(0);
        pts->SetNumberOfPoints(0);
        polydata->Allocate();

        std::auto_ptr<ChildrenListType> children(fibergroup->GetChildren(0));
        typedef ChildrenListType::const_iterator IteratorType;

        for (IteratorType it = children->begin(); it != children->end(); it++)
        {
            itk::SpatialObject<3>* tmp = (*it).GetPointer();
            itk::DTITubeSpatialObject<3>* tube = dynamic_cast<itk::DTITubeSpatialObject<3>* >(tmp);
            unsigned int nPointsOnFiber = tube->GetNumberOfPoints();
            vtkIdType currentId = ids->GetNumberOfIds();

            for (unsigned int k = 0; k < nPointsOnFiber; k++)
            {
                itk::Point<double, 3> v(tube->GetPoint(k)->GetPosition());
                itk::Vector<double, 3> spacing(tube->GetSpacing());
                itk::Vector<double, 3> origin(tube->GetObjectToWorldTransform()->GetOffset());

                // convert origin from LPS -> RAS
                /*       origin[0] = -origin[0];
        origin[1] = -origin[1];*/

                vtkIdType id;
                // Need to multiply v by spacing and origin
                // Also negate the first to convert from LPS -> RAS
                // for slicer 3
                /*        id = pts->InsertNextPoint(-v[0] * spacing[0] + origin[0],
                                  -v[1] * spacing[1] + origin[1],
                                  v[2] * spacing[2] + origin[2]);*/

                id = pts->InsertNextPoint(v[0] * spacing[0] + origin[0],
                        v[1] * spacing[1] + origin[1],
                        v[2] * spacing[2] + origin[2]);

                ids->InsertNextId(id);

                itk::DTITubeSpatialObjectPoint<3>* sopt = dynamic_cast<itk::DTITubeSpatialObjectPoint<3>* >(tube->GetPoint(k));
                float vtktensor[9];
                vtktensor[0] = sopt->GetTensorMatrix()[0];
                vtktensor[1] = sopt->GetTensorMatrix()[1];
                vtktensor[2] = sopt->GetTensorMatrix()[2];
                vtktensor[3] = sopt->GetTensorMatrix()[1];
                vtktensor[4] = sopt->GetTensorMatrix()[3];
                vtktensor[5] = sopt->GetTensorMatrix()[4];
                vtktensor[6] = sopt->GetTensorMatrix()[2];
                vtktensor[7] = sopt->GetTensorMatrix()[4];
                vtktensor[8] = sopt->GetTensorMatrix()[5];

                tensorsdata->InsertNextTupleValue(vtktensor);

            }
            polydata->InsertNextCell(VTK_POLY_LINE, nPointsOnFiber, ids->GetPointer(currentId));
        }

        polydata->GetPointData()->SetTensors(tensorsdata);

        // Legacy
        if (filename.rfind(".vtk") != std::string::npos)
        {
            vtkSmartPointer<vtkPolyDataWriter> fiberwriter = vtkPolyDataWriter::New();
            //       fiberwriter->SetFileTypeToBinary();
            fiberwriter->SetFileName(filename.c_str());
#if (VTK_MAJOR_VERSION < 6)
            fiberwriter->SetInput(polydata);
#else
            fiberwriter->SetInputData(polydata);
#endif
            fiberwriter->Update();
        }
        // XML
        else if (filename.rfind(".vtp") != std::string::npos)
        {
            vtkSmartPointer<vtkXMLPolyDataWriter> fiberwriter = vtkXMLPolyDataWriter::New();
            fiberwriter->SetFileName(filename.c_str());
#if (VTK_MAJOR_VERSION < 6)
            fiberwriter->SetInput(polydata);
#else
            fiberwriter->SetInputData(polydata);
#endif
            fiberwriter->Update();
        }
        else
        {
            throw itk::ExceptionObject("Unknown file format for fibers");
        }
    }
    else
    {
        throw itk::ExceptionObject("Unknown file format for fibers");
    }
}

GroupType::Pointer fiberprocessing::readFiberFile(std::string filename)
{

    // ITK Spatial Object
    if(filename.rfind(".fib") != std::string::npos)
    {
        typedef itk::SpatialObjectReader<3, unsigned char> SpatialObjectReaderType;

        // Reading spatial object
        SpatialObjectReaderType::Pointer soreader = SpatialObjectReaderType::New();

        soreader->SetFileName(filename);
        soreader->Update();

        return soreader->GetGroup();
    }
    // VTK Poly Data
    else if (filename.rfind(".vt") != std::string::npos)
    {
        // Build up the principal data structure for fiber tracts
        GroupType::Pointer fibergroup = GroupType::New();

        vtkSmartPointer<vtkPolyData> fibdata(NULL);

        // Legacy
        if (filename.rfind(".vtk") != std::string::npos)
        {
            vtkSmartPointer<vtkPolyDataReader> reader(vtkPolyDataReader::New());
            reader->SetFileName(filename.c_str());
            reader->Update();
            fibdata = reader->GetOutput();

        }
        else if (filename.rfind(".vtp") != std::string::npos)
        {
            vtkSmartPointer<vtkXMLPolyDataReader> reader(vtkXMLPolyDataReader::New());
            reader->SetFileName(filename.c_str());
            reader->Update();
            fibdata = reader->GetOutput();
        }
        else
        {
            throw itk::ExceptionObject("Unknown file format for fibers");
        }

        typedef  itk::SymmetricSecondRankTensor<double,3> ITKTensorType;
        typedef  ITKTensorType::EigenValuesArrayType LambdaArrayType;

        // Iterate over VTK data
        const int nfib = fibdata->GetNumberOfCells();
        int pindex = -1;
        for(int i = 0; i < nfib; ++i)
        {
            itk::DTITubeSpatialObject<3>::Pointer dtiTube = itk::DTITubeSpatialObject<3>::New();
            vtkSmartPointer<vtkCell> fib = fibdata->GetCell(i);

            vtkSmartPointer<vtkPoints> points = fib->GetPoints();

            typedef itk::DTITubeSpatialObjectPoint<3> DTIPointType;
            std::vector<DTIPointType> pointsToAdd;

            vtkSmartPointer<vtkDataArray> fibtensordata = fibdata->GetPointData()->GetTensors();
            for(int j = 0; j < points->GetNumberOfPoints(); ++j)
            {
                ++pindex;

                vtkFloatingPointType* coordinates = points->GetPoint(j);
                DTIPointType pt;
                // Convert from RAS to LPS for vtk
                pt.SetPosition(coordinates[0], coordinates[1], coordinates[2]);
                pt.SetRadius(0.5);
                pt.SetColor(0.0, 1.0, 0.0);

                vtkFloatingPointType* vtktensor = fibtensordata->GetTuple9(pindex);
                float floattensor[6];
                ITKTensorType itktensor;

                floattensor[0] = itktensor[0] = vtktensor[0];
                floattensor[1] = itktensor[1] = vtktensor[1];
                floattensor[2] = itktensor[2] = vtktensor[2];
                floattensor[3] = itktensor[3] = vtktensor[4];
                floattensor[4] = itktensor[4] = vtktensor[5];
                floattensor[5] = itktensor[5] = vtktensor[8];

                pt.SetTensorMatrix(floattensor);

                LambdaArrayType lambdas;

                // Need to do do eigenanalysis of the tensor
                itktensor.ComputeEigenValues(lambdas);

                float md = (lambdas[0] + lambdas[1] + lambdas[2])/3;
                float fa = sqrt(1.5) * sqrt((lambdas[0] - md)*(lambdas[0] - md) +
                        (lambdas[1] - md)*(lambdas[1] - md) +
                        (lambdas[2] - md)*(lambdas[2] - md))
                        / sqrt(lambdas[0]*lambdas[0] + lambdas[1]*lambdas[1] + lambdas[2]*lambdas[2]);

                float logavg = (log(lambdas[0])+log(lambdas[1])+log(lambdas[2]))/3;

                float ga =  sqrt( SQ2(log(lambdas[0])-logavg) \
                        + SQ2(log(lambdas[1])-logavg) \
                        + SQ2(log(lambdas[2])-logavg) );

                float fro = sqrt(lambdas[0]*lambdas[0] + lambdas[1]*lambdas[1] + lambdas[2]*lambdas[2]);
                float ad = lambdas[2];
                float rd = (lambdas[0] + lambdas[1])/2;

                pt.AddField("FA",fa);
                pt.AddField("MD",md);
                pt.AddField("FRO",fro);
                pt.AddField("l2",lambdas[1]);
                pt.AddField("l3",lambdas[0]);
                pt.AddField("l1",ad);
                pt.AddField("RD",rd);
                pt.AddField("GA",ga);

                pointsToAdd.push_back(pt);
            }

            dtiTube->SetPoints(pointsToAdd);
            fibergroup->AddSpatialObject(dtiTube);
        }
        return fibergroup;
    } // end process .vtk .vtp
    else
    {
        throw itk::ExceptionObject("Unknown fiber file");
    }
}

//Writes the parametrized fiber without resampling the fiber. The output fiber file contains
//all the input fiber point and additionally contains the parametrized distance to the plane
void fiberprocessing::Write_parametrized_fiber(std::string output_parametrized_fiber_file )
{
    //sort the parameter positions based on the cumulative distance
    sort_parameter();
    // writing the fibers
    vtkSmartPointer<vtkPolyData> polydata = vtkPolyData::New() ;
    vtkSmartPointer<vtkPoints> pts = vtkPoints::New() ;
    vtkSmartPointer<vtkIntArray> fiberindex = vtkIntArray::New() ;
    vtkSmartPointer<vtkFloatArray> arclength = vtkFloatArray::New() ;
    vtkSmartPointer<vtkIdList> ids = vtkIdList::New() ;
    polydata->SetPoints (pts);
    fiberindex->SetName("FiberLocationIndex");
    arclength->SetName("SamplingDistance2Origin");
    pts->SetNumberOfPoints(0);
    ids->SetNumberOfIds(0);
    polydata->Allocate();
    //loop through all the fibers
    for(size_t i = 0 ; i < parametrized_position.size() ; i++)
    {
        vtkIdType currentId = ids->GetNumberOfIds();
        for( size_t j = 0 ; j < parametrized_position[i].size() ; j++)
        {
            vtkIdType id;
            fiberindex->InsertNextTuple1(j);
            arclength->InsertNextTuple1(parametrized_position[i][j].dist);
            id = pts->InsertNextPoint(parametrized_position[i][j].x,
                                      parametrized_position[i][j].y,
                                      parametrized_position[i][j].z);
            ids->InsertNextId(id);
        }
        polydata->InsertNextCell(VTK_POLY_LINE,parametrized_position[i].size(),ids->GetPointer(currentId));
    }
    std::cout<<"writing fiber"<<output_parametrized_fiber_file.c_str()<<std::endl;
    polydata->GetPointData()->SetActiveScalars("SamplingDistance2Origin");
    polydata->GetPointData()->SetScalars(arclength);
    polydata->GetPointData()->SetActiveScalars("FiberLocationIndex");
    polydata->GetPointData()->SetScalars(fiberindex);
    vtkSmartPointer<vtkPolyDataWriter> fiberwriter = vtkPolyDataWriter::New();
    fiberwriter->SetFileTypeToASCII();
    fiberwriter->SetFileName(output_parametrized_fiber_file.c_str());
#if (VTK_MAJOR_VERSION < 6)
    fiberwriter->SetInput(polydata);
#else
    fiberwriter->SetInputData(polydata);
#endif
    fiberwriter->Update();
}




void fiberprocessing::Write_parametrized_fiber_avg_position_and_arclength(std::string input_file, std::string output_parametrized_fiber_file, double step_size )
{

    GroupType::Pointer group = readFiberFile(input_file);
    ChildrenListType* children = group->GetChildren(0);
    //sort the parameter positions based on the cumulative distance
    sort_parameter();

    ChildrenListType::iterator it;

    double min_length = find_min_dist();
    double max_length = find_max_dist();	//since length[][] is sorted
    int reg_length = ceil((max_length - min_length )/step_size);//note: min_length is negative. We want the last sampled point to be before the last actual fiber point
    std::cout<<"reg_length is  "<<reg_length<<std::endl;

    // writing the fibers
    vtkSmartPointer<vtkPolyData> polydata(vtkPolyData::New());
    vtkSmartPointer<vtkIdList> ids(vtkIdList::New());
    vtkSmartPointer<vtkPoints> pts(vtkPoints::New());
    vtkSmartPointer<vtkIntArray> fiberindex(vtkIntArray::New());
    vtkSmartPointer<vtkFloatArray> arclength(vtkFloatArray::New());
    polydata->SetPoints (pts);

    fiberindex->SetName("FiberLocationIndex");
    arclength->SetName("SamplingDistance2Origin");
    ids->SetNumberOfIds(0);
    pts->SetNumberOfPoints(0);

    polydata->Allocate();

    //loop through all the fibers
    int fiber_counter = 0; //counter of the fiber
    //int debugcounter = 0;
    double avglocation[4];
    int noptinwindow = 0;


    //initialize
    for (int pt_index = 0;pt_index<4;++pt_index)
    {
        avglocation[pt_index] = 0;
    }

    for(it = (children->begin()); it != children->end() ; it++)
    {
        //std::cout<<"debugcounter is "<<debugcounter<<std::endl;
        //debugcounter++;
        int real_no_pits_on_fiber = 0;
        vtkIdType currentId = ids->GetNumberOfIds();
        int fiber_length = parametrized_position[fiber_counter].size();
        int sampling_start=0;
        double range_min = min_length;
        double range_max =  min_length+step_size;
        //where to start sampling the data

        while (parametrized_position[fiber_counter][0].dist < range_min || parametrized_position[fiber_counter][0].dist > range_max){
            sampling_start++;

            range_min = min_length + sampling_start * step_size;
            range_max = min_length + (sampling_start + 1) * step_size;
        }


        //    std::cout<<"sampling starts at "<<sampling_start<<std::endl;
        int pos_counter = 0;
        for (int sampling_loc = sampling_start;sampling_loc<reg_length;sampling_loc++)
        {
            range_min = min_length + sampling_loc * step_size;
            range_max = min_length + (sampling_loc + 1) * step_size;
            //std::cout<<"three numbers are "<<range_min<<" "<<parametrized_position_dist[fiber_counter][pos_counter]<<" "<<range_max<<std::endl;
            for (int pt_index = 0;pt_index<4;++pt_index)
            {
                avglocation[pt_index] = 0;
            }
            noptinwindow = 0;
            //stay in the window to average the postions
            while (pos_counter < fiber_length && parametrized_position[fiber_counter][pos_counter].dist <= range_max && parametrized_position[fiber_counter][pos_counter].dist >= range_min)
            {
                //std::cout<<"Including data "<<pos_counter<<" for sampling location #"<<sampling_loc<<std::endl;
                avglocation[3] = parametrized_position[fiber_counter][pos_counter].dist + avglocation[3];
                //getting location info in this sampling window for average
                avglocation[0] = parametrized_position[fiber_counter][pos_counter].x + avglocation[0];
                avglocation[1] = parametrized_position[fiber_counter][pos_counter].y + avglocation[1];
                avglocation[2] = parametrized_position[fiber_counter][pos_counter].z + avglocation[2];

                noptinwindow++; //number of points in the window
                pos_counter++;
            }

            //std::cout<<"pos_counter is "<<pos_counter<<" v.s. size is "<<fiber_length<<std::endl;
            if(noptinwindow > 0)
            {
                //	    std::cout<<"there are #"<<noptinwindow<<" pts for sampling location #"<<sampling_loc<<std::endl;
                vtkIdType id;
                real_no_pits_on_fiber ++;
                fiberindex->InsertNextTuple1(sampling_loc);
                arclength->InsertNextTuple1(avglocation[3]/noptinwindow);
                id = pts->InsertNextPoint(avglocation[0]/noptinwindow,
                        avglocation[1]/noptinwindow,
                        avglocation[2]/noptinwindow);
                //std::cout<<avglocation[0]/noptinwindow<<" "<<avglocation[1]/noptinwindow<<" "<<avglocation[2]/noptinwindow;
                ids->InsertNextId(id);
                //std::cout<<"three numbers are "<<range_min<<" "<<parametrized_position_dist[fiber_counter][pos_counter]<<" "<<range_max<<std::endl;
            }

            if (pos_counter == fiber_length-1)
            {
                //std::cout<<"pos_counter is "<<pos_counter<<endl;
                break;
            }
        }
        //    std::cout<<"real_no_pits_on_fiber is "<<real_no_pits_on_fiber<<std::endl;
        polydata->InsertNextCell(VTK_POLY_LINE,real_no_pits_on_fiber,ids->GetPointer(currentId));
        //    std::cout<<"goes to fiber #"<<fiber_counter<<std::endl;
        fiber_counter++;
    }

    std::cout<<"writing fiber"<<output_parametrized_fiber_file.c_str()<<std::endl;

    polydata->GetPointData()->SetActiveScalars("SamplingDistance2Origin");
    polydata->GetPointData()->SetScalars(arclength);
    polydata->GetPointData()->SetActiveScalars("FiberLocationIndex");
    polydata->GetPointData()->SetScalars(fiberindex);

    vtkSmartPointer<vtkPolyDataWriter> fiberwriter = vtkPolyDataWriter::New();
    fiberwriter->SetFileTypeToASCII();
    fiberwriter->SetFileName(output_parametrized_fiber_file.c_str());
#if (VTK_MAJOR_VERSION < 6)
    fiberwriter->SetInput(polydata);
#else
    fiberwriter->SetInputData(polydata);
#endif
    fiberwriter->Update();

}



double fiberprocessing::find_min_dist()
{
    double min=100000;
    for (size_t i=0; i<all.size(); i++)
    {
        if (all[i][0]<min)
        {
            min=all[i][0];
        }
    }
    std::cout<<"min is "<<min<<endl;
    return(min);
}

double fiberprocessing::find_max_dist()
{
    double max=-100000;
    for (size_t i=0; i<all.size(); i++)
    {
        if (all[i][0]>max)
        {
            max=all[i][0];
        }
    }
    std::cout<<"max is "<<max<<endl;
    return(max);
}

bool fiberprocessing::sortFunction(parametrized_distance_struct i , parametrized_distance_struct j )
{
    return ( i.dist < j.dist ) ;
}

void fiberprocessing::sort_parameter()
{
    for( size_t fiber_counter = 0 ; fiber_counter < parametrized_position.size() ; fiber_counter++ )
    {
        std::sort(parametrized_position[fiber_counter].begin(), parametrized_position[fiber_counter].end(), sortFunction ) ;
    }
}
