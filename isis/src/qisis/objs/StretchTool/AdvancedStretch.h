#ifndef ADVANCEDSTRETCH_H
#define ADVANCEDSTRETCH_H

#include <QWidget>

class QStackedWidget;
class QComboBox;
class QLayout;
class QString;
class QColor;

namespace Isis {
  class AdvancedStretch;
  class CubeViewport;
  class Histogram;
  class Stretch;

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
   * @internal
   */
  class AdvancedStretch : public QWidget {
      Q_OBJECT

    public:
      AdvancedStretch(Histogram &, const Stretch &,
                      const QString &, const QColor &);
      ~AdvancedStretch();
      Stretch getStretch();
      void setStretch(Stretch newStretch);
      void restoreSavedStretch(Stretch newStretch);
      void setHistogram(const Histogram &newHist);

    signals:
      //! Emitted when a new stretch is available
      void stretchChanged();
      void saveToCube();
      void deleteFromCube(); 
      void loadStretch(); 

    private:
      QStackedWidget *p_stretchTypeStack; //!< StretchType's
      QComboBox *p_stretchTypeSelection; //!< ComboBox of StretchTypes
  };
};


#endif

