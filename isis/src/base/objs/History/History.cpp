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
   *  Default Constructor for history
   */
  History::History() {
    p_history.setTerminator("End");
   }

  /**
   *  Constructor for reading a blob
   *  @param blob
   */
  History::History(Isis::Blob &blob) {
    p_history.setTerminator("End");

    stringstream os;
    char *blob_buffer = blob.getBuffer();
    for (int i = 0; i < blob.Size(); i++) {
      os << blob_buffer[i];
    }
    os >> p_history;
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

  Blob *History::toBlob(const QString &name) {
    ostringstream ostr;
    ostr << p_history;
    string histStr = ostr.str();
    int nbytes = histStr.size();

    char *buffer = new char[nbytes];
    const char *ptr = histStr.c_str();
    memcpy(&buffer[0], (void *)ptr, nbytes);

    Blob *newBlob = new Blob(name, "History");
    newBlob->setData(buffer, nbytes);
    return newBlob;
  }

  /**
   * Reads p_buffer into a pvl
   *
   * @return @b Pvl
   */
  Pvl History::ReturnHist() {
    return p_history;
  }
}
