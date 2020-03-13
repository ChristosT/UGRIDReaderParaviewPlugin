paraview_add_plugin(UGRIDReaderParaview
  VERSION "2.0"
  MODULES UGRIDReader
  REQUIRED_ON_SERVER
  MODULE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/Reader/vtk.module")

