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
#include <iostream>

#include "iException.h"
#include "iException.h"
#include "Preference.h"

#include "CubeAttribute.h"

using namespace std;
namespace Isis {

  CubeAttribute::CubeAttribute () {
    p_attribute.clear();
  }


  CubeAttribute::CubeAttribute (const Isis::iString &att) {
    // Strip off the filename if there is one
    std::string::size_type pos = att.find('+');
    if (pos == 0) {
      p_attribute = att;
    }
    else if (pos != std::string::npos) {
      p_attribute = att.substr(pos-1);
    }
    else if (att.length() == 0) {
      p_attribute = "";
    }
    else if (pos == std::string::npos) {
      p_attribute = "";
    }
    else {
      string msg = "Invalid cube attribute string [" + att + "]";
      throw Isis::iException::Message(Isis::iException::Parse,msg, _FILEINFO_);
    }
  }


  CubeAttribute::~CubeAttribute () {}


  void CubeAttribute::Write(std::ostream &ostr) const {
    ostr << p_attribute;
  }


  void CubeAttribute::Write(std::string &str) const {
    str = p_attribute;
  }


  void CubeAttribute::Write(Isis::Pvl &pvl) const {
    Isis::PvlGroup atts("Attributes");
    atts += Isis::PvlKeyword("Format",  p_attribute);
    pvl.AddGroup(atts);
  }


  //---------------------------------------------------------------------------
  // CubeAttributeInput Implementation
  //---------------------------------------------------------------------------
  CubeAttributeInput::CubeAttributeInput () {
    p_bands.clear();
  }


  CubeAttributeInput::CubeAttributeInput (const Isis::iString &att)
      : CubeAttribute(att) {
    p_bands.clear();
    Parse (p_attribute);
  }


  CubeAttributeInput::~CubeAttributeInput () {}


  void CubeAttributeInput::Set (const std::string &att) {
    Parse(att);
  }


  string CubeAttributeInput::BandsStr() const {
    string str;
    for (unsigned int i=0; i<p_bands.size(); i++) {
      if (i>0) str += ",";
      str += p_bands[i];
    }

    return str;
  }


  vector<string> CubeAttributeInput::Bands() const {
      return p_bands;
  }


  void CubeAttributeInput::Bands (const std::vector<std::string> &bands) {
    p_bands.clear();
    for (unsigned int i=0; i<bands.size(); i++) {
      p_bands.push_back(bands[i]);
    }
  }


  void CubeAttributeInput::Bands (const std::string &bands) {
    p_bands.clear();
    Parse (bands);
  }


  void CubeAttributeInput::Write(std::ostream &ostr) const {
    string st;
    Write(st);
    ostr << st;
  }


  void CubeAttributeInput::Write(std::string &str) const {
    if (p_bands.size() > 0) {
      str = "+";
    }
    for (unsigned int i=0; i<p_bands.size(); i++) {
      if (i>0) str += ",";
      str += p_bands[i];
    }
  }


  void CubeAttributeInput::Write(Isis::Pvl &pvl) const {
    Isis::PvlKeyword bands("Bands");
    for (unsigned int b=0; b<p_bands.size(); b++) {
      bands += p_bands[b];
    }
    Isis::PvlGroup inatts("InputAttributes");
    inatts += bands;
    pvl.AddGroup(inatts);
  }


  void CubeAttributeInput::Reset() {
    p_bands.clear();
  }


  void CubeAttributeInput::Parse(const std::string &attStr) {

    Isis::iString str(attStr);

    // Strip off the leading "+" and put the attributes in a temporary
    std::string::size_type pos = str.find('+');
    if (pos != std::string::npos) {
      str = str.substr(pos);
    }
    else {
      str = "";
    }

    // Get rid of any white space
    str.ConvertWhiteSpace();
    str.Compress();
    str.Remove(" ");
    str.UpCase();
    str.TrimHead ("+");

    // Look at each comma delimited token
    Isis::iString commaTok;
    while ((commaTok = str.Token(",")).length() > 0) {
      // Is this token a range of bands
      if (commaTok.find('-') != std::string::npos) {
        Isis::iString dashTok;
        int start = commaTok.Token("-").ToInteger();
        int end = commaTok.Token("-").ToInteger();
        int direction;
        direction = (start<=end) ? 1 : -1;
        // Save the entire range of bands
        for (int band = start; band != end; band+=direction) {
          p_bands.push_back(Isis::iString(band));
        }
        p_bands.push_back(Isis::iString(end));
      }
      // This token is a single band specification
      else {
        p_bands.push_back(commaTok);
      }
    }
  }


