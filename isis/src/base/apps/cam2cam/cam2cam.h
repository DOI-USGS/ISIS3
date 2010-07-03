#ifndef cam2cam_h
#define cam2cam_h

#include "Transform.h"

class cam2cam : public Isis::Transform {
  private:
    Isis::Camera *p_incam;
    Isis::Camera *p_outcam;
    int p_inputSamples;
    int p_inputLines;
    int p_outputSamples;
    int p_outputLines;

  public:
    // constructor
    cam2cam (const int inputSamples, const int inputLines, Isis::Camera *incam, 
             const int outputSamples, const int outputLines, Isis::Camera *outcam);
    
    // destructor
    ~cam2cam () {};

    // Implementations for parent's pure virtual members
    bool Xform (double &inSample, double &inLine,
                    const double outSample, const double outLine);
    int OutputSamples () const;
    int OutputLines () const;
};

#endif
