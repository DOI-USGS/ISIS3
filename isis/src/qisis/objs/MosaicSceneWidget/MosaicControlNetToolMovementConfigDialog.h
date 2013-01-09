#ifndef MosaicControlNetToolMovementConfigDialog_h
#define MosaicControlNetToolMovementConfigDialog_h

#include <QDialog>
#include <QPointer>

class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QSlider;

namespace Isis {
  class MosaicControlNetTool;

  /**
   * @brief Configure qmos Control Net Tool's movement arrows
   *
   * This allows a user to configure the qmos Control Net Tool's movement arrows. These are the
   *   arrows that go from the A Priori surface point to the Adjusted surface point.
   *
   * @author 2012-12-31 Steven Lambright
   *
   * @internal
   */
  class MosaicControlNetToolMovementConfigDialog : public QDialog {
      Q_OBJECT

    public:
      MosaicControlNetToolMovementConfigDialog(MosaicControlNetTool *tool,
                                   QWidget *parent);
      ~MosaicControlNetToolMovementConfigDialog();

    public slots:
      void applySettings();
      void readSettings();
      void refreshWidgetStates();

    private:
      Q_DISABLE_COPY(MosaicControlNetToolMovementConfigDialog);

      //! The MosaicControlNetTool that we're configuring
      QPointer<MosaicControlNetTool> m_tool;

      //! Check box for enabling/disabling arrows entirely
      QPointer<QCheckBox> m_showMovementCheckBox;
      //! Combo box for all coloring possibilities
      QPointer<QComboBox> m_colorSourceComboBox;
      //! Label of max measure count input
      QPointer<QLabel> m_brightestMeasureCountValueLabel;
      //! Max measure count input
      QPointer<QLineEdit> m_brightestMeasureCountValueLineEdit;
      //! Label of max residual magnitude input
      QPointer<QLabel> m_brightestResidualMagValueLabel;
      //! Max residual magnitude input
      QPointer<QLineEdit> m_brightestResidualMagValueLineEdit;

      //! OK button at bottom of dialog
      QPointer<QPushButton> m_okayButton;
      //! Apply button at bottom of dialog
      QPointer<QPushButton> m_applyButton;
  };
}

#endif
