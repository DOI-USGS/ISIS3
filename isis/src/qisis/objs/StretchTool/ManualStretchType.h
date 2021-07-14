#ifndef ManualStretchType_H
#define ManualStretchType_H

#include "StretchType.h"

class QColor;
class QLabel;
class QLineEdit;
class QPushButton;
class QSlider;
class QString;

namespace Isis {
  class Stretch;
  class Histogram;
  class CubeStretch;

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
   *   @history 2011-11-04 Steven Lambright - This should be much, much easier
   *                           to use now. Fixes #567.
   */
  class ManualStretchType : public StretchType {
      Q_OBJECT

    public:
      ManualStretchType(const Histogram &, const Stretch &,
                        const QString &name, const QColor &color);
      ~ManualStretchType();

      virtual CubeStretch getStretch();
      virtual void setStretch(Stretch);

    private slots:
      void addButtonPressed(bool);
      void deleteButtonPressed(bool);
      void readTable();

    private:
      Stretch convertTableToStretch();

      QLabel *p_errorMessage;
  };
}

#endif
