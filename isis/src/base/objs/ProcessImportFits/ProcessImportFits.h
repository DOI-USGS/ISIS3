#ifndef ProcessImportFits_h
#define ProcessImportFits_h
/**
 * @file
 * $Revision: 
 * $ $Date:  
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


#include "ProcessImport.h"

template <typename T> class QList;

namespace Isis {
  /**
   * @brief Import a FITS file
   *
   * This class allows a programmer to develop application programs which import Fits cubes and 
   * mangles the the FITS label into appropriate ISIS labels. The entire FITS label is converted 
   * to an ISIS PVL, allowing the programmer to interagate it with existing ISIS tools. 
   *
   * @ingroup HighLevelCubeIO
   *
   * @author 2013-10-08 Stuart Sides
   *
   * @internal
   *   @history 2014-06-06 Stuart Sides - Added ability to read and process the FITS extension
   *   @history 2015-01-15 Sasha Brownsberger - Made destructor virtual.  References #2215.
   *   @history 2015-06-02 Kristin Berry - Added error for use of
   *                                  unsupported BIP
   *                                  organization.
   *             
   */

  class ProcessImportFits : public ProcessImport {

    public:
      ProcessImportFits();
      virtual ~ProcessImportFits();

      PvlGroup standardInstrumentGroup(PvlGroup fitsLabel) const;
      PvlGroup fitsLabel(int labelNumber) const;
      void setFitsFile(FileName fitsFile);
      void setProcessFileStructure(int labelNumber);

    private:
      void extractFitsLabels();

      //! Holds the PvlGroups with the converted FITS labels from the main and all extensions
      QList<PvlGroup *> *m_fitsLabels;

      //! The name of the input FITS file
      FileName m_name;

      //! The stream used to read the FITS file
      std::ifstream m_file;

      //! The number/count of 2880 byte header records for each header section
      QList<int> *m_headerSizes;

      //! The starting byte of the data for each image
      QList<int> *m_dataStarts;
  };
};

#endif


