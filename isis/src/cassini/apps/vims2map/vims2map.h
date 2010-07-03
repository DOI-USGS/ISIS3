#ifndef vims2map_h
#define vims2map_h

#include "Transform.h"

class vims2map : public Isis::Transform {
  private:
    Isis::Camera *p_incam;
    Isis::Projection *p_outmap;
  
  public:
    // constructor
    vims2map (const int inputSamples, const int inputLines, Isis::Camera *incam, 
             const int outputSamples, const int outputLines, Isis::Projection *outmap,
             bool trim);
    
    // destructor
    ~vims2map () {};

   
};

#endif
