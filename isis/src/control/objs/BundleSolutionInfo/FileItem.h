#ifndef FileItem_h
#define FileItem_h
/**
 * @file
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

#include <QSharedPointer>
#include <QObject>

namespace Isis {

  /**
   * @brief A container for a filename to be represented as a ProjectItem
   * on the project tree.
   *
   * @author 2017-05-04 J Bonn
   *
   * @internal
   */
  class FileItem : public QObject {
    public:
      FileItem(const QString filename) : m_filename(filename) {};
      QString getFileName() { return m_filename; };
    private:
      QString m_filename;
  };

  typedef QSharedPointer<FileItem> FileItemQsp; //!< A FileItem smart pointer

}

#endif // FileItem_h
