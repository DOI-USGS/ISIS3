#ifndef BandTool_h
#define BandTool_h

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
  // FIXME: uncomment this and make p_pvl a Pvl * (make cc and op=, etc)
  //class Pvl;
}

namespace Isis {
  class ToolPad;
  /**
   * @internal
   *   @history 2010-06-26 Eric Hyer - Now uses MdiCubeViewport instead of
   *            CubeViewport
   *   @history 2010-12-01 Steven Lambright - No longer calls setBandBin() if
   *            the viewport doesn't change.
   */
  class BandTool : public Tool {
      Q_OBJECT

    public:
      BandTool(QWidget *parent);

    protected:
      /**
       * returns the name of the menu.
       *
       *
       * @return QString
       */
      QString menuName() const {
        return "&View";
      };
      QAction *toolPadAction(ToolPad *pad);
      QWidget *createToolBarWidget(QStackedWidget *active);
      void updateTool();

    private slots:
      void changeView();
      void setList();
      void setBandBin(Pvl &pvl);
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
