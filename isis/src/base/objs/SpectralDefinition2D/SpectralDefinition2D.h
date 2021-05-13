#ifndef SpectralDefinition2D_h
#define SpectralDefinition2D_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */


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
