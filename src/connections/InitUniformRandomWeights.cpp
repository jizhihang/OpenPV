/*
 * InitUniformRandomWeights.cpp
 *
 *  Created on: Aug 9, 2011
 *      Author: kpeterson
 */

#include "InitUniformRandomWeights.hpp"
#include "InitUnivormRandomWeightsParams.hpp"

#include "../utils/pv_random.h"

namespace PV {

InitUniformRandomWeights::InitUniformRandomWeights()
{
   initialize_base();
}
//InitUniformRandomWeights::InitUniformRandomWeights(const char * name, HyPerCol * hc, HyPerLayer * pre, HyPerLayer * post,
//      ChannelType channel) : InitWeights() {
//
//   InitUniformRandomWeights::initialize_base();
//   InitUniformRandomWeights::initialize(name, hc, pre, post, channel);
//}

InitUniformRandomWeights::~InitUniformRandomWeights()
{
   // TODO Auto-generated destructor stub
}

int InitUniformRandomWeights::initialize_base() {
   return PV_SUCCESS;
}
//int InitUniformRandomWeights::initialize(const char * name, HyPerCol * hc,
//      HyPerLayer * pre, HyPerLayer * post, ChannelType channel) {
//   InitWeights::initialize(name, hc, pre, post, channel);
//   return PV_SUCCESS;
//}

InitWeightsParams * InitUniformRandomWeights::createNewWeightParams(HyPerConn * callingConn) {
   InitWeightsParams * tempPtr = new InitUnivormRandomWeightsParams(callingConn);
   return tempPtr;
}

int InitUniformRandomWeights::calcWeights(PVPatch * patch, int patchIndex,
      InitWeightsParams *weightParams) {
   InitUnivormRandomWeightsParams *weightParamPtr = dynamic_cast<InitUnivormRandomWeightsParams*>(weightParams);

   if(weightParamPtr==NULL) {
      fprintf(stderr, "Failed to recast pointer to weightsParam!  Exiting...");
      exit(1);
   }

   const float wMinInit = weightParamPtr->getWMin();
   const float wMaxInit = weightParamPtr->getWMax();


   uniformWeights(patch, wMinInit, wMaxInit);
   return 1;
}

/**
 * generate random weights for a patch from a uniform distribution
 * NOTES:
 *    - the pointer w already points to the patch head in the data structure
 *    - it only sets the weights to "real" neurons, not to neurons in the boundary
 *    layer. For example, if x are boundary neurons and o are real neurons,
 *    x x x x
 *    x o o o
 *    x o o o
 *    x o o o
 *
 *    for a 4x4 connection it sets the weights to the o neurons only.
 *    .
 */
int InitUniformRandomWeights::uniformWeights(PVPatch * wp, float minwgt, float maxwgt) {
      // changed variable names to avoid confusion with data members this->wMin and this->wMax
   pvdata_t * w = wp->data;

   const int nxp = wp->nx;
   const int nyp = wp->ny;
   const int nfp = wp->nf;

   const int sxp = wp->sx;
   const int syp = wp->sy;
   const int sfp = wp->sf;

   double p;
   if( maxwgt <= minwgt ) {
      if( maxwgt < minwgt ) {
         fprintf(stderr, "Warning: uniformWeights maximum less than minimum.  Changing max = %f to min value of %f\n", maxwgt, minwgt);
         maxwgt = minwgt;
      }
      p = 0;
   }
   else {
       p = (maxwgt - minwgt) / pv_random_max();
   }

   // loop over all post-synaptic cells in patch
   for (int y = 0; y < nyp; y++) {
      for (int x = 0; x < nxp; x++) {
         for (int f = 0; f < nfp; f++) {
            w[x * sxp + y * syp + f * sfp] = minwgt + p * pv_random();
         }
      }
   }

   return PV_SUCCESS;
}

} /* namespace PV */
