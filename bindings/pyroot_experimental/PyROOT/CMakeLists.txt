############################################################################
# CMakeLists.txt file for building ROOT bindings/pyroot_experimental package
############################################################################

set(py_sources
  ROOT/__init__.py
  ROOT/_application.py
  ROOT/_facade.py
  ROOT/pythonization/__init__.py
  ROOT/pythonization/_drawables.py
  ROOT/pythonization/_generic.py
  ROOT/pythonization/_rooabscollection.py
  ROOT/pythonization/_rdataframe.py
  ROOT/pythonization/_rvec.py
  ROOT/pythonization/_stl_vector.py
  ROOT/pythonization/_tarray.py
  ROOT/pythonization/_tclonesarray.py
  ROOT/pythonization/_tcollection.py
  ROOT/pythonization/_tdirectory.py
  ROOT/pythonization/_tdirectoryfile.py
  ROOT/pythonization/_tfile.py
  ROOT/pythonization/_tgraph.py
  ROOT/pythonization/_th1.py
  ROOT/pythonization/_titer.py
  ROOT/pythonization/_tobject.py
  ROOT/pythonisation/_tobjstring.py
  ROOT/pythonization/_tseqcollection.py
  ROOT/pythonisation/_tstring.py
  ROOT/pythonization/_ttree.py
  ROOT/pythonization/_tvector3.py
  ROOT/pythonization/_tvectort.py
)

set(sources
  src/CppCallablePyz.cxx
  src/PyROOTModule.cxx
  src/PyROOTStrings.cxx
  src/PyROOTWrapper.cxx
  src/RPyROOTApplication.cxx
  src/GenericPyz.cxx
  src/RDataFramePyz.cxx
  src/RVecPyz.cxx
  src/TClassPyz.cxx
  src/TClonesArrayPyz.cxx
  src/TDirectoryPyz.cxx
  src/TFilePyz.cxx
  src/TObjectPyz.cxx
  src/TTreePyz.cxx
  src/PyzCppHelpers.cxx
  src/PyzPythonHelpers.cxx
)

file(COPY python/ROOT DESTINATION ${localruntimedir})
install(DIRECTORY python/ROOT DESTINATION ${runtimedir})

set(d $ENV{DESTDIR}/${CMAKE_INSTALL_PREFIX}/${runtimedir})
foreach(py_source ${py_sources})
  install(CODE "execute_process(COMMAND ${PYTHON_EXECUTABLE} -m py_compile ${d}/${py_source})")
  install(CODE "execute_process(COMMAND ${PYTHON_EXECUTABLE} -O -m py_compile ${d}/${py_source})")
endforeach()

ROOT_LINKER_LIBRARY(ROOTPython ${sources} LIBRARIES Core Tree cppyy)
ROOT_INSTALL_HEADERS(inc)

# Disables warnings originating from deprecated register keyword in Python
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND CMAKE_CXX_STANDARD GREATER_EQUAL 11)
    target_compile_options(ROOTPython PRIVATE -Wno-register)
endif()
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang" AND CMAKE_CXX_STANDARD GREATER_EQUAL 11)
    target_compile_options(ROOTPython PRIVATE -Wno-deprecated-register)
endif()

ROOT_ADD_TEST_SUBDIRECTORY(test)
