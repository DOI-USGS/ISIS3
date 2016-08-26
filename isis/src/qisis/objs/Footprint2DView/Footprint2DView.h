#ifndef Footprint2DView_h
#define Footprint2DView_h
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
#include <QList>
#include <QMap>
#include <QSize>

#include "AbstractProjectItemView.h"

class QAction;
class QEvent;
class QToolBar;
class QWidgetAction;

namespace Isis {
  
  class Image;
  class MosaicSceneWidget;
  class ToolPad;

  /**
   * View for displaying footprints of images in a QMos like way.
   *
   * @author 2016-01-13 Jeffrey Covington
   * 
   * @internal
   *   @history 2016-01-13 Jeffrey Covington - Original version.
   *   @history 2016-06-27 Ian Humphrey - Minor updates to documentation, checked coding standards.
   *                           Fixes #4004.
   *   @history 2016-08-25 Adam Paquette - Updated documentation. Fixes #4299.
   */
  class Footprint2DView : public AbstractProjectItemView {

    Q_OBJECT

    public:
      Footprint2DView(QWidget *parent=0);
      ~Footprint2DView();

      virtual QList<QAction *> permToolBarActions();
      virtual QList<QAction *> activeToolBarActions();
      virtual QList<QAction *> toolPadActions();
 
      QSize sizeHint() const;

    protected:
      bool eventFilter(QObject *watched, QEvent *event);

    private slots:
      void onItemAdded(ProjectItem *item);
      void onQueueSelectionChanged();

    private:
      MosaicSceneWidget *m_sceneWidget; //!< The scene widget
      QMap<Image *, ProjectItem *> m_imageItemMap; //!< Maps images to their items

      QToolBar *m_permToolBar; //!< The permanent tool bar
      QToolBar *m_activeToolBar; //!< The active tool bar
      ToolPad *m_toolPad; //!< The tool pad

      QWidgetAction *m_activeToolBarAction; //!< Stores the active tool bar
  };
}

#endif
