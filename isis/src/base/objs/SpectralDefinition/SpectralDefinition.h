#ifndef SpectralDefinition_h
#define SpectralDefinition_h

/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <vector>

#include "Buffer.h"
#include "Cube.h"
#include "Spectel.h"

namespace Isis{
  class WavelengthsAndWidths;
  class Spectel;

  /**
  * @brief contains calibration info for spectral smile 
  *        correction (center wavelengths and widths for the whole nxn chip)
  *  
  * Right now doens't do any calculations; just organizes 
  * contents of input and output smile definitions. 
  *  
  * @author 2015-05-11 Kristin Berry  
  *  
  * @internal
  *  @history 2015-06-09 Stuart Sides - Made pure virtual and added constness
  *  @history 2015-08-09 Kristin Berry - Moved implemention of
  *                        getters to cpp.
  */
  class SpectralDefinition {

    public:
      SpectralDefinition(); 
      virtual ~SpectralDefinition();

      virtual Spectel findSpectel(const int sample, const int line, const int band) const = 0; 
      virtual Spectel findSpectel(const Spectel &inSpectel, int sectionNumber) const = 0;
      virtual Spectel findSpectelByWavelength(const double wavlength, int sectionNumber) const = 0; 

      virtual int sampleCount() const;
      virtual int lineCount() const;
      virtual int bandCount() const;
      virtual int sectionCount() const;

      virtual int sectionNumber(int s, int l, int b) const = 0;       

    protected:

      //!Number of samples in input Cube.
      int m_ns;

      //! Number of lines in input Cube. 
      int m_nl;

      //! Number of bands in input Cube. 
      int m_nb; 

      //! Number of sections of the chip/wavelength data
      int m_numSections; 

    private:
      void init();

  };
} 

#endif
