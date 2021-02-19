#ifndef ProgressWidget_H
#define ProgressWidget_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QProgressBar>

namespace Isis {

  /**
   * @brief Warning Widget for ipce
   *
   * @author 2012-05-29 Steven Lambright and Tracie Sucharski
   *
   * @internal
   */
  class ProgressWidget : public QWidget {
      Q_OBJECT
    public:
      ProgressWidget(QWidget *parent = 0);
      virtual ~ProgressWidget();


    private:
      Q_DISABLE_COPY(ProgressWidget);
      QList<QProgressBar *> m_progressBars;
  };
}

#endif
