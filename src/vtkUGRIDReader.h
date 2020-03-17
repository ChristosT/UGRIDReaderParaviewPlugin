#ifndef vtkUGRIDReader_h
#define vtkUGRIDReader_h

#include "vtkUnstructuredGridAlgorithm.h"
#include "vtkObjectFactory.h"

class vtkUGRIDReader : public vtkUnstructuredGridAlgorithm
{
public:
  static vtkUGRIDReader* New();
  vtkTypeMacro(vtkUGRIDReader,vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  vtkSetStringMacro(MeshFile);
  vtkGetStringMacro(MeshFile);

protected:
  vtkUGRIDReader();
  ~vtkUGRIDReader() override;

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;
private:
  char* MeshFile;
  vtkUGRIDReader(const vtkUGRIDReader&) = delete; 
  void operator=(const vtkUGRIDReader&) = delete; 
};

#endif
