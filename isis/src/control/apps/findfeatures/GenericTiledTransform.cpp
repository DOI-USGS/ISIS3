/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <string>
#include <vector>
#include <cmath>
#include <QString>
#include <QSharedPointer>
#include <opencv2/opencv.hpp>
#include "GenericTiledTransform.h"

namespace Isis {
/** Generic constructor is simply an identity transform */
GenericTiledTransform::GenericTiledTransform(const int tileSize) : GenericTransform("GenericTiledTransform"), m_tileSize(tileSize) {
  setMatrix(cv::Mat::eye(3, 3, CV_64FC1));
  setSize(cv::Size(0,0));
}

/** Named generic identity matrix */
GenericTiledTransform::GenericTiledTransform(const QString &name, const int tileSize) : GenericTransform(name), m_tileSize(tileSize) {
  setMatrix(cv::Mat::eye(3, 3, CV_64FC1));
  setSize(cv::Size(0,0));
}

/** Construct named transform with a 3x3 transformation matrix and a tilesize*/
GenericTiledTransform::GenericTiledTransform(const QString &name, const cv::Mat &matrix, const int tileSize) :
                                   GenericTransform(name), m_tileSize(tileSize){
  setMatrix(matrix);
  setSize(cv::Size(0,0));
}

/** Construct named transfrom with 3x3 matrix with a size specification and a tilesize*/
GenericTiledTransform::GenericTiledTransform(const QString &name, const cv::Mat &matrix,
                                   const cv::Size &size,
                                   const int tileSize) :
                                   GenericTransform(name), m_tileSize(tileSize){
  setMatrix(matrix);
  setSize(size);
}

/** Construct named transfrom with 3x3 matrix with a subarea specification and tilesize */
GenericTiledTransform::GenericTiledTransform(const QString &name, const cv::Mat &matrix,
                                   const GenericTransform::RectArea &subarea,
                                   const int tileSize) :
                                   GenericTransform(name), m_tileSize(tileSize) {
  cv::Mat tmatrix = ImageTransform::translation(-subarea.x, -subarea.y);
  setMatrix( tmatrix * matrix );
  setSize( subarea.size() );
}

int GenericTiledTransform::getTileSize() const{
  return m_tileSize;
}


/**
 * Computes the size of the image after applying the transformation matrix.
 *
 * @param tmat The transformation matrix
 * @param imSize The size of the input image for which to calculate the
 *               transformed size
 *
 * @return GenericTiledTransform::RectArea The bounding box of the transformed
 *         image.
 */
GenericTiledTransform::RectArea GenericTiledTransform::transformedSize(const cv::Mat &tmat,
                                                        const cv::Size &imSize) {

  std::vector<cv::Point2f> t_corners;
  cv::perspectiveTransform(corners(imSize), t_corners, tmat);

  double xmin(t_corners[0].x), xmax(t_corners[0].x);
  double ymin(t_corners[0].y), ymax(t_corners[0].y);
  for (unsigned int i = 1 ; i < t_corners.size() ; i++) {
    if ( t_corners[i].x < xmin) xmin = t_corners[i].x;
    if ( t_corners[i].x > xmax) xmax = t_corners[i].x;
    if ( t_corners[i].y < ymin) ymin = t_corners[i].y;
    if ( t_corners[i].y > ymax) ymax = t_corners[i].y;
  }

  return ( RectArea(xmin, ymin, (int) (xmax-xmin+0.5), (int) (ymax-ymin+0.5)) );
}


/**
 * Returns the number of tiles in an image with the specified size.
 *
 * @param size The size of the input image for which to calculate the number of
 *             tiles.
 *
 * @return  std::tuple<int, int> A tuple storing the number of xTiles and yTiles
 *          in the image.
 */
std::tuple<int, int> GenericTiledTransform::getNumTiles(const cv::Size size) const{
  int tileSize = getTileSize();
  int nxTiles = std::ceil(size.width/double(tileSize));
  int nyTiles = std::ceil(size.height/double(tileSize));
  return {nxTiles, nyTiles};
}


/**
 * Returns the rectangle bounds of the specified tile. Tiles are laid out such
 *  that x increases first, e.g.:
 *                          0 1 2 3
 *                          4 5 6 7
 *
 * @param tileID The numeric identifier of the tile for which to calculate the
 *               bounds
 * @param size The size of the image
 *
 * @return GenericTiledTransform::RectArea The rectangular boundary of the tile.
 */
GenericTiledTransform::RectArea GenericTiledTransform::getTile(int tileID, cv::Size size) const{
  int tileSize = getTileSize();
  std::tuple<int, int> xyTiles = getNumTiles(size);
  int nxTiles = std::get<0>(xyTiles);

  int xTile = tileID % nxTiles;
  int yTile = tileID / nxTiles;

  int ulx = xTile * tileSize;
  int uly = yTile * tileSize;

  return RectArea(ulx, uly, std::min(getTileSize(), size.width - ulx), std::min(getTileSize(), size.height - uly));
}


/**
 * Computes the region of interest (ROI) in the source image that maps to the
 * specified destination ROI.
 *
 * @param destROI The region of interest in the destination image for which we
 *                wish to calculate the source ROI.
 *
 * @return GenericTiledTransform::RectArea The rectangular boundary of the
 *         source ROI.
 */
GenericTiledTransform::RectArea GenericTiledTransform::computeSourceRect(RectArea destROI) const{
  std::vector<cv::Point2f> dstCorners = corners(destROI);
  std::vector<cv::Point2f> srcCorners;
  for (auto & corner : dstCorners) {
    srcCorners.push_back(inverse(corner));
  }
  int xmin(srcCorners[0].x), xmax(srcCorners[0].x);
  int ymin(srcCorners[0].y), ymax(srcCorners[0].y);
  for (unsigned int i = 1 ; i < srcCorners.size() ; i++) {
    if ( srcCorners[i].x < xmin) xmin = int(srcCorners[i].x);
    if ( srcCorners[i].x > xmax) xmax = int(srcCorners[i].x);
    if ( srcCorners[i].y < ymin) ymin = int(ceil(srcCorners[i].y));
    if ( srcCorners[i].y > ymax) ymax = int(ceil(srcCorners[i].y));
  }
  return RectArea(xmin, ymin, xmax-xmin, ymax-ymin);
}


/**
 * Compute the mapping between source / destination pixels their respective ROI
 *
 * @param srcROI The region of interest in the source image
 * @param destROI The region of interest in the destination image
 *
 * @return cv::Mat_<cv::Vec2f> The matrix that stores the mapping between src
 *         and destination ROI.
 */
cv::Mat_<cv::Vec2f> GenericTiledTransform::computeMapping(const RectArea &srcROI,
                                                          const RectArea &destROI) const {
  cv::Mat_<cv::Vec2f> mapXY(destROI.size());
  const cv::Point2f offset = cv::Point2i(srcROI.x, srcROI.y);
  for (int y = destROI.y; y < destROI.y + destROI.height; ++y){
    for (int x = destROI.x; x < destROI.x + destROI.width; ++x){
      cv::Point2f srcPt = inverse(cv::Point2i(x,y)) - offset;
      mapXY(y - destROI.y, x - destROI.x) = srcPt;
    }
  }
  return mapXY;
}


/**
 * Perform the transformation on an image matrix.
 *
 * @param image The input image data matrix to transform.
 *
 * @return @b cv::Mat The transformed matrix.
 */
cv::Mat GenericTiledTransform::render(const cv::Mat &image) const{
  cv::Size tFormSize = transformedSize(getMatrix(), image.size()).size();
  cv::Mat result = cv::Mat(tFormSize, image.type());
  std::tuple<int, int> xyTiles = getNumTiles(tFormSize);
  int nxTiles = std::get<0>(xyTiles);
  int nyTiles = std::get<1>(xyTiles);
  int n_tiles = nxTiles*nyTiles;
  for (int i = 0; i < n_tiles; i++){
    const RectArea destROI = getTile(i, tFormSize);
    RectArea srcROI= computeSourceRect(destROI);
    // Ensure that we don't attempt to access outside the source image.
    srcROI.x = std::max(srcROI.x, 0);
    srcROI.y = std::max(srcROI.y, 0);
    srcROI.width = std::min(srcROI.width, image.size().width);
    srcROI.height= std::min(srcROI.height, image.size().height);
    const cv::Mat_<cv::Vec2f> mapXY = computeMapping(srcROI, destROI);
    const cv::Mat roiSrc = image(srcROI).clone();
    cv::Mat outputROI = result(destROI);
    cv::remap(roiSrc, outputROI, mapXY, cv::noArray(), cv::INTER_LINEAR, cv::BORDER_TRANSPARENT, cv::Scalar(0));
  }
  return result;
}
}
