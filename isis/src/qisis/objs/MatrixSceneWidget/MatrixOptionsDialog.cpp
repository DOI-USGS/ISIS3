#include "MatrixOptionsDialog.h"

#include <QDebug>
#include <QVariant>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QColorDialog>
#include <QComboBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QSizePolicy>
#include <QSlider>
#include <QSpacerItem>
#include <QComboBox>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>

#include "IString.h"
#include "MatrixOptions.h"

namespace Isis {

  /**
   * Main constructor
   *
   * Layouts:
   *
   * Widget
   *   TabWidget
   *       colorTab (colorOptionsPage)
   *           colorToleranceLayout (everything tolerance related)
   *                colorToleranceOptionsLayout (not including radiobutton, has slider)
   *                     colorToleranceEditsLayout (not including slider)
   *                          colorBadCorrLayout (bad corr label and color button)
   *                          colorEnterTolLayout (tol label and line edit)
   *                          colorGoodCorrLayout (good corr label and color button)
   *           colorGradientLayout (radiobutton)
   * 
   *       focusTab (focusOptionsPage)
   *            focusPageLayout
   *                 focusTolLayout (has tolerance radiobutton)
   *                      focusTolEditLayout (tolerance label and line edit)
   *                      elementsLayout
   *                           focusBadElementsLayout (label and spinbox)
   *                           focusGoodElementsLayout (label and spinbox)
   *                 focusOptionsLayout (best, worst radiobuttons)
   *                      focusSpecParamLayout (radiobutton, img1 label, img2 label)
   *                           focusSpecParam1Layout (img1 combobox, param1 combobox)
   *                           focusSpecParam2Layout (img2 combobox, param2 combobox)
   * 
   *   currentCorrDataLayout
   *          currentCorrLayout (label, value)
   *          currentCorrImg1Layout (img1 name, param1 name)
   *          currentCorrImg2Layout (img2 name, param2 name)
   *
   * @param matrixOptionsWidget
   * @param parent
   */
  MatrixOptionsDialog::MatrixOptionsDialog(MatrixOptions *options,
                                           QWidget *parent) : QDialog(parent) {

//     connect( this, SIGNAL( optionsUpdated() ),
//              parent, SLOT( redrawItems() ) );

    connect(parent, SIGNAL( elementClicked(QString) ),
            this, SLOT( updateCorrelationData(QString) ) );
                           
    m_options = options;

    setWindowTitle("Matrix Options");
    resize(200, 200);
//     setWindowFlags(Qt::WindowStaysOnTopHint);

    QTabWidget *optionsTabs = new QTabWidget();
    optionsTabs->setGeometry( QRect(9, 9, 100, 100) );

    /********************************************
     * Color  Options
     * *****************************************/

    QWidget *colorOptionsPage = new QWidget();

    QGridLayout *mainLayout = new QGridLayout();
    colorOptionsPage->setLayout(mainLayout);
    QHBoxLayout *colorPageLayout = new QHBoxLayout(); // = new QHBoxLayout();

      /********************************************
      * Color Tolerance Settings
      * *****************************************/
    QVBoxLayout *colorToleranceLayout = new QVBoxLayout();

    m_colorToleranceRadioButton = new QRadioButton();
    m_colorToleranceRadioButton->setText("Pick Tolerance");
    colorToleranceLayout->addWidget(m_colorToleranceRadioButton);
    connect( m_colorToleranceRadioButton, SIGNAL( clicked() ),
             this, SLOT( refreshWidgetStates() ) );

    QHBoxLayout *colorToleranceOptionsLayout = new QHBoxLayout();

    m_colorToleranceSlider = new QSlider();
    m_colorToleranceSlider->setOrientation(Qt::Vertical);
    m_colorToleranceSlider->setRange(0, 1000);
    colorToleranceOptionsLayout->addWidget(m_colorToleranceSlider);
    connect( m_colorToleranceSlider, SIGNAL( valueChanged(int) ),
             this, SLOT( updateToleranceLineEdit(int) ) );

    QVBoxLayout *colorToleranceEditsLayout = new QVBoxLayout();
    QHBoxLayout *colorBadCorrLayout = new QHBoxLayout();

    QLabel *colorBadCorrLabel = new QLabel();
    colorBadCorrLabel->setText("Bad Correlation");

    colorBadCorrLayout->addWidget(colorBadCorrLabel);

    m_badCorrelationColorButton = new QPushButton();
    colorBadCorrLayout->addWidget(m_badCorrelationColorButton);
    connect( m_badCorrelationColorButton, SIGNAL( clicked() ),
             this, SLOT( askUserForBadColor() ) );


    colorToleranceEditsLayout->addLayout(colorBadCorrLayout);

    QHBoxLayout *colorEnterTolLayout = new QHBoxLayout();

    QLabel *colorTolLabel = new QLabel();
    colorTolLabel->setText("Tolerance");
    colorEnterTolLayout->addWidget(colorTolLabel);

    m_colorToleranceLineEdit = new QLineEdit();
    m_colorToleranceLineEdit->setValidator( new QDoubleValidator(-1, 1, 10, this) );
    colorEnterTolLayout->addWidget(m_colorToleranceLineEdit);
    connect( m_colorToleranceLineEdit, SIGNAL( textChanged(const QString &) ),
             this, SLOT( updateToleranceSlider(const QString &) ) );

    colorToleranceEditsLayout->addLayout(colorEnterTolLayout);

    QHBoxLayout *colorGoodCorrLayout = new QHBoxLayout();
    QLabel *colorGoodCorrLabel = new QLabel();
    colorGoodCorrLabel->setText("Good Correlation");

    colorGoodCorrLayout->addWidget(colorGoodCorrLabel);

    m_goodCorrelationColorButton = new QPushButton();
    colorGoodCorrLayout->addWidget(m_goodCorrelationColorButton);
    connect( m_goodCorrelationColorButton, SIGNAL( clicked() ),
             this, SLOT( askUserForGoodColor() ) );

    colorToleranceEditsLayout->addLayout(colorGoodCorrLayout);

    colorToleranceOptionsLayout->addLayout(colorToleranceEditsLayout);

    colorToleranceLayout->addLayout(colorToleranceOptionsLayout);

    colorPageLayout->addLayout(colorToleranceLayout);

      /********************************************
      * Color gradient area
      * *****************************************/
    QVBoxLayout *colorGradientLayout = new QVBoxLayout();
    
    m_gradientRadioButton = new QRadioButton();
    m_gradientRadioButton->setText("Use Gradient");
    colorGradientLayout->addWidget(m_gradientRadioButton);
    connect( m_gradientRadioButton, SIGNAL( clicked() ),
             this, SLOT( refreshWidgetStates() ) );

    QSpacerItem *colorGradSpacer = new QSpacerItem(20,
                                                   40,
                                                   QSizePolicy::Minimum,
                                                   QSizePolicy::Expanding);
    colorGradientLayout->addItem(colorGradSpacer);

    colorPageLayout->addLayout(colorGradientLayout);

    mainLayout->addLayout(colorPageLayout, 0, 1, 1, 1);

    optionsTabs->addTab(colorOptionsPage, QString() );
    optionsTabs->setTabText(optionsTabs->indexOf(colorOptionsPage), "Color Options");

    /********************************************
    * Focus Options
    * *****************************************/

    QWidget *focusOptionsPage = new QWidget();
    QHBoxLayout *focusPageLayout = new QHBoxLayout();
    focusPageLayout->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout *focusOptionsLayout = new QVBoxLayout();

    m_bestCorrelationRadioButton = new QRadioButton();
    m_bestCorrelationRadioButton->setText("Best Correlation");
    focusOptionsLayout->addWidget(m_bestCorrelationRadioButton);
    connect( m_bestCorrelationRadioButton, SIGNAL( clicked() ),
             this, SLOT( refreshWidgetStates() ) );

    m_worstCorrelationRadioButton = new QRadioButton();
    m_worstCorrelationRadioButton->setText("Worst Correlation");
    focusOptionsLayout->addWidget(m_worstCorrelationRadioButton);
    connect( m_worstCorrelationRadioButton, SIGNAL( clicked() ),
             this, SLOT( refreshWidgetStates() ) );

      /********************************************
      * Focus, specific parameters settings
      * *****************************************/

    QVBoxLayout *focusSpecParamLayout = new QVBoxLayout();
    
    m_specificCorrelationRadioButton = new QRadioButton();
    m_specificCorrelationRadioButton->setText("Specific Parameters");
    focusSpecParamLayout->addWidget(m_specificCorrelationRadioButton);
    connect( m_specificCorrelationRadioButton, SIGNAL( clicked() ),
             this, SLOT( refreshWidgetStates() ) );

    QLabel *focusSpecParam1Label = new QLabel();
    focusSpecParam1Label->setText("Image 1:");
    focusSpecParamLayout->addWidget(focusSpecParam1Label);

    QHBoxLayout *focusSpecParam1Layout = new QHBoxLayout();
    
    m_image1ComboBox = new QComboBox();
    focusSpecParam1Layout->addWidget(m_image1ComboBox);
    connect( m_image1ComboBox, SIGNAL( currentIndexChanged(const QString &) ),
             this, SLOT( updateSpecParam1ComboBox(const QString &) ) );


    m_parameter1ComboBox = new QComboBox();
//     m_parameter1ComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    focusSpecParam1Layout->addWidget(m_parameter1ComboBox);

    focusSpecParamLayout->addLayout(focusSpecParam1Layout);

    QLabel *focusSpecParam2Label = new QLabel();
    focusSpecParam2Label->setText("Image 2:");
    focusSpecParamLayout->addWidget(focusSpecParam2Label);
    
    QHBoxLayout *focusSpecParam2Layout = new QHBoxLayout();

    m_image2ComboBox = new QComboBox();
    focusSpecParam2Layout->addWidget(m_image2ComboBox);
    connect( m_image2ComboBox, SIGNAL( currentIndexChanged(const QString &) ),
             this, SLOT( updateSpecParam2ComboBox(const QString &) ) );

    m_parameter2ComboBox = new QComboBox();
    focusSpecParam2Layout->addWidget(m_parameter2ComboBox);

    focusSpecParamLayout->addLayout(focusSpecParam2Layout);

    focusOptionsLayout->addLayout(focusSpecParamLayout);

    focusPageLayout->addLayout(focusOptionsLayout);

      /********************************************
      * Focus, tolerance stuff
      * *****************************************/
    QVBoxLayout *focusTolLayout = new QVBoxLayout();
    
    m_focusToleranceRadioButton = new QRadioButton();
    m_focusToleranceRadioButton->setText("Tolerance");
    focusTolLayout->addWidget(m_focusToleranceRadioButton);
    connect( m_focusToleranceRadioButton, SIGNAL( clicked() ),
             this, SLOT( refreshWidgetStates() ) );

    QHBoxLayout *focusTolEditLayout = new QHBoxLayout();
    
    QLabel *focusTolLabel = new QLabel();
    focusTolLabel->setText("Tolerance:");
    focusTolEditLayout->addWidget(focusTolLabel);

    m_focusToleranceLineEdit = new QLineEdit();
    m_focusToleranceLineEdit->setValidator( new QDoubleValidator(-1, 1, 10, this) );
    focusTolEditLayout->addWidget(m_focusToleranceLineEdit);
//     connect( m_focusToleranceLineEdit, SIGNAL( textChanged(const QString &) ),
//              this, SLOT( refreshWidgetStates() ) );

    focusTolLayout->addLayout(focusTolEditLayout);

    QHBoxLayout *focusElementsLayout = new QHBoxLayout();
    QVBoxLayout *focusGoodElementsLayout = new QVBoxLayout();
    
    QLabel *focusGoodElementsLabel = new QLabel();
    focusGoodElementsLabel->setFocusPolicy(Qt::NoFocus);
    focusGoodElementsLabel->setText("Good");
    focusGoodElementsLayout->addWidget(focusGoodElementsLabel);

    m_goodElementsComboBox = new QComboBox();
    focusGoodElementsLayout->addWidget(m_goodElementsComboBox);

    focusElementsLayout->addLayout(focusGoodElementsLayout);

    QVBoxLayout *focusBadElementsLayout = new QVBoxLayout();
    
    QLabel *focusBadElementsLabel = new QLabel();
    focusBadElementsLabel->setText("Bad");
    focusBadElementsLayout->addWidget(focusBadElementsLabel);

    m_badElementsComboBox = new QComboBox();
    focusBadElementsLayout->addWidget(m_badElementsComboBox);

    focusElementsLayout->addLayout(focusBadElementsLayout);

    focusTolLayout->addLayout(focusElementsLayout);

    focusPageLayout->addLayout(focusTolLayout);

    focusOptionsPage->setLayout(focusPageLayout);
    optionsTabs->addTab( focusOptionsPage, QString() );
    optionsTabs->setTabText(optionsTabs->indexOf(focusOptionsPage), "Focus Options");

    /********************************************
    * Current Correlation Stuff
    * *****************************************/
    QVBoxLayout *m_currentElementData = new QVBoxLayout();
    m_currentElementData->setContentsMargins(0, 0, 0, 0);

//     QHBoxLayout *currentCorrLayout = new QHBoxLayout();
    
    QLabel *currentCorrLabel = new QLabel();
    currentCorrLabel->setText("Current Correlation Info:");
    m_currentElementData->addWidget(currentCorrLabel);
//     currentCorrLayout->addWidget(currentCorrLabel);

    m_currentValueLabel = new QLabel();
    m_currentValueLabel->setText("-");
    m_currentElementData->addWidget(m_currentValueLabel);
//     currentCorrLayout->addWidget(m_currentValueLabel);

//     m_currentElementData->addLayout(currentCorrLayout);

//     QHBoxLayout *currentCorrImg1Layout = new QHBoxLayout();
    
//     m_image1NameLabel = new QLabel();
//     m_image1NameLabel->setText("Image 1:  Sub4-AS15-M-0585_msk");
//     currentCorrImg1Layout->addWidget(m_image1NameLabel);

//     m_parameter1NameLabel = new QLabel();
//     m_parameter1NameLabel->setText("   Parameter 1: X");
//     currentCorrImg1Layout->addWidget(m_parameter1NameLabel);

//     m_currentElementData->addLayout(currentCorrImg1Layout);

//     QHBoxLayout *currentCorrImg2Layout = new QHBoxLayout();
    
//     m_image2NameLabel = new QLabel();
//     m_image2NameLabel->setText("Image 2:  Sub4-AS15-M-0586_msk");
//     currentCorrImg2Layout->addWidget(m_image2NameLabel);

//     m_parameter2NameLabel = new QLabel;
//     m_parameter2NameLabel->setText("   Parameter 2: TWI");
//     currentCorrImg2Layout->addWidget(m_parameter2NameLabel);

//     m_currentElementData->addLayout(currentCorrImg2Layout);

    optionsTabs->setCurrentIndex(0);

    QPushButton *applyButton = new QPushButton("&Apply");
    applyButton->setIcon( QIcon::fromTheme("dialog-ok-apply") );
    connect( applyButton, SIGNAL( clicked() ),
             this, SLOT( applyOptions() ) );

    
    QVBoxLayout *topLayout = new QVBoxLayout();
    topLayout->addWidget(optionsTabs);
    topLayout->addWidget(applyButton);
    topLayout->addLayout(m_currentElementData); // currentCorrDataLayout - not a layout...
    setLayout(topLayout);

    QMetaObject::connectSlotsByName(this);

    readOptions();
  }
  


