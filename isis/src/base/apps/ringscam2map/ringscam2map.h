#ifndef ringscam2map_h
#define ringscam2map_h

#include "Transform.h"

using namespace Isis;

/**
 * @author ????-??-?? Unknown
 *
 * @internal
 */
class ringscam2mapReverse : public Transform {
  private:
    Camera *p_incam;
    RingPlaneProjection *p_outmap;
    //    Planar *p_outmap;
    int p_inputSamples;
    int p_inputLines;
    bool p_trim;
    int p_outputSamples;
    int p_outputLines;

  public:
    // constructor
    ringscam2mapReverse(const int inputSamples, const int inputLines,
                   Camera *incam,
                   const int outputSamples, const int outputLines, 
                   RingPlaneProjection *outmap,
                   bool trim);
    //                   Planar *outmap,
    //                   bool trim);

    // destructor
    ~ringscam2mapReverse() {};

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
class ringscam2mapForward : public Transform {
  private:
    Camera *p_incam;
    RingPlaneProjection *p_outmap;
    //    Planar *p_outmap;
    int p_inputSamples;
    int p_inputLines;
    bool p_trim;
    int p_outputSamples;
    int p_outputLines;

  public:
    // constructor
    ringscam2mapForward(const int inputSamples, const int inputLines,
                   Camera *incam,
                   const int outputSamples, const int outputLines, 
                   RingPlaneProjection *outmap,
                   bool trim);
    //                   Planar *outmap,
    //                   bool trim);

    // destructor
    ~ringscam2mapForward() {};

    // Implementations for parent's pure virtual members
    bool Xform(double &outSample, double &outLine,
               const double inSample, const double inLine);
    int OutputSamples() const;
    int OutputLines() const;
};


#endif
