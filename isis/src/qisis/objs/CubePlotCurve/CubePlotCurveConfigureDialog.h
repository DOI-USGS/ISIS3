#ifndef CubePlotCurveConfigureDialog_h
#define CubePlotCurveConfigureDialog_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QDialog>

#include <QPointer>

class QCheckBox;
class QComboBox;
class QLineEdit;
class QPushButton;
class QWidget;

namespace Isis {
  class CubePlotCurve;
  class PlotWindow;


  /**
   * This should be an inner class for CubePlotCurve, but Qt doesn't support
   *   having a QObject as an inner class.
   *
   * @author 2012-01-18 Steven Lambright
   *
   * @internal
   *   @history 2012-01-20 Steven Lambright and Jai Rideout - Completed
   *                           documentation.
   *   @history 2014-07-25 Ian Humphrey - Added configure tool menu item. This allows user to
   *                           right-click a curve (as previously) or select  configure tool menu
   *                           item to configure a plot curve's color, symbol, line style, etc.
   *                           Fixes #2089.
   */
  class CubePlotCurveConfigureDialog : public QDialog {
      Q_OBJECT

    public:
      CubePlotCurveConfigureDialog(CubePlotCurve *curve,
                                   QWidget *parent = NULL);
      virtual ~CubePlotCurveConfigureDialog();

    public slots:
      void applySettingsToCurve();
      void readSettingsFromCurve();
      void updateComboIndex(int selected);
      void updateCurvesList();

    private slots:
      void askUserForColor();

    private:
      /**
       * Don't allow copying of this class.
       *
       * @param other The dialog to not be copied.
       */
      CubePlotCurveConfigureDialog(const CubePlotCurveConfigureDialog &other);

      /**
       * Don't allow assignments of this class.
       *
       * @param other The dialog we are not taking the properties of.
       *
       * @return Nothing.
       */
      CubePlotCurveConfigureDialog &operator=(
          const CubePlotCurveConfigureDialog &other);

    private:
      //! The selection/combo box for the cube plot curve
      QPointer<QComboBox>   m_curvesCombo;
      //! The line edit containing the cube plot curve's name
      QPointer<QLineEdit>   m_nameEdit;
      //! The button for changing the cube plot curve's color
      QPointer<QPushButton> m_colorButton;
      //! The parent widget of the configuration dialog
      QPointer<QWidget> m_parent;
      //! The current plot curve to configure
      QPointer<CubePlotCurve> m_plotCurve;
      //! The list of plot curves to configure
      QList<CubePlotCurve *> m_plotCurvesList;
      //! The index of the selected curve in m_curvesCombo
      int m_selectedCurve;
      //! The selection/combo box for the cube plot curve's size/thickness
      QPointer<QComboBox>   m_sizeCombo;
      //! The selection/combo box for the cube plot curve's line style
      QPointer<QComboBox>   m_styleCombo;
      //! The selection/combo box for the cube plot curve's marker style
      QPointer<QComboBox>   m_symbolCombo;
  };
}

#endif
