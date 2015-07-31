## See http://www.cmake.org/Wiki/CMake_Useful_Variables for more variables to set
cmake_minimum_required(VERSION 2.8)

string(FIND ${CMAKE_CURRENT_SOURCE_DIR} "/" pos REVERSE)
MATH(EXPR pos ${pos}+1)
string(SUBSTRING ${CMAKE_CURRENT_SOURCE_DIR} ${pos} -1 PV_AUXLIB_NAME)

#Set CMAKE_MODULE_PATH
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/Modules/")


########
# March 31, 2015: renamed several cache variables to agree with C/C++ preprocessor directives
# so that they could be put into cMakeHeader.h easily instead of passing -D arguments to the compile command.
# Accordingly, test if old variable name exists, and if so, use its value as the new variable.
# then delete the old variable from the cache.  After a while, no one
# should have the old variables in the cache and this section can be deleted.
if (DEFINED OPEN_MP_THREADS)
   set(PV_USE_OPENMP_THREADS ${OPEN_MP_THREADS} CACHE BOOL "Defines whether PetaVision uses OpenMP threads")
   unset(OPEN_MP_THREADS CACHE)
endif (DEFINED OPEN_MP_THREADS)
if (DEFINED OPEN_CL_GPU)
   set(PV_USE_OPENCL ${OPEN_CL_GPU} CACHE BOOL "Defines whether PetaVision uses OpenCL GPU")
   unset(OPEN_CL_GPU CACHE)
endif (DEFINED OPEN_CL_GPU)
if (DEFINED CUDA_GPU)
   set(PV_USE_CUDA ${CUDA_GPU} CACHE BOOL "Defines whether PetaVision uses CUDA GPU")
   unset(CUDA_GPU CACHE)
endif (DEFINED CUDA_GPU)
if (DEFINED CUDNN)
   set(PV_USE_CUDNN ${CUDNN} CACHE BOOL "Defines whether PetaVision uses CUDA's cuDNN library")
   unset(CUDNN CACHE)
endif (DEFINED CUDNN)
########


# Variables to set on the command line
set(PV_USE_OPENMP_THREADS FALSE CACHE BOOL "Defines if PetaVision uses OpenMP threads")
set(PV_USE_OPENCL FALSE CACHE BOOL "Defines if PetaVision uses OpenCL GPU")
set(OpenCL_dir /usr/local/cuda-6.0/ CACHE PATH "Location of opencl headers")
set(PV_USE_CUDA FALSE CACHE BOOL "Defines if PetaVision uses CUDA GPU")
set(CUDA_RELEASE FALSE CACHE BOOL "Defines if Cuda compiles with optimization")
set(PV_USE_CUDNN FALSE CACHE BOOL "Defines if PetaVision uses Cuda's CUDNN library")
set(CUDNN_PATH /nh/compneuro/Data/cuDNN/cudnn-6.5-linux-R1/ CACHE PATH "Location of cuDNN libraries")
set(PV_BUILD_SHARED FALSE CACHE BOOL "Defines if the pv object is built as a shared object")
set(CLANG_OMP FALSE CACHE BOOL "Defines if we're compiling with Clang + omp support")
set(PV_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../trunk CACHE PATH "The base PetaVision directory")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PV_DIR}/docs/cmake/Modules/")

IF(PV_USE_CUDA AND PV_USE_OPENCL)
   message(FATAL_ERROR "PV_USE_CUDA and PV_USE_OPENCL cannot both be true")
ENDIF(PV_USE_CUDA AND PV_USE_OPENCL)

IF(NOT EXISTS "${PV_DIR}")
   message(FATAL_ERROR "PV_DIR \"${PV_DIR}\" does not exist.")
ENDIF(NOT EXISTS "${PV_DIR}")

IF(APPLE AND CLANG_OMP AND NOT DEFINED APPLE_CLANG_OMP_FLAGS_ADDED)
   message(WARNING "Using OpenMP threads with Clang. Make sure that your version of Clang supports OpenMP.")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libstdc++")
   set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -stdlib=libstdc++")
   set(APPLE_CLANG_OMP_FLAGS_ADDED TRUE)
ENDIF(APPLE AND CLANG_OMP AND NOT DEFINED APPLE_CLANG_OMP_FLAGS_ADDED)

if(PV_USE_OPENMP_THREADS AND NOT DEFINED PV_USE_OPENMP_THREADS_FLAGS_ADDED)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp")
   set(PV_USE_OPENMP_THREADS_FLAGS_ADDED TRUE)
endif (PV_USE_OPENMP_THREADS AND NOT DEFINED PV_USE_OPENMP_THREADS_FLAGS_ADDED)

if(PV_USE_OPENCL AND NOT DEFINED PV_USE_OPENCL_FLAGS_ADDED)
   IF(APPLE)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -framework opencl")
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -framework opencl")
   ELSE(APPLE)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I ${OpenCL_dir}/include -L/${OpenCL_dir}/lib64 -lOpenCL")
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I ${OpenCL_dir}/include -L/${OpenCL_dir}/lib64 -lOpenCL")
   ENDIF(APPLE)
   set(PV_USE_OPENCL_FLAGS_ADDED)
endif (PV_USE_OPENCL AND NOT DEFINED PV_USE_OPENCL_FLAGS_ADDED)

