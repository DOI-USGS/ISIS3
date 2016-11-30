#ifndef SpectralDefinition1D_h
#define SpectralDefinition1D_h

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

#include <vector>

#include "SpectralDefinition.h"
#include "Spectel.h"
#include "FileName.h"

template<typename T> class QList;

namespace Isis {

  /**
   * @brief A Spectral definition that includes wavelength and
   *        center values for each (line, sample) coordinate.
   *
   * @author 2015-05-21 Kristin Berry
   *
   * @internal
   *  @history 2015-06-09 Stuart Sides - Added getSpectel(spectel) and constness
   *  @history 2015-08-09 Kristin Berry - Added documentation,
   *                        error-checking, updated code to follow
   *                        standards, made destructor virtual.
   *  @history 2016-08-28 Kelvin Rodriguez - Changed definition of sectionCount
   *                        to match parent's function to avoid hidden parent virtual function
   *                        warning. Part of porting to OS X 10.11.  
   *
   */
  class SpectralDefinition1D : public SpectralDefinition {
    public:
      SpectralDefinition1D(FileName smileDefFilename);
      SpectralDefinition1D();
      virtual ~SpectralDefinition1D();

      Spectel findSpectel(const int sample, const int line, const int band) const;
      Spectel findSpectel(const Spectel &inSpectel, const int sectionNumber) const;
      Spectel findSpectelByWavelength(double wavelength, int sectionNumber) const;

      virtual int sectionCount() const;
      int sectionNumber(int s, int l, int b) const;

      QString toString();

  private:
      // the outer list is the section #, inside is the band
      //! Stores each center wavelength and width
      QList< QList<Spectel> *> *m_spectelList;
      //! The number of different sections of the Spectral Definition.
      int m_numSections;
      //! Do the wavelengths in a given section ascend? Used to determine sections.
      bool m_ascendingWavelengths;
  };
}

#endif
