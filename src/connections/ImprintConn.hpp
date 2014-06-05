/*
 * ImprintConn.hpp
 *
 *  Created on: Feburary 27, 2014
 *      Author: slundquist
 */

#ifndef IMPRINTCONN_HPP_
#define IMPRINTCONN_HPP_

#include "HyPerConn.hpp"
namespace PV {

class ImprintConn: public HyPerConn {

public:
   ImprintConn();
   ImprintConn(const char * name, HyPerCol * hc);
   virtual ~ImprintConn();

   //virtual int communicateInitInfo();
   virtual int allocateDataStructures();
   // virtual int setPatchSize(const char * filename); // Now a protected method.

   virtual int update_dW(int arbor_ID);
   //virtual pvdata_t updateRule_dW(pvdata_t pre, pvdata_t post);
   //virtual int updateWeights(int arbor_ID);

protected:
   //int initialize(const char * name, HyPerCol * hc);
   virtual int ioParamsFillGroup(enum ParamsIOFlag ioFlag);
   virtual void ioParam_sharedWeights(enum ParamsIOFlag ioFlag);
   virtual void ioParam_imprintTimeThresh(enum ParamsIOFlag ioFlag);
   bool imprintFeature(int arborId, int kExt);
   double imprintTimeThresh;
   double* lastActiveTime;

private:
   int initialize_base();
   bool * imprinted;
   //bool allDidImprint;

}; // end class 

}  // end namespace PV

#endif /* CLONEKERNELCONN_HPP_ */
