#ifndef ProcessImportFits_h
#define ProcessImportFits_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */


#include "ProcessImport.h"

template <typename T> class QList;

namespace Isis {
  /**
   * @brief Import a FITS file.
   *
   * This class allows a programmer to develop application programs which import FITS cubes and 
   * mangles the FITS label into appropriate ISIS labels. The entire FITS label is converted 
   * to an ISIS PVL, allowing the programmer to interrogate it with existing ISIS tools. 
   *
   * @ingroup HighLevelCubeIO
   *
   * @author 2013-10-08 Stuart Sides
   *
   * @internal
   *   @history 2014-06-06 Stuart Sides - Added ability to read and process the FITS extension
   *   @history 2015-01-15 Sasha Brownsberger - Made destructor virtual.  References #2215.
   *   @history 2015-06-02 Kristin Berry - Added error for use of unsupported BIP organization.
   *   @history 2016-04-20 Jeannie Backer - Merged Janet Barret's changes to handle SignedInteger
   *                           imports. Brought code closer to coding standards.
   *   @history 2017-07-07 Jeannie Backer and Kaj Williams - Modified extractFitsLabels() to
   *                           handle labels with extra image information but no image data. Changed
   *                           fitsLabel() method name to fitsImageLabel(). Added extraFitsLabel().
   *                           Brought code closer to coding standards. Improved documentation
   *                           and error messages.
   */

  class ProcessImportFits : public ProcessImport {

    public:
      ProcessImportFits();
      virtual ~ProcessImportFits();

      PvlGroup standardInstrumentGroup(PvlGroup fitsLabel) const;
      PvlGroup extraFitsLabel(int labelNumber) const;
      PvlGroup fitsImageLabel(int labelNumber) const;
      void setFitsFile(FileName fitsFile);
      void setProcessFileStructure(int labelNumber);

    private:
      void extractFitsLabels();

      QList<PvlGroup *> *m_fitsImageLabels; /**< Holds the PvlGroups with the converted FITS image 
                                                 labels from the main and all extensions.*/
      QList<PvlGroup *> *m_extraFitsLabels; /**< Holds the PvlGroups with the converted extra FITS
                                                 labels  from the main and all extensions. This 
                                                 included label that contain BITPIX and NAXIS 
                                                 keyword but are not followed by image data.*/
      FileName m_name;                      /**< The name of the input FITS file.*/
      std::ifstream m_file;                 /**< The stream used to read the FITS file.*/
      QList<int> *m_headerSizes;            /**< The number, or count, of 2880 byte header records 
                                                 for each image header section.*/
      QList<int> *m_dataStarts;             /**< The starting byte of the data for each image.*/
  };
};

#endif


