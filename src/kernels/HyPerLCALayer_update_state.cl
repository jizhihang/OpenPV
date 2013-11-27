#include "../layers/updateStateFunctions.h"


#ifndef PV_USE_OPENCL
#  define CL_KERNEL
#  define CL_MEM_GLOBAL
#  define CL_MEM_CONST
#  define CL_MEM_LOCAL
#else  /* compiling with OpenCL */
#  define CL_KERNEL       __kernel
#  define CL_MEM_GLOBAL   __global
#  define CL_MEM_CONST    __constant
#  define CL_MEM_LOCAL    __local
//#  include "conversions.hcl"
#endif


//
// update the state of an HyPerLCA layer
//
// To allow porting to GPUs, functions called from this function must be
// static inline functions.  If a subclass needs new behavior, it needs to
// have its own static inline function.
//
CL_KERNEL
void HyPerLCALayer_update_state(
    const int numNeurons,
    const int nx,
    const int ny,
    const int nf,
    const int nb,
    const int numChannels,

    CL_MEM_GLOBAL float * V,
    const float Vth,
    const float VMax,
    const float VMin,
    const float VShift,
    const float VWidth,
    const float tau_max,
    const float tau_min,
    const float slope_error_std,
    CL_MEM_GLOBAL float * dt_tau,
    CL_MEM_GLOBAL float * GSynHead,
    CL_MEM_GLOBAL float * activity,
    CL_MEM_GLOBAL double * error_mean,
    CL_MEM_GLOBAL double * error_std)
{
   updateV_HyPerLCALayer(numNeurons, V, GSynHead, activity, error_mean, error_std,
		   VMax, VMin, Vth, VShift, VWidth, dt_tau, tau_max, tau_min, slope_error_std, nx, ny, nf, nb, numChannels);
}