  /**
   * Default Destructor
   */
  MatrixOptionsDialog::~MatrixOptionsDialog() {
  }


  
/***************************
 * Slots
 * *************************/



  /**
   * Send changes back to scene widget (MatrixOptions) so it can redraw the elements in the right
   * color. This will be called when the appy button is pressed.
   */
  void MatrixOptionsDialog::applyOptions() {
    // Color Options
    QPalette colorPalette( m_goodCorrelationColorButton->palette() );
    m_options->setGoodCorrelationColor( colorPalette.color(QPalette::Button) );
    
    colorPalette = m_badCorrelationColorButton->palette();
    m_options->setBadCorrelationColor( colorPalette.color(QPalette::Button) );

    m_options->setColorScheme( m_colorToleranceRadioButton->isChecked() );

    m_options->setColorTolerance( m_colorToleranceLineEdit->text().toDouble() );

    // Focus Options
//     m_options->setFocusValue( m_colorToleranceLineEdit->text().toDouble() );
// 
//     QList<double> goodElements, badElements;
//     
// //     m_options->setGoodElements(*m_goodElementsComboBox);
// //     m_options->setBadElements(*m_badElementsComboBox);
// 
//     if ( m_bestCorrelationRadioButton->isChecked() )  {
//       m_options->setFocusOption(MatrixOptions::Best);
//     }
//     else if ( m_worstCorrelationRadioButton->isChecked() ) {
//       m_options->setFocusOption(MatrixOptions::Worst);
//     }
//     else if ( m_focusToleranceRadioButton->isChecked() ) {
//       m_options->setFocusOption(MatrixOptions::Tolerance);
//     }
//     else {
//       m_options->setFocusOption(MatrixOptions::Specific);
//     }

    emit optionsUpdated();
  }



