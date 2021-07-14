/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "Color.h"

#include <QColor>
#include <QString>

#include "IException.h"

namespace Isis {
  
  /**
   * Converts a QString to its QColor
   * 
   * @returns A QColor given a QString
   */
  QColor Color::fromRGBAString(QString string) {
    QColor result;

    if (colorRGBAFormat().exactMatch(string)) {
      result = QColor(string.mid(1, 2).toInt(NULL, 16), string.mid(3, 2).toInt(NULL, 16),
                      string.mid(5, 2).toInt(NULL, 16), string.mid(7, 2).toInt(NULL, 16));
    }

    return result;
  }


  /**
   * Convert a QColor to its QString
   * 
   * @returns A QString given a QColor
   */
  QString Color::toRGBAString(QColor color) {
    QString result;

    if (color.isValid()) {
      result = QString("#%1%2%3%4")
          .arg(color.red(), 2, 16, QChar('0'))
          .arg(color.green(), 2, 16, QChar('0'))
          .arg(color.blue(), 2, 16, QChar('0'))
          .arg(color.alpha(), 2, 16, QChar('0'));
    }
    else {
      throw IException(IException::Unknown,
          "Can not convert an invalid color to an RGBA string.  There is no string representation "
            "of an invalid color.",
          _FILEINFO_);
    }

    return result;
  }


  /**
   * Get the colorRGBAFormat
   * 
   * @returns A QRegExp of the color format
   */
  QRegExp Color::colorRGBAFormat() {
    return QRegExp("^#[0-9a-fA-F]{8}$");
  }
}
