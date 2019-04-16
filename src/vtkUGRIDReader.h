#ifndef vtkSU2eader_h
#define vtkSU2Reader_h

#include "vtkUnstructuredGridAlgorithm.h"
#include "vtkObjectFactory.h"

class vtkUGRIDReader : public vtkUnstructuredGridAlgorithm
{
public:
  static vtkUGRIDReader* New();
  vtkTypeMacro(vtkUGRIDReader,vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  vtkSetStringMacro(MeshFile);

  vtkGetStringMacro(MeshFile);

protected:
  vtkUGRIDReader();
  ~vtkUGRIDReader() VTK_OVERRIDE;

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) VTK_OVERRIDE;
private:
  char* MeshFile;
  vtkUGRIDReader(const vtkUGRIDReader&) VTK_DELETE_FUNCTION;
  void operator=(const vtkUGRIDReader&) VTK_DELETE_FUNCTION;
};

#endif
