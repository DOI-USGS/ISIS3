#ifndef HiCalData_h
#define HiCalData_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <string>
#include <vector>

#include "IString.h"
#include "HiCalTypes.h"
#include "HiCalUtil.h"
#include "HiBlob.h"
#include "Cube.h"

#include "IException.h"

namespace Isis {

  /**
   * @brief Container for HiRISE calibration data
   *
   * @ingroup Utility
   *
   * @author 2007-10-09 Kris Becker
   *
   * @internal
   *  @history 2008-05-12 Steven Lambright - Removed references to CubeInfo
   */
  class HiCalData {

    public:
      //  Constructors and Destructor
      HiCalData() : _calimage(), _calbuffer(), _caldark(), _buffer(),
        _dark(), _binning(0), _tdi(0), _cpmm(0), _channelNo(0),
        _firstReverseLine(0), _lastReverseLine(0),
        _firstMaskLine(0), _lastMaskLine(0),
        _firstRampLine(0), _lastRampLine(0) { }
      HiCalData(Cube &cube) {
        load(cube);
      }

      /** Destructor */
      virtual ~HiCalData() { }

      void load(Cube &cube) {
        Progress progress;
        progress.SetText("HiCalData");

        _calimage  = HiBlob(cube, "HiRISE Calibration Image", "Calibration").buffer();
        _calbuffer = HiBlob(cube, "HiRISE Calibration Ancillary", "BufferPixels").buffer();
        _caldark   = HiBlob(cube, "HiRISE Calibration Ancillary", "DarkPixels").buffer();
        _buffer    = HiBlob(cube, "HiRISE Ancillary", "BufferPixels").buffer();
        _dark      = HiBlob(cube, "HiRISE Ancillary", "DarkPixels").buffer();

        PvlGroup &instrument = cube.group("Instrument");

// Extract what is needed
        _binning = instrument["Summing"];
        _tdi = instrument["Tdi"];
        _cpmm = instrument["CpmmNumber"];
        _channelNo = instrument["ChannelNumber"];

//  Determine start/end lines
        _firstReverseLine = 0;
        _lastReverseLine = 19;

// Set the mask depending on the binning mode
        _firstMaskLine = 20;
        _lastMaskLine = _firstMaskLine + 20 / _binning - 1;
        _firstRampLine = _lastMaskLine + 1;
        _lastRampLine = _calimage.dim1() - 1;
      }


      HiMatrix getReverseClock() {
        return (cropLines(_calimage, _firstReverseLine, _lastReverseLine));
      }
      HiMatrix getMask() {
        return (cropLines(_calimage, _firstMaskLine, _lastMaskLine));
      }
      HiMatrix getRamp() {
        return (cropLines(_calimage, _firstRampLine, _lastRampLine));
      }

      HiMatrix getDark()   {
        return (_dark.copy());
      }
      HiMatrix getBuffer() {
        return (_buffer.copy());
      }

      HiMatrix getReverseClockExtended() {
        return (
                 appendSamples(
                   appendSamples(
                     cropLines(_calbuffer, _firstReverseLine, _lastReverseLine),
                     cropLines(_calimage, _firstReverseLine, _lastReverseLine)),
                   cropLines(_caldark, _firstReverseLine, _lastReverseLine)
                 )
               );
      }
      HiMatrix getMaskExtended() {
        return (
                 appendSamples(
                   appendSamples(cropLines(_calbuffer, _firstMaskLine, _lastMaskLine),
                                 cropLines(_calimage, _firstMaskLine, _lastMaskLine)),
                   cropLines(_caldark, _firstMaskLine, _lastMaskLine)
                 )
               );
      }
      HiMatrix getRampExtended() {
        return (
                 appendSamples(
                   appendSamples(cropLines(_calbuffer, _firstRampLine, _lastRampLine),
                                 cropLines(_calimage, _firstRampLine, _lastRampLine)),
                   cropLines(_caldark, _firstRampLine, _lastRampLine)
                 )
               );

      }
      HiMatrix getDarkExtended() {
        return (appendLines(_caldark, _dark));
      }
      HiMatrix getBufferExtended() {
        return (appendLines(_calbuffer, _buffer));
      }

    private:

      HiMatrix _calimage;
      HiMatrix _calbuffer;
      HiMatrix _caldark;
      HiMatrix _buffer;
      HiMatrix _dark;

      int _binning;
      int _tdi;
      int _cpmm;
      int _channelNo;

      int _firstReverseLine;
      int _lastReverseLine;
      int _firstMaskLine;
      int _lastMaskLine;
      int _firstRampLine;
      int _lastRampLine;
  };

}     // namespace Isis
#endif
