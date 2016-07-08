#include "MatrixOptions.h"

#include <QColor>
#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>

#include "CorrelationMatrix.h"
#include "MatrixSceneWidget.h"
#include "MatrixOptionsDialog.h"

namespace Isis {
  /**
   * Default Constructor.
   *
   * Constructs a MatrixOptions object used for changing various options on the passed
   * CorrelationMatrix. Options include tolerances and colors for matching certain elements
   * in the CorrelationMatrix based on their correlation values.
   *
   * @param parent CorrelationMatrix to set options for.
   * @param scene Pointer to the MatrixSceneWidget for creating the MatrixOptionsDialog.
   */
  MatrixOptions::MatrixOptions(CorrelationMatrix parent, MatrixSceneWidget *scene) { 
      // give parent? MatrixSceneWidget
    m_parentMatrix = new CorrelationMatrix(parent);
    //Color Options
    m_tolerance = false; // if tolerance is false then we use gradient
    m_goodColor = Qt::cyan;
    m_badColor = Qt::magenta;
    m_colorTolerance = 0.2;

    // Focus Options
    m_focusOption = MatrixOptions::Tolerance;

    // Specific Parameters
    m_image1 = "Specific Img1";
    m_parameter1 = "Specific Param1";
    m_image2 = "Specific Img2";
    m_parameter2 = "Specific Param2";

    // pointer the the one in correlation matrix?
    // m_imagesAndParameters = m_parentMatrix->imagesAndParameters();

    // Tolerance
    m_focusTolSelectedElement = 1.0;
    m_goodElements = QList<double>();
    m_badElements = QList<double>();

    for (int i = 0; i < 10; i++) {
      m_goodElements.append(i);
      m_badElements.append(i + 10);
    }

    // Current Correlation Information
    m_currentValue = 0.0;
    m_currentImg1 = "Current Image 1";
    m_currentParam1 = "Current Parameter 1";
    m_currentImg2 = "Current Image 2";
    m_currentParam2 = "Current Parameter 2";

    MatrixOptionsDialog *optionsDialog = new MatrixOptionsDialog(this, scene);
    optionsDialog->setAttribute(Qt::WA_DeleteOnClose);
    optionsDialog->show();

    connect(optionsDialog, SIGNAL( optionsUpdated() ),
            this, SIGNAL( optionsUpdated() ) );
  }


  /**
   * Constructor that sets up all the variables
   */
//   MatrixOptions::MatrixOptions(CorrelationMatrix *parent) { // give parent? MatrixSceneWidget
//   }


  /**
   * Default Destructor.
   */
  MatrixOptions::~MatrixOptions() {
  }

  
  // Color Options
  /**
   * Gets the color selected for the correlation values that are below the given threshold.
   *
   * @return @b QColor Returns the selected color for good correlation values.
   */
  QColor MatrixOptions::goodCorrelationColor() {
    return m_goodColor;
  }


  /**
   * Gets the color selected for the correlation values that are above the given threshold.
   * 
   * @return @b QColor Returns the selected color for bad correlation values
   */
  QColor MatrixOptions::badCorrelationColor() {
    return m_badColor;
  }


  /**
   * Gets the threshold value for what is considered a bad correlation.
   * 
   * @return @b double Returns the threshold for distinguishing good vs bad correlation.
   */
  double MatrixOptions::colorTolerance() {
    return m_colorTolerance;
  }


  /**
   * Gets the current colorScheme being used on the correlation matrix to show good vs bad
   * correlation values.
   *
   * Use the green-red gradient if false.
   * Use the 50/50 color split if true. You need the good/bad correlation color methods if true.
   * 
   * @return @b bool Returns whether to use a color split (true) or gradient (false).
   */
  bool MatrixOptions::colorScheme() {
    return m_tolerance; // if tolerance is false then we use gradient
  }


  /**
   * Sets the color scheme to gradient (false) or split based on tolerance (true).
   *
   * @param tolerance Use 50/50 split based on tolerance if true; otherwise, use color gradient.
   */
  void MatrixOptions::setColorScheme(bool tolerance) {
    m_tolerance = tolerance;
  }


  /**
   * Sets the color tolerance for what is considered a bad correlation.
   *
   * @param tolerance Value to establish the color tolerance
   */
  void MatrixOptions::setColorTolerance(double tolerance) {
    m_colorTolerance = tolerance;
  }


  /**
   * Set the color for good correlation values.
   *
   * @param color The QColor for good correlation.
   */
  void MatrixOptions::setGoodCorrelationColor(QColor color) {
    m_goodColor = color;
  }


  /**
   * Set the color for bad correlation values.
   *
   * @param color The QColor for bad correlation.
   */
  void MatrixOptions::setBadCorrelationColor(QColor color) {
    m_badColor = color;
  }

  
  // Focus Options
  /**
   * Returns the focus option for obtaining a specific correlation in the matrix.
   *
   * @return @b MatrixOptions::FocusOption Option to determine which correlation to focus.
   */
  MatrixOptions::FocusOption MatrixOptions::focusOption() {
    return m_focusOption;
  }