if(PV_USE_CUDA AND NOT DEFINED PV_USE_CUDA_FLAGS_ADDED)
    #Set include file
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I ${CUDA_TOOLKIT_INCLUDE}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I ${CUDA_TOOLKIT_INCLUDE}")
    if(PV_USE_CUDNN)
       set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I ${CUDNN_PATH}")
       set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I ${CUDNN_PATH}")
    endif(PV_USE_CUDNN)
    set(PV_USE_CUDA_FLAGS_ADDED)
endif(PV_USE_CUDA AND NOT DEFINED PV_USE_CUDA_FLAGS_ADDED)

set(CMAKE_C_FLAGS_DEBUG "-g3 -O0")
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0")
set(CMAKE_C_FLAGS_RELEASE "-g0 -O3")
set(CMAKE_CXX_FLAGS_RELEASE "-g0 -O3")

set(PV_AUXLIB_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(PV_AUXLIB_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")

######################################################################################
######################################################################################
##
## MAIN CODE
##
######################################################################################
######################################################################################

set(LIBRARY_OUTPUT_PATH ${PV_AUXLIB_LIB_DIR})

file(GLOB libSrcCPP ${PV_AUXLIB_SRC_DIR}/*.cpp)
set(PVauxlibSrc ${PVauxlibSrc} ${libSrcCPP})
file(GLOB libSrcC ${PV_AUXLIB_SRC_DIR}/*.c)
set(PVauxlibSrc ${PVauxlibSrc} ${libSrcC})
if(PV_USE_CUDA)
   file(GLOB libSrcCU ${PV_AUXLIB_SRC_DIR}/*.cu)
   set(PVauxlibSrcCu ${PVauxlibSrcCu} ${libSrcCU})
endif(PV_USE_CUDA)

# The user may specify a non-standard compiler name for MPI
#  For example, the user may type:
#    cmake CMakeLists.txt -DCMAKE_C_COMPILER=openmpicc -DCMAKE_CXX_COMPILER=openmpic++
#

if (NOT DEFINED MPI_C_COMPILER OR NOT DEFINED MPI_CXX_COMPILER)
    MESSAGE("-- Searching for MPI library ............")
    find_package(MPI REQUIRED)
endif (NOT DEFINED MPI_C_COMPILER OR NOT DEFINED MPI_CXX_COMPILER)
if (NOT GDAL_FOUND)
    MESSAGE("-- Searching for GDAL library ...........")
    find_package(GDAL REQUIRED)
endif (NOT GDAL_FOUND)

set(CMAKE_C_COMPILER "${MPI_C_COMPILER}")
set(CMAKE_CXX_COMPILER "${MPI_CXX_COMPILER}")

if (PV_USE_CUDA)
    MESSAGE("-- Searching for CUDA library ............")
    find_package(CUDA)
endif (PV_USE_CUDA)

#IF(CMAKE_BUILD_TYPE MATCHES DEBUG)
#   set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS}; -g3; -G3; -O0)
#ELSE(CMAKE_BUILD_TYPE MATCHES RELEASE)
#   set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS}; -g0; -G0; -O3)
#endif(CMAKE_BUILD_TYPE)

if(PV_USE_CUDNN)
   set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS}; -arch=sm_30; )
else(PV_USE_CUDNN)
   set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS}; -arch=sm_20;)
endif(PV_USE_CUDNN)


if(CUDA_RELEASE)
   set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS}; -O;)
else(CUDA_RELEASE)
   set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS}; -Xptxas; -v; -keep; -lineinfo; -g; -G;)
endif(CUDA_RELEASE)
#cuda_compile(CUDA_O ${PVauxlibSrcCu}) 

# Add PetaVision
include_directories(${PV_DIR}/src)
link_directories(${PV_DIR}/lib)

if(PV_USE_CUDA)
   if(PV_BUILD_SHARED)
      cuda_add_library(${PV_AUXLIB_NAME} SHARED ${PVauxlibSrc} ${PVauxlibSrcCu})
   else(PV_BUILD_SHARED)
      cuda_add_library(${PV_AUXLIB_NAME} STATIC ${PVauxlibSrc} ${PVauxlibSrcCu})
   endif(PV_BUILD_SHARED)
else(PV_USE_CUDA)
   if(PV_BUILD_SHARED)
      add_library(${PV_AUXLIB_NAME} SHARED ${PVauxlibSrc})
   else(PV_BUILD_SHARED)
      add_library(${PV_AUXLIB_NAME} STATIC ${PVauxlibSrc})
   endif(PV_BUILD_SHARED)
endif(PV_USE_CUDA)

# Link executable to external libraries
target_link_libraries(${PV_AUXLIB_NAME} pv)
target_link_libraries(${PV_AUXLIB_NAME} ${GDAL_LIBRARY})
target_link_libraries(${PV_AUXLIB_NAME} ${MPI_LIBRARY})

IF(PV_USE_CUDNN)
   IF(APPLE)
      target_link_libraries(${PV_AUXLIB_NAME} ${CUDNN_PATH}/libcudnn.dylib)
   ELSE(APPLE)
      target_link_libraries(${PV_AUXLIB_NAME} ${CUDNN_PATH}/libcudnn.so)
   ENDIF(APPLE)
ENDIF(PV_USE_CUDNN)
