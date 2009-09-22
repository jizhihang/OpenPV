/*
 * HyPerConnection.hpp
 *
 *  Created on: Oct 21, 2008
 *      Author: rasmussn
 */

#ifndef HYPERCONN_HPP_
#define HYPERCONN_HPP_

#include "PVConnection.h"
#include "../include/pv_types.h"
#include "../io/PVParams.hpp"
#include "../layers/HyPerLayer.hpp"

#define PROTECTED_NUMBER 13
#define MAX_ARBOR_LIST (1+MAX_NEIGHBORS)

namespace PV {

class HyPerCol;
class HyPerLayer;
class ConnectionProbe;

/**
 * A PVConnection identifies a connection between two layers
 */
typedef struct {
   int delay; // current output delay in the associated f ring buffer (should equal fixed delay + varible delay for valid connection)
   int fixDelay; // fixed output delay. TODO: should be float
   int varDelayMin; // minimum variable conduction delay
   int varDelayMax; // maximum variable conduction delay
   int numDelay;
   int isGraded; //==1, release is stochastic with prob = (activity <= 1), default is 0 (no graded release)
   float vel;  // conduction velocity in position units (pixels) per time step--added by GTK
   float rmin; // minimum connection distance
   float rmax; // maximum connection distance
} PVConnParams;

class HyPerConn {

   friend class HyPerCol;

public:

   HyPerConn(const char * name, HyPerCol * hc, HyPerLayer * pre, HyPerLayer * post,
             int channel);
   virtual ~HyPerConn();

   virtual int deliver(PVLayerCube * cube, int neighbor);

   virtual int insertProbe(ConnectionProbe * p);
   virtual int outputState(float time);
   virtual int updateState(float time, float dt);
   virtual int updateWeights(PVLayerCube * preActivity, int arbor);

   inline  int numberOfAxonalArborLists()            {return numAxonalArborLists;}
   virtual int numberOfWeightPatches(int arbor);
   virtual int writeWeights(float time);
   virtual int writeWeights(int k);
   virtual int writeWeights(const char * filename, int k);
   virtual int writePostSynapticWeights(int ioAppend);

   virtual PVPatch * getWeights(int kPre, int arbor);
   virtual PVPatch * getPlasticityIncrement(int k, int arbor);

   inline PVLayerCube * getPlasticityDecrement()     {return pDecr;}

   inline PVPatch ** weights(int neighbor)           {return wPatches[neighbor];}

   inline const char * getName()                     {return name;}
   inline int          getDelay()                    {return params->delay;}

   inline float minWeight()                          {return 0.0;}
   inline float maxWeight()                          {return wMax;}

   inline PVAxonalArbor * axonalArbor(int kPre, int neighbor)
      {return &axonalArborList[neighbor][kPre];}

   HyPerLayer * preSynapticLayer()     {return pre;}
   HyPerLayer * postSynapticLayer()    {return post;}

   int  getConnectionId()              {return connId;}
   void setConnectionId(int id)        {connId = id;}

   int setParams(PVParams * params, PVConnParams * p);

   PVPatch ** convertPreSynapticWeights(float time);

   int randomWeights(PVPatch * wp, float wMin, float wMax, int seed);

   int gauss2DCalcWeights(PVPatch * wp, int fPre, int no, int xScale, int yScale,
                          int numFlanks, float flankShift, float rotate,
                          float aspect, float sigma, float r2Max, float strength);

protected:
   HyPerLayer     * pre;
   HyPerLayer     * post;
   HyPerCol       * parent;
   PVLayerCube    * pDecr;      // plasticity decrement variable (Mi) for pre-synaptic layer
   PVPatch       ** pIncr;      // list of stdp patches Psij variable
   PVPatch       ** wPatches[MAX_ARBOR_LIST]; // list of weight patches, one set per neighbor
   PVPatch       ** wPostPatches;  // post-synaptic linkage of weights
   PVAxonalArbor  * axonalArborList[MAX_ARBOR_LIST]; // list of axonal arbors for each neighbor

   int channel;              // which channel of the post to update (e.g. inhibit)
   int connId;               // connection id

   char * name;
   float nxp, nyp, nfp;      // size of weight dimensions

   int numParams;
   PVConnParams * params;

   int numAxonalArborLists;  // number of axonal arbors (weight patches) for presynaptic layer

   // STDP parameters for modifying weights
   float ampLTP;  // long term potentiation amplitude
   float ampLTD;  // long term depression amplitude
   float tauLTP;
   float tauLTD;
   float dWMax;
   float wMax;

   int numProbes;
   ConnectionProbe ** probes;   // probes used to output data

   int stdpFlag;                // presence of spike timing dependent plasticity
   int ioAppend;                // controls opening of binary files
   float wPostTime;             // time of last conversion to wPostPatches

private:
   // this is only called from primary constructor
   int initialize_base();

protected:

   // this is protected so that only derived classes can call it
   HyPerConn(const char * name, HyPerCol * hc, HyPerLayer * pre, HyPerLayer * post,
             int channel, int primary_constructor);

   virtual int initialize();
   virtual int initializeGaussianWeights();
   virtual int initializeRandomWeights(int seed);
   virtual int createWeights(int nxPatch, int nyPatch, int nfPatch);
   virtual int readWeights(const char * filename);
   virtual PVPatch ** createWeights(int nxPatch, int nyPatch, int nfPatch, int numPatches);
   virtual int        deleteWeights();

   virtual int createAxonalArbors();
   virtual int adjustAxonalArborWeights();

   int kIndexFromNeighbor(int k, int neighbor);

   // static member functions

public:
   static PVPatch ** createPatches(int numBundles, int nx, int ny, int nf)
   {
      PVPatch ** patches = (PVPatch**) malloc(numBundles*sizeof(PVPatch*));

      for (int i = 0; i < numBundles; i++) {
         patches[i] = pvpatch_inplace_new(nx, ny, nf);
      }

      return patches;
   }

   static int deletePatches(int numBundles, PVPatch ** patches)
   {
      for (int i = 0; i < numBundles; i++) {
         pvpatch_inplace_delete(patches[i]);
      }
      free(patches);

      return 0;
   }

};

} // namespace PV

#endif /* HYPERCONN_HPP_ */
