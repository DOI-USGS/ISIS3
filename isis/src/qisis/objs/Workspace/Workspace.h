#ifndef Workspace_h
#define Workspace_h
/**
 * @file
 * $Revision: 1.9 $
 * $Date: 2010/06/28 08:36:36 $
 *
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


#include <QMdiArea>

template< class T > class QVector;

namespace Isis {
  class Cube;
}

namespace Isis {
  class MdiCubeViewport;

  /**
  * @brief
  *
  * @ingroup Visualization Tools
  *
  * @author ????-??-?? Jeff Anderson
  *
  * @internal
  * @history  2007-03-21  Tracie Sucharski - Changed call from fitScale to
  *                           fitScaleMinDimension so that the minimum cube
  *                           dimension is displayed in its entirety.
  * @history  2008-05-27  Noah Hilt - Now allows cubes to be
  *           opened with additional arguments read by the
  *           CubeAttributeInput class, specifically to open
  *           certain bands as well as open 3 bands in RGB mode.
  * @history 2008-12-04 Jeannie Walldren - Fixed bug in
  *          addCubeViewport(cubename).  Added exception catch to
  *          addCubeViewport(cube) to close the CubeViewport from
  *          the ViewportMainWindow if it cannot be shown.
  * @history 2009-03-27 Noah Hilt, Steven Lambright - Changed parent class from
  *          QWorkspace to QMdiArea since QWorkspace is now an obsolete class.
  *          Also changed how CubeViewports are created.
  * @history 2010-04-08 Steven Lambright and Eric Hyer - Added progress bar
  * @history 2010-06-26 Eric Hyer - Now uses MdiCubeViewport instead of
  *          CubeViewport.  Also fixed include issues.
  * @history 2010-11-18 Eric Hyer - addBrowseView now deletes the last viewport
  *              in the subWindowList instead of the first one
  * @history 2011-09-19 Steven Lambright - Fixed addBrowseView to actually close
  *                         the old viewports instead of hiding them. Fixes #418
  */
  class Workspace : public QMdiArea {
      Q_OBJECT

    public:
      Workspace(QWidget *parent = 0);
      Workspace(const Workspace &other);
      virtual ~Workspace();
      QVector< MdiCubeViewport * > * cubeViewportList();
      const Workspace &operator=(Workspace other);

    signals:
      void cubeViewportAdded(MdiCubeViewport *);
      void cubeViewportActivated(MdiCubeViewport *);

    public slots:
      void addCubeViewport(QString cube);
      MdiCubeViewport *addCubeViewport(Cube *cube);

      void addBrowseView(QString cube);

    protected slots:
      void activateViewport(QMdiSubWindow *w);

    private:
      QVector< MdiCubeViewport * > * p_cubeViewportList;
  };
};

#endif
