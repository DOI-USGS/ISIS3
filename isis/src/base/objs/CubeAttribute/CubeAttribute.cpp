/**
 * @file
 * $Revision: 1.2 $
 * $Date: 2007/01/30 22:12:22 $
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
#include "CubeAttribute.h"

#include <iostream>

#include <QDebug>

#include "FileName.h"
#include "IException.h"
#include "Preference.h"
#include "SpecialPixel.h"


using namespace std;

namespace Isis {
  //---------------------------------------------------------------------------
  // CubeAttributeInput Implementation
  //---------------------------------------------------------------------------
  CubeAttributeInput::CubeAttributeInput() : CubeAttribute<CubeAttributeInput>(testers()) {
  }


  CubeAttributeInput::CubeAttributeInput(const FileName &fileName) :
      CubeAttribute<CubeAttributeInput>(testers(), fileName) {
  }


  CubeAttributeInput::~CubeAttributeInput() {
  }


  /**
   *
   * Parse the string parameter and populate the private
   * variable accordinly.
   *
   * @param att    A string containing the file attributes. All characters
   *               before the first "+" are assumed to be the filename
   *               and are ignored.
   */
//   void CubeAttributeInput::Set(const FileName &fileName) {
//     Reset();
//
//     QString str(fileName.attributes());
//
//     // Get rid of any white space
//     str.ConvertWhiteSpace();
//     str.Compress();
//     str.Remove(" ");
//     str.UpCase();
//
//     // Look at each comma delimited token
//     QString commaTok;
//     while((commaTok = str.Token(",")).length() > 0) {
//       // Is this token a range of bands
//       if(commaTok.find('-') != string::npos) {
//         QString dashTok;
//         int start = commaTok.Token("-").ToInteger();
//         int end = commaTok.Token("-").ToInteger();
//         int direction;
//         direction = (start <= end) ? 1 : -1;
//         // Save the entire range of bands
//         for(int band = start; band != end; band += direction) {
//           m_bands.push_back(Isis::QString(band));
//         }
//         m_bands.push_back(Isis::QString(end));
//       }
//       // This token is a single band specification
//       else {
//         m_bands.push_back(commaTok);
//       }
//     }
//   }


  vector<QString> CubeAttributeInput::bands() const {
    vector<QString> result;

    QString str = toString().remove(QRegExp("^\\+"));

    QStringList strSplit = str.split(",", QString::SkipEmptyParts);
    foreach (QString commaTok, strSplit) {
      // Is this token a range of bands
      if(commaTok.contains('-')) {
        QString dashTok;
        int start = toInt(commaTok.mid(0, commaTok.indexOf("-")));
        int end =  toInt(commaTok.mid(commaTok.indexOf("-") + 1));
        int direction;
        direction = (start <= end) ? 1 : -1;
        // Save the entire range of bands
        for(int band = start; band != end; band += direction) {
          result.push_back(Isis::toString(band));
        }
        result.push_back(Isis::toString(end));
      }
      // This token is a single band specification
      else {
        result.push_back(commaTok);
      }
    }

    return result;
  }


  QString CubeAttributeInput::bandsString() const {
    return toString(bands());
  }


  void CubeAttributeInput::setBands(const vector<QString> &bands) {
    setAttributes(toString(bands));
  }


  bool CubeAttributeInput::isBandRange(QString attribute) const {
    return QRegExp("[0-9,\\-]+").exactMatch(attribute);
  }


  QString CubeAttributeInput::toString(const vector<QString> &bands) {
    QString result;
    for(unsigned int i = 0; i < bands.size(); i++) {
      if(i > 0)
        result += ",";

      result += bands[i];
    }

    return result;
  }


  QList<bool (CubeAttributeInput::*)(QString) const> CubeAttributeInput::testers() {
    QList<bool (CubeAttributeInput::*)(QString) const> result;

    result.append(&CubeAttributeInput::isBandRange);

    return result;
  }


  //---------------------------------------------------------------------------
  // CubeAttributeOutput Implementation
  //---------------------------------------------------------------------------
  CubeAttributeOutput::CubeAttributeOutput() : CubeAttribute<CubeAttributeOutput>(testers()) {
  }


  CubeAttributeOutput::CubeAttributeOutput(const FileName &fileName)
      : CubeAttribute<CubeAttributeOutput>(testers(), fileName) {
  }


  CubeAttributeOutput::~CubeAttributeOutput() {
  }


  bool CubeAttributeOutput::propagatePixelType() const {
    bool result = false;

    QStringList pixelTypeAtts = attributeList(&CubeAttributeOutput::isPixelType);

    if (pixelTypeAtts.isEmpty() || pixelTypeAtts.last() == "PROPAGATE")
      result = true;

    return result;
  }


  bool CubeAttributeOutput::propagateMinimumMaximum() const {
    return attributeList(&CubeAttributeOutput::isRange).isEmpty();
  }


