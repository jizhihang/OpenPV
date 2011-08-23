/*
 * InitPoolWeightsParams.hpp
 *
 *  Created on: Aug 13, 2011
 *      Author: kpeterson
 */

#ifndef INITPOOLWEIGHTSPARAMS_HPP_
#define INITPOOLWEIGHTSPARAMS_HPP_

#include "InitWeightsParams.hpp"

namespace PV {

class InitPoolWeightsParams: public PV::InitWeightsParams {
public:
   InitPoolWeightsParams();
   InitPoolWeightsParams(HyPerConn * parentConn);
   virtual ~InitPoolWeightsParams();
   void calcOtherParams(PVPatch * patch, int patchIndex);

   //get/set methods:
   inline float getStrength()        {return strength;}

protected:
   virtual int initialize_base();
   int initialize(HyPerConn * parentConn);

private:
   float strength;

};

} /* namespace PV */
#endif /* INITPOOLWEIGHTSPARAMS_HPP_ */
