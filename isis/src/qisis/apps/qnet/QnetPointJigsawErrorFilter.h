#ifndef QnetPointJigsawErrorFilter_h
#define QnetPointJigsawErrorFilter_h
/**
 * @file
 * $Date: 2010/06/03 20:13:53 $
 * $Revision: 1.6 $
 *
 *  Unless noted otherwise, the portions of Isis written by the USGS are public domain. See
 *  individual third-party library and package descriptions for intellectual property information,
 *  user agreements, and related information.
 *
 *  Although Isis has been used by the USGS, no warranty, expressed or implied, is made by the
 *  USGS as to the accuracy and functioning of such software and related material nor shall the
 *  fact of distribution constitute any such warranty, and no responsibility is assumed by the
 *  USGS in connection therewith.
 *
 *  For additional information, launch $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *  in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *  http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *  http://www.usgs.gov/privacy.html.
 */



#include <QAction>
#include <QDialog>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QList>
#include "QnetFilter.h"


namespace Isis {
  /**
   * Defines the Jigsaw Error filter for the QnetNavTool's Points
   * section.  The user may enter values for Less Than and/or
   * Greater Than. This class is designed to remove points from
   * the current filtered list that lie outside of the given
   * range.
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
      QnetPointJigsawErrorFilter(QWidget *parent = 0);
      virtual void filter();

    public slots:

    private:
      QCheckBox *p_lessThanCB;
      QCheckBox *p_greaterThanCB;
      QLineEdit *p_lessErrorEdit;
      QLineEdit *p_greaterErrorEdit;

    private slots:
      void clearEdit();
  };
};

#endif
