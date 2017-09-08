#ifndef EditTool_h
#define EditTool_h

/**
 * @file
 * $Revision: 1.8 $
 * $Date: 2010/06/28 09:00:39 $
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

#include "Tool.h"


#include <QMap>
#include <QStack>


class QToolButton;
class QLine;
class QLineEdit;
class QComboBox;

namespace Isis {
  class Brick;
  class Cube;
  class MdiCubeViewport;

  /**
   * @brief Interactive image edit tool
   *
   * This tool is part of the Qisis namespace and allows interactive editing
   * of displayed images.
   *
   * @ingroup Visualization Tools
   *
   * @author  2006-06-09  Tracie Sucharski
   *
   * @internal
   *   @history 2006-10-20 Tracie Sucharski - If rubberband for rectangle
   *                           option goes off the right side
   *                           or bottom of image, set
   *                           esamp/eline to image
   *                           nsamps/nlines.
   *   @history 2008-05-23 Noah Hilt - Added RubberBandToolfunctionality and
   *                           changed the mouseButtonReleased method. Also added
   *                           a writeToCube method that is used by both the
   *                           rubberBandComplete and mouseButtonRelease methods.
   *   @history 2008-06-19 Noah Hilt - Added methods and signals for saving and
   *                           discarding changes to the current cube.
   *   @history 2010-06-26 Eric Hyer - uses MdiCubeViewport instead of
   *                           CubeViewport.  Fixed many include issues but some
   *                           still remain!
   *   @history 2010-11-15 Eric Hyer - valid rectangle dimensions for banding
   *                           must now be >= 1 instead of >= 5
   *   @history 2011-09-15 Steven Lambright - Enumerated values no longer
   *                           conflict with global variable names and increased
   *                           safety with usage of the valType combo box to make
   *                           it less likely to break in the future. Fixes #345.
   *   @history  2012-05-24 Steven Lambright - Minor changes to support prompting to save on exit
   *                            once again (this has been broken for a very long time). The prompt
   *                            now appears if you have edited your file but not saved it - not when
   *                            clicking "Save." This was a minimal fix (I left a lot of problems
   *                            to be solved at a later date). Fixes #854.
   *   @history  2017-08-11 Adam Goins - Added a line of code to recreate a cube with "r" permissions
   *                            attempting to open it with "rw" permission failed. This fixes an issue
   *                            where the cube would segfault if it was being edited without "w" permission.
   *                            Fixes # 2097 
   */
  class EditTool : public Tool {
      Q_OBJECT

    public:
      /**
       * Enum for possible shapes
       */
      enum EditShape {
        Point, //!< point
        HorizLine,//!< horizontal line
        VertLine,//!< vertical line
        StartEndLine,//!< start-end line
        Rectangle//!< rectangle
      };


      /**
       * Enum for DN values
       */
      enum ReplacementValue {
        UserDnComboValue, //!< User Selected DN value
        NullComboValue, //!< Null DN value
        HrsComboValue, //!< High representation saturation DN value
        LrsComboValue, //!< Low representation saturation DN value
        HisComboValue, //!< High instrument saturation DN value
        LisComboValue //!< Low instrument satruation DN value
      };

      EditTool(QWidget *parent);

      void addTo(Workspace *);

    signals:
      void cubeChanged(bool); //!< Emitted when cube changed
      void save(); //!< Emitted when cube should be saved
      void saveAs(); //!< Emitted when cube should be saved as another file

    protected:
      QAction *toolPadAction(ToolPad *pad);
      QWidget *createToolBarWidget(QStackedWidget *active);
      void updateTool();

    protected slots:
      void mouseButtonRelease(QPoint p, Qt::MouseButton m);
      virtual void enableRubberBandTool();
      void rubberBandComplete();

    private slots:
      void listenToViewport(MdiCubeViewport *);
      void selectValType(int index);
      void changeDn();
      void undoEdit();
      void undoAll(CubeViewport *vp);
      void redoEdit();
      void save(CubeViewport *vp);
      void removeViewport(QObject *vp);

    private:
      QList<QPoint *> *LineToPoints(const QLine &line);
      void writeToCube(int iesamp, int issamp, int ieline, int isline, QList<QPoint *> *linePts);
      QComboBox *p_shapeComboBox; //!< Shape combobox
      QComboBox *p_valTypeComboBox; //!< Value type combobox
      QLineEdit *p_dnLineEdit; //!< DN edit line
      QToolButton *p_undoButton; //!< Undo button
      QToolButton *p_redoButton; //!< Redo button
      QToolButton *p_saveButton; //!< Save button
      QToolButton *p_saveAsButton; //!< Save as button
QWidget *m_container;
      double p_dn; //!< DN value

      QMap <CubeViewport *, QStack <Brick *> *> p_undoEdit; //!< Viewport to brick map for undo
      QMap <CubeViewport *, QStack <Brick *> *> p_redoEdit; //!< Viewport to brick map for redo
      QMap <CubeViewport *, int> p_saveMarker; //!< Marker for last save
  };
};

#endif
