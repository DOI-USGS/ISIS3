#ifndef BandTool_h
#define BandTool_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Tool.h"

// FIXME: remove this include
#include "Pvl.h"

class QAction;
class QComboBox;
class QHBoxLayout;
class QLabel;
class QRadioButton;
class QSpinBox;
class QStackedWidget;

namespace Isis {
  class Cube;
  class ToolPad;

  /**
   * @author ????-??-?? Unknown
   * @internal
   *   @history 2010-06-26 Eric Hyer - Now uses MdiCubeViewport instead of
   *                           CubeViewport
   *   @history 2010-12-01 Steven Lambright - No longer calls setBandBin() if
   *                           the viewport doesn't change.
   *   @history 2012-09-18 Steven Lambright - setBandBin() now takes a Cube instead of a label.
   *                           This is because the method needs the band count, which is no longer
   *                           always readily available in the labels.
   *   @history 2013-11-04 Janet Barrett - Fixed the setBandBin and setList
   *            methods so that they retain the band bin settings for the current
   *            viewport. The band bin combo box will now be set to the user
   *            setting when a viewport is reactivated. Before this change was
   *            made, the band bin combo box would always reset to 'Center' when
   *            the user reactivated a viewport. Fixes #1612.
   */
  class BandTool : public Tool {
      Q_OBJECT

    public:
      BandTool(QWidget *parent);

      /**
       * returns the name of the menu.
       *
       *
       * @return QString
       */
      QString menuName() const {
        return "&View";
      };
    protected:
      QAction *toolPadAction(ToolPad *pad);
      QWidget *createToolBarWidget(QStackedWidget *active);
      void updateTool();

    private slots:
      void changeView();
      void setList();
      void setBandBin(Cube *cube);
      void setDisplay();
      void copyLinkedViewports();
      void copyAllViewports();

    private:
      QRadioButton *p_rgbButton;//!< RGB radio button
      QRadioButton *p_blackwhiteButton;//!< Black and White radio button
      QSpinBox *p_graySpin;//!< Gray spin box
      QSpinBox *p_redSpin;//!< Red spin box
      QSpinBox *p_grnSpin;//!< Green spin box
      QSpinBox *p_bluSpin;//!< Blue spin box
      QLabel *p_grayDisplay;//!< gray display
      QLabel *p_redDisplay;//!< red display
      QLabel *p_greenDisplay;//!< green display
      QLabel *p_blueDisplay;//!< blue display
      QComboBox *p_comboBox;//!< display selection combo box
      QStackedWidget *p_stack; //!< Stacked widget for spin boxes
      QStackedWidget *p_stack2;//!< Stacked widget for displays and combo box
      CubeViewport *p_bandBinViewport;

      int p_bands;//!< Number of possible bands
      Pvl p_pvl;//!< pvl
      QStringList p_lineEditValueList;//!< Value list
  };
};

#endif
