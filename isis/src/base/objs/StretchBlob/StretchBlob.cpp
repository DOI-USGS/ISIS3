/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <iostream>
#include <fstream>

#include "StretchBlob.h"
#include "IException.h"

using namespace std;
namespace Isis {

  /**
   * Default constructor
   */
  StretchBlob::StretchBlob() : Blob("CubeStretch", "Stretch"), m_stretch() {
  }


  /**
   * Construct a StretchBlob from a CubeStretch.
   */
  StretchBlob::StretchBlob(CubeStretch stretch) : Blob("CubeStretch", "Stretch"), m_stretch(stretch){
    p_blobPvl["Name"] = m_stretch.getName();
    p_blobPvl += PvlKeyword("StretchType", m_stretch.getType());
    p_blobPvl += PvlKeyword("BandNumber", QString::number(m_stretch.getBandNumber()));
  }


  /**
   * Construct a StretchBlob with provided name.
   *
   * @param name Name to use for Stretch
   */
  StretchBlob::StretchBlob(QString name) : Blob(name, "Stretch"), m_stretch(name) {
  }


  /**
   * Default Destructor
   */
  StretchBlob::~StretchBlob() {
  }


  CubeStretch StretchBlob::getStretch() {
    return m_stretch;
  }

  /**
   * Read saved Stretch data from a Cube into this object. 
   *  
   * This is called by Blob::Read() and is the actual data reading function 
   * ultimately called when running something like cube->read(stretch);  
   * 
   * @param is input stream containing the saved Stretch information
   */
  void StretchBlob::ReadData(std::istream &is) {
    // Set the Stretch Type
     m_stretch.setType(p_blobPvl["StretchType"][0]);
     m_stretch.setBandNumber(p_blobPvl["BandNumber"][0].toInt());

     // Read in the Stretch Pairs
     streampos sbyte = p_startByte - 1;
     is.seekg(sbyte, std::ios::beg);
     if (!is.good()) {
       QString msg = "Error preparing to read data from " + m_stretch.getType() +
                    " [" + p_blobName + "]";
       throw IException(IException::Io, msg, _FILEINFO_);
     }

     char *buf = new char[p_nbytes+1];
     memset(buf, 0, p_nbytes + 1);

     is.read(buf, p_nbytes);

     // Read buffer data into a QString so we can call Parse()
     std::string stringFromBuffer(buf);
     m_stretch.Parse(QString::fromStdString(stringFromBuffer));

     delete [] buf;

     if (!is.good()) {
       QString msg = "Error reading data from " + p_type + " [" +
                    p_blobName + "]";
       throw IException(IException::Io, msg, _FILEINFO_);
     }
   }


  /**
   *  Initializes for writing stretch to cube blob
   */
  void StretchBlob::WriteInit() {
    p_nbytes = m_stretch.Text().toStdString().size(); 
  }


  /**
   * Writes the stretch information to a cube. 
   *  
   * This is called by Blob::write() and is ultimately the function 
   * called when running something like cube->write(stretch);  
   *  
   * @param os output stream to write the stretch data to.
   */
  void StretchBlob::WriteData(std::fstream &os) {
    os.write(m_stretch.Text().toStdString().c_str(), p_nbytes);
  }

} // end namespace isis


