#ifndef INSTRUMENT_CHOWNING
#define INSTRUMENT_CHOWNING

#include <vector>
#include <string>
#include "instrument.h"
#include "envelope_adsr.h"

namespace upc {
  class InstrumentChowning: public upc::Instrument {
    EnvelopeADSR adsr;
    
    float phase;
    float phase_vibrato;

    float step;
    float step_vibrato;

    float F0;
    float fm;

    float A;
    float I1;
    float N1;
    float N2;

    
  public:
    InstrumentChowning(const std::string &param = "");
    void command(long cmd, long note, long velocity=1); 
    const std::vector<float> & synthesize();
    bool is_active() const {return bActive;} 
  };
}

#endif
