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


// FIXME: remove these two includes!
#include <QMap>
#include <QStack>


class QToolButton;
class QLine;
class QLineEdit;
class QComboBox;

namespace Isis {
  class Brick;
  class Cube;
}

namespace Isis {
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
   *  @todo  The following fix is a bandaid- there is probably a bug in
   *         tiling class, buffer class?  The following fix is for rectangle
   *         option only, the start/end line still has a problem when going
   *         off the right side of image.
   *  @history 2006-10-20 Tracie Sucharski - If rubberband for rectangle
   *                                         option goes off the right side
   *                                         or bottom of image, set
   *                                         esamp/eline to image
   *                                         nsamps/nlines.
   *  @history 2008-05-23 Noah Hilt - Added RubberBandToolfunctionality and
   *           changed the mouseButtonReleased method. Also added a writeToCube
   *           method that is used by both the rubberBandComplete and
   *           mouseButtonRelease methods.
   *  @history 2008-06-19 Noah Hilt - Added methods and signals for saving and
   *           discarding changes to the current cube.
   *  @history 2010-06-26 Eric Hyer - uses MdiCubeViewport instead of
   *           CubeViewport.  Fixed many include issues but some still remain!
   *  @history 2010-11-15 Eric Hyer - valid rectangle dimensions for banding
   *               must now be >= 1 instead of >= 5
   *
   */
  class EditTool : public Tool {
      Q_OBJECT

    signals:
      void cubeChanged(bool); //!< Emitted when cube changed
      void save(); //!< Emitted when cube should be saved
      void saveAs(); //!< Emitted when cube should be saved as another file

    public:
      EditTool(QWidget *parent);

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
        UserDn,//!< User Selected DN value
        Null,//!< Null DN value
        Hrs,//!< High representation saturation DN value
        Lrs,//!< Low representation saturation DN value
        His,//!< High instrument saturation DN value
        Lis//!< Low instrument satruation DN value
      };

    protected:
//      QString menuName() const { return "&Options"; };
      QAction *toolPadAction(ToolPad *pad);
      QWidget *createToolBarWidget(QStackedWidget *active);
      void updateTool();

    protected slots:
      void mouseButtonRelease(QPoint p, Qt::MouseButton m);
      virtual void enableRubberBandTool();
      void rubberBandComplete();

    private slots:
      void selectValType(int index);
      void changeDn();
      void undoEdit();
      void undoAll(MdiCubeViewport *vp);
      void redoEdit();
      void save(MdiCubeViewport *vp);
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

      double p_dn; //!< DN value

      QMap <MdiCubeViewport *, QStack <Brick *> *> p_undoEdit; //!< Viewport to brick map for undo
      QMap <MdiCubeViewport *, QStack <Brick *> *> p_redoEdit; //!< Viewport to brick map for redo
      QMap <MdiCubeViewport *, int> p_saveMarker; //!< Marker for last save
  };
};

#endif
