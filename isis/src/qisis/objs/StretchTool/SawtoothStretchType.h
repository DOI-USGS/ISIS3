#ifndef SAWTOOTHSTRETCHTYPE_H
#define SAWTOOTHSTRETCHTYPE_H

#include "StretchType.h"

namespace Isis {
  class Stretch;
  class Histogram;
}

class QSlider;
class QLineEdit;
class QColor;
class QString;

namespace Qisis {
  class SawtoothStretchType : public StretchType {
      Q_OBJECT

    public:
      SawtoothStretchType(const Isis::Histogram &, const Isis::Stretch &,
                          const QString &name, const QColor &color);
      ~SawtoothStretchType();

      virtual void setStretch(Isis::Stretch);


    private slots:
      void offsetSliderMoved(int);
      void offsetEditChanged(const QString &);
      void widthSliderMoved(int);
      void widthEditChanged(const QString &);

    private:
      Isis::Stretch calculateNewStretch();
      Isis::Stretch calculateNewStretch(double, double);

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
