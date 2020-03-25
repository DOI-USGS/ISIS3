#ifndef map2cam_h
#define map2cam_h

#include "Camera.h"
#include "Transform.h"
#include "TProjection.h"
#include "UserInterface.h"
/**
 * @author ????-??-?? Unknown
 *
 * @internal
 *   @history 2012-11-06 Debbie A. Cook - Changed to use TProjection  instead of Projection.
 *                            References #775.
 */
namespace Isis {
  class map2cam : public Isis::Transform {
    private:
      Isis::Camera *p_outcam;
      Isis::TProjection *p_inmap;
      int p_inputSamples;
      int p_inputLines;
      int p_outputSamples;
      int p_outputLines;

    public:
      // constructor
      map2cam(const int inputSamples, const int inputLines, Isis::TProjection *inmap,
              const int outputSamples, const int outputLines, Isis::Camera *outcam);

      // destructor
      ~map2cam() {};

      // Implementations for parent's pure virtual members
      bool Xform(double &inSample, double &inLine,
                 const double outSample, const double outLine);
      int OutputSamples() const;
      int OutputLines() const;
  };

  extern void map2cam_f(UserInterface &ui);
  extern void BandChange(const int band);
}
#endif
