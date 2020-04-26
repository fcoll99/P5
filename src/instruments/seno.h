#ifndef SENO
#define SENO

#include <vector>
#include <string>
#include "instrument.h"
#include "envelope_adsr.h"
#include "wavfile_mono.h"

namespace upc {
  class Seno: public upc::Instrument {
    EnvelopeADSR adsr;
    unsigned int index, N;
	  float A;
    std::vector<float> tbl;
    float f0, stepA, stepB, phs;
  public:
    Seno(const std::string &param = "");
    void command(long cmd, long note, long velocity=100); 
    const std::vector<float> & synthesize();
    bool is_active() const {return bActive;} 
  };
}

#endif
