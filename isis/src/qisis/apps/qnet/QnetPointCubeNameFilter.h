#ifndef QnetPointCubeNameFilter_h
#define QnetPointCubeNameFilter_h
/**
 * @file
 * $Date: 2010/06/03 20:13:53 $
 * $Revision: 1.2 $
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
#include <QList>
#include <QListWidget>
#include "QnetFilter.h"


namespace Isis {
  /**
   * Defines the Cube Name filter for the QnetNavTool's
   * Points section. The user must choose from a list of cubes.
   * This class is designed to remove points from the current
   * filtered list if no measures match the chosen image.
   *
   * @author 2009-01-26 Jeannie Walldren
   * @internal
   *   @history 2009-01-26 Jeannie Walldren - Original Version
   *   @history 2010-06-03 Jeannie Walldren - Initialized pointer
   *                          to null in constructor
   */

  class QnetPointCubeNameFilter : public QnetFilter {
      Q_OBJECT

    public:
      QnetPointCubeNameFilter(QWidget *parent = 0);
      virtual void filter();

    public slots:
      void createCubeList();

    private:
      QListWidget *p_listBox;
  };
};

#endif
