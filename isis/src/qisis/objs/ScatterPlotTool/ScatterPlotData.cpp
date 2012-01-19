#include "IsisDebug.h"
#include "ScatterPlotData.h"

#include <algorithm>

#include <qwt_double_range.h>

#include "Brick.h"
#include "SpecialPixel.h"

namespace Isis {

  /**
   * ScatterPlotDataConstructor
   *
   *
   * @param cube1
   * @param band1
   * @param cube2
   * @param band2
   * @param numBins
   */
  ScatterPlotData::ScatterPlotData(
      Cube *xCube, int xCubeBand, int xBinCount,
      Cube *yCube, int yCubeBand, int yBinCount,
      QwtDoubleRange sampleRange, QwtDoubleRange lineRange) : QwtRasterData(),
      m_xDnToBinStretch(new Stretch), m_yDnToBinStretch(new Stretch),
      m_counts(
        new QVector< QVector<int> >(yBinCount, QVector<int>(xBinCount))),
      m_alarmedBins(new QMap<int, bool>) {
    int startLine = qRound(lineRange.minValue());
    int endLine = qRound(lineRange.maxValue());
    ASSERT(xCube->getLineCount() == yCube->getLineCount());

    Histogram *xCubeHist = new Histogram(*xCube, xCubeBand, NULL,
        sampleRange.minValue(), lineRange.minValue(),
        sampleRange.maxValue(), lineRange.maxValue(),
        xBinCount, true);
    m_xCubeMin = xCubeHist->Minimum();
    m_xCubeMax = xCubeHist->Maximum();


    Histogram *yCubeHist = new Histogram(*yCube, yCubeBand, NULL,
        sampleRange.minValue(), lineRange.minValue(),
        sampleRange.maxValue(), lineRange.maxValue(),
        yBinCount, true);
    m_yCubeMin = yCubeHist->Minimum();
    m_yCubeMax = yCubeHist->Maximum();


    m_xDnToBinStretch->AddPair(m_xCubeMin, 0);
    m_xDnToBinStretch->AddPair(m_xCubeMax, xBinCount - 1);

    m_yDnToBinStretch->AddPair(m_yCubeMin, 0);
    m_yDnToBinStretch->AddPair(m_yCubeMax, yBinCount - 1);

    setBoundingRect(QwtDoubleRect(m_xCubeMin,
                                  m_yCubeMin,
                                  m_xCubeMax - m_xCubeMin,
                                  m_yCubeMax - m_yCubeMin));

    m_maxCount = 0;

    Brick brick1((int)(sampleRange.maxValue() - sampleRange.minValue() + 1),
                 1, 1, xCube->getPixelType());
    Brick brick2((int)(sampleRange.maxValue() - sampleRange.minValue() + 1),
                 1, 1, yCube->getPixelType());
    ASSERT(xCube->getSampleCount() == yCube->getSampleCount());
    ASSERT(brick1.size() == brick2.size());

    for (int line = startLine; line <= endLine; line++) {
      brick1.SetBasePosition(qRound(sampleRange.minValue()), line, xCubeBand);
      xCube->read(brick1);

      brick2.SetBasePosition(qRound(sampleRange.minValue()), line, yCubeBand);
      yCube->read(brick2);

      for (int i = 0; i < brick1.size(); i++) {
        double xDn = brick1[i];
        double yDn = brick2[i];

        if (!IsSpecial(xDn) && !IsSpecial(yDn)) {
          double x = m_xDnToBinStretch->Map(xDn);
          double y = m_yDnToBinStretch->Map(yDn);

          if (!IsSpecial(x) && !IsSpecial(y)) {
            int roundedX = qRound(x);
            int roundedY = qRound(y);

            if (roundedX >= 0 && roundedX < xBinCount &&
                roundedY >= 0 && roundedY < yBinCount) {
              int value = (*m_counts)[roundedY][roundedX] + 1;
              (*m_counts)[roundedY][roundedX] = value;

              m_maxCount = qMax(m_maxCount, value);
            }
          }
        }
      }
    }
  }


