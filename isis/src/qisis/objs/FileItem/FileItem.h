#ifndef FileItem_h
#define FileItem_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QSharedPointer>
#include <QObject>
#include <QString>

namespace Isis {

  /**
   * @brief A container for a filename to be represented as a ProjectItem
   * on the project tree.
   *
   * @author 2017-05-04 J Bonn
   *
   * @internal
   *   @history 2017-05-05 Tracie Sucharski - Renamed accessor method to match Isis standards. Moved
   *                           class to a Directory under qisis/objs.
   */
  class FileItem : public QObject {
    public:
      FileItem(const QString filename) : m_filename(filename) {};
      QString fileName() { return m_filename; };
    private:
      QString m_filename;
  };

  typedef QSharedPointer<FileItem> FileItemQsp; //!< A FileItem smart pointer

}

#endif // FileItem_h