  /**
   * Gets the value of the spot on the matrix that we need to focus on. This can come from the best
   * or worst option as well as the good/bad lists when a tolerance is entered.
   *
   * @return @b double Returns the correlation value to focus on.
   */
  double MatrixOptions::focusValue() {
    return m_focusTolSelectedElement;
  } // best, worst, the one selected from tolerance?


  /**
   * @return
   */
  /*
  QString MatrixOptions::focusImage1() {
    return m_image1;
  }
  */


  /**
   * @return
   */
  /*
  QString MatrixOptions::focusParameter1() {
    return m_parameter1;
  }
  */


  /**
   * @return
   */
  /*
  QString MatrixOptions::focusImage2() {
    return m_image2;
  }
  */


  /**
   * @return
   */
  /*
  QString MatrixOptions::focusParameter2() {
    return m_parameter2;
  }
  */


  /**
   * Return the list of good correlation values based on set tolerance.
   *
   * @return @b QList<double> The List of good correlation values.
   */
  QList<double> MatrixOptions::goodElements() {
    return m_goodElements;
  }


  /**
   * Return the list of bad correlation values based on set tolerance
   *
   * @return @b QList<double> The List of bad correlation values.
   */
  QList<double> MatrixOptions::badElements() {
    return m_badElements;
  }


  /**
   * Sets the focus option to focus on an element.
   *
   * @param option The option to specify how to set focus.
   */
  void MatrixOptions::setFocusOption(FocusOption option) {
    m_focusOption = option;
  }


  /**
   * @param value
   */
  /*
  void MatrixOptions::setFocusValue(double value) {
    m_focusTolSelectedElement = value;
  }
  */
  
  
  /**
   * Set the elements with good correlation values (defined by tolerance)
   *
   * @param goodElements QList of elements with good correlation values
   */
  /*
  void MatrixOptions::setGoodElements(QList<double> goodElements) {
    m_goodElements = goodElements;
  }
  */


  /**
   * Set the elements with bad correlation values (defined by tolerance)
   * 
   * @param badElements QList of elements with bad correlation values
   */
  /*
  void MatrixOptions::setBadElements(QList<double> badElements) {
    m_badElements = badElements;
  }
  */
  

  // Current Correlation Information
  /**
   * Returns the current selected correlation value.
   *
   * @return @b double The current correlation value.
   */
  double MatrixOptions::currentCorrelation() {
    return m_currentValue;
  }


  /**
   * @return
   */
  /*
  QString MatrixOptions::currentImage1() {
    return m_currentImg1;
  }
  */


  /**
   * @return
   */
  /*
  QString MatrixOptions::currentParameter1() {
    return m_currentParam1;
  }
  */


  /**
   * @return
   */
  /*
  QString MatrixOptions::currentImage2() {
    return m_currentImg2;
  }
  */


  /**
   * @return
   */
  /*
  QString MatrixOptions::currentParameter2() {
    return m_currentParam2;
  }
  */

  
  /**
   * Updates the value of the current correlation.
   *
   * @param value Value to set the current correlation to.
   */
  void MatrixOptions::setCurrentCorrelation(double value) {
    m_currentValue = value;
  }


  /**
   * Sets the value of the current image1 to the given QString.
   * 
   * @param current Name to set the current image1 to.
   */
  void MatrixOptions::setCurrentImage1(QString current) {
    m_currentImg1 = current;
  }


  /**
   * Sets the value of the current parameter1 to the given QString.
   *
   * @param current Name to set current parameter1 to.
   */
  void MatrixOptions::setCurrentParameter1(QString current) {
    m_currentParam1= current;
  }


  /**
   * Set the current image 2 to the given QString.
   *
   * @param current QString to set the current image 2 to.
   */
  void MatrixOptions::setCurrentImage2(QString current) {
    m_currentImg2 = current;
  }


  /**
   * Set the current parameter 2 to the given QString.
   *
   * @param current QString to set current parameter 2 to.
   */
  void MatrixOptions::setCurrentParameter2(QString current) {
    m_currentParam2 = current;
  }


  //Slots 
  /**
   * This slot will be called when a matrix element is clicked on.
   */
//   void MatrixOptions::updateCurrentCorrData(QString currentData) {
//     // update "current" values
//     emit changedCurrentCorrData();
//   }


  /**
   * Gets the parent correlation matrix's images and parameters mapping.
   *
   * @return @b QMap<QString,StringList> QMap of the correlation matrix image names to their
   *                                     parameters.
   */
  QMap<QString, QStringList> MatrixOptions::matrixImgsAndParams() {
    return *m_parentMatrix->imagesAndParameters();
  }


  /**
   * Accessor that returns the parent correlation matrix we are setting options for.
   *
   * @return @b CorrelationMatrix* Pointer to the correlation matrix we are setting options for.
   */
  CorrelationMatrix *MatrixOptions::parentMatrix() {
    return m_parentMatrix;
  }
};
