#ifndef map2map_h
#define map2map_h

#include "Transform.h"

class map2map : public Isis::Transform {
  private:
    Isis::Projection *p_inmap;
    Isis::Projection *p_outmap;
    int p_inputSamples;
    int p_inputLines;
    bool p_trim;
    int p_outputSamples;
    int p_outputLines;
    int p_inputWorldSize;

  public:
    // constructor
    map2map (const int inputSamples, const int inputLines, Isis::Projection *inmap, 
           const int outputSamples, const int outputLines, Isis::Projection *outmap,
           bool trim);
    
    // destructor
    ~map2map () {};

    // Implementations for parent's pure virtual members
    bool Xform (double &inSample, double &inLine,
                    const double outSample, const double outLine);
    int OutputSamples () const;
    int OutputLines () const;
};

#endif
