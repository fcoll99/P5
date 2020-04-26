#include <iostream>
#include <math.h>
#include "audio.h"
#include "keyvalue.h"
#include "sndfile.h"

#include <stdlib.h>

using namespace upc;
using namespace std;

Audio::Audio(const std::string &param) 
  : adsr(SamplingRate, param) {
  bActive = false;
  x.resize(BSIZE);

  KeyValue kv(param);
  
  file_name = kv("file");
  inputFile = sf_open(file_name.c_str(), SFM_READ, &inputFileData);
}


void Audio::command(long cmd, long note, long vel) {
    bActive = true;
    A = vel/127.0;
}


const vector<float> & Audio::synthesize() {

static double *content = new double[BSIZE];

  reader = sf_read_double(inputFile, content, BSIZE);
  if(reader > 0){
    for (unsigned int i = 0; i < reader; i++) x[i] = 0.5*A*content[i];
  }else{
    x.assign(x.size(), 0);
    bActive = false;
    sf_close(inputFile);
  }
  
  return x;
}
