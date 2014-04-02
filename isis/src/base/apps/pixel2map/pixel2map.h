#ifndef pixel2map_h
#define pixel2map_h

#include "Transform.h"

/**
 * @author 2008-02-13 Stacy Alley
 *
 * @internal 
 *   @history 2013-07-30 Stuart C. Sides & Tracie Sucharski Renamed from vim2map
 */
class pixel2map : public Isis::Transform {
  private:
    Isis::Camera *p_incam;
    Isis::Projection *p_outmap;

  public:
    // constructor
    pixel2map(const int inputSamples, const int inputLines, Isis::Camera *incam,
             const int outputSamples, const int outputLines, Isis::Projection *outmap,
             bool trim);

    // destructor
    ~pixel2map() {};


};

#endif
