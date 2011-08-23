/*
 * InitPoolWeights.hpp
 *
 *  Created on: Aug 13, 2011
 *      Author: kpeterson
 */

#ifndef INITPOOLWEIGHTS_HPP_
#define INITPOOLWEIGHTS_HPP_

#include "InitWeights.hpp"

namespace PV {

class InitWeightsParams;
class InitPoolWeightsParams;

class InitPoolWeights: public PV::InitWeights {
public:
   InitPoolWeights();
   virtual ~InitPoolWeights();

   virtual int calcWeights(PVPatch * patch, int patchIndex,
         InitWeightsParams *weightParams);
   virtual InitWeightsParams * createNewWeightParams(HyPerConn * callingConn);
   void calcOtherParams(PVPatch * patch, int patchIndex);


protected:
   virtual int initialize_base();

private:
   int poolWeights(PVPatch * patch, InitPoolWeightsParams * weightParamPtr);
};

} /* namespace PV */
#endif /* INITPOOLWEIGHTS_HPP_ */
