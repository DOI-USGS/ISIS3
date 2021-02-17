#ifndef trans2d3p_h
#define trans2d3p_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Cube.h"
#include "Transform.h"
#include <math.h>

using namespace Isis;

/**
 * @brief Brief coming soon
 *
 * Description coming soon
 *
 * @author 2011-09-19 Orrin Thomas
 *
 * @internal
 *   @history 2011-09-19 Orrin Thomas - Original version
 */
class Trans2d3p : public Transform {
public:
  Trans2d3p(double theta, double sampOffset, double lineOffset,int samples, int lines) {
    m_lines = lines;
    m_samples = samples;
    m_ct = cos(theta);
    m_st = sin(theta);
    m_lineOffset = lineOffset;
    m_sampOffset = sampOffset;
  }

  ~Trans2d3p() {}

  bool Xform(double &inSample, double &inLine, const double outSample, const double outLine) {
    inSample  = outSample*m_ct - outLine*m_st + m_sampOffset;
    inLine    = outSample*m_st + outLine*m_ct + m_lineOffset;

    return true;
  }

  int OutputSamples() const {
    return m_samples;
  }

  int OutputLines() const {
    return m_lines;
  }

private:
  double m_sampOffset;
  double m_lineOffset;
  double m_ct;
  double m_st;
  int m_lines;
  int m_samples;
};

#endif