  /**
   * Get the current options from the matrix scene widget. This will be called when the widget is
   * first opened and when the displayed correlation matrix is changed.
   *
   * @internal
   *   @history 2016-12-01 Ian Humphrey - Removed goodElements() and badElements() QList<double>
   *                           declarations, as these are unused and were causing [-Wvexing-parse]
   *                           warnings on the clang compiler by using the ()'s.
   */
  void MatrixOptionsDialog::readOptions() {

    // Color Options
    setColorToleranceStatus( m_options->colorScheme() );
    m_colorToleranceRadioButton->setChecked( m_options->colorScheme() );
    m_gradientRadioButton->setChecked( !m_options->colorScheme() );

    if ( m_options->colorScheme() ) {
      QPalette colorPalette;
      colorPalette.setColor( QPalette::Button, m_options->goodCorrelationColor() );
      m_goodCorrelationColorButton->setPalette(colorPalette);
      
      colorPalette.setColor( QPalette::Button, m_options->badCorrelationColor() );
      m_badCorrelationColorButton->setPalette(colorPalette);
      
      m_colorToleranceLineEdit->setText( QString::number( m_options->colorTolerance() ) );
      m_colorToleranceSlider->blockSignals(true);
      m_colorToleranceSlider->setValue( qRound( 1000 * m_colorToleranceLineEdit->text().toDouble() ) );
      m_colorToleranceSlider->blockSignals(false);
    }

    // Focus Options
    m_bestCorrelationRadioButton->setChecked(m_options->focusOption() == MatrixOptions::Best);
    m_worstCorrelationRadioButton->setChecked(m_options->focusOption() == MatrixOptions::Worst);
    m_focusToleranceRadioButton->setChecked(m_options->focusOption() == MatrixOptions::Tolerance);
    m_specificCorrelationRadioButton->setChecked(m_options->focusOption() == MatrixOptions::Specific);

    // tolerance focus
    if (m_options->focusOption() == MatrixOptions::Tolerance) {
      m_focusToleranceLineEdit->setText( QString::number( m_options->focusValue() ) );
      //QList<double> goodElements();
      //QList<double> badElements();
    }
    setFocusToleranceStatus(m_options->focusOption() == MatrixOptions::Tolerance);

    // specific focus
    QMapIterator<QString, QStringList> img( m_options->matrixImgsAndParams() );
    img.next();
    for (int param = 0; param < img.value().size(); param++) {
      m_parameter1ComboBox->addItem(img.value()[param]);
      m_parameter2ComboBox->addItem(img.value()[param]);
    }
    while ( img.hasNext() ) {
      m_image1ComboBox->addItem( img.key() );
      m_image2ComboBox->addItem( img.key() );
      img.next();
    }
    setSpecificParametersStatus(m_options->focusOption() == MatrixOptions::Specific);

    refreshWidgetStates();
  }



