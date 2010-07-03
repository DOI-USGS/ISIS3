#ifndef enlarge_h
#define enlarge_h

#include "Transform.h"

#include <cmath>
#include <iostream>

class Enlarge : public Isis::Transform {
  private:
    int p_outputSamples; // Number of samples for output
    int p_outputLines;   // Number of lines for output

    double p_sampleScale;
    double p_lineScale;

  public:
    // constructor
    Enlarge (const double sampleScale, const double lineScale, 
             const int outputSamples, const int outputLines){

      // Save off the sample and line magnification
      p_sampleScale = sampleScale;
      p_lineScale = lineScale;

      p_outputSamples = outputSamples;
      p_outputLines = outputLines;
    }

    // destructor
    ~Enlarge () {};

    // Implementations for parent's pure virtual members
    // Convert the requested output samp/line to an input samp/line
    bool Xform (double &inSample, double &inLine,
                         const double outSample, const double outLine) {
      inSample = (outSample - 0.5) / p_sampleScale + 0.5;
      inLine = (outLine - 0.5) / p_lineScale + 0.5;
      return true;
    }

    // Return the output number of samples
    int OutputSamples () const {
      return p_outputSamples;
    }

    // Return the output number of lines
    int OutputLines () const {
       return p_outputLines;
    }

};

#endif

