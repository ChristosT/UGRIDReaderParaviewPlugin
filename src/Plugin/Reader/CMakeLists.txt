set(classes
    vtkUGRIDReader
    vtkUGRIDReaderInternal
    )

vtk_module_add_module(UGRIDReader
  CLASSES ${classes}
  )

paraview_add_server_manager_xmls(
  XMLS  vtkUGRIDReader.xml)
