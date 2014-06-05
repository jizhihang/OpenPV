/*
 * IdentConn.cpp
 *
 *  Created on: Nov 17, 2010
 *      Author: pschultz
 */

#include "IdentConn.hpp"
#include "../weightinit/InitIdentWeights.hpp"

namespace PV {

IdentConn::IdentConn() {
    initialize_base();
}

IdentConn::IdentConn(const char * name, HyPerCol * hc) {
   initialize_base();
   initialize(name, hc);
}

int IdentConn::initialize_base() {
   // no IdentConn-specific data members to initialize
   return PV_SUCCESS;
}  // end of IdentConn::initialize_base()

int IdentConn::initialize(const char * name, HyPerCol * hc) {
   int status = HyPerConn::initialize(name, hc);
   return status;
}

int IdentConn::ioParamsFillGroup(enum ParamsIOFlag ioFlag) {
   int status = HyPerConn::ioParamsFillGroup(ioFlag);

   return status;
}

// Note this is one of the subclasses of the former kernelconn where it doesn't make sense to allow sharedWeights to be false
void IdentConn::ioParam_sharedWeights(enum ParamsIOFlag ioFlag) {
   sharedWeights = true;
   if (ioFlag == PARAMS_IO_READ) {
      fileType = PVP_KERNEL_FILE_TYPE;
      parent->parameters()->handleUnnecessaryParameter(name, "sharedWeights", true/*correctValue*/);
   }
}

void IdentConn::ioParam_weightInitType(enum ParamsIOFlag ioFlag) {
   if (ioFlag==PARAMS_IO_READ) {
      weightInitializer = new InitIdentWeights(this);
      parent->parameters()->handleUnnecessaryStringParameter(name, "weightInitType", NULL);
   }
}

void IdentConn::ioParam_normalizeMethod(enum ParamsIOFlag ioFlag) {
   normalizer = NULL;
}

void IdentConn::ioParam_numAxonalArbors(enum ParamsIOFlag ioFlag) {
   if (ioFlag == PARAMS_IO_READ) {
      numAxonalArborLists = 1;
      parent->parameters()->handleUnnecessaryParameter(name, "numAxonalArbors", numAxonalArborLists);
   }
}

void IdentConn::ioParam_plasticityFlag(enum ParamsIOFlag ioFlag) {
   if (ioFlag == PARAMS_IO_READ) {
      plasticityFlag = false;
      parent->parameters()->handleUnnecessaryParameter(name, "plasticityFlag", plasticityFlag);
   }
}

void IdentConn::ioParam_pvpatchAccumulateType(enum ParamsIOFlag ioFlag) {
   if (ioFlag == PARAMS_IO_READ) {
      pvpatchAccumulateTypeString = strdup("convolve");
      pvpatchAccumulateType = ACCUMULATE_CONVOLVE;
      parent->parameters()->handleUnnecessaryStringParameter(name, "pvpatchAccumulateType", "convolve", true/*case insensitive*/);
   }
}

void IdentConn::ioParam_preActivityIsNotRate(enum ParamsIOFlag ioFlag) {
   if (ioFlag == PARAMS_IO_READ) {
      preActivityIsNotRate = false;
      parent->parameters()->handleUnnecessaryParameter(name, "preActivityIsNotRate", preActivityIsNotRate);
   }
}
void IdentConn::ioParam_writeCompressedWeights(enum ParamsIOFlag ioFlag) {
   if (ioFlag == PARAMS_IO_READ) {
      writeCompressedWeights = true;
      parent->parameters()->handleUnnecessaryParameter(name, "writeCompressedWeights", writeCompressedWeights);
   }
}
void IdentConn::ioParam_writeCompressedCheckpoints(enum ParamsIOFlag ioFlag) {
   if (ioFlag == PARAMS_IO_READ) {
      writeCompressedCheckpoints = true;
      parent->parameters()->handleUnnecessaryParameter(name, "writeCompressedCheckpoints", writeCompressedCheckpoints);
   }
}

void IdentConn::ioParam_selfFlag(enum ParamsIOFlag ioFlag) {
   if (ioFlag == PARAMS_IO_READ) {
      selfFlag = false;
      parent->parameters()->handleUnnecessaryParameter(name, "selfFlag", selfFlag);
   }
}

void IdentConn::ioParam_combine_dW_with_W_flag(enum ParamsIOFlag ioFlag) {
   assert(plasticityFlag==false);
   // readCombine_dW_with_W_flag only used if when plasticityFlag is true, which it never is for IdentConn
   if (ioFlag == PARAMS_IO_READ) {
      parent->parameters()->handleUnnecessaryParameter(name, "combine_dW_with_W_flag", combine_dW_with_W_flag);
   }
   return;
}

void IdentConn::ioParam_keepKernelsSynchronized(enum ParamsIOFlag ioFlag) {
   if (ioFlag == PARAMS_IO_READ) {
      keepKernelsSynchronized_flag = true;
      parent->parameters()->handleUnnecessaryParameter(name, "keepKernelsSynchronized", keepKernelsSynchronized_flag);
   }
}

void IdentConn::ioParam_weightUpdatePeriod(enum ParamsIOFlag ioFlag) {
   if (ioFlag == PARAMS_IO_READ) {
      weightUpdatePeriod = 1.0f;
      parent->parameters()->handleUnnecessaryParameter(name, "weightUpdatePeriod", weightUpdatePeriod);
   }
}

void IdentConn::ioParam_initialWeightUpdateTime(enum ParamsIOFlag ioFlag) {
   if (ioFlag == PARAMS_IO_READ) {
      initialWeightUpdateTime = 0.0f;
      parent->parameters()->handleUnnecessaryParameter(name, "initialWeightUpdateTime", initialWeightUpdateTime);
      weightUpdateTime = initialWeightUpdateTime;
   }
}

void IdentConn::ioParam_nxp(enum ParamsIOFlag ioFlag) {
   if (ioFlag == PARAMS_IO_READ) {
      nxp = 1;
      parent->parameters()->handleUnnecessaryParameter(name, "nxp", 1);
   }
}

void IdentConn::ioParam_nxpShrunken(enum ParamsIOFlag ioFlag) {
   if (ioFlag == PARAMS_IO_READ) {
      nxpShrunken = 1;
      parent->parameters()->handleUnnecessaryParameter(name, "nxpShrunken", 1);
   }
}

void IdentConn::ioParam_nyp(enum ParamsIOFlag ioFlag) {
   if (ioFlag == PARAMS_IO_READ) {
      nyp = 1;
      parent->parameters()->handleUnnecessaryParameter(name, "nyp", 1);
   }
}

void IdentConn::ioParam_nypShrunken(enum ParamsIOFlag ioFlag) {
   if (ioFlag == PARAMS_IO_READ) {
      nypShrunken = 1;
      parent->parameters()->handleUnnecessaryParameter(name, "nypShrunken", 1);
   }
}

void IdentConn::ioParam_nfp(enum ParamsIOFlag ioFlag) {
   if (ioFlag == PARAMS_IO_READ) {
      parent->parameters()->handleUnnecessaryParameter(name, "nfp", -1);
   }
}

void IdentConn::ioParam_shrinkPatches(enum ParamsIOFlag ioFlag) {
   if (ioFlag == PARAMS_IO_READ) {
      shrinkPatches_flag = false;
      parent->parameters()->handleUnnecessaryParameter(name, "shrinkPatches", shrinkPatches_flag);
   }
}

void IdentConn::ioParam_updateGSynFromPostPerspective(enum ParamsIOFlag ioFlag){
   if (ioFlag == PARAMS_IO_READ) {
      updateGSynFromPostPerspective = false;
      parent->parameters()->handleUnnecessaryParameter(name, "updateGSynFromPostPerspective", updateGSynFromPostPerspective);
   }
}

int IdentConn::setWeightInitializer() {
   weightInitializer = (InitWeights *) new InitIdentWeights(this);
   if( weightInitializer == NULL ) {
      fprintf(stderr, "IdentConn \"%s\": Rank %d process unable to create InitIdentWeights object.  Exiting.\n", name, parent->icCommunicator()->commRank());
      exit(EXIT_FAILURE);
   }
   return PV_SUCCESS;
}

int IdentConn::communicateInitInfo() {
   int status = HyPerConn::communicateInitInfo();
   assert(pre && post);
   const PVLayerLoc * preLoc = pre->getLayerLoc();
   const PVLayerLoc * postLoc = post->getLayerLoc();
   if( preLoc->nx != postLoc->nx || preLoc->ny != postLoc->ny || preLoc->nf != postLoc->nf ) {
      if (parent->columnId()==0) {
         fprintf( stderr,
                  "IdentConn \"%s\" Error: %s and %s do not have the same dimensions.\n Dims: %dx%dx%d vs. %dx%dx%d\n",
                  name, preLayerName,postLayerName,preLoc->nx,preLoc->ny,preLoc->nf,postLoc->nx,postLoc->ny,postLoc->nf);
      }
      exit(EXIT_FAILURE);
   }
   parent->parameters()->handleUnnecessaryParameter(name, "nfp", nfp); // nfp is set during call to HyPerConn::communicateInitInfo, so don't check for unnecessary int parameter until after that.
   return status;
}

void IdentConn::handleDefaultSelfFlag() {
   assert(selfFlag==false);
}

}  // end of namespace PV block
