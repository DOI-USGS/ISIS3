#include "MatrixOptions.h"

#include <QColor>
#include <QString>
#include <QList>

#include "CorrelationMatrix.h"
#include "MatrixSceneWidget.h"
#include "MatrixOptionsDialog.h"

namespace Isis {
  /**
   * Default Constructor
   */
  MatrixOptions::MatrixOptions(CorrelationMatrix parent, MatrixSceneWidget *scene) { // give parent? MatrixSceneWidget
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
//       m_imagesAndParameters = parentMatrix->imagesAndParameters(); // pointer the the one in correlation matrix?

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
   * Default Destructor
   */
  MatrixOptions::~MatrixOptions() {
  }


  
  // Color Options
  /**
   * The color selected for the correlation values that are below the given threshold.
   *
   * @return
   */
  QColor MatrixOptions::goodCorrelationColor() {
    return m_goodColor;
  }



  /**
   * The color selected for the correlation values that are above the given threshold.
   * 
   * @return
   */
  QColor MatrixOptions::badCorrelationColor() {
    return m_badColor;
  }



  /**
   * Threshold for what is considered a bad correlation.
   * 
   * @return
   */
  double MatrixOptions::colorTolerance() {
    return m_colorTolerance;
  }


  
  /**
   * Use the green-red gradient if false.
   * Use the 50/50 color split if true. You need the good/bad correlation color methods if true.
   * 
   * @return
   */
  bool MatrixOptions::colorScheme() {
    return m_tolerance; // if tolerance is false then we use gradient
  }



  /**
   *
   * @param
   */
  void MatrixOptions::setColorScheme(bool tolerance) {
    m_tolerance = tolerance;
  }



  /**
   *
   * @param
   */
  void MatrixOptions::setColorTolerance(double tolerance) {
    m_colorTolerance = tolerance;
  }



  /**
   *
   * @param
   */
  void MatrixOptions::setGoodCorrelationColor(QColor color) {
    m_goodColor = color;
  }



  /**
   *
   * @param
   */
  void MatrixOptions::setBadCorrelationColor(QColor color) {
    m_badColor = color;
  }

  
  // Focus Options
  /**
   *
   * @return
   */
  MatrixOptions::FocusOption MatrixOptions::focusOption() {
    return m_focusOption;
  }


  
  /**
   * The value of the spot on the matrix that we need to focus on. This can come from the best or
   * worst option as well as the good/bad lists when a tolerance is entered.
   *
   * @return
   */
  double MatrixOptions::focusValue() {
    return m_focusTolSelectedElement;
  } // best, worst, the one selected from tolerance?



  /**
   * @return
   */
  QString MatrixOptions::focusImage1() {
    return m_image1;
  }



  /**
   * @return
   */
  QString MatrixOptions::focusParameter1() {
    return m_parameter1;
  }



  /**
   * @return
   */
  QString MatrixOptions::focusImage2() {
    return m_image2;
  }



  /**
   * @return
   */
  QString MatrixOptions::focusParameter2() {
    return m_parameter2;
  }



  /**
   * @return
   */
  QList<double> MatrixOptions::goodElements() {
    return m_badElements;
  }



  /**
   * @return
   */
  QList<double> MatrixOptions::badElements() {
    return m_goodElements;
  }



  /**
   * @param
   */
  void MatrixOptions::setFocusOption(FocusOption option) {
    m_focusOption = option;
  }



  /**
   * @param
   */
  void MatrixOptions::setFocusValue(double value) {
    m_focusTolSelectedElement = value;
  }

  
  
  /**
   * @param
   */
  void MatrixOptions::setGoodElements(QList<double> goodElements) {
    m_goodElements = goodElements;
  }



  /**
   * @param
   */
  void MatrixOptions::setBadElements(QList<double> badElements) {
    m_badElements = badElements;
  }

  

  // Current Correlation Information



  /**
   * @return
   */
  double MatrixOptions::currentCorrelation() {
    return m_currentValue;
  }



  /**
   * @return
   */
  QString MatrixOptions::currentImage1() {
    return m_currentImg1;
  }



  /**
   * @return
   */
  QString MatrixOptions::currentParameter1() {
    return m_currentParam1;
  }



  /**
   * @return
   */
  QString MatrixOptions::currentImage2() {
    return m_currentImg2;
  }



  /**
   * @return
   */
  QString MatrixOptions::currentParameter2() {
    return m_currentParam2;
  }


  
  /**
   * @param
   */
  void MatrixOptions::setCurrentCorrelation(double value) {
    m_currentValue = value;
  }



  /**
   * @param
   */
  void MatrixOptions::setCurrentImage1(QString current) {
    m_currentImg1 = current;
  }



  /**
   * @param
   */
  void MatrixOptions::setCurrentParameter1(QString current) {
    m_currentParam1= current;
  }



  /**
   * @param
   */
  void MatrixOptions::setCurrentImage2(QString current) {
    m_currentImg2 = current;
  }



  /**
   * @param
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
   *
   * @return
   */
  QMap< QString, QStringList> MatrixOptions::matrixImgsAndParams() {
    return *m_parentMatrix->imagesAndParameters();
  }



  /**
   *
   *
   */
  CorrelationMatrix *MatrixOptions::parentMatrix() {
    return m_parentMatrix;
  }
};