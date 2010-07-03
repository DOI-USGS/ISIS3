#ifndef rotate_h
#define rotate_h

#include "Transform.h"
#include <cmath>
#include <iostream>

class Rotate : public Isis::Transform {
  private:
    double p_degrees;    // The degree of rotation
    double p_radians;    // p_degrees converted to radians
    double p_cosRad;     // The cosine of p_radians
    double p_sinRad;     // The sine of p_radians
    int p_outputSamples; // Number of samples for output
    int p_outputLines;   // Number of lines for output
    double p_minSamp;    // The minimum sample after rotation
    double p_minLine;    // The minimum line after rotation
    double p_maxSamp;    // The maximum sample after rotation
    double p_maxLine;    // The maximum line after rotation


  public:
    // constructor
    Rotate (const double inputSamples, const double inputLines,
            const double rotation) {
      // Angles for SIN & COS are measured positive counter-clockwise
      // but the program is assuming positive clockwise, so use the negative angle
    
      // Setup private data for calculating where a pixel in the output comes from
      double PI = acos (-1.0);
      
      p_degrees = -rotation;
      p_radians = p_degrees * PI / 180.0;
    
      p_cosRad = cos (p_radians);
      p_sinRad = sin (p_radians);
    
      // Rotate the corners of the input to find the output size
      double samples[4];
      double lines[4];
    
      // Rotate (1,1) = (0.5,0.5)
      samples[0] = 0.5 * p_sinRad + 0.5 * p_cosRad;
      lines[0] = 0.5 * p_cosRad - 0.5 * p_sinRad;
      // Rotate (ns,1) = (ns+0.5, .5)
      samples[1] = 0.5 * p_sinRad + (inputSamples+0.5) * p_cosRad;
      lines[1] = 0.5 * p_cosRad - (inputSamples+0.5) * p_sinRad;
      // Rotate (1,nl) = (.5, nl+0.5)
      samples[2] = (inputLines+0.5) * p_sinRad + 0.5 * p_cosRad;
      lines[2] = (inputLines+0.5) * p_cosRad - 0.5 * p_sinRad;
      // Rotate (ns,nl) = (ns+0.5, nl+0.5)
      samples[3] = (inputLines+0.5) * p_sinRad + (inputSamples+0.5) * p_cosRad;
      lines[3] = (inputLines+0.5) * p_cosRad - (inputSamples+0.5) * p_sinRad;
    
      // Find the min and max samp and line
      p_minSamp = samples[0];
      for (int i=1; i<=3; i++) if (samples[i] < p_minSamp) p_minSamp = samples[i];
      p_maxSamp = samples[0];
      for (int i=1; i<=3; i++) if (samples[i] > p_maxSamp) p_maxSamp = samples[i];
      p_minLine = lines[0];
      for (int i=1; i<=3; i++) if (lines[i] < p_minLine) p_minLine = lines[i];
      p_maxLine = lines[0];
      for (int i=1; i<=3; i++) if (lines[i] > p_maxLine) p_maxLine = lines[i];
    
      // Calculate the output size. If there is a fractional pixel, round up
      p_outputSamples = (int)ceil (p_maxSamp - p_minSamp);
      p_outputLines = (int)ceil (p_maxLine - p_minLine);
    
      if ((p_degrees == 90.0) || (p_degrees == -90)) {
        p_outputSamples = (int)inputLines;
        p_outputLines = (int)inputSamples;
      }
      if ((p_degrees == 180.0) || (p_degrees == -180.0)) {
        p_outputSamples = (int)inputSamples;
        p_outputLines = (int)inputLines;
      }
    }

    // destructor
    ~Rotate () {};

    // Implementations for parent's pure virtual members
    // Convert the requested output samp/line to an input samp/line
    bool Xform (double &inSample, double &inLine,
                const double outSample, const double outLine) {
      // First calculate the rotated input position (uses equation of a line)
      double inRotSamp = outSample - 0.5 + p_minSamp;
      double inRotLine = outLine - 0.5 + p_minLine;
    
      // Now unrotate the position from above to get the original input position
      //  inSample = (-inRotLine * p_sinRad) + (inRotSamp * p_cosRad);
      inSample = (inRotSamp * p_cosRad) - (inRotLine * p_sinRad);
      inLine = (inRotLine * p_cosRad) + (inRotSamp * p_sinRad);
    
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

