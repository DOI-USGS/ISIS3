#ifndef LinearStretchType_H
#define LinearStretchType_H

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
  class LinearStretchType : public StretchType {
      Q_OBJECT

    public:
      LinearStretchType(const Isis::Histogram &, const Isis::Stretch &,
                        const QString &name, const QColor &color);
      ~LinearStretchType();

      virtual Isis::Stretch getStretch();
      virtual void setStretch(Isis::Stretch);


    private slots:
      void startSliderMoved(int);
      void startEditChanged(const QString &);
      void endSliderMoved(int);
      void endEditChanged(const QString &);

    private:
      QSlider *p_startSlider; //!< Line start pt slider
      QLineEdit *p_startEdit; //!< Line start pt edit
      QSlider *p_endSlider; //!< Line end pt slider
      QLineEdit *p_endEdit; //!< Line end pt edit

      //! This is used to let the edits be changed to where sliders cant go
      bool p_sliderOverride;

      //! This is used to let the edits be changed without updating the stretch
      bool p_editOverride;
  };
}

#endif
