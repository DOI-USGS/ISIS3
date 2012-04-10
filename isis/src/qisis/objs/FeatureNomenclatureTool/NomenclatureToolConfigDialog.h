#ifndef NomenclatureToolConfigDialog_h
#define NomenclatureToolConfigDialog_h

#include <QDialog>
#include <QPointer>

class QCheckBox;
class QComboBox;
class QPushButton;

namespace Isis {
  class FeatureNomenclatureTool;

  /**
   * @brief Configure user's settings for the nomenclature tool
   *
   * This dialog enables the user to configure the given nomenclature tool.
   *
   * @author 2012-03-22 Steven Lambright and Jai Rideout
   *
   * @internal
   */
  class NomenclatureToolConfigDialog : public QDialog {
      Q_OBJECT

    public:
      NomenclatureToolConfigDialog(FeatureNomenclatureTool *tool,
                                   QWidget *parent);
      ~NomenclatureToolConfigDialog();

    public slots:
      void applySettings();
      void readSettings();

    private slots:
      void askUserForColor();

    private:
      //! The tool we're configuring
      FeatureNomenclatureTool *m_tool;

      //! Turn on the nomenclature tool when the application starts
      QPointer<QCheckBox> m_defaultOnCheckBox;
      //! Turn on the nomenclature tool when the application starts
      QPointer<QCheckBox> m_showVectorsCheckBox;
      //! Font size of the labels
      QPointer<QComboBox> m_fontSizeCombo;
      //! Color to use when rendering the nomenclature
      QPointer<QPushButton> m_fontColorButton;
  };
};

#endif
