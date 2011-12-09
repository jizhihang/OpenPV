/*
 * Retina.cpp
 *
 *  Created on: Jul 29, 2008
 *
 */

#include "HyPerLayer.hpp"
#include "Retina.hpp"
#include "../io/io.h"
#include "../include/default_params.h"
#include "../utils/cl_random.h"
#include "../utils/pv_random.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

void Retina_spiking_update_state (
    const float time,
    const float dt,
    const int nx,
    const int ny,
    const int nf,
    const int nb,
    Retina_params * params,
    uint4 * rnd,
    float * phiExc,
    float * phiInh,
    float * activity,
    float * prevTime);

void Retina_nonspiking_update_state (
    const float time,
    const float dt,
    const int nx,
    const int ny,
    const int nf,
    const int nb,
    Retina_params * params,
    float * phiExc,
    float * phiInh,
    float * activity);

#ifdef __cplusplus
}
#endif


namespace PV {

Retina::Retina() {
   initialize_base();
   // Default constructor to be called by derived classes.
   // It doesn't call Retina::initialize; instead, the derived class
   // should explicitly call Retina::initialize in its own initialization,
   // the way that Retina::initialize itself calls HyPerLayer::initialization.
   // This way, virtual methods called by initialize will be overridden
   // as expected.
}

Retina::Retina(const char * name, HyPerCol * hc) {
   initialize_base();
   initialize(name, hc, TypeRetina);
}

Retina::~Retina()
{
   free(rand_state);

// Moved to HyPerLayer since evList is a HyPerLayer member variable
// #ifdef PV_USE_OPENCL
//    free(evList);
// #endif
}

int Retina::initialize_base() {
   rand_state = NULL;
   return PV_SUCCESS;
}

int Retina::initialize(const char * name, HyPerCol * hc, PVLayerType type) {
   int status = HyPerLayer::initialize(name, hc, NUM_RETINA_CHANNELS);
   clayer->layerType = type;

   PVLayer * l = clayer;

   // clayer->layerType = type; // done during call to HyPerLayer::initialize
   setParams(parent->parameters());

   // the size of the Retina may have changed due to size of image
   //
   const int nx = l->loc.nx;
   const int ny = l->loc.ny;
   const int nf = l->loc.nf;
   const int nb = l->loc.nb;
   l->numNeurons  = nx * ny * nf;
   l->numExtended = (nx + 2*nb) * (ny + 2*nb) * nf;

   // random seed should be different for different layers
   unsigned int seed = (unsigned int) (parent->getRandomSeed() + getLayerId());

   // a random state variable is needed for every neuron/clthread
   rand_state = cl_random_init(l->numNeurons, seed);

   // status = parent->addLayer(this); // done during call to HyPerLayer::initializie

#ifdef PV_USE_OPENCL
   CLDevice * device = parent->getCLDevice();

   numWait = 0;
   numEvents = NUM_RETINA_EVENTS;
   evList = (cl_event *) malloc(numEvents*sizeof(cl_event));
   assert(evList != NULL);

   // TODO - fix to use device and layer parameters
   if (device->id() == 1) {
      nxl = 1;  nyl = 1;
   }
   else {
      nxl = 16; nyl = 8;
   }

   const char * kernel_name;
   if (spikingFlag) {
      kernel_name = "Retina_spiking_update_state";
   }
   else {
      kernel_name = "Retina_nonspiking_update_state";
   }

   initializeThreadBuffers(kernel_name);
   initializeThreadKernels(kernel_name);
#endif

   return status;
}

#ifdef PV_USE_OPENCL
/**
 * Initialize OpenCL buffers.  This must be called after PVLayer data have
 * been allocated.
 */
int Retina::initializeThreadBuffers(const char * kernel_name)
{
   int status = HyPerLayer::initializeThreadBuffers(kernel_name);

   CLDevice * device = parent->getCLDevice();

   clParams = device->createBuffer(CL_MEM_COPY_HOST_PTR, sizeof(rParams), &rParams);
   clRand   = device->createBuffer(CL_MEM_COPY_HOST_PTR, getNumNeurons()*sizeof(uint4), rand_state);

   return status;
}

int Retina::initializeThreadKernels(const char * kernel_name)
{
   char kernelPath[256];
   char kernelFlags[256];

   int status = CL_SUCCESS;
   CLDevice * device = parent->getCLDevice();

   const char * pvRelPath = "../PetaVision";
   sprintf(kernelPath,  "%s/%s/src/kernels/Retina_update_state.cl", parent->getPath(), pvRelPath);
   sprintf(kernelFlags, "-D PV_USE_OPENCL -cl-fast-relaxed-math -I %s/%s/src/kernels/", parent->getPath(), pvRelPath);

   // create kernels
   //
   if (spikingFlag) {
      krUpdate = device->createKernel(kernelPath, kernel_name, kernelFlags);
   }
   else {
      krUpdate = device->createKernel(kernelPath, "Retina_nonspiking_update_state", kernelFlags);
   }

   int argid = 0;

   status |= krUpdate->setKernelArg(argid++, parent->simulationTime());
   status |= krUpdate->setKernelArg(argid++, parent->getDeltaTime());

   status |= krUpdate->setKernelArg(argid++, clayer->loc.nx);
   status |= krUpdate->setKernelArg(argid++, clayer->loc.ny);
   status |= krUpdate->setKernelArg(argid++, clayer->loc.nf);
   status |= krUpdate->setKernelArg(argid++, clayer->loc.nb);

   status |= krUpdate->setKernelArg(argid++, clParams);
   if (spikingFlag) {
      status |= krUpdate->setKernelArg(argid++, clRand);
   }

   status |= krUpdate->setKernelArg(argid++, getChannelCLBuffer(CHANNEL_EXC));
   status |= krUpdate->setKernelArg(argid++, getChannelCLBuffer(CHANNEL_INH));
   status |= krUpdate->setKernelArg(argid++, clActivity);
   if (spikingFlag) {
      status |= krUpdate->setKernelArg(argid++, clPrevTime);
   }

   return status;
}
#endif

int Retina::initializeV() {
   assert(parent->parameters()->value(name, "restart", 0.0f, false)==0.0f); // initializeV should only be called if restart is false
   // Retina doesn't use the V buffer so free it and set the pointer to null.
   free(clayer->V);
   clayer->V = NULL;
   return PV_SUCCESS;
}

int Retina::setParams(PVParams * p)
{
   float dt_sec = parent->getDeltaTime() * .001;  // seconds

   clayer->loc.nf = 1;
   clayer->loc.nb = (int) p->value(name, "marginWidth", 0.0);

   clayer->params = &rParams;

   spikingFlag = (int) p->value(name, "spikingFlag", 1);

   float probStim = 1.0f;
   float probBase = 0.0f;
   if (p->present(name, "noiseOnFreq")) {
      probStim = p->value(name, "noiseOnFreq") * dt_sec;
   }
   else {
      probStim = p->value(name, "poissonEdgeProb" , 1.0f);
   }
   if (probStim > 1.0) probStim = 1.0f;
   if (p->present(name, "noiseOffFreq")) {
      probBase = p->value(name, "noiseOffFreq") * dt_sec;
   }
   else {
      probBase = p->value(name, "poissonBlankProb", 0.0f);
   }
   if (probBase > 1.0) probBase = 1.0f;

   // default parameters
   //
   rParams.probStim  = probStim;
   rParams.probBase  = probBase;
   rParams.beginStim = p->value(name, "beginStim", 0.0f);
   rParams.endStim   = p->value(name, "endStim"  , 99999999.9f);
   rParams.burstFreq = p->value(name, "burstFreq", 40);         // frequency of bursts
   rParams.burstDuration = p->value(name, "burstDuration", 20); // duration of each burst, <=0 -> sinusoidal
   rParams.refactory_period = REFACTORY_PERIOD;
   rParams.abs_refactory_period = REFACTORY_PERIOD;

   return 0;
}


int Retina::updateStateOpenCL(float time, float dt)
{
   int status = CL_SUCCESS;

#ifdef PV_USE_OPENCL
   // wait for memory to be copied to device
   status |= clWaitForEvents(numWait, evList);
   for (int i = 0; i < numWait; i++) {
      clReleaseEvent(evList[i]);
   }
   numWait = 0;

   status |= krUpdate->setKernelArg(0, time);
   status |= krUpdate->setKernelArg(1, dt);
   status |= krUpdate->run(getNumNeurons(), nxl*nyl, 0, NULL, &evUpdate);
   krUpdate->finish();

#if PV_CL_COPY_BUFFERS
   status |= clPhiE    ->copyFromDevice(1, &evUpdate, &evList[EV_R_PHI_E]);
   status |= clPhiI    ->copyFromDevice(1, &evUpdate, &evList[EV_R_PHI_I]);
   status |= clActivity->copyFromDevice(1, &evUpdate, &evList[EV_R_ACTIVITY]);
   numWait += 3;
#endif
#endif

   return status;
}

int Retina::triggerReceive(InterColComm* comm)
{
   int status = HyPerLayer::triggerReceive(comm);

   // copy data to device
   //
#ifdef PV_USE_OPENCL
#if PV_CL_COPY_BUFFERS
   status |= clPhiE->copyToDevice(&evList[EV_R_PHI_E]);
   status |= clPhiI->copyToDevice(&evList[EV_R_PHI_I]);
   numWait += 2;
#endif
#endif

   return status;
}

int Retina::waitOnPublish(InterColComm* comm)
{
   int status = HyPerLayer::waitOnPublish(comm);

   // copy activity to device
   //
#ifdef PV_USE_OPENCL
#if PV_CL_COPY_BUFFERS
   status |= clActivity->copyToDevice(&evList[EV_R_ACTIVITY]);
#endif
   numWait += 1;
#endif

   return status;
}

//! Updates the state of the Retina
/*!
 * REMARKS:
 *      - prevActivity[] buffer holds the time when a neuron last spiked.
 *      - not used if nonspiking
 *      - it sets the probStim and probBase.
 *              - probStim = noiseOnFreq * dt_sec * (phiExc - phiInh); the last ()  is V[k];
 *              - probBase = noiseOffFreq * dt_sec;
 *              .
 *      - activity[] is set to 0 or 1 depending on the return of spike()
 *      - this depends on the last time a neuron spiked as well as on V[]
 *      at the location of the neuron. This V[] is set by calling updateImage().
 *      - V points to the same memory space as data in the Image so that when Image
 *      is updated, V gets updated too.
 *      .
 *      .
 *
 *
 */
int Retina::updateState(float time, float dt)
{
   update_timer->start();
#ifndef PV_USE_OPENCL

   const int nx = clayer->loc.nx;
   const int ny = clayer->loc.ny;
   const int nf = clayer->loc.nf;
   const int nb = clayer->loc.nb;

   pvdata_t * phiExc   = getChannel(CHANNEL_EXC);
   pvdata_t * phiInh   = getChannel(CHANNEL_INH);
   pvdata_t * activity = clayer->activity->data;

   if (spikingFlag == 1) {
      Retina_spiking_update_state(time, dt, nx, ny, nf, nb,
                                  &rParams, rand_state,
                                  phiExc, phiInh, activity, clayer->prevActivity);
   }
   else {
      Retina_nonspiking_update_state(time, dt, nx, ny, nf, nb,
                                     &rParams, phiExc, phiInh, activity);
   }

#else

   updateStateOpenCL(time, dt);

#endif

#ifdef DEBUG_PRINT
   char filename[132];
   sprintf(filename, "r_%d.tiff", (int)(2*time));
   this->writeActivity(filename, time);

   printf("----------------\n");
   for (int k = 0; k < 6; k++) {
      printf("host:: k==%d h_exc==%f h_inh==%f\n", k, phiExc[k], phiInh[k]);
   }
   printf("----------------\n");

#endif
   update_timer->stop();
   return 0;
}

int Retina::updateBorder(float time, float dt)
{
   // wait for OpenCL data transfers to finish
   HyPerLayer::updateBorder(time, dt);

   // Data has arrived from OpenCL device now safe to add background
   // activity to border regions.  Update all of the regions regions
   // even if using MPI and the border may be from an adjacent processor.
   // TODO - check that MPI will overwrite the border regions after this
   // function has been called.

   const PVLayerLoc * loc = getLayerLoc();

   const int nb = loc->nb;
   if (nb == 0) return 0;

   const int nx = loc->nx;
   const int ny = loc->ny;
   const int nf = loc->nf;

   const int nx_ex = nx + 2*nb;
   const int sy = nf*nx_ex;

   pvdata_t * activity_top = &clayer->activity->data[0];
   pvdata_t * activity_bot = &clayer->activity->data[(nb+ny)*sy];

   pvdata_t * activity_l = &clayer->activity->data[nb*sy];
   pvdata_t * activity_r = &clayer->activity->data[nb*sy + nb+nx];

   // top and bottom borders (including corners)
   for (int kex = 0; kex < nx_ex*nf*nb; kex++) {
      activity_top[kex] = (pv_random_prob() < rParams.probBase) ? 1.0 : 0.0;
      activity_bot[kex] = (pv_random_prob() < rParams.probBase) ? 1.0 : 0.0;
   }

   // left and right borders
   for (int y = 0; y < ny; y++) {
      for (int x = 0; x < nf*nb; x++) {
         activity_l[x] = (pv_random_prob() < rParams.probBase) ? 1.0 : 0.0;
         activity_r[x] = (pv_random_prob() < rParams.probBase) ? 1.0 : 0.0;
      }
      activity_l += sy;
      activity_r += sy;
   }

   return 0;
}

int Retina::writeState(float time)
{
   int status = HyPerLayer::writeState(time);

   // print activity at center of image

#ifdef DEBUG_OUTPUT
   int sx = clayer->numFeatures;
   int sy = sx*clayer->loc.nx;
   pvdata_t * a = clayer->activity->data;

   for (int k = 0; k < clayer->numExtended; k++) {
      if (a[k] == 1.0) printf("a[%d] == 1\n", k);
   }

  int n = (int) (sy*(clayer->loc.ny/2 - 1) + sx*(clayer->loc.nx/2));
  for (int f = 0; f < clayer->numFeatures; f++) {
     printf("a[%d] = %f\n", n, a[n]);
     n += 1;
  }
#endif

   return status;
}

int Retina::outputState(float time, bool last)
{
   if( spikingFlag ) updateActiveIndices();
   return HyPerLayer::outputState(time, last);

#ifdef OBSOLETE // Marked obsolete Aug. 31, 2011.
       // writeActivity (called by HyPerLayer::outputState) now writes clayer->activity->data, not clayer->V
       // so it's not necessary to copy A to V before calling
   int status = 0;

   if (spikingFlag != 0){
      updateActiveIndices();
      status = HyPerLayer::outputState(time, last);
      return status;
   }

   else if (writeNonspikingActivity != 0){
      float * Vtmp = this->getV();
      const PVLayerCube * activity_cube = this->getCLayer()->activity;
      const float * atmp = activity_cube->data; //this->getLayerData(); // write activity on this time step
      int nx = this->getCLayer()->loc.nx;
      int ny = this->getCLayer()->loc.ny;
      int nf = this->getCLayer()->loc.nf;
      int nb = this->getCLayer()->loc.nb;
      for (int k = 0; k < this->getNumNeurons(); k++) {
         int kext = kIndexExtended( k, nx, ny, nf, nb);
         Vtmp[k] = atmp[kext];
      }
      status = HyPerLayer::outputState(time, last);
   }
   return status;
#endif // OBSOLETE
}


//! Spiking method for Retina
/*!
 * Returns 1 if an event should occur, 0 otherwise. This is a stochastic model.
 * REMARKS:
 *      - During ABS_REFACTORY_PERIOD a neuron does not spike
 *      - The neurons that correspond to stimuli (on Image pixels)
 *       spike with probability probStim.
 *      - The neurons that correspond to background image pixels
 *      spike with probability probBase.
 *      - After ABS_REFACTORY_PERIOD the spiking probability
 *        grows exponentially to probBase and probStim respectively.
 *      - The burst of the retina is periodic with period T set by
 *        T = 1000/burstFreq in miliseconds
 *      - When the time t is such that mT < t < mT + burstDuration, where m is
 *      an integer, the burstStatus is set to 1.
 *      - The burstStatus is also determined by the condition that
 *      beginStim < t < endStim. These parameters are set in the input
 *      params file params.stdp
 *      - sinAmp modulates the spiking probability only when burstDuration <= 0
 *      or burstFreq = 0
 *      - probSpike is set to probBase for all neurons.
 *      - for neurons exposed to Image on pixels, probSpike increases
 *       with probStim.
 *      - When the probability is negative, the neuron does not spike.
 *      .
 * NOTES:
 *      - time is measured in milliseconds.
 *      .
 */
#ifdef OBSOLETE
int Retina::spike(float time, float dt, float prev, float probBase, float probStim, float * probSpike)
{
   fileread_params * params = (fileread_params *) clayer->params;
   float burstStatus = 1;
   float sinAmp = 1.0;

   // see if neuron is in a refactory period
   //
   if ((time - prev) < ABS_REFACTORY_PERIOD) {
      return 0;
   }
   else {
      float delta = time - prev - ABS_REFACTORY_PERIOD;
      float refact = 1.0f - expf(-delta/REFACTORY_PERIOD);
      refact = (refact < 0) ? 0 : refact;
      probBase *= refact;
      probStim *= refact;
   }

   if (params->burstDuration < 0 || params->burstFreq == 0) {
      sinAmp = cos( 2 * PI * time * params->burstFreq / 1000. );
   }
   else {
      burstStatus = fmodf(time, 1000. / params->burstFreq);
      burstStatus = burstStatus <= params->burstDuration;
   }

   burstStatus *= (int) ( (time >= params->beginStim) && (time < params->endStim) );
   *probSpike = probBase;

   if ((int)burstStatus) {
      *probSpike += probStim * sinAmp;  // negative prob is OK
    }
   return ( pv_random_prob() < *probSpike );
}
#endif

} // namespace PV

///////////////////////////////////////////////////////
//
// implementation of Retina kernels
//

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PV_USE_OPENCL
#  include "../kernels/Retina_update_state.cl"
#endif

#ifdef __cplusplus
}
#endif
