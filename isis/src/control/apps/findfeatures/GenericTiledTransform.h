#ifndef GenericTiledTransform_h
#define GenericTiledTransform_h

#include <QString>
#include <QSharedPointer>

#include <opencv2/opencv.hpp>

#include "GenericTransform.h"

namespace Isis {

/**
 * @brief Apply a generic transform using a matrix with various options
 *
 * This class duplicates the functionality of GenericTransform, but allows for
 * tiled processing via the specification of a tile size.
 */
class GenericTiledTransform : public GenericTransform {
  public:
    typedef GenericTransform::RectArea   RectArea;
    typedef std::pair<cv::Range, cv::Range> RangeXY;
    GenericTiledTransform(const int tileSize);
    GenericTiledTransform(const QString &name, const int tileSize);
    GenericTiledTransform(const QString &name, const cv::Mat &matrix, const int tileSize);
    GenericTiledTransform(const QString &name, const cv::Mat &matrix,
                     const cv::Size &imSize, const int tileSize);
    GenericTiledTransform(const QString &name, const cv::Mat &matrix,
                     const RectArea &subArea, const int tileSize);

    cv::Size getSize(const cv::Mat &image = cv::Mat()) const;
    int getTileSize() const;
    static RectArea transformedSize(const cv::Mat &tmat, const cv::Size &imSize);

    std::pair<cv::Range, cv::Range> getTile(int tileID, cv::Size tForm) const;
    std::tuple<int, int> getNumTiles(const cv::Size area) const;
    std::pair<cv::Range, cv::Range> computeSourceRange(std::pair<cv::Range, cv::Range> destROI) const;
    std::pair<cv::Range, cv::Range> addSourceInterpMargin(const std::pair<cv::Range, cv::Range> &srcRangeXY,
                                                          const cv::Mat &image, const int margin) const;
    cv::Mat_<cv::Vec2f> computeMapping(const RangeXY& srcRangeXY, const RangeXY& dstRangeXY) const;
    virtual cv::Mat render(const cv::Mat &image) const;

  protected:
    void setTileSize(const int tileSize);

  private:
    const int m_tileSize;      //!< Size of tiles used to process the image
};

}  // namespace Isis
#endif
