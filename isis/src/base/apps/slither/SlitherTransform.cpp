#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>

#include "Cube.h"
#include "ControlNet.h"
#include "NumericalApproximation.h"
#include "Statistics.h"
#include <gsl/gsl_math.h>

#include "SlitherTransform.h"

using namespace std;
namespace Isis {

  /**
   * @brief Constructor where all the business is taking place
   * 
   * This constructor accepts a cube to be transformed and the control net file
   * generated after matching it to a reference image.  It is assumed that the
   * control net has the reference image identified via the ControlMeasure class.
   * 
   * It computes the interpolations for line and samples from the control net
   * registration data.  This interpolation preserves lines whole, shifting them
   * up and/or down and left or right.
   * 
   * @param cube Input cube to be transformed
   * @param cnet Control net that will be used to compute the spline offsets
   * @param lInterp Type of spline to compute for line offsets
   * @param sInterp Type of spline to compute for sample offsets
   * @see NumericalApproximation
   */
  SlitherTransform::SlitherTransform(Cube &cube, ControlNet &cnet, 
                                   InterpType lInterp, InterpType sInterp) :
                                   _rows(), _badRows(), 
                                   _pntsTotal(0), _pntsUsed(0), _pntsTossed(0), 
                                   _iDir(1.0), 
                                   _outputLines(cube.Lines()),
                                   _outputSamples(cube.Samples()),
                                   _lineOffset(0.0), _sampOffset(0.0) {

    // Collect the points from the control file
    _lineSpline.SetInterpType(lInterp);
    _sampSpline.SetInterpType(sInterp);
    vector<PointData> points;
    for (int i=0; i < cnet.Size(); i++) {
      ControlPoint &cp = cnet[i];
      _pntsTotal++;
      if (!cp.Ignore()) {
        if (cp.Size() != 2) {
//          cout << "Point " << i << " doesn't have two measures but " 
//               << cp.Size() << endl;
          _pntsTossed++;
        }
        else {
          // Determine reference image assuming first one is the reference
          // if it is not expressly identified
          int snIndex(0), mnIndex(1);
          if (!cp[snIndex].IsReference()) {
            snIndex = 1;
            mnIndex = 0;
          }
           //  Add the point set to the list
            PointData p;
            p.refPoint = cp[snIndex];
            p.chpPoint = cp[mnIndex];
            points.push_back(p);
            _pntsUsed++;
        }
      }
    }

    // Points must be sorted and then collapsed into one column
    sort(points.begin(), points.end(), PointLess);
    ControlByRow pts = for_each(points.begin(), points.end(), 
                                ControlByRow(1.0));

    // Now retrieve the collapsed points identifying good ones and bad ones
    _rows.clear();
    _badRows.clear();
    for (unsigned int n = 0 ; n < pts.size() ; n++) {
      RowPoint p = pts[n];
      if (p.count > 0) {
        _rows.push_back(p);
      }
      else {
        _badRows.push_back(p);
      }
    }

    // Add the points to the spline interpolators.  It is important to use
    // the offsets only in this case so the reverse tranform can be provided
    // as well.
    for (unsigned int n = 0 ; n < _rows.size() ; n++) {
      RowPoint rp = _rows[n];
      _lineSpline.AddData(rp.refLine, rp.cLOffset.Average());
      _sampSpline.AddData(rp.refLine, rp.cSOffset.Average());
    }
  }

  /**
   * @brief Convert the requested output samp/line to an input samp/line
   * 
   * Computes the incoming line and sample for the given output line and
   * sample.  These coordinates are determined entirely by the interpolations of
   * the control net coregistrations upon instantiation of this object.
   * 
   * @param inSample  Returns the input sample coordinate for this outSample,
   *                  outLine
   * @param inLine    Returns the input line coordinate for this outSample,
   *                  outLine
   * @param outSample Current sample pixel location requested
   * @param outLine   Current line location requested
   * 
   * @return bool  Always returns true as the value will always be provided
   */
  bool SlitherTransform::Xform (double &inSample, double &inLine,
                                const double outSample, const double outLine) {
    inLine   = getLineXform(outLine);
    inSample = getSampXform(outLine, outSample);
    return (true);
  }

  /**
   * @brief Computes statistics for each line in the output image
   * 
   * This method computes (gathers, really) the statistics as determined via the
   * application of the interpolation from the transform.  For every valid line,
   * sample, it accumulates the line offsets as provided via the tranform.
   * 
   * @return Statistics Provides the statistics class that contains the
   *         statistics information
   * @see Statistics 
   * @history 2008-11-05 Jeannie Walldren - removed const from 
   *          method so that _lineSpline is not const. 
   *  
   */
  Statistics SlitherTransform::LineStats() { 
    Statistics stats;
    for (int line = 0 ; line < _outputLines ; line++) {
      double outLine(line+1), inLine;
      inLine = getLineXform(outLine);
      if ((inLine >= 1.0) && (inLine <= _outputLines)) {
        outLine = getOffset(outLine, _lineSpline);
        stats.AddData(&outLine, 1);
      }
    }
    return (stats);
  }

