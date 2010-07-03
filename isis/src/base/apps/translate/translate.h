#ifndef Translate_h
#define Translate_h

#include "Transform.h"
#include <cmath>
#include <iostream>

class Translate : public Isis::Transform {
  private:
    int p_outputSamples;
    int p_outputLines;

    double p_sampleTrans;
    double p_lineTrans;

  public:
    // constructor
    Translate (const double inputSamples, const double inputLines, 
               const double sampleTrans, const double lineTrans){
      // Save off the sample and line translation
      p_sampleTrans = sampleTrans;
      p_lineTrans = lineTrans;

      p_outputSamples = (int) inputSamples;
      p_outputLines = (int) inputLines;
    }

    // destructor
    ~Translate () {};

    // Implementations for parent's pure virtual members
    // Convert the requested output samp/line to an input samp/line
    bool Xform (double &inSample, double &inLine,
                           const double outSample, const double outLine) {
      inSample = outSample - p_sampleTrans;
      inLine = outLine - p_lineTrans;
      return true;
    }

    //  Return the output number of samples
    int OutputSamples () const {
      return p_outputSamples;
    }

    //  Return the output number of lines
    int OutputLines () const {
      return p_outputLines;
    }
};

#endif

