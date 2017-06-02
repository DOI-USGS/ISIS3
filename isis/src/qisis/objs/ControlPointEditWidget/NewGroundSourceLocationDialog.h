#ifndef NewGroundSourceLocationDialog_h
#define NewGroundSourceLocationDialog_h

/**
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */
 

#include <QCheckBox>
#include <QDir>
#include <QFileDialog>
#include <QPointer>
#include <QString>
#include <QWidget>

namespace Isis {

  /**
  * @brief Dialog used by ControlPointEditWidget to select a new location for ground source files. 
  * Gives option of using new location for all subsequent ground points and whether to update the 
  * control net to reflect the new location.
  *
  * @ingroup Visualization Tools
  *
  * @author 2017-01-05 Tracie Sucharski
  *
  * @internal
  *   @history 2017-01-05 Tracie Sucharski - Initial Version
  */
  class NewGroundSourceLocationDialog : public QFileDialog {
      Q_OBJECT
    public:
      NewGroundSourceLocationDialog(QString title, QString &directory, QWidget *parent = 0);
      
//    QDir newGroundSourceLocation();
      bool changeAllGroundSourceLocation();
      bool changeControlNet();
      
    private:
      QPointer<QCheckBox> m_changeAllGround;   //!< Change location of all subsequent ground control points
      QPointer<QCheckBox> m_changeControlNet;  //!< Change location of ground source in the control network
  };
};

#endif

