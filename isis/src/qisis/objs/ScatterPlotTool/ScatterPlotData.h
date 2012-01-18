#ifndef ScatterPlotData_h
#define ScatterPlotData_h

#include <qwt_raster_data.h>

#include <QScopedPointer>
#include <QVector>

#include "Stretch.h"

template <typename A, typename B> class QPair;
template <typename T> class QVector;

class QwtDoubleRange;

namespace Isis {
  class Cube;


  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   */
  class ScatterPlotData : public QwtRasterData {
    public:
      ScatterPlotData(Cube *xCube, int xCubeBand, int xBinCount,
                      Cube *yCube, int yCubeBand, int yBinCount,
                      QwtDoubleRange sampleRange, QwtDoubleRange lineRange);
      ScatterPlotData(const ScatterPlotData &other);

      ~ScatterPlotData();
      virtual QwtRasterData *copy() const;
      virtual QwtDoubleInterval range() const;
      virtual double value(double x, double y) const;

      double xCubeMin() const;
      double xCubeMax() const;
      double yCubeMin() const;
      double yCubeMax() const;

      void swap(ScatterPlotData &other);

      QPair<double, double> binXY(int binIndex) const;
      int binCount(int binIndex) const;
      int numberOfBins() const;

      QVector<double> discreteXValues() const;

      void alarm(double x, double y) const;
      void clearAlarms() const;

      ScatterPlotData &operator=(const ScatterPlotData &other);

    private:
      int binCount(int xIndex, int yIndex) const;
      int binIndex(int xIndex, int yIndex) const;
      int binIndex(double x, double y) const;
      QPair<int, int> binXYIndices(int binIndex) const;
      QPair<int, int> binXYIndices(double x, double y) const;

      QScopedPointer<Stretch> m_xDnToBinStretch; //!< Stretch 1
      QScopedPointer<Stretch> m_yDnToBinStretch; //!< Stretch 2

      QScopedPointer< QVector< QVector<int> > > m_counts;
      int m_maxCount;

      // map from bin index to alarm state (true for alarmed)
      mutable QScopedPointer< QMap<int, bool> > m_alarmedBins;

      double m_xCubeMin;
      double m_xCubeMax;
      double m_yCubeMin;
      double m_yCubeMax;
  };
};

#endif
