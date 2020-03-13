#include "vtkUGRIDReader.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"

#include "vtkUnstructuredGrid.h"


#include "vtkSmartPointer.h"

#include "vtkPointData.h"

#include "vtkCellType.h"
#include "vtkCellData.h"
#include "vtkIntArray.h"


#include <vector>
#include <string>


namespace 
{
    bool read_ugrid_mesh(const char* filename, vtkUnstructuredGrid* ugrid);
}

vtkStandardNewMacro(vtkUGRIDReader);

// Construct object with merging set to true.
vtkUGRIDReader::vtkUGRIDReader()
{
    this->MeshFile = NULL;

    this->SetNumberOfInputPorts(0);
}


vtkUGRIDReader::~vtkUGRIDReader()
{
    delete [] this->MeshFile;
}


int vtkUGRIDReader::RequestData( vtkInformation *vtkNotUsed(request), 
                                 vtkInformationVector **vtkNotUsed(inputVector), 
                                 vtkInformationVector *outputVector)
{
    // get the info object
    vtkInformation *outInfo = outputVector->GetInformationObject(0);

    // get the ouptut
    vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast( outInfo->Get(vtkDataObject::DATA_OBJECT())); 

    if (!this->MeshFile)
    {
        vtkErrorMacro(<<"A File Name for the Mesh must be specified.");
        return 0;
    }

    bool res = read_ugrid_mesh(this->MeshFile,output);
    output->Squeeze();
    if (not res)
        return 0;
    return 1;
}

void vtkUGRIDReader::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os,indent);

    os  << indent << "MeshFile: "
        << (this->MeshFile ? this->MeshFile : "(none)") << "\n";
}

namespace 
{
    void tri_from_ugrid(int32_t* in, vtkIdType* out)
    {
        // just copy
        out[0] = in[0]; 
        out[1] = in[1]; 
        out[2] = in[2]; 
    }
    void quad_from_ugrid(int32_t* in, vtkIdType* out)
    {
        // just copy
        out[0] = in[0]; 
        out[1] = in[1]; 
        out[2] = in[2]; 
        out[3] = in[3]; 
    }
    void tet_from_ugrid(int32_t* in, vtkIdType* out)
    {
        // just copy
        out[0] = in[0]; 
        out[1] = in[1]; 
        out[2] = in[2]; 
        out[3] = in[3]; 
    }

    void pyramid_from_ugrid(int32_t* in, vtkIdType* out)
    {
        out[0] = in[3];
        out[1] = in[4];
        out[2] = in[1];
        out[3] = in[0];
        out[4] = in[2];
    }
    void prism_from_ugrid(int32_t* in, vtkIdType* out)
    {

        out[0] = in[1]; 
        out[1] = in[0]; 
        out[2] = in[2]; 

        out[3] = in[4]; 
        out[4] = in[3]; 
        out[5] = in[5]; 
    }
    void hex_from_ugrid(int32_t* in, vtkIdType* out)
    {
        // just copy
        out[0] = in[0]; 
        out[1] = in[1]; 
        out[2] = in[2]; 
        out[3] = in[3]; 

        out[4] = in[4]; 
        out[5] = in[5]; 
        out[6] = in[6]; 
        out[7] = in[7]; 
    }

    namespace sys {

        // check host machine endianess
        // source https://stackoverflow.com/a/26315033
        const unsigned one = 1U;

        bool IamLittleEndian()
        {
            return reinterpret_cast<const char*>(&one) + sizeof(unsigned) - 1;
        }

        inline bool IamBigEndian()
        {
            return !IamLittleEndian();
        }

        template <typename T>
         void reverse_bytes(T* ptr)
         {
             static_assert(sizeof(char) == 1, "unexpected size of char");

             char* p = reinterpret_cast<char*>(ptr);
             for(short i = 0 ; i < sizeof(T)/2; i++)
                 std::swap(p[i],p[sizeof(T) -1 - i]);
         }
    }
    std::string remove_last_suffix(const std::string& str)
    {
        std::size_t lastindex = str.find_last_of(".");
        std::string basename  = str.substr(0, lastindex);
        return basename;
    }
    std::string get_last_suffix(const std::string& str)
    {
        std::size_t lastindex = str.find_last_of(".");
        if (lastindex == std::string::npos)
            return "";
        return  str.substr(lastindex);
    }

