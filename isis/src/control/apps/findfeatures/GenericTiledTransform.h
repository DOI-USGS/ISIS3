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

    RectArea getTile(int tileID, cv::Size tForm) const;
    std::tuple<int, int> getNumTiles(const cv::Size area) const;
    RectArea computeSourceRect(const RectArea destROI) const;
    cv::Mat_<cv::Vec2f> computeMapping(const RectArea &srcROI, const RectArea &destROI) const;
    virtual cv::Mat render(const cv::Mat &image) const;

  protected:
    void setTileSize(const int tileSize);

  private:
    const int m_tileSize;      //!< Size of tiles used to process the image
};

}  // namespace Isis
#endif
