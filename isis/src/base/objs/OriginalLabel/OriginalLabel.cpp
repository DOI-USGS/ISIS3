/**
 * @file
 * $Revision: 1.5 $
 * $Date: 2010/05/14 19:20:28 $
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
#include <fstream>
#include <sstream>
#include <string>
#include "OriginalLabel.h"
#include "Application.h"
#include "PvlObject.h"

using namespace std;
namespace Isis {
  /**
   * Constructor for creating an original blob with a given name
   */
  OriginalLabel::OriginalLabel() : Isis::Blob("IsisCube", "OriginalLabel") {
    m_originalLabel.setTerminator("");
  }

  /**
   * Constructor for creating an original blob with a given name and file to
   * read labels from.
   *
   * @param file File to read labels from
   */
  OriginalLabel::OriginalLabel(const QString &file) :
    Isis::Blob("IsisCube", "OriginalLabel") {
    Blob::Read(file);
  }

  /**
   * Constructor for creating an original blob with a given name and Pvl
   * container.
   *
   * @param pvl  Pvl containing labels of the source
   */
  OriginalLabel::OriginalLabel(Pvl pvl) : Isis::Blob("IsisCube", "OriginalLabel") {
    m_originalLabel = pvl;
  }

  // Destructor
  OriginalLabel::~OriginalLabel() {
  }

  /**
   * Prepare labels for writing to the output cube.
   */
  void OriginalLabel::WriteInit() {
    ostringstream ostr;
    if(p_nbytes > 0) ostr << std::endl;

    // store labels
    ostr << m_originalLabel;
    string orglblStr = ostr.str();
    int bytes = orglblStr.size();

    // Copy label data to bytes variable
    char *temp = p_buffer;
    p_buffer = new char[p_nbytes+bytes];
    if(temp != NULL) memcpy(p_buffer, temp, p_nbytes);
    const char *ptr = orglblStr.c_str();
    memcpy(&p_buffer[p_nbytes], (void *)ptr, bytes);
    p_nbytes += bytes;

    if(temp != NULL) delete [] temp;
  }

  /**
   * Returns the labels in a Pvl object
   *
   * @return (Isis::Pvl) original labels
   */
  Pvl OriginalLabel::ReturnLabels() {
    Pvl pvl;
    stringstream os;
    for(int i = 0; i < p_nbytes; i++) os << p_buffer[i];
    os >> pvl;
    return pvl;
  }
}
