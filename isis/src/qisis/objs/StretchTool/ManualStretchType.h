#ifndef ManualStretchType_H
#define ManualStretchType_H

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

  /**
   * @brief This handles arbitrary user-input stretches
   *
   * This is designed to take any stretch pairs the user wants that we can
   *   handle.
   *
   * @ingroup Visualization Tools
   *
   * @author 2010-05-20 Steven Lambright
   *
   * @internal
   */
  class ManualStretchType : public StretchType {
      Q_OBJECT

    public:
      ManualStretchType(const Isis::Histogram &, const Isis::Stretch &,
                        const QString &name, const QColor &color);
      ~ManualStretchType();

      virtual void setStretch(Isis::Stretch);

    private slots:
      void addButtonPressed(bool);
      void deleteButtonPressed(bool);

    private:
      QLineEdit *p_inputEdit; //!< Input Stretch Value
      QLineEdit *p_outputEdit; //!< Output Stretch Value
  };
}

#endif
