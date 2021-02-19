#ifndef Workspace_h
#define Workspace_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */



#include <QMdiArea>
#include <QPointer>

template< class T > class QVector;

namespace Isis {
  class Cube;
  class Image;
  class ImageList;
  class MdiCubeViewport;
  class ToolList;

  /**
  * @brief
  *
  * @ingroup Visualization Tools
  *
  * @author ????-??-?? Jeff Anderson
  *
  * @internal
  *   @history 2007-03-21 Tracie Sucharski - Changed call from fitScale to
  *                           fitScaleMinDimension so that the minimum cube
  *                           dimension is displayed in its entirety.
  *   @history 2008-05-27 Noah Hilt - Now allows cubes to be opened with additional arguments read
  *                           by the CubeAttributeInput class, specifically to open certain bands as
  *                           well as open 3 bands in RGB mode.
  *   @history 2008-12-04 Jeannie Walldren - Fixed bug in
  *                           addCubeViewport(cubename).  Added exception catch to
  *                           addCubeViewport(cube) to close the CubeViewport from the
  *                           ViewportMainWindow if it cannot be shown.
  *   @history 2009-03-27 Noah Hilt, Steven Lambright - Changed parent class from
  *                           QWorkspace to QMdiArea since QWorkspace is now an obsolete class. Also
  *                           changed how CubeViewports are created.
  *   @history 2010-04-08 Steven Lambright and Eric Hyer - Added progress bar
  *   @history 2010-06-26 Eric Hyer - Now uses MdiCubeViewport instead of
  *                           CubeViewport.  Also fixed include issues.
  *   @history 2010-11-18 Eric Hyer - addBrowseView now deletes the last viewport
  *                           in the subWindowList instead of the first one
  *   @history 2011-09-19 Steven Lambright - Fixed addBrowseView to actually close
  *                           the old viewports instead of hiding them. Fixes #418
  *   @history 2012-05-29 Steven Lambright - Now utilizes ViewportMdiSubWindow instead of
  *                           QMdiSubWindow. References #854.
  *   @history 2012-09-18 Steven Lambright - Added the selfContained option to the constructor.
  *                           This allows us to show a work space without handling the tool and
  *                           status areas externally. No longer inherits from QMdiArea because
  *                           of the need to place widgets around the mdi area.
  *   @history 2015-05-13 Ian Humphrey - Modified addCubeViewport(QString) to handle any exceptions
  *                           that are thrown within the slot. This prevents undefined behavior
  *                           when an exception is not handled within a connected slot.
  *                           References #2210.
  *   @history 2016-09-08 Tracie Sucharski - Changed imageToMdiWidget to cubeToMdiWidget.  Workspace
  *                           deals with cubes.  Since Image contains a cube, simply pass in cube
  *                           instead of Image.  This was done to handle the new IPCE container
  *                           class, Shape, which also contains a cube, but not an Image.
  *   @history 2017-09-11 Adam Goins - Added the ability to accept cubelists under any file format.
  *                           Fixes #5099.
  *   @history 2018-04-13 Christopher Combs - Added .lbl files to the list of single-cube file-extensions
  *                           to check before reading a cube list in addCubeViewport. Fixes #5350.
  *  @history 2018-09-12 Adam Goins - Modified logic to attempt to open the file as a cube or
  *                          detached label first, if that fails attempt to open it as a cube list
  *                          and if that fails, throw an error to the user. This allows cubes and
  *                          cube lists to be saved under any extension and opened. Fixes #5439,
  *                          Fixes #5476.
  */
  class Workspace : public QWidget {
      Q_OBJECT

    public:
      /**
       * Constructor for Workspace
       *
       * @param selfContained if this Workspace should be self contained or note
       * @param parent The parent QWidget, defaults to 0
       *
       */
      Workspace(bool selfContained, QWidget *parent = 0);

      /**
       * Constructor for Workspace
       *
       * @param other The other Workspace to load from.
       */
      Workspace(const Workspace &other);

      /**
       * Deconstructor
       *
       */
      virtual ~Workspace();

      /**
       * This method returns a Vector of MdiCubeViewports
       *
       * @return QVector a vector of MdiCubeViewports
       */
      QVector< MdiCubeViewport * > * cubeViewportList();

      /**
       * Is equal to comparsion
       *
       * @param other The Workspace to compare against
       * @return bool True of False if they're equal to eachother .
       */
      Workspace &operator=(Workspace other);

      /**
       * Adds a list of Images to a viewport
       *
       * @param images The ImageList of images to add to the Workspace
       */
      void addImages(ImageList *images);

      /**
       * Confirms that the user wishes toc lose the Workspace
       *
       * @return True of False if the user wishes to close the Workspace
       */
      bool confirmClose();

      /**
       * Converts a cube to an MdiWidget
       *
       * @param cube The cube to reference
       * @return QWidget The widget associated with the cube
       */
      QWidget *cubeToMdiWidget(Cube *cube);

      /**
       * This method returns the QMdiArea
       *
       * @return QMdiArea the Area of the mdi
       */
      QMdiArea *mdiArea();

    signals:
      /**
       * Signal triggered when a Cube is added to the Workspace
       */
      void cubeViewportAdded(MdiCubeViewport *);

      /**
       * Signal triggered when a Cube is activated in the Workspace
       *
       */
      void cubeViewportActivated(MdiCubeViewport *);

    public slots:

      /**
       * Method adds the name of a cube into Workspace as a CubeViewport
       *
       * @param cubename The cube to be added to the Workspace
       */
      void addCubeViewport(QString cubename);

      /**
       * Method adds cubes into Workspace as a CubeViewport from a list of cubes.
       *
       * @param cubename The name of the cube list file.
       */
      void addCubeViewportFromList(QString cubelist);


      /**
       * Method adds a cube into the Workspace as a CubeViewport.
       *
       * @param cube The cube to be added into the Workspace.
       */
      MdiCubeViewport *addCubeViewport(Cube *cube);

      /**
       * Method is called to add a Cube from BrowseView.
       *
       * @param cube The cube being browsed
       */
      void addBrowseView(QString cube);

    protected slots:

      /**
       * This method activates the Viewport.
       *
       * @param w The subwindow to activate
       */
      void activateViewport(QMdiSubWindow *w);

    private:
      //! The mdi area
      QPointer<QMdiArea> m_mdi;
      //! List of cube viewports
      QVector< MdiCubeViewport * > * m_cubeViewportList;
      //! List of all of the tools.
      ToolList *m_tools;
  };
};

#endif