  //---------------------------------------------------------------------------
  // CubeAttributeOutput Implementation
  //---------------------------------------------------------------------------
  CubeAttributeOutput::CubeAttributeOutput () {
    Initialize ();
  }


  CubeAttributeOutput::CubeAttributeOutput (const Isis::iString &att)
      : CubeAttribute (att) {

    Initialize ();
    p_attribute = att;
    Parse (p_attribute);
  }


  CubeAttributeOutput::~CubeAttributeOutput () {}


  void CubeAttributeOutput::Set (const std::string &att) {
    Parse(att);
  }


  string CubeAttributeOutput::FileFormatStr() const {
    return Isis::CubeFormatName(p_format);
  }


  Isis::CubeFormat CubeAttributeOutput::FileFormat() const {
    return p_format;
  }


  void CubeAttributeOutput::Format (const Isis::CubeFormat fmt) {
    p_format = fmt;
  }


  double CubeAttributeOutput::Minimum () const {
    return p_minimum;
  }


  double CubeAttributeOutput::Maximum () const {
    return p_maximum;
  }


  void CubeAttributeOutput::Minimum (const double min) {
    p_minimum = min;
    p_rangeType = Isis::RangeSet;
  }


  void CubeAttributeOutput::Maximum (const double max) {
    p_maximum = max;
    p_rangeType = Isis::RangeSet;
  }


  Isis::PixelType CubeAttributeOutput::PixelType() const {
    if (p_pixelType == Isis::None) {
      string msg;
      msg = msg + "Request for CubeAttributeOutput::PixelType failed. " +
             "PixelType has not been set. Use PropagatePixelType or " +
             "UserPixelType to determine how to set PixelType.";
      throw Isis::iException::Message(Isis::iException::Programmer,msg, _FILEINFO_);
    }
    return p_pixelType;
  }


  void CubeAttributeOutput::PixelType (const Isis::PixelType type) {
    p_pixelType = type;
    if (p_pixelType == Isis::None) {
      p_pixelTypeDef = "PROPAGATE";
    }
    else {
      p_pixelTypeDef = "SET";
    }
  }


  string CubeAttributeOutput::ByteOrderStr() const {
    return Isis::ByteOrderName(p_order);
  }


  Isis::ByteOrder CubeAttributeOutput::ByteOrder() const {
    return p_order;
  }


  void CubeAttributeOutput::Order (const Isis::ByteOrder order) {
    p_order = order;
  }


  void CubeAttributeOutput::Write(std::ostream &ostr) const {
    string st;
    Write(st);
    ostr << st;
  }


  void CubeAttributeOutput::Write(std::string &str) const {
    str.clear();
    if (p_pixelTypeDef != "PROPAGATE") {
      str += "+" + Isis::PixelTypeName (p_pixelType);
    }
    if (p_pixelType != Isis::Real && p_pixelType != Isis::None) {
      str += "+" + Isis::iString(p_minimum) + ":" + Isis::iString(p_maximum);
    }
    str += "+" + FileFormatStr();
    str += "+" + ByteOrderStr();
    str += "+" + Isis::LabelAttachmentName (p_labelAttachment);
  }


