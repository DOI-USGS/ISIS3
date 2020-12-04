#ifndef NoCam2Map_h
#define NoCam2Map_h

#include "Transform.h"
#include "UserInterface.h"

namespace Isis {  
  extern void nocam2map(Cube *cube, UserInterface &ui, Pvl *log=nullptr);
  extern void nocam2map(UserInterface &ui, Pvl *log=nullptr);
  
  class Cube;
  class LeastSquares;
  class TProjection;
  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   */
  class NoCam2Map : public Isis::Transform {
    public:
      // constructor
      NoCam2Map(Isis::LeastSquares sample, Isis::LeastSquares line, Isis::TProjection *outmap,
                Isis::Cube *latCube, Isis::Cube *lonCube,
                bool isOcentric , bool isPosEast,
                double tolerance, int iterations,
                const int inputSamples, const int inputLines,
                const int outputSamples, const int outputLines);
  
      // destructor
      ~NoCam2Map() {};
  
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
