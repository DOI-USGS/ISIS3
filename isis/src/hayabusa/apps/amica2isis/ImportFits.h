#ifndef ImportFits_h
#define ImportFits_h
/**
 * @file
 * $Revision$
 * $Date$
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include <QString>

#include <fstream>
#include <vector>
#include <string>

#include "FileName.h"
#include "PvlGroup.h"

namespace Isis {

  /**
   * @brief Import a FITS file with a label description
   *
   * This class interrogates a \b simple file formatted with the Flexible Image
   * Transport System (FITS) file and provides tools to converts it to an ISIS 
   * image cube. 
   * 
   * Example of interrogation of a FITS file in an ImportFits object. 
   * <code>
   *   ImportFits fits(fitsfile, fitsLabelName);
   *   Pvl label;
   *   label.addGroup(fits.label());
   * </code>
   * 
   * @ingroup Utility
   *
   * @author 2013-11-07 Kris Becker
   *
   * @internal 
   */
  class ImportFits {
    public:
      ImportFits();
      ImportFits(const FileName &fitsfile, 
                 const QString &fitsLabelName="FitsLabel");
      ~ImportFits();
  
      int samples() const;
      int lines() const;
      int bands() const;

      PvlGroup label() const;
  
      void load(const QString &fitsfile, 
                const QString &fitsLabelName = "FitsLabel");
  
    private:
      void init();
      PvlGroup parseLabel(std::ifstream &in, const QString &fitLabelName);

      // Private instance variables
      FileName  m_file;       //!< FITS file name
      int       m_lines;      //!< Number lines in image
      int       m_samples;    //!< Number samples in image
      int       m_bands;      //!< Number bands in image
      PvlGroup  m_label;      //!< FITS label converted to ISIS format

  };

}
#endif


