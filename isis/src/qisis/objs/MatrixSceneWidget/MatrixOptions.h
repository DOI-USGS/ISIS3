#ifndef matrixOptions_h
#define matrixOptions_h

#include <QColor>
#include <QMap>
#include <QObject>
#include <QString>
#include <QStringList>

namespace Isis {

  class CorrelationMatrix;
  class MatrixSceneWidget;  

  /**
   * @brief This class holds the matrix color and focus information.
   *
   * This class is the go-between for the MatrixOptionsDialog and the MatrixSceneWidget.
   *
   * @ingroup Visualization Tools
   *
   * @author 2014-07-14 Kimberly Oyama
   *
   * @internal
   *   @history 2014-07-14 Kimberly Oyama - Original Version
   *   @history 2014-07-21 Kimberly Oyama - Connected apply button to scene. The color options are
   *                           now functional.
   *   @history 2016-07-08 Ian Humphrey - Updated documentation and reviewed coding standards in
   *                           preparing to add to trunk. Fixes #4095, #4081.
   */
  class MatrixOptions : public QObject {
      Q_OBJECT
    public:

      /**
       * Determines how to focus to a correlation element on the matrix.
       */
      enum FocusOption {
        Best, //!< Focus to the best correlation
        Worst, //!< Focus to the worst correlation
        Specific, //!< Focus to a specific correlation based on images and parameters
        Tolerance //TODO ????
      };
//       MatrixOptions(); // give parent? MatrixSceneWidget
//
      // give parent? MatrixSceneWidget
      MatrixOptions(CorrelationMatrix parent, MatrixSceneWidget *scene);
      ~MatrixOptions();

      // Color Options
      QColor goodCorrelationColor();
      QColor badCorrelationColor();
      double colorTolerance();
      bool colorScheme();

      void setColorScheme(bool tolerance);
      void setColorTolerance(double tolerance);
      void setGoodCorrelationColor(QColor color);
      void setBadCorrelationColor(QColor color);

      // Focus Options
      FocusOption focusOption();
      double focusValue(); // best, worst, the one selected from tolerance?
      QString focusImage1();
      QString focusParameter1();
      QString focusImage2();
      QString focusParameter2();
      QList<double> goodElements();
      QList<double> badElements();

      void setFocusOption(FocusOption option);
      void setFocusValue(double value);
      void setGoodElements(QList<double> goodElements);
      void setBadElements(QList<double> badElements);

      // Current Correlation Information
      double currentCorrelation();
      QString currentImage1();
      QString currentParameter1();
      QString currentImage2();
      QString currentParameter2();

      void setCurrentCorrelation(double value);
      void setCurrentImage1(QString current);
      void setCurrentParameter1(QString current);
      void setCurrentImage2(QString current);
      void setCurrentParameter2(QString current);

      QMap<QString, QStringList> matrixImgsAndParams();

      CorrelationMatrix *parentMatrix();
      signals:
        void optionsUpdated(); //!< Emitted when the options are updated
/*

      public slots:
        void updateOptions(QString currentData);*/
      
    private:

      CorrelationMatrix *m_parentMatrix; //!< Parent correlation matrix to set options for
      //Color Options
      bool m_tolerance; //!< If tolerance is false then we use gradient (i.e. if picking tolerance)
      QColor m_goodColor; //!< Color for good correlation
      QColor m_badColor; //!< Color for bad correlation
      double m_colorTolerance; //!< Threshold for what is considered bad correlation
      
      // Focus Options
      FocusOption m_focusOption; //!< Current focus option selected

      // Specific Parameters
      QString m_image1; //!< File name of image 1 (column) in focus
      QString m_parameter1; //!< Name of the parameter 1 (column) in focus
      QString m_image2; //!< File name of image 2 (row) in focus
      QString m_parameter2; //!< File name of parameter 2 (row) in focus
//       QMap<QString, QStringList> m_imagesAndParameters; // pointer the one in correlation matrix?

      // Tolerance
      double m_focusTolSelectedElement; //!< Tolerance value when selecting elements by focus
      QList<double> m_goodElements; //!< List of good correlation values in matrix
      QList<double> m_badElements;  //!< List of bad correlation values in matrix

      // Current Correlation Information
      double m_currentValue; //!< Current correlation value in focus
      QString m_currentImg1; //!< Current focused image 1 (column) in focus
      QString m_currentParam1; //!< Current focused parameter 1 (column) in focus
      QString m_currentImg2; //!< Current focused image 2 (row) in focus
      QString m_currentParam2; //!< Current focused parameter 2 (row) in focus
  };
};

#endif
