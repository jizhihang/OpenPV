/*
 * OjaConnProbe.cpp
 *
 *  Created on: Oct 15, 2012
 *      Author: dpaiton
 */

#include "OjaConnProbe.hpp"

namespace PV {

OjaConnProbe::OjaConnProbe() {
   initialize_base();
}

OjaConnProbe::OjaConnProbe(const char * probename, const char * filename, HyPerConn * conn, int postIndex, bool isPostProbe)
{
   initialize_base();
   initialize(probename, filename, conn, INDEX_METHOD, postIndex, -1, -1, -1, isPostProbe);
}

OjaConnProbe::OjaConnProbe(const char * probename, const char * filename, HyPerConn * conn, int kxPost, int kyPost, int kfPost, bool isPostProbe)
{
   initialize_base();
   initialize(probename, filename, conn, COORDINATE_METHOD, -1, kxPost, kyPost, kfPost, isPostProbe);
}

OjaConnProbe::~OjaConnProbe()
{
   if (inBounds){
      free(preStdpTrs);
      free(preOjaTrs);
      free(preWeights);
#ifdef DEBUG_POST
      free(preWeightsDebug);
#endif
   }
}

int OjaConnProbe::initialize_base() {
   preStdpTrs = NULL;
   preOjaTrs  = NULL;
   preWeights = NULL;
   kLocal      = -1;
   return PV_SUCCESS;
}

int OjaConnProbe::initialize(const char * probename, const char * filename,
      HyPerConn * conn, PatchIDMethod pidMethod, int kPost,
      int kxPost, int kyPost, int kfPost, bool isPostProbe)
{
   ojaConn = dynamic_cast<OjaSTDPConn *>(conn);
   assert(ojaConn != NULL);

   const PVLayerLoc * postLoc;
   postLoc = ojaConn->postSynapticLayer()->getLayerLoc();

   int nxGlobal = postLoc->nxGlobal;
   int nyGlobal = postLoc->nyGlobal;
   int nf = postLoc->nf;

   if (pidMethod == INDEX_METHOD) {
      kxPost = kxPos(kPost,nxGlobal,nyGlobal,nf);
      kyPost = kyPos(kPost,nxGlobal,nyGlobal,nf);
      kfPost = featureIndex(kPost,nxGlobal,nyGlobal,nf);
   }
   else if(pidMethod == COORDINATE_METHOD) {
      assert(kfPost >= 0);
      kPost = kIndex(kxPost,kyPost,kfPost,nxGlobal,nyGlobal,nf); // nx, ny, nf NOT in extended space
   }
   else assert(false);
   assert(kPost != -1);
   assert(kfPost < nf);

   BaseConnectionProbe::initialize(probename, filename, conn,kxPost,kyPost,kfPost,isPostProbe);

// Now convert from global coordinates to local coordinates
   int kxPostLocal = kxPost - postLoc->kx0;
   int kyPostLocal = kyPost - postLoc->ky0;
   int nxLocal = postLoc->nx;
   int nyLocal = postLoc->ny;
   kLocal = kIndex(kxPostLocal,kyPostLocal,kfPost,nxLocal,nyLocal,nf);
   // assert(kLocal >=0 && kLocal < ojaConn->postSynapticLayer()->getNumExtended());

   inBounds = !(kxPostLocal < 0 || kxPostLocal >= postLoc->nx || kyPostLocal < 0 || kyPostLocal >= postLoc->ny);

   if (inBounds){
      // Get post layer sizes
      int nxpPost = ojaConn->getNxpPost();
      int nypPost = ojaConn->getNypPost();
      int nfpPost = ojaConn->getNfpPost();

      int numArbors = ojaConn->numberOfAxonalArborLists(); //will loop through arbors
      int numPostPatch = nxpPost * nypPost * nfpPost; // Post-synaptic weights are never shrunken

      // Allocate buffers for pre info
      preStdpTrs = (float *) calloc(numPostPatch*numArbors, sizeof(float));
      preOjaTrs  = (float *) calloc(numPostPatch*numArbors, sizeof(float));
      preWeights = (float *) calloc(numPostPatch*numArbors, sizeof(float));
#ifdef DEBUG_POST
      preWeightsDebug = (float *) calloc(numPostPatch*numArbors, sizeof(float));
      assert (preWeightsDebug != NULL);
#endif
      assert(preStdpTrs != NULL);
      assert(preOjaTrs != NULL);
      assert(preWeights != NULL);
   }

   return PV_SUCCESS;
}

int OjaConnProbe::outputState(double timef)
{
   if (!inBounds) {
      return PV_SUCCESS;
   }

   // Get post layer sizes
   int nxpPost = ojaConn->getNxpPost();
   int nypPost = ojaConn->getNypPost();
   int nfpPost = ojaConn->getNfpPost();

   int numArbors = ojaConn->numberOfAxonalArborLists(); //will loop through arbors
   int numPostPatch = nxpPost * nypPost * nfpPost; // Post-synaptic weights are never shrunken

   int num_weights_in_patch = ojaConn->xPatchSize()*ojaConn->yPatchSize()*ojaConn->fPatchSize();
   int preTraceIdx = 0;

   for (int arborID=0; arborID < numArbors; arborID++)
   {
      postWeightsp = ojaConn->getPostWeightsp(arborID,kLocal); // Pointer array full of addresses pointing to the weights for all of the preNeurons connected to the given postNeuron's receptive field

#ifdef DEBUG_POST
      ojaConn->convertPreSynapticWeights(timef);
      postWeights = ojaConn->getWPostData(arborID, kLocal);
#endif

      float * startAdd = ojaConn->get_wDataStart(arborID);                    // Address of first preNeuron in pre layer
      for (int postKPatch=0; postKPatch<numPostPatch; postKPatch++)
      {
         float * kPreAdd = postWeightsp[postKPatch];  // Address of first preNeuron in receptive field of postNeuron
         assert(kPreAdd != NULL);
         int kPre = (kPreAdd-startAdd) / num_weights_in_patch;

         assert(preTraceIdx < numArbors*numPostPatch);
         preWeights[preTraceIdx] = *(postWeightsp[postKPatch]); // One weight per arbor per preNeuron in postNeuron's receptive field
#ifdef DEBUG_POST
         preWeightsDebug[preTraceIdx] = postWeights[postKPatch];
         assert(preWeights[preTraceIdx] == preWeightsDebug[preTraceIdx]);
#endif

         preStdpTrs[preTraceIdx] = ojaConn->getPreStdpTr(kPre,arborID); // Trace with STDP-related time scale (tauLTD)
         preOjaTrs[preTraceIdx]  = ojaConn->getPreOjaTr(kPre,arborID);  // Trace with Oja-related time scale (tauOja)

         preTraceIdx++;
      }
   }

   postStdpTr = ojaConn->getPostStdpTr(kLocal);
   postOjaTr  = ojaConn->getPostOjaTr(kLocal);
   postIntTr  = ojaConn->getPostIntTr(kLocal);
   ampLTD     = ojaConn->getAmpLTD(kLocal);

   // Write out to file
   PV_Stream * pvstream = getStream();
   assert(pvstream); // invalid pointer

   const char * msg = getName(); // Message to precede the probe's output line

   fprintf(pvstream->fp, "%s:      t=%.1f kLocal=%d", msg, timef, kLocal);
   fprintf(pvstream->fp, " poStdpTr=%-6.3f",postStdpTr);
   fprintf(pvstream->fp, " poOjaTr=%-6.3f",postOjaTr);
   fprintf(pvstream->fp, " poIntTr=%-6.3f",postIntTr);
   fprintf(pvstream->fp, " ampLTD=%-6.3f",ampLTD);
   int weightIdx = 0;

   for (int arborID=0; arborID < numArbors; arborID++) {
      for (int patchID=0; patchID < numPostPatch; patchID++) {
         fprintf(pvstream->fp, " prStdpTr_%d_%d=%-6.3f",arborID,patchID,preStdpTrs[weightIdx]);
         fprintf(pvstream->fp, " prOjaTr_%d_%d=%-6.3f",arborID,patchID,preOjaTrs[weightIdx]);
         fprintf(pvstream->fp, " weight_%d_%d=%-6.3f",arborID,patchID,preWeights[weightIdx]);

         weightIdx++;
      }
   }
   fprintf(pvstream->fp, "\n");
   fflush(pvstream->fp);

   return PV_SUCCESS;
}
} //end of namespacePV




