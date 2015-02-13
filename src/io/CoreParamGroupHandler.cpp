/*
 * CoreParamGroupHandler.cpp
 *
 *  Created on: Jan 5, 2015
 *      Author: pschultz
 */

#include "CoreParamGroupHandler.hpp"
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "../layers/ANNErrorLayer.hpp"
#include "../layers/ANNLayer.hpp"
#include "../layers/ANNNormalizedErrorLayer.hpp"
#include "../layers/ANNSquaredLayer.hpp"
#include "../layers/ANNWhitenedLayer.hpp"
#include "../layers/BIDSCloneLayer.hpp"
#include "../layers/BIDSLayer.hpp"
#include "../layers/BIDSMovieCloneMap.hpp"
#include "../layers/BIDSSensorLayer.hpp"
#include "../layers/BinningLayer.hpp"
#include "../layers/CloneVLayer.hpp"
#include "../layers/ConstantLayer.hpp"
#include "../layers/CreateMovies.hpp"
#include "../layers/FilenameParsingGroundTruthLayer.hpp"
#include "../layers/GapLayer.hpp"
#include "../layers/GenerativeLayer.hpp"
#include "../layers/HyPerLCALayer.hpp"
#include "../layers/Image.hpp"
#include "../layers/ImageFromMemoryBuffer.hpp"
#include "../layers/IncrementLayer.hpp"
#include "../layers/KmeansLayer.hpp"
#include "../layers/LCALIFLayer.hpp"
#include "../layers/LIF.hpp"
#include "../layers/LIFGap.hpp"
#include "../layers/LabelErrorLayer.hpp"
#include "../layers/LabelLayer.hpp"
#include "../layers/LeakyIntegrator.hpp"
#include "../layers/LogLatWTAGenLayer.hpp"
#include "../layers/MLPErrorLayer.hpp"
#include "../layers/MLPForwardLayer.hpp"
#include "../layers/MLPOutputLayer.hpp"
#include "../layers/MLPSigmoidLayer.hpp"
#include "../layers/MatchingPursuitLayer.hpp"
#include "../layers/MatchingPursuitResidual.hpp"
#include "../layers/Movie.hpp"
#include "../layers/Patterns.hpp"
#include "../layers/PoolingANNLayer.hpp"
#include "../layers/PtwiseProductLayer.hpp"
#include "../layers/RescaleLayer.hpp"
#include "../layers/Retina.hpp"
#include "../layers/ShuffleLayer.hpp"
#include "../layers/SigmoidLayer.hpp"
#include "../layers/TextStream.hpp"
#include "../layers/TrainingLayer.hpp"
#include "../layers/WTALayer.hpp"
#ifdef PV_USE_SNDFILE
#include "../layers/NewCochlear.h"
#include "../layers/SoundStream.hpp"
#endif // PV_USE_SNDFILE
#include "../connections/HyPerConn.hpp"
#include "../connections/BIDSConn.hpp"
#include "../connections/CloneConn.hpp"
#include "../connections/CloneKernelConn.hpp"
#include "../connections/CopyConn.hpp"
#include "../connections/FeedbackConn.hpp"
#include "../connections/GapConn.hpp"
#include "../connections/IdentConn.hpp"
#include "../connections/ImprintConn.hpp"
#include "../connections/KernelConn.hpp"
#include "../connections/LCALIFLateralConn.hpp"
#include "../connections/OjaSTDPConn.hpp"
#include "../connections/PlasticCloneConn.hpp"
#include "../connections/PoolingConn.hpp"
#include "../connections/TransposeConn.hpp"
#include "../connections/PoolingConn.hpp"
#include "../connections/TransposeConn.hpp"
#include "L2NormProbe.hpp"
#include "LayerFunctionProbe.hpp"
#include "LogLatWTAProbe.hpp"
#include "PointLIFProbe.hpp"
#include "PointProbe.hpp"
#include "RequireAllZeroActivityProbe.hpp"
#include "SparsityLayerProbe.hpp"
#include "StatsProbe.hpp"
#include "TextStreamProbe.hpp"
#include "KernelProbe.hpp"
#include "../weightinit/InitWeights.hpp"
#include "../weightinit/InitGauss2DWeights.hpp"
#include "../weightinit/InitCocircWeights.hpp"
#include "../weightinit/InitSmartWeights.hpp"
#include "../weightinit/InitUniformRandomWeights.hpp"
#include "../weightinit/InitGaussianRandomWeights.hpp"
#include "../weightinit/InitGaborWeights.hpp"
#include "../weightinit/InitBIDSLateral.hpp"
#include "../weightinit/InitOneToOneWeights.hpp"
#include "../weightinit/InitOneToOneWeightsWithDelays.hpp"
#include "../weightinit/InitIdentWeights.hpp"
#include "../weightinit/InitUniformWeights.hpp"
#include "../weightinit/InitSpreadOverArborsWeights.hpp"
#include "../weightinit/InitMaxPoolingWeights.hpp"

