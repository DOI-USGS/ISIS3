#ifndef LinearStretchType_H
#define LinearStretchType_H

#include "StretchType.h"

class QColor;
class QLineEdit;
class QSlider;
class QString;

namespace Isis {
  class Histogram;
  class Stretch;

  /**
   * @brief This handles the advanced linear stretch
   *
   * The linear stretch is what happens without the advanced stretch tool. This
   *   is useful for visualizing and making adjustments of the linear stretch
   *   with a histogram of the data.
   *
   * @ingroup Visualization Tools
   *
   * @author 2010-05-20 Steven Lambright
   *
   * @internal
   */
  class LinearStretchType : public StretchType {
      Q_OBJECT

    public:
      LinearStretchType(const Histogram &, const Stretch &,
                        const QString &name, const QColor &color);
      ~LinearStretchType();

      virtual Stretch getStretch();
      virtual void setStretch(Stretch);

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