  ScatterPlotData::ScatterPlotData(const ScatterPlotData &other) {
    m_xDnToBinStretch.reset(new Stretch(*other.m_xDnToBinStretch));
    m_yDnToBinStretch.reset(new Stretch(*other.m_yDnToBinStretch));
    m_counts.reset(new QVector< QVector<int> >(*other.m_counts));
    m_alarmedBins.reset(new QMap< int, bool>(*other.m_alarmedBins));
    m_maxCount = other.m_maxCount;
    m_xCubeMin = other.m_xCubeMin;
    m_xCubeMax = other.m_xCubeMax;
    m_yCubeMin = other.m_yCubeMin;
    m_yCubeMax = other.m_yCubeMax;
  }


  /**
   * Destructor
   *
   */
  ScatterPlotData::~ScatterPlotData() {
  }


  /**
   * Returns a copy of the ScatterPlotData object.
   *
   */
  QwtRasterData *ScatterPlotData::copy() const {
    return new ScatterPlotData(*this);
  }


  /**
   * Returns the range of the counts.
   *
   */
  QwtDoubleInterval ScatterPlotData::range() const {
    return QwtDoubleInterval(0.0, m_maxCount);
  }


  /**
   * This gets called every time the scatter plot is re-drawn.
   * It returns the counts for each DN (x), DN (y).
   */
  double ScatterPlotData::value(double x, double y) const {
    double value = 0;

    QPair<int, int> indices = binXYIndices(x, y);
    int index = binIndex(indices.first, indices.second);

    if (index != -1 && (*m_alarmedBins)[index])
      value = m_maxCount;
    else
      value = binCount(indices.first, indices.second);

    return value;
  }


  double ScatterPlotData::xCubeMin() const {
    return m_xCubeMin;
  }


  double ScatterPlotData::xCubeMax() const {
    return m_xCubeMax;
  }


  double ScatterPlotData::yCubeMin() const {
    return m_yCubeMin;
  }


  double ScatterPlotData::yCubeMax() const {
    return m_yCubeMax;
  }


  void ScatterPlotData::swap(ScatterPlotData &other) {
    m_xDnToBinStretch.swap(other.m_xDnToBinStretch);
    m_yDnToBinStretch.swap(other.m_yDnToBinStretch);
    m_counts.swap(other.m_counts);
    m_alarmedBins.swap(other.m_alarmedBins);
    std::swap(m_maxCount, other.m_maxCount);
    std::swap(m_xCubeMin, other.m_xCubeMin);
    std::swap(m_xCubeMax, other.m_xCubeMax);
    std::swap(m_yCubeMin, other.m_yCubeMin);
    std::swap(m_yCubeMax, other.m_yCubeMax);
  }


  QPair<double, double> ScatterPlotData::binXY(int index) const {
    QPair<int, int> indices = binXYIndices(index);
    int xIndex = indices.first;
    int yIndex = indices.second;

    if (xIndex != -1 && yIndex != -1) {
      int xSize = 0;
      int ySize = m_counts->size();

      // Assume square 2D structurs
      if (ySize > 0)
        xSize = (*m_counts)[0].size();

      double percentAcrossXRange = ((double)xIndex / (double)xSize);
      double xDnValue = m_xCubeMin +
          percentAcrossXRange * (m_xCubeMax - m_xCubeMin);

      double percentAcrossYRange = ((double)yIndex / (double)ySize);
      double yDnValue = m_yCubeMin +
          percentAcrossYRange * (m_yCubeMax - m_yCubeMin);

      return QPair<double, double>(xDnValue, yDnValue);
    }


    iString msg = "Bin at index [" + iString(index) + "] not found. "
                  "There are [" + iString(numberOfBins()) + "] bins";
    throw iException::Message(iException::Programmer, msg, _FILEINFO_);
  }


