/*
 * CLDevice.hpp
 *
 *  Created on: Oct 24, 2009
 *      Author: Craig Rasmussen
 */

#ifndef CLDEVICE_HPP_
#define CLDEVICE_HPP_

#include "pv_opencl.h"
#include <stdlib.h>

////////////////////////////////////////////////////////////////////////////////

namespace PV {
   
class CLBuffer;
class CLKernel;

class CLDevice {

protected:
   int device;                           // device id (normally 0 for GPU, 1 for CPU)

public:
   CLDevice(int device);

   int initialize(int device);

   static void print_error_code(int code);

   CLBuffer * createBuffer(cl_mem_flags flags, size_t size, void * host_ptr);

   CLBuffer * createReadBuffer(size_t size)   { return createBuffer(CL_MEM_READ_ONLY, size, NULL); }
   CLBuffer * createWriteBuffer(size_t size)  { return createBuffer(CL_MEM_WRITE_ONLY, size, NULL); }
   CLBuffer * createBuffer(size_t size, void * host_ptr)
                                              { return createBuffer(CL_MEM_USE_HOST_PTR, size, host_ptr); }

   CLKernel * createKernel(const char * filename, const char * name);
   
#ifdef PV_USE_OPENCL

//   int copyResultsBuffer(cl_mem output, void * results, size_t size);

   int query_device_info();
   int query_device_info(int id, cl_device_id device);

protected:

   cl_uint num_devices;                  // number of computing devices

   cl_device_id device_ids[MAX_DEVICES]; // compute device id
   cl_context context;                   // compute context
   cl_command_queue commands;            // compute command queue

   //size_t global;                        // global domain size for our calculation
   //size_t local;                         // local domain size for our calculation

#endif /* PV_USE_OPENCL */
};

} // namespace PV

#endif /* CLDEVICE_HPP_ */
