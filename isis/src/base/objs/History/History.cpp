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

    char *blob_buffer = blob.getBuffer();
    p_bufferSize = blob.Size();
    p_histBuffer = new char[p_bufferSize];
    memcpy(p_histBuffer, blob_buffer, p_bufferSize);
  }

  //! Destructor
  History::~History() {
    if (p_histBuffer != NULL) {
      delete [] p_histBuffer;
    }
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
   * Converts a history object into a new blob object
   *
   * @param name Name of the History object to create
   *
   * @return @b Blob
   */
  Blob *History::toBlob(const QString &name) {
    ostringstream ostr;
    if (p_bufferSize > 0) ostr << std::endl;
    ostr << p_history;
    string histStr = ostr.str();
    int bytes = histStr.size();

    char *temp = p_histBuffer;
    p_histBuffer = new char[p_bufferSize+bytes];
    if (temp != NULL) memcpy(p_histBuffer, temp, p_bufferSize);
    const char *ptr = histStr.c_str();
    memcpy(&p_histBuffer[p_bufferSize], (void *)ptr, bytes);
    p_bufferSize += bytes;

    if (temp != NULL) delete [] temp;

    Blob *newBlob = new Blob(name, "History");
    newBlob->setData(p_histBuffer, p_bufferSize);
    return newBlob;
  }

  /**
   * Reads p_histBuffer into a pvl
   *
   * @return @b Pvl
   */
  Pvl History::ReturnHist() {
    Pvl pvl;
    stringstream os;
    for (int i = 0; i < p_bufferSize; i++) os << p_histBuffer[i];

    for (int i = 0; i < p_history.objects(); i++) {
      os << p_history.object(i);
    }
    os >> pvl;
    return pvl;
  }
}
