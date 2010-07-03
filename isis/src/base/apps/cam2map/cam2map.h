#ifndef cam2map_h
#define cam2map_h

#include "Transform.h"

class cam2map : public Isis::Transform {
  private:
    Isis::Camera *p_incam;
    Isis::Projection *p_outmap;
    int p_inputSamples;
    int p_inputLines;
    bool p_trim;
    int p_outputSamples;
    int p_outputLines;

  public:
    // constructor
    cam2map (const int inputSamples, const int inputLines, Isis::Camera *incam, 
             const int outputSamples, const int outputLines, Isis::Projection *outmap,
             bool trim);
    
    // destructor
    ~cam2map () {};

    // Implementations for parent's pure virtual members
    bool Xform (double &inSample, double &inLine,
                    const double outSample, const double outLine);
    int OutputSamples () const;
    int OutputLines () const;
};

#endif
