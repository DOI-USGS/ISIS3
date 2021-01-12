#ifndef nocam2map_h
#define nocam2map_h

#include "Transform.h"

namespace Isis {

  class Cube;
  class LeastSquares;
  class TProjection;
  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   */
  class nocam2map : public Isis::Transform {
    public:
      // constructor
      nocam2map(Isis::LeastSquares sample, Isis::LeastSquares line, Isis::TProjection *outmap,
                Isis::Cube *latCube, Isis::Cube *lonCube,
                bool isOcentric , bool isPosEast,
                double tolerance, int iterations,
                const int inputSamples, const int inputLines,
                const int outputSamples, const int outputLines);
  
      // destructor
      ~nocam2map() {};
  
      // Implementations for parent's pure virtual members
      bool Xform(double &inSample, double &inLine,
                 const double outSample, const double outLine);
      int OutputSamples() const;
      int OutputLines() const;

    private:
      Isis::LeastSquares *p_sampleSol;
      Isis::LeastSquares *p_lineSol;
      Isis::TProjection *p_outmap;
      Isis::Cube *p_latCube;
      Isis::Cube *p_lonCube;
      bool p_isOcentric;
      bool p_isPosEast;
      int p_inputSamples;
      int p_inputLines;
      int p_outputSamples;
      int p_outputLines;
      double p_latCenter;
      double p_lonCenter;
      double p_radius;
      double p_tolerance;
      int p_iterations;
  
  };
};

#endif
