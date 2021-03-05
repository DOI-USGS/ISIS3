
#ifndef IsisXMLChTrans_h
#define IsisXMLChTrans_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <string>
#include <iostream>
#include <cstdlib>

namespace XERCES = XERCES_CPP_NAMESPACE;

//  This class converts from the internal XMLCh character format to simple c strings

/**
 * @author ????-??-?? Unknown
 *
 * @internal
 */
class XMLChTrans {
  public :
    //  Constructors and Destructor
    XMLChTrans(const XMLCh *const toTranscode) {
      // Call the private transcoding method
      fLocalForm = XERCES::XMLString::transcode(toTranscode);
    }

    ~XMLChTrans() {
      delete [] fLocalForm;
    }

    //  Getter methods
    const char *localForm() const {
      return fLocalForm;
    }

  private :
    char   *fLocalForm;
};


inline std::ostream &operator<<(std::ostream &target, const XMLChTrans &toDump) {
  target << toDump.localForm();
  return target;
}

#endif