namespace PV {

CoreParamGroupHandler::CoreParamGroupHandler() {
}

CoreParamGroupHandler::~CoreParamGroupHandler() {
}

ParamGroupType CoreParamGroupHandler::getGroupType(char const * keyword) {
   struct keyword_grouptype_entry  {char const * kw; ParamGroupType type;};
   struct keyword_grouptype_entry keywordtable[] = {
         // HyPerCol
         {"HyPerCol", HyPerColGroupType},

         // Layers
         {"HyPerLayer", LayerGroupType},
         {"ANNErrorLayer", LayerGroupType},
         {"ANNLayer", LayerGroupType},
         {"ANNNormalizedErrorLayer", LayerGroupType},
         {"ANNSquaredLayer", LayerGroupType},
         {"ANNTriggerUpdateOnNewImageLayer", LayerGroupType},
         {"ANNWhitenedLayer", LayerGroupType},
         {"BIDSCloneLayer", LayerGroupType},
         {"BIDSLayer", LayerGroupType},
         {"BIDSMovieCloneMap", LayerGroupType},
         {"BIDSSensorLayer", LayerGroupType},
         {"BinningLayer", LayerGroupType},
         {"CloneVLayer", LayerGroupType},
         {"ConstantLayer", LayerGroupType},
         {"CreateMovies", LayerGroupType},
         {"FilenameParsingGroundTruthLayer", LayerGroupType},
         {"GapLayer", LayerGroupType},
         {"GenerativeLayer", LayerGroupType},
         {"HyPerLCALayer", LayerGroupType},
         {"Image", LayerGroupType},
         {"ImageFromMemoryBuffer", LayerGroupType},
         {"IncrementLayer", LayerGroupType},
         {"KmeansLayer", LayerGroupType},
         {"LCALIFLayer", LayerGroupType},
         {"LIF", LayerGroupType},
         {"LIFGap", LayerGroupType},
         {"LabelErrorLayer", LayerGroupType},
         {"LabelLayer", LayerGroupType},
         {"LeakyIntegrator", LayerGroupType},
         {"LogLatWTAGenLayer", LayerGroupType},
         {"MLPErrorLayer", LayerGroupType},
         {"MLPForwardLayer", LayerGroupType},
         {"MLPOutputLayer", LayerGroupType},
         {"MLPSigmoidLayer", LayerGroupType},
         {"MatchingPursuitLayer", LayerGroupType},
         {"MatchingPursuitResidual", LayerGroupType},
         {"MaxPooling", LayerGroupType},
         {"Movie", LayerGroupType},
         {"Patterns", LayerGroupType},
         {"PoolingANNLayer", LayerGroupType},
         {"PtwiseProductLayer", LayerGroupType},
         {"RescaleLayer", LayerGroupType},
         {"Retina", LayerGroupType},
         {"ShuffleLayer", LayerGroupType},
         {"SigmoidLayer", LayerGroupType},
         {"TextStream", LayerGroupType},
         {"TrainingLayer", LayerGroupType},
         {"WTALayer", LayerGroupType},
#ifdef PV_USE_SNDFILE
         {"NewCochlearLayer", LayerGroupType},
         {"SoundStream", LayerGroupType},
#endif

         // Connections
         {"HyPerConn", ConnectionGroupType},
         {"BIDSConn", ConnectionGroupType},
         {"CloneConn", ConnectionGroupType},
         {"CloneKernelConn", ConnectionGroupType},
         {"CopyConn", ConnectionGroupType},
         {"FeedbackConn", ConnectionGroupType},
         {"GapConn", ConnectionGroupType},
         {"IdentConn", ConnectionGroupType},
         {"ImprintConn", ConnectionGroupType},
         {"KernelConn", ConnectionGroupType},
         {"LCALIFLateralConn", ConnectionGroupType},
         {"OjaSTDPConn", ConnectionGroupType},
         {"PlasticCloneConn", ConnectionGroupType},
         {"PoolingConn", ConnectionGroupType},
         {"TransposeConn", ConnectionGroupType},

         // ColProbes
         {"ColProbe", ColProbeGroupType},
         {"GenColProbe", ColProbeGroupType},

         // Probes
         // // Layer probes
         {"LayerProbe", ProbeGroupType},
         {"L2NormProbe", ProbeGroupType},
         {"LayerFunctionProbe", ProbeGroupType},
         {"LogLatWTAProbe", ProbeGroupType},
         {"PointLIFProbe", ProbeGroupType},
         {"PointProbe", ProbeGroupType},
         {"RequireAllZeroActivityProbe", ProbeGroupType},
         {"SparsityLayerProbe", ProbeGroupType},
         {"StatsProbe", ProbeGroupType},
         {"TextStreamProbe", ProbeGroupType},

         // // Connection probes
         {"KernelProbe", ProbeGroupType},

         // Weight initializers
         {"Gauss2DWeight", WeightInitializerGroupType},
         {"CoCircWeight", WeightInitializerGroupType},
         {"UniformWeight", WeightInitializerGroupType},
         {"SmartWeight", WeightInitializerGroupType},
         {"BIDSLateral", WeightInitializerGroupType},
         {"UniformRandomWeight", WeightInitializerGroupType},
         {"GaussianRandomWeight", WeightInitializerGroupType},
         {"GaborWeight", WeightInitializerGroupType},
         {"IdentWeight", WeightInitializerGroupType},
         {"OneToOneWeights", WeightInitializerGroupType},
         {"OneToOneWeightsWithDelays", WeightInitializerGroupType},
         {"SpreadOverArborsWeight", WeightInitializerGroupType},
         {"MaxPoolingWeight", WeightInitializerGroupType},
         {"FileWeight", WeightInitializerGroupType},

         {NULL, UnrecognizedGroupType}
   };
   ParamGroupType result = UnrecognizedGroupType;

   for (int k=0; keywordtable[k].kw != NULL; k++) {
      if (!strcmp(keywordtable[k].kw, keyword)) {
         result = keywordtable[k].type;
         break;
      }
   }
   return result;
}

HyPerCol * CoreParamGroupHandler::createHyPerCol(char const * keyword, char const * name, HyPerCol * hc) {
   HyPerCol * addedHyPerCol = NULL;
   if ( keyword && !strcmp(keyword, "HyPerCol")) {
      addedHyPerCol = hc;
   }

   if (addedHyPerCol==NULL) {
      if (addedHyPerCol==NULL) {
         if (hc->columnId()==0) {
            fprintf(stderr, "createHyPerCol error: unable to add %s\n", keyword);
         }
         MPI_Barrier(hc->icCommunicator()->communicator());
         exit(EXIT_FAILURE);
      }
   }

   return addedHyPerCol;
}

HyPerLayer * CoreParamGroupHandler::createLayer(char const * keyword, char const * name, HyPerCol * hc) {
   HyPerLayer * addedLayer = NULL;
   if (keyword==NULL) {
      addedLayer = NULL;
   }
   else if( !strcmp(keyword, "HyPerLayer") ) {
      fprintf(stderr, "Group \"%s\": abstract class HyPerLayer cannot be instantiated.\n", name);
      addedLayer = NULL;
   }
   else if( !strcmp(keyword, "ANNErrorLayer") ) {
      addedLayer = new ANNErrorLayer(name, hc);
   }
   else if( !strcmp(keyword, "ANNLayer") ) {
      addedLayer = new ANNLayer(name, hc);
   }
   else if( !strcmp(keyword, "ANNNormalizedErrorLayer") ) {
      addedLayer = new ANNNormalizedErrorLayer(name, hc);
   }
   else if( !strcmp(keyword, "ANNSquaredLayer") ) {
      addedLayer = new ANNSquaredLayer(name, hc);
   }
      if( !strcmp(keyword, "ANNTriggerUpdateOnNewImageLayer") ) {
         // ANNTriggerUpdateOnNewImageLayer is obsolete as of April 23, 2014.  Leaving it in the code for a while for a useful error message.
         // Use ANNLayer with triggerFlag set to true and triggerLayerName for the triggering layer
         if (hc->columnId()==0) {
            fprintf(stderr, "Error: ANNTriggerUpdateOnNewImageLayer is obsolete.\n");
            fprintf(stderr, "    Use ANNLayer with parameter triggerFlag set to true\n");
            fprintf(stderr, "    and triggerLayerName set to the triggering layer.\n");
         }
   #ifdef PV_USE_MPI
         MPI_Barrier(hc->icCommunicator()->communicator());
   #endif
         exit(EXIT_FAILURE);
      }
   else if( !strcmp(keyword, "ANNWhitenedLayer") ) {
      addedLayer = new ANNWhitenedLayer(name, hc);
   }
   else if( !strcmp(keyword, "BIDSCloneLayer") ) {
      addedLayer = new BIDSCloneLayer(name, hc);
   }
   else if( !strcmp(keyword, "BIDSLayer") ) {
      addedLayer = new BIDSLayer(name, hc);
   }
   else if( !strcmp(keyword, "BIDSMovieCloneMap") ) {
      addedLayer = new BIDSMovieCloneMap(name, hc);
   }
   else if( !strcmp(keyword, "BIDSSensorLayer") ) {
      addedLayer = new BIDSSensorLayer(name, hc);
   }
   else if( !strcmp(keyword, "BinningLayer") ) {
      addedLayer = new BinningLayer(name, hc);
   }
   else if( !strcmp(keyword, "CloneVLayer") ) {
      addedLayer = new CloneVLayer(name, hc);
   }
   else if( !strcmp(keyword, "ConstantLayer") ) {
      addedLayer = new ConstantLayer(name, hc);
   }
   else if( !strcmp(keyword, "CreateMovies") ) {
      addedLayer = new CreateMovies(name, hc);
   }
   else if( !strcmp(keyword, "FilenameParsingGroundTruthLayer") ) {
      addedLayer = new FilenameParsingGroundTruthLayer(name, hc);
   }
   else if( !strcmp(keyword, "GapLayer") ) {
      addedLayer = new GapLayer(name, hc);
   }
   else if( !strcmp(keyword, "GenerativeLayer") ) {
      addedLayer = new GenerativeLayer(name, hc);
   }
   else if( !strcmp(keyword, "HyPerLCALayer") ) {
      addedLayer = new HyPerLCALayer(name, hc);
   }
   else if( !strcmp(keyword, "Image") ) {
      addedLayer = new Image(name, hc);
   }
   else if( !strcmp(keyword, "ImageFromMemoryBuffer") ) {
      addedLayer = new ImageFromMemoryBuffer(name, hc);
   }
   else if( !strcmp(keyword, "IncrementLayer") ) {
      addedLayer = new IncrementLayer(name, hc);
   }
   else if( !strcmp(keyword, "KmeansLayer") ) {
      addedLayer = new KmeansLayer(name, hc);
   }
   else if( !strcmp(keyword, "LCALIFLayer") ) {
      addedLayer = new LCALIFLayer(name, hc);
   }
   else if( !strcmp(keyword, "LIF") ) {
      addedLayer = new LIF(name, hc);
   }
   else if( !strcmp(keyword, "LIFGap") ) {
      addedLayer = new LIFGap(name, hc);
   }
   else if( !strcmp(keyword, "LabelErrorLayer") ) {
      addedLayer = new LabelErrorLayer(name, hc);
   }
   else if( !strcmp(keyword, "LabelLayer") ) {
      addedLayer = new LabelLayer(name, hc);
   }
   else if( !strcmp(keyword, "LeakyIntegrator") ) {
      addedLayer = new LeakyIntegrator(name, hc);
   }
   else if( !strcmp(keyword, "LogLatWTAGenLayer") ) {
      addedLayer = new LogLatWTAGenLayer(name, hc);
   }
   else if( !strcmp(keyword, "MLPErrorLayer") ) {
      addedLayer = new MLPErrorLayer(name, hc);
   }
   else if( !strcmp(keyword, "MLPForwardLayer") ) {
      addedLayer = new MLPForwardLayer(name, hc);
   }
   else if( !strcmp(keyword, "MLPOutputLayer") ) {
      addedLayer = new MLPOutputLayer(name, hc);
   }
   else if( !strcmp(keyword, "MLPSigmoidLayer") ) {
      addedLayer = new MLPSigmoidLayer(name, hc);
   }
   else if( !strcmp(keyword, "MatchingPursuitLayer") ) {
      addedLayer = new MatchingPursuitLayer(name, hc);
   }
   else if( !strcmp(keyword, "MatchingPursuitResidual") ) {
      addedLayer = new MatchingPursuitResidual(name, hc);
   }
   else if( !strcmp(keyword, "MaxPooling") ) {
      // MaxPooling was marked obsolete Oct 30, 2014
      if (hc->columnId()==0) {
         fprintf(stderr, "Params group \"%s\": MaxPooling is obsolete.  Use a different layer type and set the connections going to \"%s\" to use pvpatchAccumulateType = \"maxpooling\".\n", name, name);
      }
      MPI_Barrier(hc->icCommunicator()->communicator());
      exit(EXIT_FAILURE);
   }
   else if( !strcmp(keyword, "Movie") ) {
      addedLayer = new Movie(name, hc);
   }
   else if( !strcmp(keyword, "Patterns") ) {
      addedLayer = new Patterns(name, hc);
   }
   else if( !strcmp(keyword, "PoolingANNLayer") ) {
      addedLayer = new PoolingANNLayer(name, hc);
   }
   else if( !strcmp(keyword, "PtwiseProductLayer") ) {
      addedLayer = new PtwiseProductLayer(name, hc);
   }
   else if( !strcmp(keyword, "RescaleLayer") ) {
      addedLayer = new RescaleLayer(name, hc);
   }
   else if( !strcmp(keyword, "Retina") ) {
      addedLayer = new Retina(name, hc);
   }
   else if( !strcmp(keyword, "ShuffleLayer") ) {
      addedLayer = new ShuffleLayer(name, hc);
   }
   else if( !strcmp(keyword, "SigmoidLayer") ) {
      addedLayer = new SigmoidLayer(name, hc);
   }
   else if( !strcmp(keyword, "TextStream") ) {
      addedLayer = new TextStream(name, hc);
   }
   else if( !strcmp(keyword, "TrainingLayer") ) {
      addedLayer = new TrainingLayer(name, hc);
   }
   else if( !strcmp(keyword, "WTALayer") ) {
      addedLayer = new WTALayer(name, hc);
   }
#ifdef PV_USE_SNDFILE // TODO: move SoundFile stuff into its own library
   else if( !strcmp(keyword, "NewCochlearLayer") ) {
      addedObject = new NewCochlearLayer(name, hc);
   }
   else if( !strcmp(keyword, "SoundStream") ) {
      addedObject = new SoundStream(name, hc);
   }
#endif

   if (addedLayer==NULL) {
      if (addedLayer==NULL) {
         if (hc->columnId()==0) {
            fprintf(stderr, "createLayer error: unable to add %s\n", keyword);
         }
         MPI_Barrier(hc->icCommunicator()->communicator());
         exit(EXIT_FAILURE);
      }
   }

   return addedLayer;
}

BaseConnection * CoreParamGroupHandler::createConnection(char const * keyword, char const * name, HyPerCol * hc, InitWeights * weightInitializer, NormalizeBase * weightNormalizer) {
   BaseConnection * addedConnection = NULL;

   if (keyword==NULL) {
      addedConnection = NULL;
   }
   else if( !strcmp(keyword, "HyPerConn") ) {
      addedConnection = new HyPerConn(name, hc, weightInitializer, weightNormalizer);
   }
   else if( !strcmp(keyword, "BIDSConn") ) {
      addedConnection = new BIDSConn(name, hc, weightInitializer, weightNormalizer);
   }
   else if( !strcmp(keyword, "CloneConn") ) {
      addedConnection = new CloneConn(name, hc);
   }
   else if( !strcmp(keyword, "CloneKernelConn") ) {
      // Deprecated as of June 6, 2014.  Use CloneConn with sharedWeight = true
      addedConnection = new CloneKernelConn(name, hc);
   }
   else if( !strcmp(keyword, "CopyConn") ) {
      addedConnection = new CopyConn(name, hc, weightNormalizer);
   }
   else if( !strcmp(keyword, "FeedbackConn") ) {
      addedConnection = new FeedbackConn(name, hc);
   }
   else if( !strcmp(keyword, "GapConn") ) {
      addedConnection = new GapConn(name, hc, weightInitializer, weightNormalizer);
   }
   else if( !strcmp(keyword, "IdentConn") ) {
      addedConnection = new IdentConn(name, hc);
   }
   else if( !strcmp(keyword, "ImprintConn") ) {
      addedConnection = new ImprintConn(name, hc, weightInitializer, weightNormalizer);
   }
   else if( !strcmp(keyword, "KernelConn") ) {
      // Deprecated as of June 5, 2014.  Use HyPerConn with sharedWeight = true
      addedConnection = new KernelConn(name, hc, weightInitializer, weightNormalizer);
   }
   else if( !strcmp(keyword, "LCALIFLateralConn") ) {
      addedConnection = new LCALIFLateralConn(name, hc, weightInitializer, weightNormalizer);
   }
   else if( !strcmp(keyword, "OjaSTDPConn") ) {
      addedConnection = new OjaSTDPConn(name, hc, weightInitializer, weightNormalizer);
   }
   else if( !strcmp(keyword, "PlasticCloneConn") ) {
      addedConnection = new PlasticCloneConn(name, hc);
   }
   else if( !strcmp(keyword, "PoolingConn") ) {
      addedConnection = new PoolingConn(name, hc, weightInitializer, weightNormalizer);
   }
   else if( !strcmp(keyword, "TransposeConn") ) {
      addedConnection = new TransposeConn(name, hc);
   }

   if (addedConnection==NULL) {
      if (weightInitializer==NULL) {
         if (hc->columnId()==0) {
            fprintf(stderr, "createConnection error: unable to add %s\n", keyword);
         }
         MPI_Barrier(hc->icCommunicator()->communicator());
         exit(EXIT_FAILURE);
      }
   }

   return addedConnection;
}

ColProbe * CoreParamGroupHandler::createColProbe(char const * keyword, char const * name, HyPerCol * hc) {
   ColProbe * addedColProbe = NULL;

   if (keyword==NULL) {
      addedColProbe = NULL;
   }
   else if( !strcmp(keyword, "ColProbe") ) {
      addedColProbe = new ColProbe(name, hc);
   }
   else if( !strcmp(keyword, "GenColProbe") ) {
      addedColProbe = new GenColProbe(name, hc);
   }

   if (addedColProbe==NULL) {
      if (addedColProbe==NULL) {
         if (hc->columnId()==0) {
            fprintf(stderr, "createColProbe error: unable to add %s\n", keyword);
         }
         MPI_Barrier(hc->icCommunicator()->communicator());
         exit(EXIT_FAILURE);
      }
   }

   return addedColProbe;
}

BaseProbe * CoreParamGroupHandler::createProbe(char const * keyword, char const * name, HyPerCol * hc) {
   BaseProbe * addedProbe = NULL;

   // Layer probe keywords
   if (keyword==NULL) {
      addedProbe = NULL;
   }
   else if( !strcmp(keyword, "LayerProbe") ) {
      fprintf(stderr, "LayerProbe \"%s\": Abstract class LayerProbe cannot be instantiated.\n", name);
      addedProbe = NULL;
   }
   else if( !strcmp(keyword, "L2NormProbe") ) {
      addedProbe = new L2NormProbe(name, hc);
   }
   else if( !strcmp(keyword, "LayerFunctionProbe") ) {
      addedProbe = new LayerFunctionProbe(name, hc);
   }
   else if( !strcmp(keyword, "LogLatWTAProbe") ) {
      addedProbe = new LogLatWTAProbe(name, hc);
   }
   else if( !strcmp(keyword, "PointLIFProbe") ) {
      addedProbe = new PointLIFProbe(name, hc);
   }
   else if( !strcmp(keyword, "PointProbe") ) {
      addedProbe = new PointProbe(name, hc);
   }
   else if( !strcmp(keyword, "RequireAllZeroActivityProbe") ) {
      addedProbe = new RequireAllZeroActivityProbe(name, hc);
   }
   else if( !strcmp(keyword, "SparsityLayerProbe") ) {
      addedProbe = new SparsityLayerProbe(name, hc);
   }
   else if( !strcmp(keyword, "StatsProbe") ) {
      addedProbe = new StatsProbe(name, hc);
   }
   else if( !strcmp(keyword, "TextStreamProbe") ) {
      addedProbe = new TextStreamProbe(name, hc);
   }

   // Connection probe keywords
   else if( !strcmp(keyword, "KernelProbe") ) {
      addedProbe = new KernelProbe(name, hc);
   }

   if (addedProbe==NULL) {
      if (addedProbe==NULL) {
         if (hc->columnId()==0) {
            fprintf(stderr, "createProbe error: unable to add %s\n", keyword);
         }
         MPI_Barrier(hc->icCommunicator()->communicator());
         exit(EXIT_FAILURE);
      }
   }

   return addedProbe;
}

InitWeights * CoreParamGroupHandler::createWeightInitializer(char const * keyword, char const * name, HyPerCol * hc) {
   InitWeights * weightInitializer = NULL;

   if (keyword==NULL) {
      weightInitializer = NULL;
   }
   else if (!strcmp(keyword, "Gauss2DWeight")) {
      weightInitializer = new InitGauss2DWeights(name, hc);
   }
   else if (!strcmp(keyword, "CoCircWeight")) {
      weightInitializer = new InitCocircWeights(name, hc);
   }
   else if (!strcmp(keyword, "UniformWeight")) {
      weightInitializer = new InitUniformWeights(name, hc);
   }
   else if (!strcmp(keyword, "SmartWeight")) {
      weightInitializer = new InitSmartWeights(name, hc);
   }
   else if (!strcmp(keyword, "BIDSLateral")) {
      weightInitializer = new InitBIDSLateral(name, hc);
   }
   else if (!strcmp(keyword, "UniformRandomWeight")) {
      weightInitializer = new InitUniformRandomWeights(name, hc);
   }
   else if (!strcmp(keyword, "GaussianRandomWeight")) {
      weightInitializer = new InitGaussianRandomWeights(name, hc);
   }
   else if (!strcmp(keyword, "GaborWeight")) {
      weightInitializer = new InitGaborWeights(name, hc);
   }
   else if (!strcmp(keyword, "IdentWeight")) {
      weightInitializer = new InitIdentWeights(name, hc);
   }
   else if (!strcmp(keyword, "OneToOneWeights")) {
      weightInitializer = new InitOneToOneWeights(name, hc);
   }
   else if (!strcmp(keyword, "OneToOneWeightsWithDelays")) {
      weightInitializer = new InitOneToOneWeightsWithDelays(name, hc);
   }
   else if (!strcmp(keyword, "SpreadOverArborsWeight")) {
      weightInitializer = new InitSpreadOverArborsWeights(name, hc);
   }
   else if (!strcmp(keyword, "MaxPoolingWeight")) {
      weightInitializer = new InitMaxPoolingWeights(name, hc);
   }
   else if (!strcmp(keyword, "FileWeight")) {
      weightInitializer = new InitWeights(name, hc);
   }

   if (weightInitializer==NULL) {
      if (hc->columnId()==0) {
         fprintf(stderr, "createWeightInitializer error: unable to add %s\n", keyword);
      }
      MPI_Barrier(hc->icCommunicator()->communicator());
      exit(EXIT_FAILURE);
   }

   return weightInitializer;
}

NormalizeBase * CoreParamGroupHandler::createWeightNormalizer(char const * keyword, char const * name, HyPerCol * hc) {
   NormalizeBase * weightNormalizer = NULL;
//   bool normalizeMethodChosen = true;
//
//   if (keyword==NULL) {
//      weightNormalizer = NULL;
//   }
//   else if (!strcmp(keyword, "normalizeSum")) {
//      // Coming soon! Still need to rework the normalizer hierarchy so that normalizers can be constructed before connections.
//   }
//   else if (!strcmp(keyword, "normalizeL2")) {
//
//   }
//   else if (!strcmp(keyword, "normalizeMax")) {
//
//   }
//   else if (!strcmp(keyword, "normalizeContrastZeroMean")) {
//
//   }
//   else if (!strcmp(keyword, "normalizeScale")) {
//
//   }
//   else if (!strcmp(keyword, "normalizeGroup")) {
//
//   }
//   else if (!strcmp(keyword, "none")) {
//      normalizeMethodChosen = false; // make sure that a null weightNormalizer is not treated as an error in this case.
//   }
//   else {
//      if (hc->columnId()==0) {
//         fprintf(stderr, "createWeightNormalizer error: unrecognized method %s\n", keyword);
//      }
//      MPI_Barrier(hc->icCommunicator()->communicator());
//      exit(EXIT_FAILURE);
//   }
//
//   if (weightNormalizer==NULL && normalizeMethodChosen==true) {
//      if (hc->columnId()==0) {
//         fprintf(stderr, "createWeightNormalizer error: unable to add %s\n", keyword);
//      }
//      MPI_Barrier(hc->icCommunicator()->communicator());
//      exit(EXIT_FAILURE);
//   }

   return weightNormalizer;
}

} /* namespace PV */