  /**
   * @brief Computes statistics for samples in the output image
   * 
   * This method computes (gathers, really) the statistics as determined via the
   * application of the interpolation from the transform.  For every valid line,
   * sample, it accumulates the sample offsets as provided via the tranform.

   * 
   * @return Statistics Provides the statistics class that contains the
   *         statistics information
   * @see Statistics 
   * @history 2008-11-05 Jeannie Walldren - removed const from 
   *          method so that _sampSpline is not const. 
   *  
   */
  Statistics SlitherTransform::SampleStats() { 
    Statistics stats;
    for (int line = 0 ; line < _outputLines ; line++) {
      double outLine(line+1), inLine;
      inLine = getLineXform(outLine);
      if ((inLine >= 1.0) && (inLine <= _outputLines)) {
        double outSamp = getOffset(outLine, _sampSpline);
        stats.AddData(&outSamp, 1);
      }
    }
    return (stats);
  }

  /**
   * @brief Provides detailed information and statistics for the current transform
   * 
   * This method produces a large volume of information pertaining to the computed
   * transform.  This information is written to the provided stream and assumes
   * the caller has created a valid stream.
   * 
   * @param out  Output stream to write data to
   * 
   * @return std::ostream&  Returns the output stream
   */
  std::ostream &SlitherTransform::dumpState(std::ostream &out) {

    std::ios::fmtflags oldFlags = out.flags();
    out.setf(std::ios::fixed);

    out << "#  General line, sample statistics\n";
    out << setw(10) << "Axis" 
        << setw(10) << "Spline"
        << setw(12) << "Average"
        << setw(12) << "StdDev"
        << setw(12) << "Minimum"
        << setw(12) << "Maximum"
        << endl;


    Statistics lstats = LineStats();
    double lStd(lstats.StandardDeviation());
    out << setw(10) << "Line"
        << setw(10) << _lineSpline.Name()
        << setw(12) <<  setprecision(4) << lstats.Average()
        << setw(12) <<  setprecision(4) << ((IsSpecial(lStd)) ? 0.0 : lStd)
        << setw(12) <<  setprecision(4) << lstats.Minimum()
        << setw(12) <<  setprecision(4) << lstats.Maximum()
        << endl;

    Statistics sstats = SampleStats();
    double sStd(sstats.StandardDeviation());
    out << setw(10) << "Sample"
        << setw(10) << _sampSpline.Name()
        << setw(12) <<  setprecision(4) << sstats.Average()
        << setw(12) <<  setprecision(4) << ((IsSpecial(sStd)) ? 0.0 : sStd)
        << setw(12) <<  setprecision(4) << sstats.Minimum()
        << setw(12) <<  setprecision(4) << sstats.Maximum()
        << endl;



    int allPoints(numberPointsUsed() + numberBadPoints());
    out << "\n\n" << setw(10) << "BadRows" << setw(10) << numberBadRows() 
        << "  (Rows with no valid points)\n";
    out << setw(10) << "Points" << setw(10) << numberPointsUsed() 
        <<  " of " << allPoints << "  (Points with 2 measures)\n";
    out << setw(10) << "AllPoints" << setw(10) << totalPoints() 
        << " (Including ignored points)\n";


    out << "\n\n#  Statistics of collapsed column registrations for each row\n";
    //  Write headers
    out << setw(10) << "FromLine"
        << setw(10) << "FromSamp"
        << setw(10) << "MatchLine"
        << setw(10) << "MatchSamp" 
        << setw(12) << "LineOffset"
        << setw(12) << "SampOffset"
        << setw(12) << "LineStdDev"
        << setw(12) << "SampStdDev"
        << setw(10) << "RegGOFAvg"
        << setw(10) << "ValidCols"
        << setw(10) << "TotalCols"
        << endl;

    RowList::const_iterator rItr;
    for (rItr = _rows.begin() ; rItr != _rows.end() ; ++rItr) {
      double clStd(rItr->cLOffset.StandardDeviation());
      double csStd(rItr->cSOffset.StandardDeviation());
      out << setw(10) << setprecision(2) << rItr->chpLine
          << setw(10) << setprecision(2) << rItr->chpSamp
          << setw(10) << setprecision(2) << rItr->refLine
          << setw(10) << setprecision(2) << rItr->refSamp
          << setw(12) << setprecision(4) << rItr->cLOffset.Average()
          << setw(12) << setprecision(4) << rItr->cSOffset.Average()
          << setw(12) << setprecision(4) << (IsSpecial(clStd) ? 0.0 : clStd)
          << setw(12) << setprecision(4) << (IsSpecial(csStd) ? 0.0 : csStd)
          << setw(10) << setprecision(4) << rItr->GOFStats.Average()
          << setw(10) << setprecision(0) << rItr->count
          << setw(10) << setprecision(0) << rItr->total
          << endl;
    }

    out << "\n\n#  Map of each output line and sample with relative offsets\n";
    //  Write headers
    out << setw(10) << "InLine" 
        << setw(10) << "InSamp" 
        << setw(10) << "OutLine" 
        << setw(10) << "OutSamp" 
        << setw(12) << "LineOffset"
        << setw(12) << "SampOffset"
        << endl;

    double outSamp(OutputSamples()/2);
    for (int line = 1 ; line <= OutputLines() ; line++) {
      double outLine(line), inLine, inSamp;
      double sampOffset, lineOffset;
      
      Xform(inSamp, inLine, outSamp, outLine);
      sampOffset = getOffset(outLine, _sampSpline);
      lineOffset = getOffset(outLine, _lineSpline);

      out << setw(10) << setprecision(2) << inLine
          << setw(10) << setprecision(2) << inSamp
          << setw(10) << setprecision(2) << outLine
          << setw(10) << setprecision(2) << outSamp
          << setw(12) << setprecision(4) << lineOffset
          << setw(12) << setprecision(4) << sampOffset
          << endl;
    }

    out.setf(oldFlags);
    return (out);
  }
}
