#ifndef map2map_h
#define map2map_h

#include "Transform.h"

/**
 * @author ????-??-?? Unknown
 *
 * @internal
 *   @history 2012-12-06 Debbie A. Cook - Changed to use TProjection instead of Projection.
 *                          References #775.
 */
class map2map : public Isis::Transform {
  private:
    Isis::TProjection *p_inmap;
    Isis::TProjection *p_outmap;
    int p_inputSamples;
    int p_inputLines;
    bool p_trim;
    int p_outputSamples;
    int p_outputLines;
    int p_inputWorldSize;

  public:
    // constructor
    map2map(const int inputSamples, const int inputLines, Isis::TProjection *inmap,
            const int outputSamples, const int outputLines, Isis::TProjection *outmap,
            bool trim);

    // destructor
    ~map2map() {};

    // Implementations for parent's pure virtual members
    bool Xform(double &inSample, double &inLine,
               const double outSample, const double outLine);
    int OutputSamples() const;
    int OutputLines() const;
};

#endif
