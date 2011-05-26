/*
 * PtwiseProductLayer.cpp
 *
 * The output V is the pointwise product of phiExc and phiInh
 *
 * "Exc" and "Inh" are really misnomers for this class, but the
 * terminology is inherited from the base class.
 *
 *  Created on: Apr 25, 2011
 *      Author: peteschultz
 */

#include "PtwiseProductLayer.hpp"

namespace PV {

PtwiseProductLayer::PtwiseProductLayer(const char * name, HyPerCol * hc) : ANNLayer(name, hc) {
}  // end PtwiseProductLayer::PtwiseProductLayer(const char *, HyPerCol *)

int PtwiseProductLayer::updateV() {
    pvdata_t * V = getV();
    pvdata_t * phiExc = getChannel(CHANNEL_EXC);
    pvdata_t * phiInh = getChannel(CHANNEL_INH);
    for( int k=0; k<getNumNeurons(); k++ ) {
        V[k] = phiExc[k] * phiInh[k];
    }
    return PV_SUCCESS;
}  // end PtwiseProductLayer::updateV()

}  // end namespace PV
