#ifndef PushFrameCameraCcdLayout_h
#define PushFrameCameraCcdLayout_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QString>
#include "Kernels.h"


namespace Isis {
  /** 
   * Provide image coordinates that map to the push frame detector.
   *
   * @ingroup Camera
   *
   * @see Camera
   *
   * @author 2017-08-11 Kris Becker
   *
   * @internal
   *   @history 2017-08-11 Kris Becker - Original Version
   *   @history 2017-08-21 Jesse Mapel - Improved documentation.
   */
  class PushFrameCameraCcdLayout {
    public:
      /** 
       * Container for the layout of a specific framelet on the detector.
       *
       * @ingroup Camera
       *
       * @see Camera
       *
       * @author 2017-08-11 Kris Becker
       *
       * @internal
       *   @history 2017-08-11 Kris Becker - Original Version
       *   @history 2017-08-21 Jesse Mapel - Added documentation
       *   @history 2017-09-15 Jesse Mapel - Added constructor from field values
       */
      struct FrameletInfo {
        FrameletInfo() : m_frameId(-1), m_filterName(),
                         m_startSample(0), m_startLine(0),
                         m_samples(0), m_lines(0) { }
        FrameletInfo(const int frameid) : m_frameId(frameid), m_filterName(),
                                          m_startSample(0), m_startLine(0),
                                          m_samples(0), m_lines(0) { }
        FrameletInfo(const int frameid, QString filterName, int startSample, 
                     int startLine, int samples, int lines) : 
                     m_frameId(frameid), m_filterName(filterName), m_startSample(startSample), 
                     m_startLine(startLine), m_samples(samples), m_lines(lines) { }
        int     m_frameId; //!< The NAIF ID of the framelet.
        QString m_filterName; //!< The name of the framelet.
        int     m_startSample; //!< The first sample of the framelet on the detector.
        int     m_startLine; //!< The first line of the framelet on the detector.
        int     m_samples; //!< The number of samples in the framelet.
        int     m_lines; //!< The number of lines in the framelet.
      };

    public:
      PushFrameCameraCcdLayout( );
      PushFrameCameraCcdLayout( const int ccdId );

      virtual ~PushFrameCameraCcdLayout();

      bool addKernel(const QString &kernel);

      int ccdSamples() const;
      int ccdLines() const;

      FrameletInfo getFrameInfo(const int &frameId, 
                                const QString &name = "") const;


    private:
      int      m_ccdId;     //!< NAIF ID of the CCD
      Kernels  m_kernels;   //!< NAIF kernel manager

      int      getSpiceInt(const QString &var, const int index = 0) const;
      double   getSpiceDouble(const QString &var, const int index = 0) const;
      QString  getSpiceString(const QString &var, const int index = 0) const;

  };
};
#endif
