#ifndef STRETCHTYPE_H
#define STRETCHTYPE_H

#include <QWidget>

namespace Isis {
  class Stretch;
  class Cube;
  class Histogram;
}

class QTableWidget;
class QGridLayout;


namespace Qisis {
  class HistogramWidget;

  /**
   * @brief This is the base class for advanced stretches
   *
   * This has the common functionality between the advanced stretch types. It
   *  provides a histogram, the stretch table, and layouts. It also stores the
   *  stretch pairs. Using this should keep all of the advanced stretch
   *  interfaces similar and uniformly accessible from the stretch tool.
   *
   * @ingroup Visualization Tools
   *
   * @author 2010-05-20 Steven Lambright
   *
   * @internal
   */
  class StretchType : public QWidget {
      Q_OBJECT

    public:
      StretchType(const Isis::Histogram &hist, const Isis::Stretch &stretch,
                  const QString &name, const QColor &color);

      virtual ~StretchType();

      virtual Isis::Stretch getStretch();
      /**
       * Children must re-implement this to update their stretch pairs and GUI
       *   elements appropriately. This could be called with a
       *   different histogram but not a different stretch.
       */
      virtual void setStretch(Isis::Stretch) = 0;

      virtual void setHistogram(const Isis::Histogram &);


    protected: // methods
      QTableWidget *createStretchTable();


    protected: // data
      QGridLayout *p_mainLayout; //!< Main layout
      Isis::Histogram *p_cubeHist; //!< Visible area histogram
      QTableWidget *p_table; //!< Pairs table
      Qisis::HistogramWidget *p_graph; //!< Histogram graph
      Isis::Stretch *p_stretch; //!< Current stretch pairs stored here


    signals:
      //! Emitted when a new Stretch object is available
      void stretchChanged();


    private slots:
      void updateTable();
      void savePairs();
  };
};


#endif
