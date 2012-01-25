#ifndef CubePlotCurveConfigureDialog_h
#define CubePlotCurveConfigureDialog_h

#include <QDialog>

#include <QPointer>

class QCheckBox;
class QComboBox;
class QLineEdit;
class QPushButton;
class QWidget;

namespace Isis {
  class CubePlotCurve;

  /**
   * This should be an inner class for CubePlotCurve, but Qt doesn't support
   *   having a QObject as an inner class.
   *
   * @author 2012-01-18 Steven Lambright
   *
   * @internal
   *   @history 2012-01-20 Steven Lambright and Jai Rideout - Completed
   *                           documentation.
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
      //! The line edit containing the cube plot curve's name
      QPointer<QLineEdit>   m_nameEdit;
      //! The button for changing the cube plot curve's color
      QPointer<QPushButton> m_colorButton;
      //! The selection/combo box for the cube plot curve's size/thickness
      QPointer<QComboBox>   m_sizeCombo;
      //! The selection/combo box for the cube plot curve's line style
      QPointer<QComboBox>   m_styleCombo;
      //! The selection/combo box for the cube plot curve's marker style
      QPointer<QComboBox>   m_symbolCombo;

      //! The plot curve we are configuring.
      QPointer<CubePlotCurve> m_plotCurve;
  };
}

#endif
