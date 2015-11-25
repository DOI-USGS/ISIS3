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
#include <math.h>

#include <QDebug>
#include <QString>
#include <sstream>

#include "IException.h"
#include "IString.h"
#include "LineManager.h"
#include "Preference.h"
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
    m_fitsLabels = NULL;
    m_headerSizes = NULL;
    m_dataStarts = NULL;
  }


  /**
   * Destructor for ProcessImportFits
   */
  ProcessImportFits::~ProcessImportFits() {
    delete m_fitsLabels;
    delete m_headerSizes;
    delete m_dataStarts;
    m_file.close();
  }


  /**
   * Extract all the FITS labels from the file. This includes the labels for the main and each 
   * extensions 
   *
   */
  void ProcessImportFits::extractFitsLabels() {

    m_fitsLabels = new QList< PvlGroup * >;
    m_headerSizes = new QList < int >;
    m_dataStarts = new QList < int >;

    // Process each FITS label area. Storing each in its own PvlGroup
    char readBuf[81];
    IString line = "";
    unsigned int place;

    // The main FITS label starts at the beginning of the file
    // FITS extension labels start after the previous data and on a 2080 byte boundry 
    // Each FITS keyword in all lables is store in 80 bytes (space padded to 80 if necessary)

    // Start at the beginning of the file for the main FITS label
    m_file.seekg(0, std::ios_base::beg);

    // Read the first label line (80 chars)
    // We are assuming the file pointer is set to the beginning of the first/next label
    while (m_file.read(readBuf, 80) && m_file.gcount() == 80) {

      PvlGroup *fitsLabel = new PvlGroup("FitsLabels");

      readBuf[80] = '\0';
      line = readBuf;
      place = 80;

      // Process each fits label record (80 bytes) and place keyword, value pairs into PvlKeywords 
      // with any associated comments
      while (line.substr(0, 3) != "END") {

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
          fitsLabel->addKeyword(label);
        }

        // Read the next label line
        m_file.read(readBuf, 80);
        readBuf[80] = '\0';
        line = readBuf;
        place += 80;
      }

      // Save off the PvlGroup and the number of records read from this label
      m_fitsLabels->push_back(fitsLabel);
      m_headerSizes->push_back((int)ceil(place / 2880.0));

      // The file pointer should be pointing at the end of the record that contained "END"
      // Move the file pointer past the padding after the "END" (i.e., points to start of data)
      std::streamoff jump;
      jump = m_headerSizes->last() * 2880 - place;
      m_file.seekg(jump, std::ios_base::cur);

      m_dataStarts->push_back(m_file.tellg());

      // NOTE: For now we only handle image data (i.e., keywords BITPIX & NAXIS & NAXISx must exist)
      // Does this look like a label for a FITS image? Stop after the first label that does not
      // because we don't know how to move the file pointer past a non-image data extension.
      if (fitsLabel->hasKeyword("BITPIX") && fitsLabel->hasKeyword("NAXIS") && 
          fitsLabel->hasKeyword("NAXIS1")) {

        int bytesPerPixel = 0;
        bytesPerPixel = (int)((*fitsLabel)["BITPIX"]);
        bytesPerPixel = fabs(bytesPerPixel);
        bytesPerPixel /= 8;

        unsigned int axis1 = 1;
        axis1 = toInt((*fitsLabel)["NAXIS1"]);

        unsigned int axis2 = 1;
        if (fitsLabel->hasKeyword("NAXIS2")) {
          axis2 = toInt((*fitsLabel)["NAXIS2"]);
        }

        unsigned int axis3 = 1;
        if (fitsLabel->hasKeyword("NAXIS3")) {
          axis3 = toInt((*fitsLabel)["NAXIS3"]);
        }

        jump = (int)(ceil(bytesPerPixel * axis1 * axis2 * axis3 / 2880.0) * 2880.0);
        m_file.seekg(jump, std::ios_base::cur);
      }
      // Do we have at least on header that looks like it has image data? If so, we can continue,
      // but ignore the rest of the file because we don't know how to skip over a non-image data.
      else if (m_fitsLabels->size() > 1) {
        m_fitsLabels->pop_back();
        m_headerSizes->pop_back();
        m_dataStarts->pop_back();
        break;
      }
      else {
        QString msg = QObject::tr("The FITS file does not contain a section header that looks "
                                  "like it describes an image [%1]").arg(m_name.toString());
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }
  }


  /**
   * Supplies the requested FITS label 
   * 
   * @param labelNumber FITS label number. zero (0) is the first/main label
   * @return PvlGroup version of a FITS label corrisponding to requested label number
   */
  PvlGroup ProcessImportFits::fitsLabel(int labelNumber) const {

    if (labelNumber >= m_fitsLabels->size()) {
      QString msg = QObject::tr("The requested label number [%1], from file [%2] is "
                                "past the last image in this FITS file. Image count is [%3]").arg(labelNumber).
                                arg(m_name.expanded()).arg(m_fitsLabels->size()-1);
      throw IException(IException::User, msg, _FILEINFO_);
    }

    if (!m_fitsLabels) {
      QString msg = QObject::tr("The FITS label has not been initialized, call setFitsFile first");
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
    else if (m_fitsLabels->size() < labelNumber) {
      QString msg = QObject::tr("The requested FITS label number does not exist from file [%1]").arg(m_name.toString());
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    return *(*m_fitsLabels)[labelNumber];
  }


  /**
   * Return a populated instrument group 
   *  
   * @param a FITS label after being converted to a PvlGroup
   * @return an instrument group filled with keywords from the FITS label
   */
  PvlGroup ProcessImportFits::standardInstrumentGroup(PvlGroup fitsLabel) const {

    // NOTE: This needs to be changed over to use translation files

    // Attempt to extract the standard instrument group keywords
    PvlGroup inst("Instrument");
    if (fitsLabel.hasKeyword("DATE-OBS")) {
      inst += PvlKeyword("StartTime", fitsLabel["DATE-OBS"][0]);
    }
    if (fitsLabel.hasKeyword("OBJECT")) {
      inst += PvlKeyword("Target", fitsLabel["OBJECT"][0]);
    }
    if (fitsLabel.hasKeyword("INSTRUME")) {
      inst += PvlKeyword("InstrumentId", fitsLabel["INSTRUME"][0]);
    }
    if (fitsLabel.hasKeyword("OBSERVER")) {
      inst += PvlKeyword("SpacecraftName", fitsLabel["OBSERVER"][0]);
    }

    return inst;
  }


  /**
   * Opens a FITS file and reads the FITS labels 
   * 
   * @param fitsFile Name of the FITS file to open
   */
  void ProcessImportFits::setFitsFile(FileName fitsFile) {
    m_name = fitsFile;

    SetInputFile(fitsFile.toString()); // Make sure the file exists

    m_file.open(fitsFile.expanded().toLocal8Bit().constData(), std::ios::in  | std::ios::binary);

    if (!m_file.is_open()) {
      QString msg = QObject::tr("Unable to open FITS formatted file [%1]").arg(fitsFile.toString());
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // Get the FITS labels internalized
    extractFitsLabels();

    // Check to make sure it is a FITS file we can handle
    PvlGroup label = fitsLabel(0);
    if (label["SIMPLE"][0] == "F") {
      QString msg = QObject::tr("The file [%1] can not be processed. It is an unsupported format.").
          arg(fitsFile.toString());
      throw IException(IException::User, msg, _FILEINFO_);
    }

    m_file.close();
  }


  /**
   * Sets the Process file structure parameters based on the FITS labels of choice. NOTE: The 
   * (DataPrefixBytes + DataSuffixByte) / PixelSize is subtracted from the number of samples before 
   * the output file is created. 
   *  
   * @param labelNumber FITS label number. zero (0) is the first/main label. one (1) is the first 
   * extension, ... 
   * 
   */
  void ProcessImportFits::setProcessFileStructure(int labelNumber) {

    if (labelNumber >= m_fitsLabels->size()) {
      QString msg = QObject::tr("The requested label number [%1], from file [%2] is "
                                "past the last image in this FITS file [%3]").arg(labelNumber).
                                arg(InputFile()).arg(m_fitsLabels->size()-1);
      throw IException(IException::User, msg, _FILEINFO_);
    }

    PvlGroup label = *(*m_fitsLabels)[labelNumber];

    // Set the ProcessImport to skip over all the previous images and their labels and the label for
    // this image. Don't save this info (think memory)
    SetFileHeaderBytes((*m_dataStarts)[labelNumber]);
    //SaveFileHeader();

    // Find pixel type. NOTE: There are several unsupported possiblites
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
      case -32:
        type = Isis::Real;
        break;
      case 64:
        msg = "Signed 64 bit integer (long) pixel type is not supported at this time";
        throw IException(IException::User, msg, _FILEINFO_);
        break;
      case -64:
        type = Isis::Double;
        break;
      default:
        msg = "Unknown pixel type [" + label["BITPIX"][0] + "] is not supported for imported";
        throw IException(IException::User, msg, _FILEINFO_);
        break;
    }

    SetPixelType(type);

    // It is possible to have a NAXIS value of 0 meaning no data, the file could include
    // xtensions with data, however, those aren't supported because we need the code to know
    // how to skip over them.
    // NOTE: FITS files, at least the ones seen till now, do not specify a line prefix or suffix
    // data byte count. Some FITS files do have them and ISIS needs to remove them so it is not
    // considered part of the DNs. So, use the parent class' prefix/suffix byte count to reduce
    // the number of samples.
    if (Organization() == BSQ) {
      if (toInt(label["NAXIS"][0]) == 2) {
        SetDimensions(toInt(label["NAXIS1"][0]) - (DataPrefixBytes()+DataSuffixBytes())/SizeOf(type),
                      toInt(label["NAXIS2"][0]), 1);
      }
      else if (toInt(label["NAXIS"][0]) == 3) {
        SetDimensions(toInt(label["NAXIS1"][0]) - (DataPrefixBytes()+DataSuffixBytes())/SizeOf(type),
                      toInt(label["NAXIS2"][0]), toInt(label["NAXIS3"][0]));
      }
      else {
        QString msg = "NAXIS count of [" + label["NAXIS"][0] + "] is not supported at this time";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }
    else if (Organization() == BIL) {
      if (toInt(label["NAXIS"][0]) == 2) {
        SetDimensions(toInt(label["NAXIS1"][0]) - (DataPrefixBytes()+DataSuffixBytes())/SizeOf(type),
                      1, toInt(label["NAXIS2"][0]));
      }
      else if (toInt(label["NAXIS"][0]) == 3) {
        SetDimensions(toInt(label["NAXIS1"][0]) - (DataPrefixBytes()+DataSuffixBytes())/SizeOf(type),
                      toInt(label["NAXIS3"][0]), toInt(label["NAXIS2"][0]));
      }
      else {
        QString msg = "NAXIS count of [" + label["NAXIS"][0] + "] is not supported at this time";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }
    else if (Organization() == BIP) {
      QString msg = "BIP (Band interleaved by pixel) organization is not supported at this time.";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    else {
      QString msg = "Unknown organization is not supported.";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // Base and multiplier
    if (label.hasKeyword("BZERO")) {
      SetBase(toDouble(label["BZERO"][0]));
    }
    else {
      SetBase(0.0);
    }
    if (label.hasKeyword("BSCALE")) {
      SetMultiplier(toDouble(label["BSCALE"][0]));
    }
    else {
      SetMultiplier(1.0);
    }

    // Byte order
    SetByteOrder(Isis::Msb);

  }

} // end namespace Isis

