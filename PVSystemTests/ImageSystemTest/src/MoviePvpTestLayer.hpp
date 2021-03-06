/*
 * MoviePvpTestLayer.hpp
 * Author: slundquist
 */

#ifndef MOVIEPVPTESTLAYER_HPP_
#define MOVIEPVPTESTLAYER_HPP_
#include <layers/MoviePvp.hpp>

namespace PV{

class MoviePvpTestLayer : public PV::MoviePvp{
public:
   MoviePvpTestLayer(const char * name, HyPerCol * hc);
   virtual int updateStateWrapper(double time, double dt);
};

}
#endif /* PVPTESTLAYER_HPP */
