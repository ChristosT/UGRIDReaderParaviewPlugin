#ifndef vtkUGRIDInternal_h
#define vtkUGRIDInternal_h
class vtkUnstructuredGrid;
bool read_ugrid_mesh(const char* filename, vtkUnstructuredGrid* ugrid);
#endif