  /**
   * Update the current correlation information. This slot is called when the m_options signal,
   * changedCurrentCorrData() is emit. This happens when a matrix element is clicked on.
   */
  void MatrixOptionsDialog::updateCorrelationData(QString currentData) {
    m_currentValueLabel->setText(currentData);
    //toString( m_options->currentCorrelation() ) );

//     m_image1NameLabel->setText( m_options->currentImage1() );
//     m_parameter1NameLabel->setText( m_options->currentParameter1() );

//     m_image2NameLabel->setText( m_options->currentImage2() );
//     m_parameter2NameLabel->setText( m_options->currentParameter2() );
  }



  /**
   * Update parameter combo boxes at real time, when the img combo boxes are changed.
   *
   * @param index The current index of the IMAGE combobox.
   */
  void MatrixOptionsDialog::populateParameterComboBox(int index) {
  }

  /**
   * If the tolerance radiobutton is selected, this method will be called with true passed as the
   * parameter. This will enable the color buttons and tolerance line edit.
   *
   * If the color gradient radio button is selected this method will be called with false passed
   * as the parameter. This will disable the tolerance widgets.
   *
   * @param enable True if the radiobutton for that section is selected.
   */
  void MatrixOptionsDialog::setColorToleranceStatus(bool enable) {
    m_colorToleranceLineEdit->setEnabled(enable);
    m_goodCorrelationColorButton->setEnabled(enable);
    m_badCorrelationColorButton->setEnabled(enable);
    m_colorToleranceSlider->setEnabled(enable);
  }



