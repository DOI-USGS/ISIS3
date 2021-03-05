/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
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
  History::History(const QString &name) : Isis::Blob(name, "History") {
    p_history.setTerminator("");
  }

  /**
   *  Constructor for reading a history blob
   *  @param name
   *  @param file
   */
  History::History(const QString &name, const QString &file) :
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
    p_history.addObject(obj);
  }

  /**
   *
   */
  void History::WriteInit() {
    ostringstream ostr;
    if (p_nbytes > 0) ostr << std::endl;
    ostr << p_history;
    string histStr = ostr.str();
    int bytes = histStr.size();

    char *temp = p_buffer;
    p_buffer = new char[p_nbytes+bytes];
    if (temp != NULL) memcpy(p_buffer, temp, p_nbytes);
    const char *ptr = histStr.c_str();
    memcpy(&p_buffer[p_nbytes], (void *)ptr, bytes);
    p_nbytes += bytes;

    if (temp != NULL) delete [] temp;
  }

  /**
   * Reads p_buffer into a pvl
   *
   * @return @b Pvl
   */
  Pvl History::ReturnHist() {
    Pvl pvl;
    stringstream os;
    for (int i = 0; i < p_nbytes; i++) os << p_buffer[i];
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
    catch (IException &e) {
    }
  }
}
