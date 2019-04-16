# create a plugin that adds a reader to the ParaView GUI
# it is added in the file dialog when doing open

cmake_minimum_required(VERSION 2.8)
set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries")

find_package(ParaView REQUIRED)
include(${PARAVIEW_USE_FILE})
set(UGRID_READER_LIB_NAME vtkUGRIDReader)
ADD_PARAVIEW_PLUGIN(${UGRID_READER_LIB_NAME} "1.0"
  SERVER_MANAGER_XML vtkUGRIDReader.xml
  SERVER_MANAGER_SOURCES vtkUGRIDReader.cxx
  REQUIRED_ON_SERVER)

target_link_libraries(${UGRID_READER_LIB_NAME} PUBLIC ${VTK_LIBRARIES})
