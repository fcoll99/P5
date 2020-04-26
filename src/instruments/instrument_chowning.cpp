#include <iostream>
#include <math.h>
#include "instrument_chowning.h"
#include "keyvalue.h"

#include <stdlib.h>

using namespace upc;
using namespace std;

InstrumentChowning::InstrumentChowning(const std::string &param) 
  : adsr(SamplingRate, param) {
  bActive = false;
  x.resize(BSIZE);

  /*
    You can use the class keyvalue to parse "param" and configure your instrument.
    Take a Look at keyvalue.h    
  */

  float I=0.0F;
  float N1_aux=0.0F;
  float N2_aux=0.0F;

  KeyValue kv(param);
  if (!kv.to_float("N1",N1_aux)) 
    N1_aux = 1.0; //default value
  if (!kv.to_float("N2",N2_aux)) 
    N2_aux = 1.0; //default value
  if (!kv.to_float("I",I)) 
    I = 1.0; //default value

  N1 = N1_aux;
  N2 = N2_aux;
  I1 = I;
}

void InstrumentChowning::command(long cmd, long note, long vel) {
  if (cmd == 9) {		//'Key' pressed: attack begins
    bActive = true;
    adsr.start();

    phase = 0;
    phase_vibrato = 0;

    float f0 = 440.0*pow(2,float((note-69.0)/12.0));
    F0 = f0/SamplingRate;
    step = 2*M_PI*F0;

    fm = f0*(N2/N1);
    float FM = fm/SamplingRate;
    step_vibrato = 2*M_PI*FM;

    A = 2*(vel/127.0);
  }
  else if (cmd == 8) {	//'Key' released: sustain ends, release begins
    adsr.stop();
  }
  else if (cmd == 0) {	//Sound extinguished without waiting for release to end
    adsr.end();
  }
}


const vector<float> & InstrumentChowning::synthesize() {
  if (not adsr.active()) {
    x.assign(x.size(), 0);
    bActive = false;
    return x;
  }
  else if (not bActive)
    return x;

  for (unsigned int i=0; i<x.size(); ++i) {
    x[i] = 0.25*A*sin(phase +I1*sin(phase_vibrato));
    phase+=step;
    phase_vibrato += step_vibrato;

    while ((phase +I1*sin(phase_vibrato))> 2*M_PI) phase -= 2*M_PI;
    while (phase_vibrato> 2*M_PI) phase_vibrato -= 2*M_PI;
  }
  adsr(x); //apply envelope to x and update internal status of ADSR

  return x;
}
