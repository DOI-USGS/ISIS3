/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2010/05/14 19:17:59 $
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
#include "History.h"

#include <fstream>
#include <sstream>
#include <string>

#include "Application.h"
#include "PvlObject.h"

using namespace std;

namespace Isis {

  /**
   *  Constructor for reading a history blob
   *  @param name
   */
  History::History(const std::string &name) : Isis::Blob(name, "History") {
    p_history.SetTerminator("");
  }

  /**
   *  Constructor for reading a history blob
   *  @param name
   *  @param file
   */
  History::History(const std::string &name, const std::string &file) :
    Isis::Blob(name, "History") {
    Blob::Read(file);
  }

  //! Destructor
  History::~History() {
  }

  /**
   *   Adds History PvlObject
   */
  void History::AddEntry() {
    PvlObject hist = Isis::iApp->History();
    AddEntry(hist);
  }

  /**
   * Adds given PvlObject to History Pvl
   *
   * @param obj PvlObject to be added
   */
  void History::AddEntry(Isis::PvlObject &obj) {
    p_history.AddObject(obj);
  }

  /**
   *
   */
  void History::WriteInit() {
    ostringstream ostr;
    if(p_nbytes > 0) ostr << std::endl;
    ostr << p_history;
    string histStr = ostr.str();
    int bytes = histStr.size();

    char *temp = p_buffer;
    p_buffer = new char[p_nbytes+bytes];
    if(temp != NULL) memcpy(p_buffer, temp, p_nbytes);
    const char *ptr = histStr.c_str();
    memcpy(&p_buffer[p_nbytes], (void *)ptr, bytes);
    p_nbytes += bytes;

    if(temp != NULL) delete [] temp;
  }

  /**
   * Reads p_buffer into a pvl
   *
   * @return @b Pvl
   */
  Pvl History::ReturnHist() {
    Pvl pvl;
    stringstream os;
    for(int i = 0; i < p_nbytes; i++) os << p_buffer[i];
    os >> pvl;
    return pvl;
  }

  /**
   * Reads input stream into Pvl.
   *
   * @param pvl Pvl into which the input stream will be read.
   * @param is Input stream.
   */
  void History::Read(const Isis::Pvl &pvl, std::istream &is) {
    try {
      Blob::Read(pvl, is);
    }
    catch(IException &e) {
    }
  }
}
