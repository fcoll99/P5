#include <iostream>
#include <math.h>
#include <errno.h>
#include <string.h>
#include "seno.h"
#include "keyvalue.h"

#include <stdlib.h>

using namespace upc;
using namespace std;

Seno::Seno(const std::string &param) 
  : adsr(SamplingRate, param) {
  bActive = false;
  x.resize(BSIZE);

  /*
    You can use the class keyvalue to parse "param" and configure your instrument.
    Take a Look at keyvalue.h    
  */
  KeyValue kv(param);
  std::string file_name = "table.wav";
  static string kv_null;
  unsigned int fm;
  if (readwav_mono(file_name, fm, tbl) < 0) {
    cerr << "Error: no se puede leer el fichero " << file_name << " para un instrumento FicTabla" << " (" << strerror(errno) << ")"<< endl;
    throw -1;
  }
  N = tbl.size();
  phs = 0;
  index = 0;
  stepA = 2 * M_PI /(float) N;
}


void Seno::command(long cmd, long note, long vel) {
  if (cmd == 9) {		//'Key' pressed: attack begins
    bActive = true;
    adsr.start();
    f0 = 440.0 * pow(2, (((float)note - 69.0)/12.0));
    index=0;
    phs=0;
    stepB = 2 * M_PI * f0/SamplingRate;
	  A = vel / 127.;
    phs=0;
  }
  else if (cmd == 8) {	//'Key' released: sustain ends, release begins
    adsr.stop();
  }
  else if (cmd == 0) {	//Sound extinguished without waiting for release to end
    adsr.end();
  }
}


const vector<float> & Seno::synthesize() {
  if (not adsr.active()) {
    x.assign(x.size(), 0);
    bActive = false;
    return x;
  }
  else if (not bActive) return x;
  for (unsigned int i=0; i<x.size(); ++i) {
    unsigned int i1 = (int)ceil(phs);
    unsigned int i2 = (int)floor(phs);
    float a1 = (float) i2 - phs;
    float a2 = (float) phs - i1;
    
    while(i2 >= N) i2 -= N;
    while(i1 >= N) i1 -= N;
    x[i] = A*(tbl[i1]*(a1) + tbl[i2]*(a2));
    phs += stepB/stepA;
    index = phs;
    while(index >= N){
      phs = stepB/stepA;
      index -= N;
    }
  }
  adsr(x); //apply envelope to x and update internal status of ADSR
  return x;
}