//   void CubeAttributeOutput::Set(const FileName &fileName) {
//     Reset();
//
//     Isis::QString str(fileName.attributes());
//
//     // Remove any white space
//     str.ConvertWhiteSpace();
//     str.Compress();
//     str.Remove(" ");
//     str.UpCase();
//     str.TrimHead("+");
//
//     // Look at each "+" separate attribute
//     Isis::QString tok;
//     while((tok = str.Token("+")).length() > 0) {
//
//       // If there is a ":" in this token then it is assumed to be a min:max
//       if(tok.find(":") != string::npos) {
//
//         // Pull out the minimum
//         Isis::QString colonTok = tok;
//         Isis::QString min = colonTok.Token(":");
//         if(min.length() > 0) {
//           m_minimum = min.ToDouble();
//         }
//         else {
//           m_minimum = 0.0;
//         }
//
//         // Pull out the maximum
//         Isis::QString max = colonTok.Token(":");
//         if(max.length() > 0) {
//           m_maximum = max.ToDouble();
//         }
//         else {
//           m_maximum = 0.0;
//         }
//         m_rangeType = RangeSet;
//       }
//
//       // Parse any pixel type attributes
//       else if(tok == "8BIT" || tok == "8-BIT" || tok == "UNSIGNEDBYTE") {
//         m_pixelType = Isis::UnsignedByte;
//         m_pixelTypeDef = "SET";
//       }
//       else if(tok == "16BIT" || tok == "16-BIT" || tok == "SIGNEDWORD") {
//         m_pixelType = Isis::SignedWord;
//         m_pixelTypeDef = "SET";
//       }
//       else if(tok == "32BIT" || tok == "32-BIT" || tok == "REAL") {
//         m_pixelType = Isis::Real;
//         m_pixelTypeDef = "SET";
//       }
//       else if(tok == "PROPAGATE") {
//         m_pixelType = Isis::None;
//         m_pixelTypeDef = "PROPAGATE";
//       }
//
//       // Parse any file formats
//       else if(tok == "TILE") {
//         m_format = Cube::Tile;
//       }
//       else if(tok == "BSQ" || tok == "BANDSEQUENTIAL") {
//         m_format = Cube::Bsq;
//       }
//
//       // Parse any byte order
//       else if(tok == "LSB") {
//         m_order = Isis::Lsb;
//       }
//       else if(tok == "MSB") {
//         m_order = Isis::Msb;
//       }
//
//       // Parse any label type
//       else if(tok == "ATTACHED") {
//         m_labelAttachment = Isis::AttachedLabel;
//       }
//       else if(tok == "DETACHED") {
//         m_labelAttachment = Isis::DetachedLabel;
//       }
//     }
//   }


  Cube::Format CubeAttributeOutput::fileFormat() const {
    Cube::Format result = Cube::Tile;

    QStringList formatList = attributeList(&CubeAttributeOutput::isFileFormat);

    if (!formatList.isEmpty()) {
      QString formatString = formatList.last();

      if (formatString == "BSQ" || formatString == "BANDSEQUENTIAL")
        result = Cube::Bsq;
    }

    return result;
  }


  QString CubeAttributeOutput::fileFormatString() const {
    return toString(fileFormat());
  }


  void CubeAttributeOutput::setFileFormat(Cube::Format fmt) {
    setAttribute((fmt == Cube::Tile)? "Tile" : "BandSequential",
                 &CubeAttributeOutput::isFileFormat);
  }


  double CubeAttributeOutput::minimum() const {
    double result = Null;

    if (!propagateMinimumMaximum()) {
      QString range = attributeList(&CubeAttributeOutput::isRange).last();

      QStringList rangeList = range.split(":");
      if (rangeList.count() == 2 && rangeList.first() != "")
        result = toDouble(rangeList.first());
    }

    return result;
  }


  double CubeAttributeOutput::maximum() const {
    double result = Null;

    if (!propagateMinimumMaximum()) {
      QString range = attributeList(&CubeAttributeOutput::isRange).last();

      QStringList rangeList = range.split(":");
      if (rangeList.count() == 2 && rangeList.last() != "")
        result = toDouble(rangeList.last());
    }

    return result;
  }


  void CubeAttributeOutput::setMinimum(double min) {
    if (!IsSpecial(min)) {
      QString newRange = Isis::toString(min) + ":";

      if (!IsSpecial(maximum()))
        newRange += Isis::toString(maximum());

      setAttribute(newRange, &CubeAttributeOutput::isRange);
    }
    else if (!IsSpecial(maximum())) {
      setAttribute(":" + Isis::toString(maximum()), &CubeAttributeOutput::isRange);
    }
    else {
      setAttribute("", &CubeAttributeOutput::isRange);
    }
  }


  void CubeAttributeOutput::setMaximum(double max) {
    if (!IsSpecial(max)) {
      QString newRange = ":" + Isis::toString(max);

      if (!IsSpecial(minimum()))
        newRange = Isis::toString(minimum()) + newRange;

      setAttribute(newRange, &CubeAttributeOutput::isRange);
    }
    else if (!IsSpecial(minimum())) {
      setAttribute(Isis::toString(minimum()) + ":", &CubeAttributeOutput::isRange);
    }
    else {
      setAttribute("", &CubeAttributeOutput::isRange);
    }
  }


  PixelType CubeAttributeOutput::pixelType() const {
    PixelType result = None;

    if (!propagatePixelType()) {
      QString pixelTypeAtt = attributeList(&CubeAttributeOutput::isPixelType).last();

      if(pixelTypeAtt == "8BIT" || pixelTypeAtt == "8-BIT" || pixelTypeAtt == "UNSIGNEDBYTE") {
        result = UnsignedByte;
      }
      else if(pixelTypeAtt == "16BIT" || pixelTypeAtt == "16-BIT" || pixelTypeAtt == "SIGNEDWORD") {
        result = SignedWord;
      }
      else if(pixelTypeAtt == "32BIT" || pixelTypeAtt == "32-BIT" || pixelTypeAtt == "REAL") {
        result = Real;
      }
    }

    return result;
  }


  void CubeAttributeOutput::setPixelType(PixelType type) {
    setAttribute(PixelTypeName(type), &CubeAttributeOutput::isPixelType);
  }


  void CubeAttributeOutput::setLabelAttachment(LabelAttachment attachment) {
    setAttribute(LabelAttachmentName(attachment), &CubeAttributeOutput::isLabelAttachment);
  }


  LabelAttachment CubeAttributeOutput::labelAttachment() const {
    LabelAttachment result = AttachedLabel;

    QStringList labelAttachmentAtts = attributeList(&CubeAttributeOutput::isLabelAttachment);
    if (!labelAttachmentAtts.isEmpty()) {
      QString labelAttachmentAtt = labelAttachmentAtts.last();

      if (labelAttachmentAtt == "DETACHED")
        result = DetachedLabel;
      else if (labelAttachmentAtt == "EXTERNAL")
        result = ExternalLabel;
    }

    return result;
  }


  bool CubeAttributeOutput::isByteOrder(QString attribute) const {
    return QRegExp("(M|L)SB").exactMatch(attribute);
  }


  bool CubeAttributeOutput::isFileFormat(QString attribute) const {
    return QRegExp("(BANDSEQUENTIAL|BSQ|TILE)").exactMatch(attribute);
  }


  bool CubeAttributeOutput::isLabelAttachment(QString attribute) const {
    return QRegExp("(ATTACHED|DETACHED|EXTERNAL)").exactMatch(attribute);
  }


  bool CubeAttributeOutput::isPixelType(QString attribute) const {
    return QRegExp("(8-?BIT|16-?BIT|32-?BIT|UNSIGNEDBYTE|SIGNEDWORD|REAL)").exactMatch(attribute);
  }



  bool CubeAttributeOutput::isRange(QString attribute) const {
    return QRegExp("[\\-+E0-9.]*:[\\-+E0-9.]*").exactMatch(attribute);
  }


  QString CubeAttributeOutput::toString(Cube::Format format) {
    QString result = "Tile";

    if (format == Cube::Bsq)
      result = "BandSequential";

    return result;
  }


  ByteOrder CubeAttributeOutput::byteOrder() const {
    ByteOrder result = IsLsb()? Lsb : Msb;

    QStringList byteOrderAtts = attributeList(&CubeAttributeOutput::isByteOrder);

    if (!byteOrderAtts.isEmpty()) {
      QString byteOrderAtt = byteOrderAtts.last();
      result = (byteOrderAtt == "LSB")? Lsb : Msb;
    }

    return result;
  }


  QString CubeAttributeOutput::byteOrderString() const {
    return ByteOrderName(byteOrder());
  }


  void CubeAttributeOutput::setByteOrder(ByteOrder order) {
    setAttribute((order == Msb)? "MSB" : "LSB",
                 &CubeAttributeOutput::isByteOrder);
  }


  QList<bool (CubeAttributeOutput::*)(QString) const> CubeAttributeOutput::testers() {
   QList<bool (CubeAttributeOutput::*)(QString) const> result;

    result.append(&CubeAttributeOutput::isByteOrder);
    result.append(&CubeAttributeOutput::isFileFormat);
    result.append(&CubeAttributeOutput::isLabelAttachment);
    result.append(&CubeAttributeOutput::isPixelType);
    result.append(&CubeAttributeOutput::isRange);

    return result;
  }
}