    bool read_ugrid_mesh(const char* filename, vtkUnstructuredGrid* ugrid)
    {
        std::FILE* file = std::fopen(filename,"r");
        if (file == NULL)
        {
            //vtkErrorMacro(<<"Could not open UGRID file");
            return false;
        }

        std::string name(filename);

        std::string name2 = remove_last_suffix(name);
        std::string suffix = get_last_suffix(name2);

        bool ascii = false;
        bool different_endianess = false;

        if( suffix != ".lb8" and suffix != ".b8")
        {
            ascii = true;
        }
        else
            // binary file
        {
            // is the endianess the same ?

            if (suffix == ".lb8" and sys::IamBigEndian())
            {
                different_endianess = true;
            }
            else if (  suffix == ".b8" and sys::IamLittleEndian())
            {
                different_endianess = true;
            }
        }

        // read header;
        int32_t nnodes,ntria,nquad,ntet,npyra,nprism,nhex;

        if ( not ascii)
        {
            std::fread( &nnodes,sizeof(int32_t),1,file);
            std::fread( &ntria ,sizeof(int32_t),1,file);
            std::fread( &nquad ,sizeof(int32_t),1,file);
            std::fread( &ntet  ,sizeof(int32_t),1,file);
            std::fread( &npyra ,sizeof(int32_t),1,file);
            std::fread( &nprism,sizeof(int32_t),1,file);
            std::fread( &nhex  ,sizeof(int32_t),1,file);
            if(different_endianess)
            {
                sys::reverse_bytes(&nnodes);
                sys::reverse_bytes(&ntria);
                sys::reverse_bytes(&nquad);
                sys::reverse_bytes(&ntet);
                sys::reverse_bytes(&npyra);
                sys::reverse_bytes(&nprism);
                sys::reverse_bytes(&nhex);
            }
        }
        else
        {
            std::fscanf( file, "%d", &nnodes);
            std::fscanf( file, "%d", &ntria );
            std::fscanf( file, "%d", &nquad );
            std::fscanf( file, "%d", &ntet  );
            std::fscanf( file, "%d", &npyra );
            std::fscanf( file, "%d", &nprism);
            std::fscanf( file, "%d", &nhex  );
        }

        vtkSmartPointer<vtkPoints> pts = vtkSmartPointer<vtkPoints>::New();
        pts->SetDataTypeToDouble();
        pts->SetNumberOfPoints(nnodes);

        double pt[3];
        for(int32_t i = 0 ; i < nnodes ; i++)
        {
            if ( not ascii)
            {
                int read = std::fread(pt,sizeof(double),3,file);
                assert(read == 3);
                if(different_endianess)
                {
                    sys::reverse_bytes(&pt[0]);
                    sys::reverse_bytes(&pt[1]);
                    sys::reverse_bytes(&pt[2]);
                }
            }
            else
            {
                std::fscanf( file, "%lf",&pt[0]);
                std::fscanf( file, "%lf",&pt[1]);
                std::fscanf( file, "%lf",&pt[2]);
            }

            pts->SetPoint(i,pt);

        }
        ugrid->SetPoints(pts);

        int32_t totalNumElements = ntria + nquad + ntet + npyra + nprism + nhex;
        ugrid->Allocate(totalNumElements);

        // Create a struct to save boundary information
        // Due to vtk restrictions? we have to save a value for every element
        vtkSmartPointer<vtkIntArray> markers = vtkSmartPointer<vtkIntArray>::New();
        markers->SetNumberOfComponents(1);
        markers->SetNumberOfTuples(totalNumElements);
        markers->SetName("elements_tag");

        int32_t p[8];
        vtkIdType p2[8];
        for(int32_t i = 0 ; i < ntria ; i++)
        {
            if(not ascii)
            {
                std::fread(p,sizeof(int32_t),3,file);
                if(different_endianess)
                {
                    for(int j = 0 ; j < 3; j++) 
                        sys::reverse_bytes(&p[j]);
                }
            }
            else
            {
                for(int j = 0 ; j < 3; j++) 
                    std::fscanf( file, "%d",&p[j]);
            }

            // UGRID is 1-based
            for(int j = 0 ; j < 3;j++) p[j]--;
            tri_from_ugrid(p,p2);
            ugrid->InsertNextCell(VTK_TRIANGLE,3,p2);
        }

        for(int32_t i = 0 ; i < nquad ; i++)
        {
            if ( not ascii)
            {
                std::fread(p,sizeof(int32_t),4,file);
                if(different_endianess)
                {
                    for(int j = 0 ; j < 4; j++) 
                        sys::reverse_bytes(&p[j]);
                }
            }
            else
            {
                for(int j = 0 ; j < 4; j++) 
                    std::fscanf( file, "%d",&p[j]);
            }

            // UGRID is 1-based
            for(int j = 0 ; j < 4;j++) p[j]--;
            quad_from_ugrid(p,p2);
            ugrid->InsertNextCell(VTK_QUAD,4,p2);
        }

        int32_t mindex = 0;

        // surface ids
        for(int32_t i = 0 ; i < ntria ; i++)
        {
            if ( not ascii)
            {
                std::fread(p,sizeof(int32_t),1,file);
                if(different_endianess)
                    sys::reverse_bytes(&p[0]);
            }
            else
                std::fscanf( file, "%d",&p[0]);
            markers->SetValue(mindex,p[0]);
            mindex++;
        }
        for(int32_t i = 0 ; i < nquad ; i++)
        {
            if ( not ascii)
            {
                std::fread(p,sizeof(int32_t),1,file);
                if(different_endianess)
                    sys::reverse_bytes(&p[0]);
            }
            else
                std::fscanf( file, "%d",&p[0]);
            markers->SetValue(mindex,p[0]);
            mindex++;
        }

        int ncorners = 4;
        // tets
        for(int32_t i = 0 ; i < ntet ; i++)
        {
            if(not ascii)
            {
                std::fread(p,sizeof(int32_t),ncorners,file);
                if(different_endianess)
                {
                    for(int j = 0 ; j < ncorners; j++) 
                        sys::reverse_bytes(&p[j]);
                }
            }
            else
            {
                for(int j = 0 ; j < ncorners; j++) 
                    std::fscanf( file, "%d",&p[j]);
            }
            for(int j = 0 ; j < ncorners;j++) p[j]--;
            tet_from_ugrid(p,p2);
            ugrid->InsertNextCell(VTK_TETRA,ncorners,p2);
            markers->SetValue(mindex,0);
            mindex++;
        }
        // pyramids 
        ncorners = 5;
        for(int32_t i = 0 ; i < npyra; i++)
        {
            if( not ascii)
            {
                std::fread(p,sizeof(int32_t),ncorners,file);
                if(different_endianess)
                {
                    for(int j = 0 ; j < ncorners; j++) 
                        sys::reverse_bytes(&p[j]);
                }
            }
            else
            {
                for(int j = 0 ; j < ncorners; j++) 
                    std::fscanf( file, "%d",&p[j]);
            }
            for(int j = 0 ; j < ncorners;j++) p[j]--;
            pyramid_from_ugrid(p,p2);
            ugrid->InsertNextCell(VTK_PYRAMID,ncorners,p2);
            markers->SetValue(mindex,0);
            mindex++;
        }
        // prisms 
        ncorners = 6;
        for(int32_t i = 0 ; i < nprism; i++)
        {
            if( not ascii)
            {
                std::fread(p,sizeof(int32_t),ncorners,file);
                if(different_endianess)
                {
                    for(int j = 0 ; j < ncorners; j++) 
                        sys::reverse_bytes(&p[j]);
                }
            }
            else
            {
                for(int j = 0 ; j < ncorners; j++) 
                    std::fscanf( file, "%d",&p[j]);
            }
            for(int j = 0 ; j < ncorners;j++) p[j]--;
            prism_from_ugrid(p,p2);
            ugrid->InsertNextCell(VTK_WEDGE,ncorners,p2);
            markers->SetValue(mindex,0);
            mindex++;
        }
        // hexahedra 
        ncorners = 8;
        for(int32_t i = 0 ; i < nhex; i++)
        {
            if( not ascii)
            {
                std::fread(p,sizeof(int32_t),ncorners,file);
                if(different_endianess)
                {
                    for(int j = 0 ; j < ncorners; j++) 
                        sys::reverse_bytes(&p[j]);
                }
            }
            else
            {
                for(int j = 0 ; j < ncorners; j++) 
                    std::fscanf( file, "%d",&p[j]);
            }
            for(int j = 0 ; j < ncorners;j++) p[j]--;
            hex_from_ugrid(p,p2);
            ugrid->InsertNextCell(VTK_HEXAHEDRON,ncorners,p2);
            markers->SetValue(mindex,0);
            mindex++;
        }

        ugrid->GetCellData()->AddArray(markers);
        return true;
    }
}
