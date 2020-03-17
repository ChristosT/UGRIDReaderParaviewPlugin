#include "vtkUGRIDReader.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"

#include "vtkUnstructuredGrid.h"

#include "vtkUGRIDReaderInternal.h"



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

