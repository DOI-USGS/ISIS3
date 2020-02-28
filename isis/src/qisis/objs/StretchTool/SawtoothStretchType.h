#ifndef SAWTOOTHSTRETCHTYPE_H
#define SAWTOOTHSTRETCHTYPE_H

#include "StretchType.h"

class QColor;
class QLineEdit;
class QSlider;
class QString;

namespace Isis {
  class Stretch;
  class Histogram;

  /**
   * @brief This handles the advanced sawtooth stretch
   *
   * The sawtooth stretch is useful for highlighting things like craters. This
   *   highlights areas of DNs - for example it can highlight all of the DNs
   *   based on how close to the mean they are (user guesses the mean from the
   *   histogram). It can also highlight DNs as they differ from the mean.
   *   Anyhow, it existed in a form before so this class was needed to
   *   encapsulate it. I'm not sure what the original request for this stretch
   *   type derived from.
   *
   * @ingroup Visualization Tools
   *
   * @author 2010-05-20 Steven Lambright
   *
   * @internal
   */
  class SawtoothStretchType : public StretchType {
      Q_OBJECT

    public:
      SawtoothStretchType(const Histogram &, const Stretch &,
                          const QString &name, const QColor &color);
      ~SawtoothStretchType();

      virtual void setStretch(Stretch);

    private slots:
      void offsetSliderMoved(int);
      void offsetEditChanged(const QString &);
      void widthSliderMoved(int);
      void widthEditChanged(const QString &);

    private:
      Stretch calculateNewStretch();
      Stretch calculateNewStretch(double, double);

    private:
      QSlider *p_offsetSlider; //!< Offset slider
      QLineEdit *p_offsetEdit; //!< Offset edit
      QSlider *p_widthSlider;  //!< Width slider
      QLineEdit *p_widthEdit;  //!< Width edit

      //! This is used to let the edits be changed to where sliders cant go
      bool p_sliderOverride;
  };
}

#endif