  /**
   * If the focus tolerance radio button is selected, the tolerance widgets will be enabled.
   * Otherwise, they will be disabled.
   *
   * @param enable True if the radiobutton for that section is selected.
   */
  void MatrixOptionsDialog::setFocusToleranceStatus(bool enable) {
    m_focusToleranceLineEdit->setEnabled(enable);
    m_goodElementsComboBox->setEnabled(enable);
    m_badElementsComboBox->setEnabled(enable);
  }



  /**
   * If the specific parameters radio button is selected, the image and parameter spinboxes will be
   * enabled. Otherwise, they will be disabled.
   *
   * @param enable True if the radiobutton for that section is selected.
   */
  void MatrixOptionsDialog::setSpecificParametersStatus(bool enable) {
    m_image1ComboBox->setEnabled(enable);
    m_parameter1ComboBox->setEnabled(enable);
    m_image2ComboBox->setEnabled(enable);
    m_parameter2ComboBox->setEnabled(enable);
  }



  /**
   * This method will enable and disable widgets depending on which radio button is selected.
   */
  void MatrixOptionsDialog::refreshWidgetStates() {
    // color
    setColorToleranceStatus( m_colorToleranceRadioButton->isChecked() );

    // focus
    setFocusToleranceStatus( m_focusToleranceRadioButton->isChecked() );
    setSpecificParametersStatus( m_specificCorrelationRadioButton->isChecked() );
  }



