#ifndef MatrixOptionsDialog_h
#define MatrixOptionsDialog_h

#include <QDialog>
#include <QPointer>

#include "MatrixOptions.h"

class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QSlider;
class QRadioButton;

namespace Isis {

  /**
   * @brief This widget allows the user to modify the matrix display
   *
   * This dialog allows the user to select different color schemes and change the focus of the
   * display. The user can select a gradient color scheme, the default, or a two color, good/bad,
   * color scheme that is based on a tolerance. When the matrix is too large to display the whole
   * thing, the user will be able to select which part of the matrix they want to see. 
   *
   * @ingroup Visualization Tools
   *
   * @author 2014-07-14 Kimberly Oyama
   *
   * @internal
   *   @history 2014-07-14 Kimberly Oyama - Original Version
   *   @history 2014-07-21 Kimberly Oyama - Color options are now updated and applied in the scene.
   *   @history 2016-08-25 Adam Paquette - Updated documentation. Fixes #4299.
   *   @history 2016-12-01 Ian Humphrey - Removed unused variables in readOptions() as they were
   *                           causing warnings in the clang compiler.
   */
  
  class MatrixOptionsDialog : public QDialog {
        Q_OBJECT
    public:
      MatrixOptionsDialog(MatrixOptions *options, QWidget *parent);
      ~MatrixOptionsDialog();

    signals:
      void optionsUpdated();
        
    public slots:
      void applyOptions();
      void readOptions();
      void updateCorrelationData(QString currentData);
      void populateParameterComboBox(int index);
      
    private slots:
      void refreshWidgetStates();
      void askUserForGoodColor();
      void askUserForBadColor();
      void updateToleranceSlider(const QString &value);
      void updateToleranceLineEdit(int value);
      void updateSpecParam1ComboBox(const QString & key);
      void updateSpecParam2ComboBox(const QString & key);

    private:
      Q_DISABLE_COPY(MatrixOptionsDialog);
      void askUserForColor(QPushButton *button);

      // functions to enable and disable groups
      void setColorToleranceStatus(bool enable);
      void setFocusToleranceStatus(bool enable);
      void setSpecificParametersStatus(bool enable);
      void updateSpecificParameterComboBox(const QString & key, QComboBox *comboBox);

      // update widgets as radiobuttons are chagned

      // Need parent object

      //! This will be used to populate all the widgets
      MatrixOptions *m_options;
      
      // ------------------ Color widgets------------------------------------

      /**
       * Select a tolerance for the color values. All values above threshold will be one color,
       * all values below the threshold will be a different color.
       */
      QPointer<QRadioButton> m_gradientRadioButton;
      QPointer<QRadioButton> m_colorToleranceRadioButton;
      QPointer<QSlider> m_colorToleranceSlider;
      //! Color to use when for matrix elements that are above the given threshold
      QPointer<QPushButton> m_badCorrelationColorButton;
      //! Color to use for matrix elements that are below the given threshold
      QPointer<QPushButton> m_goodCorrelationColorButton;
      QPointer<QLineEdit> m_colorToleranceLineEdit;

      // ------------------ Focus Widgets ------------------------------------

      //! Focus the matrix on the best correlation................. What if there is more than one?
      QPointer<QRadioButton> m_bestCorrelationRadioButton;

      //! Focus the matrix on the worst correlation................. What if there is more than one?
      QPointer<QRadioButton> m_worstCorrelationRadioButton;

      //! Focus the matrix on the correlation corresponding to the specified images and parameters.
      QPointer<QRadioButton> m_specificCorrelationRadioButton;
      QPointer<QComboBox> m_image1ComboBox;
      QPointer<QComboBox> m_parameter1ComboBox;
      QPointer<QComboBox> m_image2ComboBox;
      QPointer<QComboBox> m_parameter2ComboBox;

      //! Focus the matrix on a selected correlation value.
      QPointer<QRadioButton> m_focusToleranceRadioButton;
      QPointer<QLineEdit> m_focusToleranceLineEdit;
      QPointer<QComboBox> m_goodElementsComboBox;
      QPointer<QComboBox> m_badElementsComboBox;
      QPointer<QWidget> m_verticalLayoutWidget;

      // ------------------ Current Element Widgets------------------------------------
      QPointer<QLabel> m_currentValueLabel;
      QPointer<QLabel> m_image1NameLabel;
      QPointer<QLabel> m_parameter1NameLabel;
      QPointer<QLabel> m_image2NameLabel;
      QPointer<QLabel> m_parameter2NameLabel;
  };
};

#endif