  int ScatterPlotData::binCount(int binIndex) const {
    QPair<int, int> indices = binXYIndices(binIndex);
    return binCount(indices.first, indices.second);
  }


  int ScatterPlotData::numberOfBins() const {
    int xSize = 0;
    int ySize = m_counts->size();

    // Assume square 2D structurs
    if (ySize > 0)
      xSize = (*m_counts)[0].size();

    return xSize * ySize;
  }


  QVector<double> ScatterPlotData::discreteXValues() const {
    QVector<double> xValues;

    int ySize = m_counts->size();
    if (ySize) {
      int xSize = (*m_counts)[0].size();
      xValues.resize(xSize);

      for (int xIndex = 0; xIndex < xSize; xIndex++) {
        double percentAcrossXRange = ((double)xIndex / (double)xSize);
        xValues[xIndex] = m_xCubeMin +
            percentAcrossXRange * (m_xCubeMax - m_xCubeMin);
      }
    }

    return xValues;
  }


  void ScatterPlotData::alarm(double x, double y) const {
    int binToAlarm = binIndex(x, y);
    if (binToAlarm != -1)
      (*m_alarmedBins)[binToAlarm] = true;
  }


  void ScatterPlotData::clearAlarms() const {
    m_alarmedBins->clear();
  }


  ScatterPlotData &ScatterPlotData::operator=(const ScatterPlotData &other) {
    ScatterPlotData tmp(other);

    swap(tmp);

    return *this;
  }


  int ScatterPlotData::binCount(int xIndex, int yIndex) const {
    int count = 0;

    if (yIndex >= 0 && yIndex < m_counts->size()) {
      if (xIndex >= 0 && xIndex < (*m_counts)[yIndex].size()) {
        count = (*m_counts)[yIndex][xIndex];
      }
    }

    return count;
  }


  int ScatterPlotData::binIndex(int xIndex, int yIndex) const {
    int xSize = 0;
    int ySize = m_counts->size();

    // Assume square 2D structurs
    if (ySize > 0)
      xSize = (*m_counts)[0].size();

    int index = -1;
    if ((xIndex >= 0 && xIndex < xSize) && (yIndex >= 0 && yIndex < ySize))
      index = xSize * yIndex + xIndex;

    return index;
  }


  int ScatterPlotData::binIndex(double x, double y) const {
    QPair<int, int> indices = binXYIndices(x, y);
    return binIndex(indices.first, indices.second);
  }


  QPair<int, int> ScatterPlotData::binXYIndices(int binIndex) const {
    int xSize = 0;
    int ySize = m_counts->size();

    // Assume square 2D structurs
    if (ySize > 0)
      xSize = (*m_counts)[0].size();

    int yIndex = (binIndex / xSize);
    binIndex -= yIndex * xSize;

    int xIndex = binIndex;

    if (xIndex < 0 || yIndex < 0 || xIndex >= xSize || yIndex >= ySize) {
      iString msg = "Bin at index [" + iString(binIndex) + "] not found. "
                    "There are [" + iString(numberOfBins()) + "] bins";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    return QPair<int, int>(xIndex, yIndex);
  }


  QPair<int, int> ScatterPlotData::binXYIndices(double x, double y) const {
    QPair<int, int> indices(-1, -1);

    if (m_counts->size() && m_counts->at(0).size()) {
      double xBinPosition = m_xDnToBinStretch->Map(x);
      double yBinPosition = m_yDnToBinStretch->Map(y);

      if (!IsSpecial(xBinPosition) && !IsSpecial(yBinPosition)) {
        int discreteXBin = qRound(xBinPosition);
        int discreteYBin = qRound(yBinPosition);

        if (discreteXBin >= 0 && discreteXBin < m_counts->at(0).size() &&
            discreteYBin >= 0 && discreteYBin < m_counts->size()) {
          indices = QPair<int, int>(discreteXBin, discreteYBin);
        }
      }
    }

    return indices;
  }
}