  void CubeAttributeOutput::Write(Isis::Pvl &pvl) const {
    Isis::PvlGroup outatt("OutputCubeAttributes");

    if (p_pixelTypeDef != "PROPAGATE") {
      outatt += Isis::PvlKeyword("Type", PixelTypeName(p_pixelType));
    }
    outatt += Isis::PvlKeyword("Format",  FileFormatStr());
    if (p_pixelType != Isis::Real) {
      outatt += Isis::PvlKeyword("Minimum", p_minimum);
      outatt += Isis::PvlKeyword("Maximum", p_maximum);
    }
    outatt += Isis::PvlKeyword("ByteOrder", ByteOrderStr());
    outatt += Isis::PvlKeyword("LabelType", Isis::LabelAttachmentName(p_labelAttachment));

    pvl.AddGroup(outatt);
  }


  void CubeAttributeOutput::Reset() {
    Initialize();
  }


  void CubeAttributeOutput::Parse (const std::string &att) {

    Isis::iString str(att);

    // Strip off the leading "+" and put the attributes in a temporary
    std::string::size_type pos = str.find('+');
    if (pos != std::string::npos) {
      str = str.substr(pos);
    }
    else {
      str = "";
    }

    // Remove any white space
    str.ConvertWhiteSpace();
    str.Compress();
    str.Remove(" ");
    str.UpCase();
    str.TrimHead ("+");

    // Look at each "+" separate attribute
    Isis::iString tok;
    while ((tok = str.Token("+")).length() > 0) {

      // If there is a ":" in this token then it is assumed to be a min:max
      if (tok.find(":") != std::string::npos) {

        // Pull out the minimum
        Isis::iString colonTok = tok;
        Isis::iString min = colonTok.Token(":");
        if (min.length() > 0) {
          p_minimum = min.ToDouble();
        }
        else {
          p_minimum = 0.0;
        }

        // Pull out the maximum
        Isis::iString max = colonTok.Token(":");
        if (max.length() > 0) {
          p_maximum = max.ToDouble();
        }
        else {
          p_maximum = 0.0;
        }
        p_rangeType = Isis::RangeSet;
      }

      // Parse any pixel type attributes
      else if (tok == "8BIT" || tok == "8-BIT" || tok == "UNSIGNEDBYTE") {
        p_pixelType = Isis::UnsignedByte;
        p_pixelTypeDef = "SET";
      }
      else if (tok == "16BIT" || tok == "16-BIT" || tok == "SIGNEDWORD") {
        p_pixelType = Isis::SignedWord;
        p_pixelTypeDef = "SET";
      }
      else if (tok == "32BIT" || tok == "32-BIT" || tok == "REAL") {
        p_pixelType = Isis::Real;
        p_pixelTypeDef = "SET";
      }
      else if (tok == "PROPAGATE") {
        p_pixelType = Isis::None;
        p_pixelTypeDef = "PROPAGATE";
      }

      // Parse any file formats
      else if (tok == "TILE") {
        p_format = Isis::Tile;
      }
      else if (tok == "BSQ" || tok == "BANDSEQUENTIAL") {
        p_format = Isis::Bsq;
      }

      // Parse any byte order
      else if (tok == "LSB") {
        p_order = Isis::Lsb;
      }
      else if (tok == "MSB") {
        p_order = Isis::Msb;
      }

      // Parse any label type
      else if (tok == "ATTACHED") {
        p_labelAttachment = Isis::AttachedLabel;
      }
      else if (tok == "DETACHED") {
        p_labelAttachment = Isis::DetachedLabel;
      }
    }
  }


  void CubeAttributeOutput::Initialize () {
    p_pixelType = Isis::None;
    p_pixelTypeDef = "PROPAGATE";
    p_rangeType = Isis::PropagateRange;
    p_minimum = 0.0;
    p_maximum = 0.0;
    p_format = Isis::Tile;

    // The byte order default is dependant on the hardware
    if (Isis::IsLsb()) {
      p_order = Isis::Lsb;
    }
    else {
      p_order = Isis::Msb;
    }

    // The type of label to produce is dependent on the preference file
    Isis::PvlGroup &cust = Isis::Preference::Preferences().FindGroup("CubeCustomization");
    p_labelAttachment = Isis::LabelAttachmentEnumeration (cust["Format"]);
  }
}
