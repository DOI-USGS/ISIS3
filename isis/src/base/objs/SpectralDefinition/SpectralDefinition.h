#ifndef SpectralDefinition_h
#define SpectralDefinition_h

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
