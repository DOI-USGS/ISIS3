#ifndef ADVANCEDSTRETCH_H
#define ADVANCEDSTRETCH_H

#include <QWidget>

class QStackedWidget;
class QLayout;
class QString;
class QColor;

namespace Isis {
  class Stretch;
  class Histogram;
};

namespace Qisis {
  class AdvancedStretch;
  class CubeViewport;

  /**
   * @brief Advanced Stretch Dialog
   *
   * This class is one of the panes on the advanced stretch dialog
   * (gray, red, green or blue). This contains advanced stretch
   * types of each kind and a selection between them.
   *
   * @ingroup Visualization Tools
   *
   * @author 2010-05-20 Steven Lambright
   *
   */

  class AdvancedStretch : public QWidget {
      Q_OBJECT

    public:
      AdvancedStretch(Isis::Histogram &, const Isis::Stretch &,
                      const QString &, const QColor &);
      ~AdvancedStretch();
      Isis::Stretch getStretch();
      void setStretch(Isis::Stretch newStretch);
      void setHistogram(const Isis::Histogram &newHist);

    signals:
      //! Emitted when a new stretch is available
      void stretchChanged();

    protected:

    private:
      QStackedWidget *p_stretchTypeStack; //!< StretchType's
  };
};


#endif

