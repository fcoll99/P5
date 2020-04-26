#ifndef AUDIO
#define AUDIO

#include <vector>
#include <string>
#include "instrument.h"
#include "envelope_adsr.h"
#include "sndfile.h"

namespace upc {
  class Audio: public upc::Instrument {
    EnvelopeADSR adsr;

    std::string file_name;
    SNDFILE *inputFile;
    SF_INFO inputFileData;

    unsigned int reader;
    float A;
 
  public:
    Audio(const std::string &param = "");
    void command(long cmd, long note, long velocity=1); 
    const std::vector<float> & synthesize();
    bool is_active() const {return bActive;} 
  };
}

#endif