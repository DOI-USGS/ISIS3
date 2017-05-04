#ifndef BundleObservationView_h
#define BundleObservationView_h
/**
 * @file
 * $Date$
 * $Revision$
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

#include <QWidgetAction>
#include <QList>
#include <QMap>
#include <QSize>

#include "AbstractProjectItemView.h"


class QAction;
class QEvent;
class QToolBar;
class QWidgetAction;



namespace Isis{

class BundleObservation;
class Directory;
class Project;



/**
 * View for displaying BundleOutput results
 *
 * @author 2017-05-01 Tyler Wilson
 *
 * @internal
 *   @history 2017-05-01 Tyler Wilson - Original version.
 */

  class BundleObservationView : public AbstractProjectItemView
  {
    Q_OBJECT
    public:
    BundleObservationView(BundleObservation * BundleObservation, QWidget *parent=0);
    //BundleObservationView(const BundleObservationView &other);
    ~BundleObservationView();



  private:


  protected:

  private slots:






  };

}
#endif