  /**
   * slot called when user clicks on good correlation color button
   */
  void MatrixOptionsDialog::askUserForGoodColor() {
    askUserForColor(m_goodCorrelationColorButton);
  }



  /**
   * slot called when user clicks on bad correlation color button
   */
  void MatrixOptionsDialog::askUserForBadColor() {
    askUserForColor(m_badCorrelationColorButton);
  }


  
  /**
   * Prompt the user for a new color.
   *
   * @param good If true, modify the good correlation button. Otherwise, modify the bad correlation
   *             button.
   */
  void MatrixOptionsDialog::askUserForColor(QPushButton *button) {
    QPalette colorPalette( button->palette() );
    
    QColor newColor = QColorDialog::getColor(colorPalette.color(QPalette::Button), this);

    if( newColor.isValid() ) {
      colorPalette.setColor(QPalette::Button, newColor);
      button->setPalette(colorPalette);
    }
  }



  /**
   * slot called when user modifies the color tolerance line edit text. It will update the slider
   * to match the new entry.
   */
  void MatrixOptionsDialog::updateToleranceSlider(const QString &value) {
    m_colorToleranceSlider->setValue( qRound( 1000 * value.toDouble() ) );
  }



  /**
   * slot called when user changes the slider position. It will update the tolerance line edit to
   * match the new value.
   */
  void MatrixOptionsDialog::updateToleranceLineEdit(int value) {
    m_colorToleranceLineEdit->setText( QString::number(value / 1000.0) );
  }



  /**
   * when the img1 combo box is changed this slot will update the parameter combo box.
   *
   * @param index The index of the new img.
   */
  void MatrixOptionsDialog::updateSpecParam1ComboBox(const QString & key) {
    updateSpecificParameterComboBox(key, m_parameter1ComboBox);
  }



  /**
   * when the img2 combo box is changed this slot will update the parameter combo box.
   *
   * @param index The index of the new img.
   */
  void MatrixOptionsDialog::updateSpecParam2ComboBox(const QString & key) {
    updateSpecificParameterComboBox(key, m_parameter2ComboBox);
  }


  
  /**
   * When the user switches the image combobox this slot will update the parameter combobox to be
   * filled with the parameters associated with the new image.
   */
  void MatrixOptionsDialog::updateSpecificParameterComboBox(const QString & key,
                                                            QComboBox *comboBox) {
    comboBox->clear();
    comboBox->addItems( m_options->matrixImgsAndParams().value(key) );
//     comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
  }
};
