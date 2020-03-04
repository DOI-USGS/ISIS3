#ifndef cam2map_h
#define cam2map_h

#include "Application.h"
#include "TProjection.h"
#include "Transform.h"
#include "UserInterface.h"
#include "Pvl.h"

namespace Isis {
  extern void cam2map(UserInterface &ui, Pvl *log=nullptr);

  extern void cam2map(Cube *icube, Pvl &userMap, PvlGroup &userGrp, UserInterface &ui, Pvl *log);

  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   *   @history 2012-12-06 Debbie A. Cook - Changed to use TProjection instead of Projection.
   *                          References #775.
   */
  class cam2mapReverse : public Transform {
    private:
      Camera *p_incam;
      TProjection *p_outmap;
      int p_inputSamples;
      int p_inputLines;
      bool p_trim;
      bool p_occlusion;
      int p_outputSamples;
      int p_outputLines;

    public:
      // constructor
      cam2mapReverse(const int inputSamples, const int inputLines,
                     Camera *incam,
                     const int outputSamples, const int outputLines,
                     TProjection *outmap,
                     bool trim,
                     bool occlusion=false);

      // destructor
      ~cam2mapReverse() {};

      // Implementations for parent's pure virtual members
      bool Xform(double &inSample, double &inLine,
                 const double outSample, const double outLine);
      int OutputSamples() const;
      int OutputLines() const;
  };

  /**
   * @author 2012-04-19 Jeff Anderson
   *
   * @internal
   */
  class cam2mapForward : public Transform {
    private:
      Camera *p_incam;
      TProjection *p_outmap;
      int p_inputSamples;
      int p_inputLines;
      bool p_trim;
      int p_outputSamples;
      int p_outputLines;

    public:
      // constructor
      cam2mapForward(const int inputSamples, const int inputLines,
                     Camera *incam,
                     const int outputSamples, const int outputLines,
                     TProjection *outmap,
                     bool trim);

      // destructor
      ~cam2mapForward() {};

      // Implementations for parent's pure virtual members
      bool Xform(double &outSample, double &outLine,
                 const double inSample, const double inLine);
      int OutputSamples() const;
      int OutputLines() const;
  };
}

#endif
