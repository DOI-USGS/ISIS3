#ifndef BinaryStretchType_H
#define BinaryStretchType_H

#include "StretchType.h"

class QColor;
class QLineEdit;
class QSlider;
class QString;

namespace Isis {
  class Histogram;
  class Stretch;
  class CubeStretch;

  /**
   * @brief This handles the advanced binary stretch
   *
   * The binary stretch consists off DNs being "ON" or "OFF" (0 or 255). This
   *   is useful for finding DNs in a certain range. Because all stretches
   *   consist of lines with a slope, we can't actually have a binary stretch.
   *   This, however, makes a stretch as close to binary as it can.
   *
   * @ingroup Visualization Tools
   *
   * @author 2010-05-20 Steven Lambright
   *
   * @internal
   */
  class BinaryStretchType : public StretchType {
      Q_OBJECT

    public:
      BinaryStretchType(const Histogram &, const Stretch &,
                        const QString &name, const QColor &color);
      ~BinaryStretchType();

      virtual CubeStretch getStretch();
      virtual void setStretch(Stretch);

    private slots:
      void startSliderMoved(int);
      void startEditChanged(const QString &);
      void endSliderMoved(int);
      void endEditChanged(const QString &);

    private:
      Stretch calculateNewStretch();

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
