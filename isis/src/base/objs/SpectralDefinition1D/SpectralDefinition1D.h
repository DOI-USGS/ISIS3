#ifndef SpectralDefinition1D_h
#define SpectralDefinition1D_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
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
