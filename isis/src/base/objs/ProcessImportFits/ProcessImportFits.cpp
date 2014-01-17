/**
 * @file
 * $Revision: 1.4 $
 * $Date: 2009/12/17 21:13:17 $
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

#include "ProcessImportFits.h"

#include <iostream>
#include <QDebug>
#include <QString>
#include <sstream>

#include "Preference.h"
#include "IException.h"
#include "IString.h"
#include "LineManager.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PixelType.h"
#include "SpecialPixel.h"
#include "UserInterface.h"

namespace Isis {

  /**
   * Constructor for ProcessImportFits
   */
  ProcessImportFits::ProcessImportFits() {
    m_fitsLabel = NULL;
  }


  /**
   * Destructor for ProcessImportFits
   */
  ProcessImportFits::~ProcessImportFits() {
    delete m_fitsLabel;
    m_file.close();
  }


  /**
   * Extract the FITS label from the file
   *
   */
  void ProcessImportFits::extractFitsLabel() {

    // The main FITS label starts at the beginning of the file
    // Each FITS keyword in the lable is store in 80 bytes (space padded to 80 if necessary)

    m_fitsLabel = new PvlGroup("FitsLabels");

    // Read in the FITS labels and convert to PVL
    char readBuf[81];
    IString line = "";
    unsigned int place = 0;

    // Read the first line
    m_file.seekg(0);
    m_file.read(readBuf, 80);
    readBuf[80] = '\0';
    line = readBuf;
    place += 80;

    // Process each fits label record (80 bytes) and place keyword, value pairs into PvlKeywords
    while(line.substr(0, 3) != "END") {

      // Check for blank lines
      if (line.substr(0, 1) != " " && line.substr(0, 1) != "/") {
        // Name of keyword
        PvlKeyword label(line.Token(" =").ToQt()); // Stop on spaces OR equal sign
        // Remove up to beginning of data
        line.TrimHead(" =");
        line.TrimTail(" ");
        if (label.name() == "COMMENT" || label.name() == "HISTORY") {
          label += line.ToQt();
        }
        else {
          // Check for a quoted value
          if (line.substr(0,1) == "'") {
            line.TrimHead("'");
            label += line.Token("'").TrimHead(" ").TrimTail(" ").ToQt();
            line.TrimHead(" '");
          }
          else {
            // Access any remaining data without the trailing comment if there is one
            IString value = line.Token("/");
            // Clear to end of data
            value.TrimTail(" ");
            label += value.ToQt();
            line.TrimHead(" ");
          }
          // If the line still has anything in it, treat it is as a comment.
          if (line.size() > 0) {
            line.TrimHead(" /");
            label.addComment(line.ToQt());
            // A possible format for units, other possiblites exist.
            if (line != line.Token("[")) {
              label.setUnits(line.Token("[").Token("]").ToQt());
            }
          }
        }
        m_fitsLabel->addKeyword(label);
      }

      // Read the next line
      m_file.seekg(place);
      m_file.read(readBuf, 80);
      readBuf[80] = '\0';
      place += 80;
      line = readBuf;
    }

    // Save off the number of header records read
    m_headers = (int)((place + 2881) / 2880);

  }


  /**
   * Supplies the FITS label 
   * 
   * @param FITS label after being converted to a PvlGroup
   */
  PvlGroup ProcessImportFits::fitsLabel() const {
    if (!m_fitsLabel) {
      QString msg = QObject::tr("The FITS label has not been initialized, call setFitsFile first");
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    return *m_fitsLabel;
  }


  /**
   * Return a populated instrument group 
   * 
   */
  PvlGroup ProcessImportFits::standardInstrumentGroup() const {

    // NOTE: This needs to be changed over to use translation files

    // Attempt to extract the standard instrument group keywords
    PvlGroup inst("Instrument");
    if (m_fitsLabel->hasKeyword("DATE-OBS")) {
      inst += PvlKeyword("StartTime", (*m_fitsLabel)["DATE-OBS"][0]);
    }
    if (m_fitsLabel->hasKeyword("OBJECT")) {
      inst += PvlKeyword("Target", (*m_fitsLabel)["OBJECT"][0]);
    }
    if (m_fitsLabel->hasKeyword("INSTRUME")) {
      inst += PvlKeyword("InstrumentId", (*m_fitsLabel)["INSTRUME"][0]);
    }
    if (m_fitsLabel->hasKeyword("OBSERVER")) {
      inst += PvlKeyword("SpacecraftName", (*m_fitsLabel)["OBSERVER"][0]);
    }

    return inst;
  }


  /**
   * Opens a FITS file and reads the FITS labels 
   * 
   * @param fitsFile Name of the FITS file to open
   */
  void ProcessImportFits::setFitsFile(FileName fitsFile) {
    SetInputFile(fitsFile.toString()); // Make sure the file exists

    m_file.open(fitsFile.expanded().toLocal8Bit().constData(), std::ios::in  | std::ios::binary);

    if (!m_file.is_open()) {
      QString msg = QObject::tr("Unable to open FITS formatted file [%1]").arg(fitsFile.toString());
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // Get the FITS labels internalized
    extractFitsLabel();

    // Check to make sure it is a FITS file we can handle
    PvlGroup label = fitsLabel();
    if (label["SIMPLE"][0] == "F") {
      QString msg = QObject::tr("The file [%1] can not be processed. It is an unsupported format.").
          arg(fitsFile.toString());
      throw IException(IException::User, msg, _FILEINFO_);
    }

    setProcessFileStructure(label);

    m_file.close();
  }


  /**
   * Sets all the Process file structure parameters. Mostly based on the FITS labels 
   * 
   */
  void ProcessImportFits::setProcessFileStructure(PvlGroup label) {

    SetFileHeaderBytes(m_headers * 2880);
    SaveFileHeader();

    // Find pixel type, there are several unsupported possiblites
    Isis::PixelType type;
    QString msg = "";
    switch (toInt(label["BITPIX"][0])) {
      case 8:
        type = Isis::UnsignedByte;
        break;
      case 16:
        type = Isis::SignedWord;
        break;
      case 32:
        msg = "Signed 32 bit integer (int) pixel type is not supported at this time";
        throw IException(IException::User, msg, _FILEINFO_);
        break;
      case 64:
        msg = "Signed 64 bit integer (long) pixel type is not supported at this time";
        throw IException(IException::User, msg, _FILEINFO_);
        break;
      case -32:
        type = Isis::Real;
        break;
      case -64:
        msg = "64 bit floating point (double) pixel type is not supported at this time";
        throw IException(IException::User, msg, _FILEINFO_);
        break;
      default:
        msg = "Unknown pixel type [" + label["BITPIX"][0] + "] is not supported for imported";
        throw IException(IException::User, msg, _FILEINFO_);
        break;
    }

    SetPixelType(type);

    // It is possible to have a NAXIS value of 0 meaning no data, the file could include
    // xtensions with data, however, those aren't supported yet
    if (toInt(label["NAXIS"][0]) == 2) {
      SetDimensions(toInt(label["NAXIS1"][0]), toInt(label["NAXIS2"][0]), 1);
    }
    else if (toInt(label["NAXIS"][0]) == 3) {
      SetDimensions(toInt(label["NAXIS1"][0]), toInt(label["NAXIS2"][0]),
                    toInt(label["NAXIS3"][0]));
    }
    else {
      QString msg = "NAXIS count of [" + label["NAXIS"][0] + "] is not supported at this time";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // Base and multiplier
    if (label.hasKeyword("BZERO")) {
      SetBase(toDouble(label["BZERO"][0]));
    }
    if (label.hasKeyword("BSCALE")) {
      SetMultiplier(toDouble(label["BSCALE"][0]));
    }

    // Byte order
    SetByteOrder(Isis::Msb);

  }






} // end namespace Isis










