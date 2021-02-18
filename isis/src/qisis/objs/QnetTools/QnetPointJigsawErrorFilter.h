#ifndef QnetPointJigsawErrorFilter_h
#define QnetPointJigsawErrorFilter_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "QnetFilter.h"

class QCheckBox;
class QLineEdit;

namespace Isis {
  /**
   * Defines the Jigsaw Error filter for the QnetNavTool's Points
   * section.  The user may enter values for Less Than and/or
   * Greater Than. This class is designed to remove points from
   * the current filtered list that lie outside of the given
   * range.
   *
   * @author ????-??-?? Unknown
   *
   * @internal
   *   @history 2009-01-08 Jeannie Walldren - Modified filter()
   *                          method to remove new filter points
   *                          from the existing filtered list.
   *   @history 2010-06-02 Jeannie Walldren - Modified clearEdit()
   *                          method to disable the line edit if
   *                          the corresponding check box is not
   *                          checked.
   *   @history 2010-06-03 Jeannie Walldren - Initialized pointers
   *                          to null in constructor.
   *   @history 2010-07-14 Tracie Sucharski - ControlPoint::MaximumError
   *                          renamed to MaximumResiudal.
   *   @history 2011-07-11 Tracie Sucharski - Renamed from QnetPointErrorFilter.
   */
  class QnetPointJigsawErrorFilter : public QnetFilter {
      Q_OBJECT

    public:
      QnetPointJigsawErrorFilter(QnetNavTool *navTool, QWidget *parent = 0);
      virtual void filter();

    private slots:
      void clearEdit();

    private:
      QCheckBox *m_lessThanCB;
      QCheckBox *m_greaterThanCB;
      QLineEdit *m_lessErrorEdit;
      QLineEdit *m_greaterErrorEdit;

  };
};

#endif
