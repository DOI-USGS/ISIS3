#ifndef BinaryStretchType_H
#define BinaryStretchType_H

#include "StretchType.h"

namespace Isis {
  class Stretch;
  class Histogram;
}

class QSlider;
class QLineEdit;
class QString;
class QColor;

namespace Qisis {
  class BinaryStretchType : public StretchType {
      Q_OBJECT

    public:
      BinaryStretchType(const Isis::Histogram &, const Isis::Stretch &,
                        const QString &name, const QColor &color);
      ~BinaryStretchType();

      virtual void setStretch(Isis::Stretch);

    private slots:
      void startSliderMoved(int);
      void startEditChanged(const QString &);
      void endSliderMoved(int);
      void endEditChanged(const QString &);

    private:
      Isis::Stretch calculateNewStretch();

    private:
      QSlider *p_startSlider; //!< Start point slider
      QLineEdit *p_startEdit; //!< Start point edit
      QSlider *p_endSlider; //!< End point slider
      QLineEdit *p_endEdit; //!< End point edit

      //! This is used to let the edits be changed to where sliders cant go
      bool p_sliderOverride;

      //! This is used to let the edits be changed without updating the stretch
      bool p_editOverride;
  };
}

#endif
