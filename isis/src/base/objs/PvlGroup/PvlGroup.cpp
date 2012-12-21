/**
 * @file
 * $Revision: 1.8 $
 * $Date: 2010/06/16 18:15:21 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public domain. See
 *   individual third-party library and package descriptions for intellectual property information,
 *   user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied, is made by the
 *   USGS as to the accuracy and functioning of such software and related material nor shall the
 *   fact of distribution constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "IException.h"
#include "PvlFormat.h"

using namespace std;
namespace Isis {
  //! Creates a blank PvlGroup object.
  PvlGroup::PvlGroup() : Isis::PvlContainer("Group", "") {}

  /**
   * Creates a PvlGroup object with a name.
   * @param name The group name.
   */
  PvlGroup::PvlGroup(const QString &name) :
    Isis::PvlContainer("Group", name) {
  }


  //! Copy constructor
  PvlGroup::PvlGroup(const PvlGroup &other) : PvlContainer(other) {}


  /**
   * Read in a group
   *
   * @param is The input stream
   * @param result The PvlGroup to read into (OUTPUT)
   *
   */
  std::istream &operator>>(std::istream &is, PvlGroup &result) {
    PvlKeyword termination("EndGroup");

    PvlKeyword errorKeywords[] = {
      PvlKeyword("Group"),
      PvlKeyword("Object"),
      PvlKeyword("EndObject")
    };

    PvlKeyword readKeyword;

    istream::pos_type beforeKeywordPos = is.tellg();
    is >> readKeyword;

    if(readKeyword != PvlKeyword("Group")) {
      if(is.eof() && !is.bad()) {
        is.clear();
      }

      is.seekg(beforeKeywordPos, ios::beg);

      QString msg = "Expected PVL keyword named [Group], found keyword named [";
      msg += readKeyword.Name();
      msg += "] when reading PVL";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }

    if(readKeyword.Size() == 1) {
      result.SetName(readKeyword[0]);
    }
    else {
      if(is.eof() && !is.bad()) {
        is.clear();
      }

      is.seekg(beforeKeywordPos, ios::beg);

      QString msg = "Expected a single value for group name, found [(";

      for(int i = 0; i < readKeyword.Size(); i++) {
        if(i != 0) msg += ", ";

        msg += readKeyword[i];
      }

      msg += ")] when reading PVL";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }


    for(int comment = 0; comment < readKeyword.Comments(); comment++) {
      result.AddComment(readKeyword.Comment(comment));
    }

    readKeyword = PvlKeyword();
    beforeKeywordPos = is.tellg();

    is >> readKeyword;
    while(is.good() && readKeyword != termination) {
      for(unsigned int errorKey = 0;
          errorKey < sizeof(errorKeywords) / sizeof(PvlKeyword);
          errorKey++) {

        if(readKeyword == errorKeywords[errorKey]) {
          if(is.eof() && !is.bad()) {
            is.clear();
          }

          is.seekg(beforeKeywordPos, ios::beg);

          QString msg = "Unexpected [";
          msg += readKeyword.Name();
          msg += "] in Group [";
          msg += result.Name();
          msg += "] when reading PVL";
          throw IException(IException::Unknown, msg, _FILEINFO_);
        }
      }

      result.AddKeyword(readKeyword);
      readKeyword = PvlKeyword();
      beforeKeywordPos = is.tellg();

      is >> readKeyword;
    }

    if(readKeyword != termination) {
      if(is.eof() && !is.bad()) {
        is.clear();
        is.unget();
      }

      is.seekg(beforeKeywordPos, ios::beg);

      QString msg = "Group [" + result.Name();
      msg += "] EndGroup not found before end of file when reading PVL";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }

    return is;
  }

  /**
   * Outputs the PvlGroup data to a specified output stream.
   * @param os The output stream to output to.
   * @param group The PvlGroup object to output.
   */
  ostream &operator<<(std::ostream &os, PvlGroup &group) {

    // Set up a Formatter
    bool removeFormatter = false;
    if(group.GetFormat() == NULL) {
      group.SetFormat(new PvlFormat());
      removeFormatter = true;
    }

    Isis::PvlGroup temp("DEFAULT");
    if(group.HasFormatTemplate()) temp = *(Isis::PvlGroup *)group.FormatTemplate();

    // Output comment from the template
    if(temp.Comments() > 0) {
      for(int k = 0; k < temp.Comments(); k++) {
        for(int l = 0; l < group.Indent(); l++) os << " ";
        os << temp.Comment(k) << group.GetFormat()->FormatEOL();
      }
//      os << group.GetFormat()->FormatEOL();
    }

    // Output the group comments and name
    os << group.GetNameKeyword() << group.GetFormat()->FormatEOL();
    group.SetIndent(group.Indent() + 2);

    // Output the keywords in this group
    if(group.Keywords() > 0) {
      os << (Isis::PvlContainer &) group << group.GetFormat()->FormatEOL();
    }

    // Output the end of the group
    group.SetIndent(group.Indent() - 2);
    for(int i = 0; i < group.Indent(); i++) os << " ";
    os << group.GetFormat()->FormatEnd("End_Group", group.GetNameKeyword());

    if(removeFormatter) {
      delete group.GetFormat();
      group.SetFormat(NULL);
    }

    return os;
  }


  //! This is an assignment operator
  const PvlGroup &PvlGroup::operator=(const PvlGroup &other) {
    this->PvlContainer::operator=(other);

    return *this;
  }

  /**
   * Validate a PvlGroup, comparing against the corresponding
   * PvlGroup in the Template file
   *
   * Template PvlGroup has the format:
   * Group = (groupName, optional/required)
   *
   * @author Sharmila Prasad (9/22/2010)
   *
   * @param pPvlGrp - PvlGroup to be validated
   */
  void PvlGroup::ValidateGroup(PvlGroup & pPvlGrp)
  {
    // Group cannot be empty - needs to have a keyword
    if(pPvlGrp.Keywords() <= 0) {
      QString sErrMsg = "Group \"" + pPvlGrp.Name() + "\" has no Keywords\n";
      throw IException(IException::User, sErrMsg, _FILEINFO_);
    }

    ValidateAllKeywords((PvlContainer &)pPvlGrp);
  }

} // end namespace isis
