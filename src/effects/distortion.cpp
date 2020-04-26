#include <iostream>
#include <math.h>
#include "distortion.h"
#include "keyvalue.h"

#include <stdlib.h>

using namespace upc;
using namespace std;

Distortion::Distortion(const std::string &param) {

  KeyValue kv(param);

  if (!kv.to_float("A", A))
    A = 0.5; //default value
}

void Distortion::command(unsigned int comm) {
  if (comm == 1);
}

void Distortion::operator()(std::vector<float> &x){
    for (unsigned int i = 0; i < x.size(); i++) {
    if (x[i] > A) x[i] = A;
    else if (x[i] < -A) x[i] = -A;
    else x[i] = (x[i] - pow(x[i],3)/3.0)*(3.0/2.0)/A;
  }
}
