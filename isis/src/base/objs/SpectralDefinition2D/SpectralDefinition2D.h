#ifndef SpectralDefinition2D_h
#define SpectralDefinition2D_h

/**
 * @file
 * $Revision: 6129 $
 * $Date: 2015-04-02 10:42:32 -0700 (Thu, 02 Apr 2015) $
 * $Id: CalculatorStrategy.h 6129 2015-04-02 17:42:32Z jwbacker@GS.DOI.NET $
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


#include "FileName.h"
#include "Spectel.h"
#include "SpectralDefinition.h"

template<typename T> class QList;

namespace Isis {

  class Spectel;

  /**
   * @brief A Spectral definition that includes wavelength and
   *        center values for each (line, sample) coordinate.
   *
   * @author 2015-05-21 Kristin Berry
   *
   * @internal
   *  @history 2015-06-09 Stuart Sides - Removed WavelengthsAndWidths storage class and used Spectel
   *  @history 2015-06-12 Stuart Sides - Added getSpectel(spectel) and constness
   *  @history 2015-06-13 Kristin Berry - Added sections (wavelengths are not all contiguous)
   *  @history 2015-06-14 Stuart Sides - Removed global varaiables
   *  @history 2015-08-09 Kristin Berry - Updated to get closer to
   *                        Isis Coding Standards.
   */
  class SpectralDefinition2D : public SpectralDefinition {
    public:
      SpectralDefinition2D(FileName smileDefFilename);
      ~SpectralDefinition2D();

      Spectel findSpectel(const int sample, const int line, const int band) const;
      Spectel findSpectelByWavelength(const double wavelength, const int sectionNumber) const;
      Spectel findSpectel(const Spectel &inSpectel, const int sectionNumber) const;

      virtual int sectionCount() const;
      int sectionNumber(int s, int l, int b) const;

      QString toString();

      //! Internal function used to help read-in a calibration cube
      void operator()(Buffer &in) const;

  private:
      //! Internally represent the samples x 2 lines x n bands calibration file
      //! Outside list is the sample index, inside list is the band
      QList<QList<Spectel> *> *m_spectelList;
      //! The number of sections of this Spectral Definition
      int m_numSections;
      //! The list of sections
      QList<int> *m_sectionList;
  };
}

#endif
