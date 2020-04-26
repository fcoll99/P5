#ifndef INSTRUMENT_VIBRATO
#define INSTRUMENT_VIBRATO

#include <vector>
#include <string>
#include "instrument.h"
#include "envelope_adsr.h"

namespace upc {
  class InstrumentVibrato: public upc::Instrument {
    EnvelopeADSR adsr;
    
    float phase;
    float phase_vibrato;

    float step;
    float step_vibrato;

    float F0;
    float A;
    float I1;
    float fm1;

    
  public:
    InstrumentVibrato(const std::string &param = "");
    void command(long cmd, long note, long velocity=1); 
    const std::vector<float> & synthesize();
    bool is_active() const {return bActive;} 
  };
}

#endif
