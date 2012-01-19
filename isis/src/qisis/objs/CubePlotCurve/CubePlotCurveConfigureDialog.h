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
      CubePlotCurveConfigureDialog(const CubePlotCurveConfigureDialog &other);
      CubePlotCurveConfigureDialog &operator=(
          const CubePlotCurveConfigureDialog &other);

    private:
      QPointer<QLineEdit>   m_nameEdit;
      QPointer<QPushButton> m_colorButton;
      QPointer<QComboBox>   m_sizeCombo;
      QPointer<QComboBox>   m_styleCombo;
      QPointer<QComboBox>   m_symbolCombo;

      QPointer<CubePlotCurve> m_plotCurve;
  };
}

#endif
