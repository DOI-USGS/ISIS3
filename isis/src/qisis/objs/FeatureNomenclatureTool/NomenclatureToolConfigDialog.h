#ifndef NomenclatureToolConfigDialog_h
#define NomenclatureToolConfigDialog_h

#include <QDialog>
#include <QPointer>
#include <QProgressDialog>

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
   *   @history 2012-06-06 Seven Lambright and Kimberly Oyama - Added a QComboBox, showVectorsCombo,
   *                           to choose the extent type from and a QCheckBox, showApprovedOnly, to
   *                           determine whether or not unapproved or dropped features are
   *                           displayed. Fixes #852. Fixes #892.
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
      //! Show feature extents
      QPointer<QComboBox> m_showVectorsCombo;
      //! Filter out unapproved features
      QPointer<QCheckBox> m_showApprovedCheckBox;
      //! Font size of the labels
      QPointer<QComboBox> m_fontSizeCombo;
      //! Color to use when rendering the nomenclature
      QPointer<QPushButton> m_fontColorButton;
      //! Visible when tool is updating the valid features.
      QPointer<QProgressDialog> m_updatingNomenclatureProgressDialog;
  };
};

#endif
