# This CMakeLists.txt file can be used to generate makefiles for several projects at once.
# It should be put in the directory containing the projects.

enable_testing()

# See http://www.cmake.org/Wiki/CMake_Useful_Variables for more variables to set
cmake_minimum_required(VERSION 2.8.10)

#Define function for adding a test to the test harness
macro(AddPVTest BaseName ParamNames inFlags inMpi)
   #Case based on size of paramNames
   list(LENGTH ${ParamNames} numParams)
   #No params case
   if(${numParams} EQUAL 0)
      set(testName ${BaseName})
      #One process
      add_test(${testName}_1 ${CMAKE_COMMAND} -E chdir ${BaseName}
         Debug/${BaseName} ${inFlags} -l
         ${testName}_1.log)
      if(${inMpi})
         #Two processes
         add_test(${testName}_2 ${CMAKE_COMMAND} -E chdir ${BaseName}
            ${MPIEXEC} -np 2 Debug/${BaseName} ${inFlags} -l
            ${testName}_2.log)
         #Four processes
         add_test(${testName}_4 ${CMAKE_COMMAND} -E chdir ${BaseName}
            ${MPIEXEC} -np 4 Debug/${BaseName} ${inFlags} -l
            ${testName}_4.log)
         #Add dependencies
         set_tests_properties(${testName}_2 PROPERTIES DEPENDS ${testName}_1)
         set_tests_properties(${testName}_4 PROPERTIES DEPENDS ${testName}_2)
      endif(${inMpi})
   else(${numParams} EQUAL 0)
      #Multiple params here
      foreach(param IN LISTS ${ParamNames})
         #Set test name based on number of parameters
         if(${numParams} GREATER 1)
            set(testName ${BaseName}_${param})
         else(${numParams} GREATER 1)
            set(testName ${BaseName})
         endif(${numParams} GREATER 1)
         #One process
         add_test(${testName}_1 ${CMAKE_COMMAND} -E chdir ${BaseName}
            Debug/${BaseName} -p input/${param}.params ${inFlags} -l
            ${testName}_1.log)
         if(${inMpi})
            #Two processes
            add_test(${testName}_2 ${CMAKE_COMMAND} -E chdir ${BaseName}
               ${MPIEXEC} -np 2 Debug/${BaseName} -p input/${param}.params ${inFlags} -l
               ${testName}_2.log)
            #Four processes
            add_test(${testName}_4 ${CMAKE_COMMAND} -E chdir ${BaseName}
               ${MPIEXEC} -np 4 Debug/${BaseName} -p input/${param}.params ${inFlags} -l
               ${testName}_4.log)
            #Add dependencies
            set_tests_properties(${testName}_2 PROPERTIES DEPENDS ${testName}_1)
            set_tests_properties(${testName}_4 PROPERTIES DEPENDS ${testName}_2)
         endif(${inMpi})
      endforeach(param)
   endif(${numParams} EQUAL 0)
endmacro(AddPVTest)

#Set CMAKE_MODULE_PATH
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PV_DIR}/docs/cmake/Modules/")


# Set variable for current working directory (workspace directory)
set(PV_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../PetaVision CACHE PATH "The base petavision directory")

set(PV_USE_OPENMP_THREADS FALSE CACHE BOOL "Defines if PetaVision uses OpenMP threads")
set(PV_USE_OPENCL FALSE CACHE BOOL "Defines if PetaVision uses OpenCL GPU")
set(PV_USE_CUDA FALSE CACHE BOOL "Defines if PetaVision uses CUDA GPU")
set(CUDA_RELEASE FALSE CACHE BOOL "Defines if Cuda compiles with optimization")
#set(PV_USE_CUDNN FALSE CACHE BOOL "Defines if PetaVision uses Cuda's CUDNN library")
set(PV_SYSTEM_TEST_THREADS 1 CACHE STRING "Defines the number of threads to use for system tests")
set(CUDNN_PATH ${CUDNN_PATH_DEFAULT} CACHE PATH "Location of cuDNN libraries")
set(OpenCL_dir /usr/local/cuda-6.5/ CACHE PATH "Location of opencl headers")
set(PV_OPENMP_COMPILER_FLAG "-fopenmp" CACHE STRING "Compiler flag for compiling with OpenMP")


if("${PV_SYSTEM_TEST_THREADS}" MATCHES "^([0-9]+|)$")
   #Do nothing, correct
else("${PV_SYSTEM_TEST_THREADS}" MATCHES "^([0-9]+|)$")
   message(FATAL_ERROR "PV_SYSTEM_TEST_THREADS must be a positive integer")
endif("${PV_SYSTEM_TEST_THREADS}" MATCHES "^([0-9]+|)$")

IF(PV_USE_CUDA AND PV_USE_OPENCL)
   message(FATAL_ERROR "PV_USE_CUDA and PV_USE_OPENCL cannot both be true")
ENDIF(PV_USE_CUDA AND PV_USE_OPENCL)

# If CUDA is being used, the PV_USE_CUDNN needs to be set
if(PV_USE_CUDA)
   set(PV_USE_CUDNN TRUE)
endif(PV_USE_CUDA)

IF(NOT EXISTS "${PV_DIR}")
   message(FATAL_ERROR "PV_DIR \"${PV_DIR}\" does not exist.")
ENDIF(NOT EXISTS "${PV_DIR}")

if(PV_USE_OPENCL AND NOT DEFINED PV_USE_OPENCL_FLAGS_ADDED)
   IF(APPLE)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -framework opencl")
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -framework opencl")
   ELSE(APPLE)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I ${OpenCL_dir}/include -L/${OpenCL_dir}/lib64 -lOpenCL")
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I ${OpenCL_dir}/include -L/${OpenCL_dir}/lib64 -lOpenCL")
   ENDIF(APPLE)
   set(PV_USE_OPENCL_FLAGS_ADDED TRUE)
endif (PV_USE_OPENCL AND NOT DEFINED PV_USE_OPENCL_FLAGS_ADDED)

add_subdirectory(MatchingPursuitTest)

#Add tests here

set(testBaseName MatchingPursuitTest) #The base name of the test
set(testParams MatchingPursuitTest) #Names of the parameter files in the input directory, leaving out .params
set(testFlags -t ${PV_SYSTEM_TEST_THREADS}) #Extra flags to pass to PetaVision test
set(testMpi TRUE) #Whether to call inside mpiexec with multiple processes
AddPVTest(${testBaseName} testParams "${testFlags}" ${testMpi})
