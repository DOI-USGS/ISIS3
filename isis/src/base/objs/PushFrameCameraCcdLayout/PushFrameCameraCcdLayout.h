#ifndef PushFrameCameraCcdLayout_h
#define PushFrameCameraCcdLayout_h
/**
 * @file
 * $Revision: 1.6 $
 * $Date: 2009/10/21 18:37:02 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

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
       */
      struct FrameletInfo {
        FrameletInfo() : m_frameId(-1), m_filterName(),
                         m_startSample(0), m_startLine(0),
                         m_samples(0), m_lines(0) { }
        FrameletInfo(const int frameid) : m_frameId(frameid), m_filterName(),
                                          m_startSample(0), m_startLine(0),
                                          m_samples(0), m_lines(0) { }
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
