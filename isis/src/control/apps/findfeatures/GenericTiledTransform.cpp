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

/** Construct named transfrom with 3x3 matrix with a size specification */
GenericTiledTransform::GenericTiledTransform(const QString &name, const cv::Mat &matrix,
                                   const cv::Size &size,
                                   const int tileSize) :
                                   GenericTransform(name), m_tileSize(tileSize){
  setMatrix(matrix);
  setSize(size);
}

/** Construct named transfrom with 3x3 matrix with a subarea specification */
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

std::tuple<int, int> GenericTiledTransform::getNumTiles(const cv::Size size) const{
  auto [width, height] = size;
  int tileSize = getTileSize();
  int nxTiles = (width + tileSize - 1) / tileSize;
  int nyTiles = (height + tileSize - 1) / tileSize;
  return {nxTiles, nyTiles};
}

std::pair<cv::Range, cv::Range> GenericTiledTransform::getTile(int tileID, cv::Size size) const{
  int tileSize = getTileSize();
  auto [width, height] = size;
  auto [nxTiles, nyTiles] = getNumTiles(size);

  int xTile = tileID % nxTiles;
  int yTile = tileID / nxTiles;

  int xStart = xTile * tileSize;
  int yStart = yTile * tileSize;
  // cv::Range is [start, stop)
  int yStop = std::min(yStart + m_tileSize, height);
  int xStop = std::min(xStart + m_tileSize, width);
  cv::Range xRange(xStart, xStop);
  cv::Range yRange(yStart, yStop);
  return std::make_pair(xRange, yRange);
}

std::pair<cv::Range, cv::Range> GenericTiledTransform::computeSourceRange(std::pair<cv::Range, cv::Range> dstROI) const{
    const cv::Range& dstRangeX = dstROI.first;
		const cv::Range& dstRangeY = dstROI.second;
		cv::Point2f dstCorner00 = cv::Point2i(dstRangeX.start,   dstRangeY.start);
		cv::Point2f dstCorner01 = cv::Point2i(dstRangeX.start,   dstRangeY.end - 1);
		cv::Point2f dstCorner10 = cv::Point2i(dstRangeX.end - 1, dstRangeY.start);
		cv::Point2f dstCorner11 = cv::Point2i(dstRangeX.end - 1, dstRangeY.end - 1);
		cv::Point2f srcCorner00 = inverse(dstCorner00);
		cv::Point2f srcCorner01 = inverse(dstCorner01);
		cv::Point2f srcCorner10 = inverse(dstCorner10);
		cv::Point2f srcCorner11 = inverse(dstCorner11);
		int srcMinX = (int)floor(std::min(std::min(srcCorner00.x, srcCorner01.x), std::min(srcCorner10.x, srcCorner11.x)));
		int srcMinY = (int)floor(std::min(std::min(srcCorner00.y, srcCorner01.y), std::min(srcCorner10.y, srcCorner11.y)));
		int srcMaxX = (int)ceil(std::max(std::max(srcCorner00.x, srcCorner01.x), std::max(srcCorner10.x, srcCorner11.x)));
		int srcMaxY = (int)ceil(std::max(std::max(srcCorner00.y, srcCorner01.y), std::max(srcCorner10.y, srcCorner11.y)));
		cv::Range srcRangeX(srcMinX, srcMaxX);
		cv::Range srcRangeY(srcMinY, srcMaxY);
		return std::make_pair(srcRangeX, srcRangeY);
}

std::pair<cv::Range, cv::Range> GenericTiledTransform::addSourceInterpMargin(const std::pair<cv::Range, cv::Range>& srcRangeXY,
                                                                             const cv::Mat &image,
                                                                             const int margin) const {
	const cv::Range& srcRangeX = srcRangeXY.first;
	const cv::Range& srcRangeY = srcRangeXY.second;
	int srcMinX = std::max(srcRangeX.start - margin, 0);
	int srcMaxX = std::min(srcRangeX.end   + margin, GenericTransform::getSize(image).width);
	int srcMinY = std::max(srcRangeY.start - margin, 0);
	int srcMaxY = std::min(srcRangeY.end   + margin, GenericTransform::getSize(image).height);
	return std::make_pair(cv::Range(srcMinX, srcMaxX), cv::Range(srcMinY, srcMaxY));
}

cv::Mat_<cv::Vec2f> GenericTiledTransform::computeMapping(const GenericTiledTransform::RangeXY& srcRangeXY,
                                                          const GenericTiledTransform::RangeXY& dstRangeXY) const {
  const cv::Range& dstRangeX = dstRangeXY.first;
  const cv::Range& dstRangeY = dstRangeXY.second;
  const cv::Range& srcRangeX = srcRangeXY.first;
  const cv::Range& srcRangeY = srcRangeXY.second;
  cv::Mat_<cv::Vec2f> mapXY(cv::Size(dstRangeX.size(), dstRangeY.size()));
  const cv::Point2f sourceTileOffset = cv::Point2i(srcRangeX.start, srcRangeY.start);
  for (int dstY = dstRangeY.start; dstY < dstRangeY.end; ++dstY)
  {
    for (int dstX = dstRangeX.start; dstX < dstRangeX.end; ++dstX)
    {
      cv::Point2f dstXY = cv::Point2i(dstX, dstY);
      cv::Point2f srcXY = inverse(dstXY) - sourceTileOffset;
      mapXY(dstY - dstRangeY.start, dstX - dstRangeX.start) = srcXY;
    }
  }
  return mapXY;
}

cv::Mat GenericTiledTransform::render(const cv::Mat &image) const{
  cv::Size tFormSize = transformedSize(getMatrix(), image.size()).size();
  cv::Mat result = cv::Mat(tFormSize, image.type());
  auto [nxTiles, nyTiles] = getNumTiles(tFormSize);
  int n_tiles = nxTiles*nyTiles;
  for (int i = 0; i < n_tiles; i++){
    const std::pair<cv::Range, cv::Range> dstRangeXY = getTile(i, tFormSize);
    const std::pair<cv::Range, cv::Range> srcRangeXY = addSourceInterpMargin(computeSourceRange(dstRangeXY), image, 3);
    const cv::Range& dstRangeX = dstRangeXY.first;
		const cv::Range& dstRangeY = dstRangeXY.second;
		const cv::Range& srcRangeX = srcRangeXY.first;
		const cv::Range& srcRangeY = srcRangeXY.second;
		const cv::Mat_<cv::Vec2f> mapXY = computeMapping(srcRangeXY, dstRangeXY);
    const cv::Mat roiSrc = image(srcRangeY, srcRangeX).clone(); // note: clone to ensure source matrix width less than 32768
    cv::Mat roiDst = result(dstRangeY, dstRangeX);
    cv::remap(roiSrc, roiDst, mapXY, cv::noArray(), cv::INTER_LINEAR, cv::BORDER_TRANSPARENT, cv::Scalar(0));
  }
  return result;
}
}
