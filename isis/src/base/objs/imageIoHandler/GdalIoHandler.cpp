#include "GdalIoHandler.h"

#include "Buffer.h"
#include "CubeCachingAlgorithm.h"
#include "IException.h"

#include <QList>
#include <QMutex>
#include <QString>

#include <iostream>

using namespace std;

namespace Isis {
  GdalIoHandler::GdalIoHandler(QString &dataFilePath, const QList<int> *virtualBandList, const Pvl &label) : 
                 ImageIoHandler(virtualBandList) {
    GDALAllRegister();
    const GDALAccess eAccess = GA_ReadOnly;
    std::string dataFilePathStr = dataFilePath.toUtf8().constData();
    const char *charDataFilePath = dataFilePathStr.c_str();
    m_geodataSet = GDALDatasetUniquePtr(GDALDataset::FromHandle(GDALOpen(charDataFilePath, eAccess)));
    if (!m_geodataSet) {
      QString msg = "Constructing GdalIoHandler failed";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
  }

  GdalIoHandler::~GdalIoHandler() {
  }

  void GdalIoHandler::read(Buffer &bufferToFill) const {
    GDALRasterBand  *poBand;
    int band = bufferToFill.Band();
    poBand = m_geodataSet->GetRasterBand(band);
    // Account for 1 based line and sample reading from ISIS process classes
    // as gdal reads 0 based lines and samples
    int lineStart = bufferToFill.Line() - 1;
    int sampleStart = bufferToFill.Sample() - 1;
    poBand->RasterIO(GF_Read, sampleStart, lineStart, 
                     bufferToFill.SampleDimension(), bufferToFill.LineDimension(),
                     bufferToFill.DoubleBuffer(), 
                     bufferToFill.SampleDimension(), bufferToFill.LineDimension(), 
                     GDT_Float64,
                     0, 0);
  }

  void GdalIoHandler::write(const Buffer &bufferToWrite) {}

  BigInt GdalIoHandler::getDataSize() const {
    return 0;
  }
  /**
   * Function to update the labels with a Pvl object
   *
   * @param labels Pvl object to update with
   */
  void GdalIoHandler::updateLabels(Pvl &labels) {}
}