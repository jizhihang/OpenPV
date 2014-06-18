/*
 * receiveFromPostProbe.cpp
 * Author: slundquist
 */

#ifdef PV_USE_SNDFILE

#include "SoundProbe.hpp"
#include <include/pv_arch.h>
#include <layers/HyPerLayer.hpp>
#include <assert.h>
#include <string.h>

namespace PV {
SoundProbe::SoundProbe(const char * probeName, HyPerCol * hc)
   : StatsProbe()
{
   init_base();
   initSoundProbe(probeName, hc);
}

SoundProbe::~SoundProbe() {
   if(soundOutputPath){
      free(soundOutputPath);
      soundOutputPath = NULL;
   }
   if(soundInputType){
      free(soundInputType);
      soundInputType = NULL;
   }
   if(soundBuf){
      free(soundBuf);
   }
   soundBuf = NULL;
   delete fileHeader;
}

int SoundProbe::init_base() {
   return PV_SUCCESS;
}

int SoundProbe::initSoundProbe(const char * probeName, HyPerCol * hc) {
   return initStatsProbe(probeName, hc);
}

int SoundProbe::communicateInitInfo(){
   StatsProbe::communicateInitInfo();
   //Open soundprobe file for writing
   SF_INFO* tmpFileHeader = new SF_INFO();
   //Grab the header to base the output file on
   SNDFILE* tmpStream = sf_open(soundInputType, SFM_READ, tmpFileHeader);
   std::cout << "frames: " << tmpFileHeader->frames << "\n";
   //New file header
   fileHeader = new SF_INFO();
   //Set new variables
   fileHeader->samplerate = tmpFileHeader->samplerate;
   fileHeader->format = tmpFileHeader->format;
   fileHeader->channels = getTargetLayer()->getLayerLoc()->nf;
   sf_close(tmpStream);
   //Only 1x1 in x and y direction allowed
   assert(getTargetLayer()->getLayerLoc()->nx == 1 && getTargetLayer()->getLayerLoc()->ny == 1);
   assert(sf_format_check(fileHeader));
   fileStream = sf_open(soundOutputPath, SFM_WRITE, fileHeader);
   soundBuf = (float*) malloc(sizeof(float) * fileHeader->channels);
   return PV_SUCCESS;
}


int SoundProbe::ioParamsFillGroup(enum ParamsIOFlag ioFlag) {
   int status = StatsProbe::ioParamsFillGroup(ioFlag);
   ioParam_soundOutputPath(ioFlag);
   ioParam_soundInputType(ioFlag);
   return status;
}

void SoundProbe::ioParam_soundOutputPath(enum ParamsIOFlag ioFlag) {
   parent->ioParamStringRequired(ioFlag, name, "soundOutputPath", &soundOutputPath);
}

void SoundProbe::ioParam_soundInputType(enum ParamsIOFlag ioFlag) {
   parent->ioParamStringRequired(ioFlag, name, "soundInputType", &soundInputType);
}

int SoundProbe::outputState(double timed){
   const pvdata_t * A = getTargetLayer()->getLayerData();
   int numNeurons = getTargetLayer()->getNumNeurons();
   const PVLayerLoc * loc = getTargetLayer()->getLayerLoc();
   for (int i = 0; i < numNeurons; i++){
      soundBuf[i] = A[i];
   }
   //Write file out
   int count = sf_writef_float(fileStream, soundBuf, 1);
   assert(count == 1);
   return PV_SUCCESS;
}

}

#endif // PV_USE_SNDFILE

