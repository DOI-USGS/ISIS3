/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <iostream>
#include <fstream>
#include <sstream>

#include "StretchBlob.h"
#include "IException.h"

using namespace std;
namespace Isis {

  /**
   * Default constructor
   */
  StretchBlob::StretchBlob() {
    m_stretch = CubeStretch("CubeStretch", "Stretch");
  }


  /**
   * Construct a StretchBlob from a CubeStretch.
   */
  StretchBlob::StretchBlob(CubeStretch stretch) {
    m_stretch = CubeStretch(stretch);
  }


  /**
   * Construct a StretchBlob with provided name.
   *
   * @param name Name to use for Stretch
   */
  StretchBlob::StretchBlob(QString name) {
    m_stretch = CubeStretch(name, "Stretch");
  }

  StretchBlob::StretchBlob(Blob blob) {
    stringstream os;
    char *buff = blob.getBuffer();
    std::string stringFromBuffer(buff);
    m_stretch = CubeStretch(blob.Name(), blob.Type());
    m_stretch.Parse(QString::fromStdString(stringFromBuffer));
  }


  /**
   * Default Destructor
   */
  StretchBlob::~StretchBlob() {
  }


  CubeStretch StretchBlob::getStretch() {
    return m_stretch;
  }


  Isis::Blob *StretchBlob::toBlob() {
    Isis::Blob *blob = new Blob(m_stretch.getName(), m_stretch.getType());
    blob->setData((char*)m_stretch.Text().toStdString().c_str(), m_stretch.Text().toStdString().size());
    return blob;
  }

//  /**
//   * Read saved Stretch data from a Cube into this object.
//   *
//   * This is called by Blob::Read() and is the actual data reading function
//   * ultimately called when running something like cube->read(stretch);
//   *
//   * @param is input stream containing the saved Stretch information
//   */
//  void StretchBlob::ReadData(std::istream &is) {
//    // Set the Stretch Type
//     m_stretch.setType(p_blobPvl["StretchType"][0]);
//     m_stretch.setBandNumber(p_blobPvl["BandNumber"][0].toInt());
//
//     // Read in the Stretch Pairs
//     streampos sbyte = p_startByte - 1;
//     is.seekg(sbyte, std::ios::beg);
//     if (!is.good()) {
//       QString msg = "Error preparing to read data from " + m_stretch.getType() +
//                    " [" + p_blobName + "]";
//       throw IException(IException::Io, msg, _FILEINFO_);
//     }
//
//     char *buf = new char[p_nbytes+1];
//     memset(buf, 0, p_nbytes + 1);
//
//     is.read(buf, p_nbytes);
//
//     // Read buffer data into a QString so we can call Parse()
//     std::string stringFromBuffer(buf);
//     m_stretch.Parse(QString::fromStdString(stringFromBuffer));
//
//     delete [] buf;
//
//     if (!is.good()) {
//       QString msg = "Error reading data from " + p_type + " [" +
//                    p_blobName + "]";
//       throw IException(IException::Io, msg, _FILEINFO_);
//     }
//   }
//

} // end namespace isis
