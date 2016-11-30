#ifndef matrixOptions_h
#define matrixOptions_h

#include <QColor>
#include <QString>
#include <QMap>
#include <QObject>
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
   */
  class MatrixOptions : public QObject {
      Q_OBJECT
    public:

      enum FocusOption {
        Best,
        Worst,
        Specific,
        Tolerance
      };
//       MatrixOptions(); // give parent? MatrixSceneWidget
      MatrixOptions(CorrelationMatrix parent, MatrixSceneWidget *scene); // give parent? MatrixSceneWidget
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

      QMap< QString, QStringList> matrixImgsAndParams();

      CorrelationMatrix *parentMatrix();
      signals:
        void optionsUpdated();
/*

      public slots:
        void updateOptions(QString currentData);*/
      
    private:

      CorrelationMatrix *m_parentMatrix;
      //Color Options
      bool m_tolerance; // if tolerance is false then we use gradient
      QColor m_goodColor;
      QColor m_badColor;
      double m_colorTolerance;
      
      // Focus Options
      FocusOption m_focusOption;

      // Specific Parameters
      QString m_image1;
      QString m_parameter1;
      QString m_image2;
      QString m_parameter2;
//       QMap<QString, QStringList> m_imagesAndParameters; // pointer the the one in correlation matrix?

      // Tolerance
      double m_focusTolSelectedElement;
      QList<double> m_goodElements;
      QList<double> m_badElements;

      // Current Correlation Information
      double m_currentValue;
      QString m_currentImg1;
      QString m_currentParam1;
      QString m_currentImg2;
      QString m_currentParam2;
  };
};

#endif