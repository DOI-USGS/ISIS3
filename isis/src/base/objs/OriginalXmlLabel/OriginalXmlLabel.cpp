/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <fstream>
#include <sstream>
#include <string>

#include <QByteArray>
#include <QFile>

#include "OriginalXmlLabel.h"
#include "Application.h"
#include "PvlObject.h"

using namespace std;
namespace Isis {
  /**
   * Constructors a default OriginalXmlLabel with an empty label.
   */
  OriginalXmlLabel::OriginalXmlLabel() : Isis::Blob("IsisCube", "OriginalXmlLabel") {
    p_blobPvl += Isis::PvlKeyword("ByteOrder", "NULL");
  }


  /**
   * Constructs an OriginalXmlLabel from a cube label file.
   *
   * @param file Xml file to read labels from
   */
  OriginalXmlLabel::OriginalXmlLabel(const QString &file) :
                    Isis::Blob("IsisCube", "OriginalXmlLabel") {
    p_blobPvl += Isis::PvlKeyword("ByteOrder", "NULL");
    Blob::Read(file);
  }


  /**
   * Destructor
   */
  OriginalXmlLabel::~OriginalXmlLabel() {
  }


  /**
   * Read the original label from an Xml file.
   * 
   * @param FileName The Xml file containing the original label.
   * 
   * @throws IException::Io "Could not open label file."
   * @throws IException::Unknown "XML read/parse error in file."
   */
  void OriginalXmlLabel::readFromXmlFile(const FileName &xmlFileName) {
    QFile xmlFile(xmlFileName.expanded());
     if ( !xmlFile.open(QIODevice::ReadOnly) ) {
       QString msg = "Could not open label file [" + xmlFileName.expanded() +
                     "].";
       throw IException(IException::Io, msg, _FILEINFO_);
     }

     QString errmsg;
     int errline, errcol;
     if ( !m_originalLabel.setContent(&xmlFile, false, &errmsg, &errline, &errcol) ) {
       xmlFile.close();
       QString msg = "XML read/parse error in file [" + xmlFileName.expanded()
            + "] at line [" + toString(errline) + "], column [" + toString(errcol)
            + "], message: " + errmsg;
       throw IException(IException::Unknown, msg, _FILEINFO_);
     }

     xmlFile.close();
  }


  /**
   * Read the xml file data from an input stream.
   * 
   * @param stream The input stream to read from.
   * 
   * @throws IException::Unknown "XML read/parse error when parsing original label."
   * 
   * @see Blob::Read(const Pvl &pvl, std::istream &is)
   */
  void OriginalXmlLabel::ReadData(std::istream &stream) {
    // Use Blob's ReadData to fill p_buffer
    Blob::ReadData(stream);

    // Setup variables for error reproting in QT's xml parser
    QString errorMessage;
    int errorLine;
    int errorColumn;

    // Attempt to use QT's xml parser to internalize the label
    if ( !m_originalLabel.setContent( QByteArray(p_buffer, p_nbytes) ) ) {
      QString msg = "XML read/parse error when parsing original label. "
                    "Error at line [" + toString(errorLine) +
                    "], column [" + toString(errorColumn) +
                    "]. Error message: " + errorMessage;
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }
  }


  /**
   * Prepare to write the label out.
   * 
   * @see Blob::Write
   */
  void OriginalXmlLabel::WriteInit() {
    p_nbytes = m_originalLabel.toByteArray(0).size();

    if (Isis::IsLsb()) {
      p_blobPvl["ByteOrder"] = Isis::ByteOrderName(Isis::Lsb);
    }
    else {
      p_blobPvl["ByteOrder"] = Isis::ByteOrderName(Isis::Msb);
    }
  }


  /**
   * Write the label out to a stream.
   * 
   * @param os The stream to write the label out to.
   * 
   * @see Blob::Write
   */
  void OriginalXmlLabel::WriteData(std::fstream &os) {
    QByteArray labelByteArray = m_originalLabel.toByteArray(0);
    os.write( labelByteArray.data(), labelByteArray.size() );
  }


  /**
   * Returns the original Xml label.
   *
   * @return @b QDomDocument The parsed original label
   */
  const QDomDocument &OriginalXmlLabel::ReturnLabels() const {
    return m_originalLabel;
  }
}
