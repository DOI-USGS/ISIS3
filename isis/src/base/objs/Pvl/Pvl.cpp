/**
 * @file
 * $Revision: 1.13 $
 * $Date: 2010/06/25 20:40:06 $
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
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"

#include <locale>
#include <fstream>

#include "Filename.h"
#include "IException.h"
#include "Message.h"
#include "PvlTokenizer.h"
#include "PvlFormat.h"

using namespace std;
namespace Isis {
  //! Constructs an empty Pvl object.
  Pvl::Pvl() : Isis::PvlObject("Root") {
    Init();
  }


  /**
   * Constructs a Pvl from a file
   *
   * @param file The file containing the pvl formatted information
   */
  Pvl::Pvl(const std::string &file) : Isis::PvlObject("Root") {
    Init();
    Read(file);
  }


  //! Copy constructor
  Pvl::Pvl(const Pvl &other) : PvlObject::PvlObject(other) {
    p_internalTemplate = false;
    p_terminator = other.p_terminator;
  }


  //! Initializes the class
  void Pvl::Init() {
    p_filename = "";
    p_terminator = "End";
    p_internalTemplate = false;
  }


  /**
   * Loads PVL information from a stream
   *
   * @param file A file containing PVL information
   *
   * @throws Isis::iException::Io
   */
  void Pvl::Read(const std::string &file) {
    // Expand the filename
    Isis::Filename temp(file);
    p_filename = temp.Expanded();

    // Open the file
    ifstream istm;
    istm.open(p_filename.c_str(), std::ios::in);
    if(!istm) {
      string message = Message::FileOpen(temp.Expanded());
      throw IException(IException::Io, message, _FILEINFO_);
    }

    // Read it
    try {
      istm >> *this;
    }
    catch(IException &e) {
      istm.close();
      string message = "Unable to read PVL file [" + temp.Expanded() + "]";
      throw IException(e, IException::Unknown, message, _FILEINFO_);
    }
    catch(...) {
      istm.close();
      string message = "Unable to read PVL file [" + temp.Expanded() + "]";
      throw IException(IException::Unknown, message, _FILEINFO_);
    }
    istm.close();
  }


  /**
   * Opens and writes PVL information to a file and handles the end of line
   * sequence
   *
   * @param file Name of the file to create. The method will overwrite any
   *             existing file.
   *
   * @throws Isis::iException::Io
   */
  void Pvl::Write(const std::string &file) {
    // Expand the filename
    Isis::Filename temp(file);

    // Set up a Formatter
    bool removeFormatter = false;
    if(GetFormat() == NULL) {
      SetFormat(new PvlFormat());
      removeFormatter = true;
    }

    // Open the file
    ofstream ostm;
    string tempName(temp.Expanded());
    ostm.open(tempName.c_str(), std::ios::out);
    ostm.seekp(0, std::ios::beg);
    if(!ostm) {
      string message = Isis::Message::FileCreate(temp.Expanded());
      throw IException(IException::Io, message, _FILEINFO_);
    }

    // Write the labels
    try {
      ostm << *this;
      if(Terminator() != "") ostm << GetFormat()->FormatEOL();
    }
    catch(IException &e) {
      ostm.close();
      string message = "Unable to write PVL to file [" + temp.Expanded() + "]";
      throw IException(e, IException::Io, message, _FILEINFO_);
    }
    catch(...) {
      ostm.close();
      string message = "Unable to write PVL to file [" + temp.Expanded() + "]";
      throw IException(IException::Io, message, _FILEINFO_);
    }

    if(removeFormatter) {
      delete GetFormat();
      SetFormat(NULL);
    }

    // Close the file
    ostm.close();
  }


  /**
   * Appends PVL information to a file and handles the end of line sequence
   *
   * @param file Name of the file to append to.
   *
   * @throws Isis::iException::Io
   */
  void Pvl::Append(const std::string &file) {
    // Set up for opening and writing
    Isis::Filename temp(file);

    // Set up a Formatter
    bool removeFormatter = false;
    if(GetFormat() == NULL) {
      SetFormat(new PvlFormat());
      removeFormatter = true;
    }

    // Open the file
    ofstream ostm;
    string tempName(temp.Expanded());
    ostm.open(tempName.c_str(), std::ios::app);
    ostm.seekp(0, std::ios::end);
    if(!ostm) {
      string message = Message::FileOpen(temp.Expanded());
      throw IException(IException::Io, message, _FILEINFO_);
    }

    // Write the labels
    try {
      ostm << *this;
      if(Terminator() != "") ostm << GetFormat()->FormatEOL();
    }
    catch(...) {
      ostm.close();
      string message = "Unable to append PVL infomation to file [" +
                       temp.Expanded() + "]";
      throw IException(IException::Io, message, _FILEINFO_);
    }

    if(removeFormatter) {
      delete GetFormat();
      SetFormat(NULL);
    }

    // Close the file
    ostm.close();
  }


  void Pvl::SetFormatTemplate(Isis::Pvl &temp) {
    if(p_internalTemplate) delete p_formatTemplate;
    p_internalTemplate = false;
    Isis::PvlObject::SetFormatTemplate(temp);
  }


  void Pvl::SetFormatTemplate(const std::string &file) {
    if(p_internalTemplate) delete p_formatTemplate;
    p_internalTemplate = true;
    p_formatTemplate = new Isis::Pvl(file);
  }


  /**
   * This stream will not handle the end of line sequence
   *
   * @param os
   * @param pvl
   *
   * @return ostream&
   */
  ostream &operator<<(std::ostream &os, Pvl &pvl) {
    // Set up a Formatter
    bool removeFormatter = false;
    if(pvl.GetFormat() == NULL) {
      pvl.SetFormat(new PvlFormat());
      removeFormatter = true;
    }

    Isis::Pvl outTemplate;
    if(pvl.HasFormatTemplate()) outTemplate = *(Isis::Pvl *)pvl.FormatTemplate();

    // Look for and process all include files and remove duplicates from the
    // format template. Include files take precedence over all other objects and
    // groups
    Isis::Pvl newTemp;
    for(int i = 0; i < outTemplate.Keywords(); i++) {
      if(outTemplate[i].IsNamed("Isis:PvlTemplate:File")) {
        string filename = outTemplate[i];
        Isis::Filename file(filename);
        if(!file.Exists()) {
          string message = "Could not open the template file [" + filename + "]";
          throw IException(IException::Io, message, _FILEINFO_);
        }
        Isis::Pvl include(file.Expanded());

        for(int j = 0; j < include.Keywords(); j++) {
          if(!newTemp.HasKeyword(include[j].Name()))
            newTemp.AddKeyword(include[j]);
        }

        for(int j = 0; j < include.Objects(); j++) {
          if(!newTemp.HasObject(include.Object(j).Name()))
            newTemp.AddObject(include.Object(j));
        }

        for(int j = 0; j < include.Groups(); j++) {
          if(!newTemp.HasGroup(include.Group(j).Name()))
            newTemp.AddGroup(include.Group(j));
        }
      }
      // If it is not an include file add it in place
      else if(!newTemp.HasKeyword(outTemplate[i].Name()))
        newTemp.AddKeyword(outTemplate[i]);
    }

    // copy over the objects
    for(int i = 0; i < outTemplate.Objects(); i++) {
      if(!newTemp.HasObject(outTemplate.Object(i).Name()))
        newTemp.AddObject(outTemplate.Object(i));
    }

    // copy over the groups
    for(int i = 0; i < outTemplate.Groups(); i++) {
      if(!newTemp.HasGroup(outTemplate.Group(i).Name()))
        newTemp.AddGroup(outTemplate.Group(i));
    }

    outTemplate = newTemp;

    // Output the pvl's comments
    for(int i = 0; i < pvl.Comments(); i++) {
      os << pvl.Comment(i) << pvl.GetFormat()->FormatEOL();
      if(i == (pvl.Comments() - 1)) os << pvl.GetFormat()->FormatEOL();
    }

    // Output the keywords
    if(pvl.Keywords() > 0) {
      os << (Isis::PvlContainer &) pvl << pvl.GetFormat()->FormatEOL();
    }

    // this number keeps track of the number of objects that have been written
    int numObjects = 0;

    // Output the objects using the format template
    for(int i = 0; i < outTemplate.Objects(); i++) {
      for(int j = 0; j < pvl.Objects(); j++) {
        if(outTemplate.Object(i).Name() != pvl.Object(j).Name()) continue;
        if(numObjects == 0 && pvl.Keywords() > 0) os << pvl.GetFormat()->FormatEOL();
        pvl.Object(j).SetIndent(pvl.Indent());
        pvl.Object(j).SetFormatTemplate(outTemplate.Object(i));
        pvl.Object(j).SetFormat(pvl.GetFormat());
        os << pvl.Object(j) << pvl.GetFormat()->FormatEOL();
        pvl.Object(j).SetFormat(NULL);
        pvl.Object(j).SetIndent(0);
        if(++numObjects < pvl.Objects()) os << pvl.GetFormat()->FormatEOL();
      }
    }

    // Output the objects that were not included in the format template pvl
    for(int i = 0; i < pvl.Objects(); i++) {
      if(outTemplate.HasObject(pvl.Object(i).Name())) continue;
      if(numObjects == 0 && pvl.Keywords() > 0) os << pvl.GetFormat()->FormatEOL();
      pvl.Object(i).SetIndent(pvl.Indent());
      pvl.Object(i).SetFormat(pvl.GetFormat());
      os << pvl.Object(i) << pvl.GetFormat()->FormatEOL();
      pvl.Object(i).SetFormat(NULL);
      pvl.Object(i).SetIndent(0);
      if(++numObjects < pvl.Objects()) os << pvl.GetFormat()->FormatEOL();
    }

    // this number keeps track of the number of groups that have been written
    int numGroups = 0;

    // Output the groups using the format template
    for(int i = 0; i < outTemplate.Groups(); i++) {
      for(int j = 0; j < pvl.Groups(); j++) {
        if(outTemplate.Group(i).Name() != pvl.Group(j).Name()) continue;
        if((numGroups == 0) &&
            (pvl.Objects() > 0 || pvl.Keywords() > 0)) os << pvl.GetFormat()->FormatEOL();
        pvl.Group(j).SetIndent(pvl.Indent());
        pvl.Group(j).SetFormatTemplate(outTemplate.Group(i));
        pvl.Group(j).SetFormat(pvl.GetFormat());
        os << pvl.Group(j) << pvl.GetFormat()->FormatEOL();
        pvl.Group(j).SetFormat(NULL);
        pvl.Group(j).SetIndent(0);
        if(++numGroups < pvl.Groups()) os << pvl.GetFormat()->FormatEOL();
      }
    }

    // Output the groups that were not in the format template
    for(int i = 0; i < pvl.Groups(); i++) {
      if(outTemplate.HasGroup(pvl.Group(i).Name())) continue;
      if((numGroups == 0) &&
          (pvl.Objects() > 0 || pvl.Keywords() > 0)) os << pvl.GetFormat()->FormatEOL();
      pvl.Group(i).SetIndent(pvl.Indent());
      pvl.Group(i).SetFormat(pvl.GetFormat());
      os << pvl.Group(i) << pvl.GetFormat()->FormatEOL();
      pvl.Group(i).SetFormat(NULL);
      pvl.Group(i).SetIndent(0);
      if(++numGroups < pvl.Groups()) os << pvl.GetFormat()->FormatEOL();
    }

    // Output the terminator
    if(pvl.Terminator() != "") {
      os << pvl.Terminator();
    }

    if(removeFormatter) {
      delete pvl.GetFormat();
      pvl.SetFormat(NULL);
    }

    return os;
  }


  /**
   * Reads keywords from the instream and appends them to the Pvl object.
   *
   * @param is A specified instream to read from.
   * @param pvl The Pvl object to append to.
   * @throws iException Invalid PVL format specified.
   * @return Returns the entered instream after reading from it.
   */
  istream &operator>>(std::istream &is, Pvl &pvl) {
    if(!is.good()) {
      string msg = "Tried to read input stream with an error state into a Pvl";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    try {
      PvlKeyword termination("End");

      PvlKeyword errorKeywords[] = {
        PvlKeyword("EndGroup"),
        PvlKeyword("EndObject")
      };

      PvlKeyword readKeyword;
      istream::pos_type beforeKeywordPos = is.tellg();

      is >> readKeyword;

      while(readKeyword != termination) {
        for(unsigned int errorKey = 0;
            errorKey < sizeof(errorKeywords) / sizeof(PvlKeyword);
            errorKey++) {
          if(readKeyword == errorKeywords[errorKey]) {
            is.seekg(beforeKeywordPos, ios::beg);

            string msg = "Unexpected [";
            msg += readKeyword.Name();
            msg += "] in PVL Object [ROOT]";
            throw IException(IException::Unknown, msg, _FILEINFO_);
          }
        }

        if(readKeyword == PvlKeyword("Group")) {
          is.seekg(beforeKeywordPos);
          PvlGroup newGroup;
          is >> newGroup;
          pvl.AddGroup(newGroup);
        }
        else if(readKeyword == PvlKeyword("Object")) {
          is.seekg(beforeKeywordPos);
          PvlObject newObject;
          is >> newObject;
          pvl.AddObject(newObject);
        }
        else {
          pvl.AddKeyword(readKeyword);
        }

        readKeyword = PvlKeyword();
        beforeKeywordPos = is.tellg();

        // non-whitespace non-ascii says we're done
        if(is.good() && (is.peek() < 32 || is.peek() > 126)) {
          // fake eof (binary data)
          break;
        }

        if(is.good()) {
          is >> readKeyword;
        }
        else {
          // eof
          break;
        }
      }

      return is;
    }
    catch(IException &e) {
      if(is.eof() && !is.bad()) {
        is.clear();
        is.unget();
      }

      istream::pos_type errorPos = is.tellg();
      if((int)errorPos == -1) throw;

      is.seekg(0, ios::end);
      istream::pos_type fileEndPos = is.tellg();

      is.seekg(0, ios::beg);
      long lineNumber = 1;

      if((int)is.tellg() == -1) throw;

      while(is.good() && is.tellg() < errorPos) {
        char next = is.get();

        if(!isprint(next) && !isspace(next)) {
          is.seekg(errorPos, ios::beg);
        }
        else if(next == '\n') {
          lineNumber ++;
        }
      }

      string msg;
      if(lineNumber > 0) {
        msg = "Error in PVL file on line [";
        msg += iString((Isis::BigInt)lineNumber);
        msg += "]";
      }

      throw IException(e, IException::Unknown, msg, _FILEINFO_);
    }
  }


  //! This is an assignment operator
  const Pvl &Pvl::operator=(const Pvl &other) {
    this->PvlObject::operator=(other);

    p_internalTemplate = other.p_internalTemplate;
    p_terminator = other.p_terminator;

    return *this;
  }


  /**
   * Validate a Pvl, comparing against corresponding Template Pvl
   * It validates all the Objects, Groups and Keywords
   *
   * @author Sharmila Prasad (9/22/2010)
   *
   * @param pPvl - User Pvl to be validated
   * @param pPvlResults - Contains the unvalidated Pvl
   */
  void Pvl::ValidatePvl(const Pvl & pPvl, Pvl & pPvlResults)
  {
    pPvlResults=Pvl(pPvl);

    // Validate Objects
    int iTmplObjSize = Objects();

    for(int i=0; i<iTmplObjSize; i++) {
      PvlObject & pvlTmplObj = Object(i);

      string sObjName = pvlTmplObj.Name();
      bool bObjFound = false;

      // Pvl contains the Object Name
      if(pPvl.HasObject(sObjName)) {
        PvlObject & pvlObj = pPvlResults.FindObject(sObjName);
        pvlTmplObj.ValidateObject(pvlObj);
        if(pvlObj.Objects()==0 && pvlObj.Groups()==0 && pvlObj.Keywords()==0) {
          pPvlResults.DeleteObject(sObjName);
        }
        bObjFound = true;
      }
      else {
        string sOption = sObjName + "__Required";
        bObjFound = true; // optional is the default
        if(pvlTmplObj.HasKeyword(sOption)) {
          PvlKeyword pvlKeyOption = pvlTmplObj.FindKeyword(sOption);
          if(pvlKeyOption[0] == "true") { // Required is true
            bObjFound = false;
          }
        }
      }
      if (bObjFound == false) {
        string sErrMsg = "Object \"" + sObjName + "\" Not Found in the Template File\n";
        throw IException(IException::User, sErrMsg, _FILEINFO_);
      }
    }

    // Validate Groups
    int iTmplGrpSize = Groups();
    for(int i=0; i<iTmplGrpSize; i++) {
      PvlGroup & pvlTmplGrp = Group(i);

      string sGrpName  = pvlTmplGrp.Name();
      bool bGrpFound = false;

      // Pvl contains the Object Name
      if(pPvl.HasGroup(sGrpName)) {
        PvlGroup & pvlGrp = pPvlResults.FindGroup(sGrpName);
        pvlTmplGrp.ValidateGroup(pvlGrp);
        if(pvlGrp.Keywords()==0) {
          pPvlResults.DeleteGroup(sGrpName);
        }
        bGrpFound = true;
      }
      else {
        bGrpFound = true;
        string sOption = sGrpName + "__Required";
        if(pvlTmplGrp.HasKeyword(sOption)) {
          PvlKeyword pvlKeyOption = pvlTmplGrp.FindKeyword(sOption);
          if(pvlKeyOption[0] == "true") { // Required is true
            bGrpFound = false;
          }
        }
      }
      if (bGrpFound == false) {
        string sErrMsg = "Group \"" + sGrpName + "\" Not Found in the Template File\n";
        throw IException(IException::User, sErrMsg, _FILEINFO_);
      }
    }

    // Validate all the Keywords
    ValidateAllKeywords((PvlContainer &)pPvlResults);
  }

} //end namespace isis
