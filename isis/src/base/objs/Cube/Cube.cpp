/**
 * @file
 * $Revision: 1.19 $
 * $Date: 2010/03/22 19:44:53 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */
#include "IsisDebug.h"
#include "Cube.h"

#include <sstream>
#include <unistd.h>

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMutex>

#include "Application.h"
#include "Camera.h"
#include "CameraFactory.h"
#include "CubeAttribute.h"
#include "CubeBsqHandler.h"
#include "CubeTileHandler.h"
#include "Endian.h"
#include "FileName.h"
#include "Histogram.h"
#include "IException.h"
#include "LineManager.h"
#include "Message.h"
#include "Preference.h"
#include "ProgramLauncher.h"
#include "Projection.h"
#include "SpecialPixel.h"
#include "Statistics.h"
#include "TProjection.h"
#include "Longitude.h"

using namespace std;

namespace Isis {
  //! Constructs a Cube object.
  Cube::Cube() {
    construct();
  }

  /**
   * Construct a cube and open it for reading or reading/writing.
   *
   * @param fileName Name of the cube file to open. Environment
   *     variables in the filename will be automatically expanded.
   * @param access Defines how the cube will be opened. Either read-only
   *     "r" or read-write "rw".
   */
  Cube::Cube(const FileName &fileName, QString access) {
    construct();
    open(fileName.toString(), access);
  }

  void Cube::fromLabel(const FileName &fileName, Pvl &label, QString access) {
    PvlObject cubeLabel = label.findObject("IsisCube");
    PvlGroup dimensions = cubeLabel.findObject("Core").findGroup("Dimensions");
    close();

    setDimensions(dimensions["Samples"],
                          dimensions["Lines"],
                          dimensions["Bands"]);

    create(fileName.expanded());

    for (auto grpIt = cubeLabel.beginGroup(); grpIt!= cubeLabel.endGroup(); grpIt++) {
      putGroup(*grpIt);
    }

    close();
    open(fileName.toString(), access);
  }

  void Cube::fromIsd(const FileName &fileName, Pvl &label, nlohmann::json &isd, QString access) {
    fromLabel(fileName, label, access);
    attachSpiceFromIsd(isd);

    close();
    open(fileName.toString(), access);
  }

  void Cube::TestCube(const FileName &fileName, Pvl &label, nlohmann::json &isd, QString access) {
    // construct();
    // open(fileName.toString(), access);
    PvlObject cubeLabel = label.findObject("IsisCube");
    PvlGroup dimensions = cubeLabel.findObject("Core").findGroup("Dimensions");
    close();

    setDimensions(dimensions["Samples"],
                          dimensions["Lines"],
                          dimensions["Bands"]);

    create(fileName.expanded());

    for (auto grpIt = cubeLabel.beginGroup(); grpIt!= cubeLabel.endGroup(); grpIt++) {
      putGroup(*grpIt);
    }
    attachSpiceFromIsd(isd);


  }


  //! Destroys the Cube object.
  Cube::~Cube() {
    close();

    delete m_mutex;
    m_mutex = NULL;

    delete m_camera;
    m_camera = NULL;


    delete m_projection;
    m_projection = NULL;

    delete m_formatTemplateFile;
    m_formatTemplateFile = NULL;
  }


  /**
   * Test if a cube file has been opened/created.
   *
   * @returns True if a cube has been opened and I/O operations are allowed
   */
  bool Cube::isOpen() const {
    bool open = (m_ioHandler != NULL);

    ASSERT(open == (bool)m_labelFile);
    ASSERT(open == (bool)m_labelFileName);
    ASSERT(open == (bool)m_label);

    return open;
  }


  /**
   * Returns true if the labels of the cube appear to have a valid mapping
   * group. This returning true does not guarantee that the cube can project or
   * that the Projection() method will succeed.
   *
   *
   * @return bool True if the file should have a valid projection
   */
  bool Cube::isProjected() const {
    return label()->findObject("IsisCube").hasGroup("Mapping");
  }


  /**
   * Test if the opened cube is read-only, that is write operations will fail
   *   if this is true. A cube must be opened in order to call this method.
   *
   * @returns True if the cube is opened read-only
   */
  bool Cube::isReadOnly() const {
    bool readOnly = false;

    if (!isOpen()) {
      QString msg = "No cube opened";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    if ((m_labelFile->openMode() & QIODevice::ReadWrite) != QIODevice::ReadWrite)
      readOnly = true;

    return readOnly;
  }


  /**
   * Test if the opened cube is read-write, that is read and write operations
   *   should succeed if this is true. A cube must be opened in order to call
   *   this method.
   *
   * @returns True if the cube is opened read-write
   */
  bool Cube::isReadWrite() const {
    return !isReadOnly();
  }


  /**
   * Test if labels are attached. If a cube is open, then this indicates
   *   whether or not the opened cube's labels are attached. If a cube is not
   *   open, then this indicates whether or not a cube will be created with
   *   attached labels if create(...) is called.
   *
   * @returns True for attached labels, false for detached
   */
  bool Cube::labelsAttached() const {
    return m_attached;
  }


  /**
   * Closes the cube and updates the labels. Optionally, it deletes the cube if
   * requested.
   *
   * @param removeIt (Default value = false) Indicates if the file should be
   * removed/deleted.
   */
  void Cube::close(bool removeIt) {
    if (isOpen() && isReadWrite())
      writeLabels();

    cleanUp(removeIt);
  }


  /**
   * Copies the cube to the new fileName
   *
   * @param newFile FileName
   * @param newFileAttributes CubeAttributeOutput
   *
   * @return Cube copy of cube
   */
  Cube *Cube::copy(FileName newFile, const CubeAttributeOutput &newFileAttributes) {
    if (!isOpen()) {
      throw IException(IException::Unknown,
                       QObject::tr("Cube::copy requires the originating cube to be open"),
                       _FILEINFO_);
    }


    Cube *result = new Cube;

    if (newFileAttributes.labelAttachment() != ExternalLabel) {
      result->setDimensions(sampleCount(), lineCount(), bandCount());
      result->setByteOrder(newFileAttributes.byteOrder());
      result->setFormat(newFileAttributes.fileFormat());

      if (newFileAttributes.labelAttachment() == DetachedLabel) {
        result->setLabelsAttached(false);
      }

      if (newFileAttributes.propagatePixelType()) {
        result->setPixelType(pixelType());
      }
      else {
        result->setPixelType(newFileAttributes.pixelType());
      }

      if (newFileAttributes.propagateMinimumMaximum()) {
        if(result->pixelType() == Isis::Real) {
          result->setBaseMultiplier(0.0, 1.0);
        }
        else if(result->pixelType() >= pixelType()) {
          result->setBaseMultiplier(base(), multiplier());
        }
        else {
          QString msg =
              QObject::tr("Cannot reduce the output PixelType for [%1] from [%2] without output "
                          "pixel range").arg(newFile.original()).arg(fileName());
          throw IException(IException::User, msg, _FILEINFO_);
        }
      }
      else {
        // Not propagating so either the user entered or the programmer did
        result->setMinMax(newFileAttributes.minimum(), newFileAttributes.maximum());
      }

      result->setLabelSize(labelSize(true) + (1024 * 6));
    }
    else {
      if (isReadWrite()) {
        writeLabels();
        m_ioHandler->clearCache(true);
      }

      result->setExternalDnData(fileName());
    }

    // Allocate the cube
    result->create(newFile.expanded());

    PvlObject &isisCube = label()->findObject("IsisCube");
    PvlObject &outIsisCube = result->label()->findObject("IsisCube");
    for(int i = 0; i < isisCube.groups(); i++) {
      outIsisCube.addGroup(isisCube.group(i));
    }

    if (label()->hasObject("NaifKeywords")) {
      result->label()->addObject(
          label()->findObject("NaifKeywords"));
    }

    for (int i = 0; i < m_label->objects(); i++) {
      PvlObject &obj = m_label->object(i);
      if (obj.isNamed("Table") || obj.isNamed("Polygon") || obj.isNamed("OriginalLabel") ||
          obj.isNamed("History")) {
        Isis::Blob t((QString)obj["Name"], obj.name());
        read(t);
        result->write(t);
      }
    }

    if (newFileAttributes.labelAttachment() != ExternalLabel) {
      BufferManager input(sampleCount(), lineCount(), bandCount(),
                          sampleCount(), 1,              1,
                          pixelType());
      BufferManager output(sampleCount(), lineCount(), bandCount(),
                           sampleCount(), 1,              1,
                           result->pixelType());

      input.begin();
      output.begin();

      while (!input.end()) {
        read(input);
        output.Copy(input, false);

        result->write(output);

        input.next();
        output.next();
      }
    }

//   Just in case the orig label doesn't work... here's original code:
//       if((p_propagateOriginalLabel) && (InputCubes.size() > 0)) {
//         Isis::Pvl &inlab = *InputCubes[0]->label();
//         for(int i = 0; i < inlab.objects(); i++) {
//           if(inlab.Object(i).isNamed("OriginalLabel")) {
//             Isis::OriginalLabel ol;
//             InputCubes[0]->read(ol);
//             cube->write(ol);
//           }
//         }
//       }

    return result;
  }


  /**
   * This method will create an isis cube for writing.   The programmer should
   * make appropriate calls to Set methods before invoking Create.  If none are
   * made there are internal defaults which are:
   * @code
   *        Dimensions     512x512x1
   *        PixelType      Real
   *        ByteOrder      Matches architecture of host machine
   *        Attached       From user preference file
   *        Label Size     65536 bytes
   *        Format         Tiled
   *        Base           0.0
   *        Multiplier     1.0
   * @endcode
   *
   * @param cubeFileName Name of the cube file to open.  If the extenstion
   *     ".cub" is not given it will be appended (i.e., the extension of .cub
   *      is forced). Environment variables in the filename will be
   *      automatically expanded as well.
   */
  void Cube::create(const QString &cubeFileName) {
    // Already opened?
    if (isOpen()) {
      string msg = "You already have a cube opened";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    if (m_samples < 1 || m_lines < 1 || m_bands < 1) {
      QString msg = "Number of samples [" + toString(m_samples) +
          "], lines [" + toString(m_lines) + "], or bands [" + toString(m_bands) +
          "] cannot be less than 1";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    if (m_pixelType == None) {
      throw IException(IException::Unknown,
          QString("Cannot create the cube [%1] with a pixel type set to None")
            .arg(cubeFileName),
          _FILEINFO_);
    }

    if (m_storesDnData) {
      // Make sure the cube is not going to exceed the maximum size preference
      BigInt size = (BigInt)m_samples * m_lines *
                    (BigInt)m_bands * (BigInt)SizeOf(m_pixelType);

      size = size / 1024; // kb
      size = size / 1024; // mb
      size = size / 1024; // gb

      int maxSizePreference = 0;

      maxSizePreference =
          Preference::Preferences().findGroup("CubeCustomization")["MaximumSize"];

      if (size > maxSizePreference) {
        QString msg;
        msg += "The cube you are attempting to create [" + cubeFileName + "] is ["
               + toString(size) + "GB]. This is larger than the current allowed "
               "size of [" + toString(maxSizePreference) + "GB]. The cube "
               "dimensions were (S,L,B) [" + toString(m_samples) + ", " +
               toString(m_lines) + ", " + toString(m_bands) + "] with [" +
               toString(SizeOf(m_pixelType)) + "] bytes per pixel. If you still "
               "wish to create this cube, the maximum value can be changed in your personal "
               "preference file located in [~/.Isis/IsisPreferences] within the group "
               "CubeCustomization, keyword MaximumSize. If you do not have an ISISPreference file, "
               "please refer to the documentation \"Environment and Preference Setup\". Error ";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }

    // Expand output name
    FileName cubFile(cubeFileName);
    PvlObject isiscube("IsisCube");
    PvlObject core("Core");

    if (m_storesDnData) {
      cubFile = cubFile.addExtension("cub");

      // See if we have attached or detached labels
      if (m_attached) {
        // StartByte is 1-based (why!!) so we need to do + 1
        core += PvlKeyword("StartByte", toString(m_labelBytes + 1));
        m_labelFileName = new FileName(cubFile);
        m_dataFileName = new FileName(cubFile);
        m_labelFile = new QFile(m_labelFileName->expanded());
      }
      else {
        core += PvlKeyword("StartByte", toString(1));
        core += PvlKeyword("^Core", cubFile.name());
        m_dataFileName = new FileName(cubFile);
        m_dataFile = new QFile(realDataFileName().expanded());

        FileName labelFileName(cubFile);
        labelFileName = labelFileName.setExtension("lbl");
        m_labelFileName = new FileName(labelFileName);
        m_labelFile = new QFile(m_labelFileName->expanded());
      }

      // Create the size of the core
      PvlGroup dims("Dimensions");
      dims += PvlKeyword("Samples", toString(m_samples));
      dims += PvlKeyword("Lines",   toString(m_lines));
      dims += PvlKeyword("Bands",   toString(m_bands));
      core.addGroup(dims);

      // Create the pixel type
      PvlGroup ptype("Pixels");
      ptype += PvlKeyword("Type", PixelTypeName(m_pixelType));

      // And the byte ordering
      ptype += PvlKeyword("ByteOrder", ByteOrderName(m_byteOrder));
      ptype += PvlKeyword("Base", toString(m_base));
      ptype += PvlKeyword("Multiplier", toString(m_multiplier));
      core.addGroup(ptype);
    }
    else {
      cubFile = cubFile.addExtension("ecub");

      ASSERT(m_dataFileName);

      core += PvlKeyword("^DnFile", m_dataFileName->original());
//       m_dataFileName = new FileName(cubFile);
      m_dataFile = new QFile(realDataFileName().expanded());

      m_labelFileName = new FileName(cubFile);
      m_labelFile = new QFile(cubFile.expanded());
    }

    isiscube.addObject(core);

    m_label = new Pvl;
    m_label->addObject(isiscube);

    // Setup storage reserved for the label
    PvlObject lbl("Label");
    lbl += PvlKeyword("Bytes", toString(m_labelBytes));
    m_label->addObject(lbl);

    const PvlGroup &pref =
        Preference::Preferences().findGroup("CubeCustomization");
    bool overwrite = pref["Overwrite"][0].toUpper() == "ALLOW";
    if (!overwrite && m_labelFile->exists() && m_labelFile->size()) {
      QString msg = "Cube file [" + m_labelFileName->original() + "] exists, " +
                   "user preference does not allow overwrite";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    if (!m_labelFile->open(QIODevice::Truncate | QIODevice::ReadWrite)) {
      QString msg = "Failed to create [" + m_labelFile->fileName() + "]. ";
      msg += "Verify the output path exists and you have permission to write to the path.";
      cleanUp(false);
      throw IException(IException::Io, msg, _FILEINFO_);
    }

    if (m_dataFile) {
      if (m_storesDnData && !m_dataFile->open(QIODevice::Truncate | QIODevice::ReadWrite)) {
        QString msg = "Failed to create [" + m_dataFile->fileName() + "]. ";
        msg += "Verify the output path exists and you have permission to write to the path.";
        cleanUp(false);
        throw IException(IException::Io, msg, _FILEINFO_);
      }
      else if (!m_storesDnData && !m_dataFile->open(QIODevice::ReadOnly)) {
        QString msg = "Failed to open [" + m_dataFile->fileName() + "] for reading. ";
        msg += "Verify the output path exists and you have permission to read from the path.";
        cleanUp(false);
        throw IException(IException::Io, msg, _FILEINFO_);
      }
    }

    bool dataAlreadyOnDisk = m_storesDnData ? false : true;

    if (m_format == Bsq) {
      m_ioHandler = new CubeBsqHandler(dataFile(), m_virtualBandList, realDataFileLabel(),
                                       dataAlreadyOnDisk);
    }
    else {
      m_ioHandler = new CubeTileHandler(dataFile(), m_virtualBandList, realDataFileLabel(),
                                        dataAlreadyOnDisk);
    }

    if (m_storesDnData)
      m_ioHandler->updateLabels(*m_label);

    // Write the labels
    writeLabels();
  }


  /**
   * This method will create an isis cube for writing.   The programmer should
   * make appropriate calls to Set methods before invoking Create.  If none are
   * made there are internal defaults which are:
   * @code
   *        Dimensions     512x512x1
   *        PixelType      Real
   *        ByteOrder      Matches architecture of host machine
   *        Attached       From user preference file
   *        Label Size     65536 bytes
   *        Format         Tiled
   *        Base           0.0
   *        Multiplier     1.0
   * @endcode
   *
   * @param cubeFileName Name of the cube file to open.  If the extenstion
   *     ".cub" is not given it will be appended (i.e., the extension of .cub
   *      is forced). Environment variables in the filename will be
   *      automatically expanded as well.
   * @param att CubeAttributeOutput
   */
  void Cube::create(
      const QString &cubeFileName, const CubeAttributeOutput &att) {

    setByteOrder(att.byteOrder());
    setFormat(att.fileFormat());
    setLabelsAttached(att.labelAttachment() == AttachedLabel);
    if (!att.propagatePixelType())
      setPixelType(att.pixelType());
    setMinMax(att.minimum(), att.maximum());

    // Allocate the cube
    create(cubeFileName);
  }


  /**
   * This method will open an isis cube for reading or reading/writing.
   *
   * @param[in] cubeFileName Name of the cube file to open. Environment
   *     variables in the filename will be automatically expanded.
   * @param[in] access (Default value of "r") Defines how the cube will be
   *     accessed. Either read-only "r" or read-write "rw".
   */
  void Cube::open(const QString &cubeFileName, QString access) {
    // Already opened?

    if (isOpen()) {
      string msg = "You already have a cube opened";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    initLabelFromFile(cubeFileName, (access == "rw"));

    // Figure out the name of the data file
    try {
      PvlObject &core = m_label->findObject("IsisCube").findObject("Core");
      // Detached labels
      if (core.hasKeyword("^Core")) {
        FileName temp(core["^Core"][0]);

        if (!temp.originalPath().startsWith("/")) {
          m_dataFileName = new FileName(m_labelFileName->path() + "/" + temp.original());
        }
        else {
          m_dataFileName = new FileName(temp);
        }

        m_attached = false;
        m_storesDnData = true;

        m_dataFile = new QFile(realDataFileName().expanded());
      }
      // External cube files (ecub), ecub contains all labels and SPICE blobs, history
      else if (core.hasKeyword("^DnFile")) {
        FileName dataFileName(core["^DnFile"][0]);

        if (dataFileName.originalPath() == ".") {
          m_dataFileName = new FileName(m_labelFileName->path() + "/" + dataFileName.name());
        }
        else {
          m_dataFileName = new FileName(dataFileName);
        }

        m_attached = true;
        m_storesDnData = false;
        *m_dataFileName = FileName(realDataFileName().expanded());
        m_dataFile = new QFile(realDataFileName().expanded());
      }
      // Typical cube containing labels, SPICE, history and dn data
      else {
        m_dataFileName = new FileName(*m_labelFileName);
        m_attached = true;
        m_storesDnData = true;
      }
    }
    catch (IException &e) {
      cleanUp(false);
      throw;
    }

    if (access == "r") {
      if (!m_labelFile->open(QIODevice::ReadOnly)) {
        QString msg = "Failed to open [" + m_labelFile->fileName() + "] with "
            "read only access";
        cleanUp(false);
        throw IException(IException::Io, msg, _FILEINFO_);
      }

      if (m_dataFile) {
        if (!m_dataFile->open(QIODevice::ReadOnly)) {
          QString msg = "Failed to open [" + m_dataFile->fileName() + "] with "
              "read only access";
          cleanUp(false);
          throw IException(IException::Io, msg, _FILEINFO_);
        }
      }
    }

    else if (access == "rw") {
      if (!m_labelFile->open(QIODevice::ReadWrite)) {
        QString msg = "Failed to open [" + m_labelFile->fileName() + "] with "
            "read/write access";
        cleanUp(false);
        throw IException(IException::Io, msg, _FILEINFO_);
      }

      if (m_dataFile) {
        if (m_storesDnData && !m_dataFile->open(QIODevice::ReadWrite)) {
          QString msg = "Failed to open [" + m_dataFile->fileName() + "] with "
              "read/write access";
          cleanUp(false);
          throw IException(IException::Io, msg, _FILEINFO_);
        }
        else if (!m_storesDnData && !m_dataFile->open(QIODevice::ReadOnly)) {
          QString msg = "Failed to open [" + m_dataFile->fileName() + "] with "
              "read access";
          cleanUp(false);
          throw IException(IException::Io, msg, _FILEINFO_);
        }
      }
    }
    else {
      QString msg = "Unknown value for access [" + access + "]. Expected 'r' "
                    " or 'rw'";
      cleanUp(false);
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    initCoreFromLabel(*m_label);

    // Determine the number of bytes in the label
    if (m_attached) {
      m_labelBytes = m_label->findObject("Label")["Bytes"];
    }
    else {
      m_labelBytes = labelSize(true);
    }

    QPair<bool, Pvl *> dataLabel = qMakePair(false, m_label);
    if (!m_storesDnData) {
      dataLabel = qMakePair(true, new Pvl(m_dataFileName->expanded()));
    }

    // Now examine the format to see which type of handler to create
    if (m_format == Bsq) {
      m_ioHandler = new CubeBsqHandler(dataFile(), m_virtualBandList,
          realDataFileLabel(), true);
    }
    else {
      m_ioHandler = new CubeTileHandler(dataFile(), m_virtualBandList,
          realDataFileLabel(), true);
    }

    if (dataLabel.first) {
      delete dataLabel.second;
      dataLabel.second = NULL;
    }

    applyVirtualBandsToLabel();
  }


  /**
   * This method will reopen an isis sube for reading or reading/writing.
   *   If access requested is read/write and the open fails, open as read only
   *   and throw error.
   *
   * @param[in]   access  (QString)  Type of access needed (read or read/write
   *
   */
  void Cube::reopen(QString access) {
    if (!m_labelFile) {
      QString msg = "Cube has not been opened yet. The filename to re-open is "
          "unknown";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    // Preserve filename and virtual bands when re-opening
    FileName filename = *m_labelFileName;
    QList<int> virtualBandList;

    if (m_virtualBandList)
      virtualBandList = *m_virtualBandList;

    close();
    open(filename.expanded(), access);

    if (virtualBandList.size()) {
      if (m_virtualBandList)
        *m_virtualBandList = virtualBandList;
      else
        m_virtualBandList = new QList<int>(virtualBandList);
    }
  }


  /**
   * This method will read data from the specified Blob object.
   *
   * @param[in] blob The Blob data to be loaded
   *
   * @return (type)return description
   */
  void Cube::read(Blob &blob) const {
    if (!isOpen()) {
      string msg = "The cube is not opened so you can't read a blob from it";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    FileName cubeFile = *m_labelFileName;
    if (m_tempCube)
      cubeFile = *m_tempCube;

    QMutexLocker locker(m_mutex);
    QMutexLocker locker2(m_ioHandler->dataFileMutex());
    blob.Read(cubeFile.toString(), *label());
  }


  /**
   * This method will read a buffer of data from the cube as specified by the
   * contents of the Buffer object.
   *
   * @param bufferToFill Buffer to be loaded
   */
  void Cube::read(Buffer &bufferToFill) const {
    if (!isOpen()) {
      string msg = "Try opening a file before you read it";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    QMutexLocker locker(m_mutex);
    m_ioHandler->read(bufferToFill);
  }


  /**
   * This method will write a blob of data (e.g. History, Table, etc)
   * to the cube as specified by the contents of the Blob object.
   *
   * @param blob data to be written
   */
  void Cube::write(Blob &blob) {
    if (!isOpen()) {
      string msg = "The cube is not opened so you can't write a blob to it";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    if (!m_labelFile->isWritable()) {
      string msg = "The cube must be opened in read/write mode, not readOnly";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    // Write an attached blob
    if (m_attached) {
      QMutexLocker locker(m_mutex);
      QMutexLocker locker2(m_ioHandler->dataFileMutex());

      // Compute the number of bytes in the cube + label bytes and if the
      // endpos of the file // is not greater than this then seek to that position.
      fstream stream(m_labelFileName->expanded().toLatin1().data(),
                     ios::in | ios::out | ios::binary);
      stream.seekp(0, ios::end);

      // End byte = end byte of the file (aka eof position, file size)
      streampos endByte = stream.tellp();
      // maxbyte = position after the cube DN data and labels
      streampos maxbyte = (streampos) m_labelBytes;

      if (m_storesDnData) {
        maxbyte += (streampos) m_ioHandler->getDataSize();
      }

      // If EOF is too early, allocate space up to where we want the blob
      if (endByte < maxbyte) {
        stream.seekp(maxbyte, ios::beg);
      }

      blob.Write(*m_label, stream);
    }

    // Write a detached blob
    else {
      FileName blobFileName = fileName();
      blobFileName = blobFileName.removeExtension();
      blobFileName = blobFileName.addExtension(blob.Type());
      blobFileName = blobFileName.addExtension(blob.Name());
      QString blobFile(blobFileName.expanded());
      ios::openmode flags = ios::in | ios::binary | ios::out | ios::trunc;
      fstream detachedStream;
      detachedStream.open(blobFile.toLatin1().data(), flags);
      if (!detachedStream) {
        QString message = "Unable to open data file [" +
                          blobFileName.expanded() + "]";
        throw IException(IException::Io, message, _FILEINFO_);
      }

// Changed to work with mods to FileName class
//      blob.Write(p_cube.label,detachedStream,blobFileName.baseName()+"."+
//                 blob.Type()+"."+
//                 blobFileName.extension());
      blob.Write(*m_label, detachedStream, blobFileName.name());
    }
  }


  /**
   * This method will write a buffer of data from the cube as specified by the
   * contents of the Buffer object.
   *
   * @param bufferToWrite Buffer to be written.
   */
  void Cube::write(Buffer &bufferToWrite) {
    if (!isOpen()) {
      string msg = "Tried to write to a cube before opening/creating it";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    if (isReadOnly()) {
      QString msg = "Cannot write to the cube [" + (QString)QFileInfo(fileName()).fileName() +
          "] because it is opened read-only";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    if (!m_storesDnData) {
      QString msg = "The cube [" + QFileInfo(fileName()).fileName() +
          "] does not support storing DN data because it is using an external file for DNs";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }

    QMutexLocker locker(m_mutex);
    m_ioHandler->write(bufferToWrite);
  }


  /**
   * Used prior to the Create method, this will specify the base and multiplier
   * for converting 8-bit/16-bit back and forth between 32-bit:
   * @f[
   * 32-bit pixel = 8-bit/16-bit pixel * multiplier + base
   * @f]
   *
   * @param base Additive constant.
   * @param mult Multiplicative constant.
   */
  void Cube::setBaseMultiplier(double base, double mult) {
    openCheck();
    m_base = base;
    m_multiplier = mult;
  }


  /**
   * Used prior to the Create method, this will compute a good base and
   * multiplier value given the minimum/maximum range of the 32bit data. For
   * example, min=0.0 and max=1.0 of 32-bit pixels will ensure the base and
   * multiplier will cause the data to be spread out fully in the 8=bit or
   * 16-bit range.
   *
   * @param min Minimum 32-bit pixel.
   * @param max Maximum 32-bit pixel.
   */
  void Cube::setMinMax(double min, double max) {
    openCheck();

    m_base = 0.0;
    m_multiplier = 1.0;

    double x1, x2;
    if (m_pixelType == UnsignedByte) {
      x1 = VALID_MIN1;
      x2 = VALID_MAX1;
      m_multiplier = (max - min) / (x2 - x1);
      m_base = min - m_multiplier * x1;
    }
    else if (m_pixelType == SignedWord) {
      x1 = VALID_MIN2;
      x2 = VALID_MAX2;
      m_multiplier = (max - min) / (x2 - x1);
      m_base = min - m_multiplier * x1;
    }
    else if (m_pixelType == UnsignedWord) {
      x1 = VALID_MINU2;
      x2 = VALID_MAXU2;
      m_multiplier = (max - min) / (x2 - x1);
      m_base = min - m_multiplier * x1;
    }
  }


  /**
   * Used prior to the Create method, this will specify the byte order of pixels,
   * either least or most significant byte.
   *
   * @param byteOrder An enumeration of either Msb or Lsb.
   */
  void Cube::setByteOrder(ByteOrder byteOrder) {
    openCheck();
    m_byteOrder = byteOrder;
  }


  /**
   * Used prior to the Create method to specify the size of the cube. If not
   * invoked, a 512 x 512 x 1 cube will be created.
   *
   *
   * @param ns Number of samples
   * @param nl Number of lines
   * @param nb Number of bands
   */
  void Cube::setDimensions(int ns, int nl, int nb) {
    openCheck();
    if ((ns < 1) || (nl < 1) || (nb < 1)) {
      string msg = "SetDimensions:  Invalid number of sample, lines or bands";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
    m_samples = ns;
    m_lines = nl;
    m_bands = nb;
  }


  /**
   * Used to set external dn data to cube
   *
   * @param cubeFileWithDnData FileName of the cube with DN Data
   */
  void Cube::setExternalDnData(FileName cubeFileWithDnData) {
    try {
      initLabelFromFile(cubeFileWithDnData, false);
      initCoreFromLabel(*m_label);

      delete m_label;
      m_label = NULL;
    }
    catch (IException &) {
      delete m_label;
      m_label = NULL;
      throw;
    }

    m_storesDnData = false;
    m_dataFileName = new FileName(cubeFileWithDnData);

    delete m_labelFile;
    m_labelFile = NULL;

    delete m_labelFileName;
    m_labelFileName = NULL;
  }


  /**
   * Used prior to the Create method, this will specify the format of the cube,
   * either band, sequential or tiled.
   * If not invoked, a tiled file will be created.
   *
   * @param format An enumeration of either Bsq or Tile.
   */
  void Cube::setFormat(Format format) {
    openCheck();
    m_format = format;
  }


  /**
   * Use prior to calling create, this sets whether or not to use separate
   *   label and data files.
   *
   * @param attach If false, the labels and data will be in separate files.
   */
  void Cube::setLabelsAttached(bool attach) {
    openCheck();
    m_attached = attach;
  }


  /**
   * Used prior to the Create method, this will allocate a specific number of
   * bytes in the label area for attached files. If not invoked, 65536 bytes will
   * be reserved by default.
   *
   * @param[in] labelBytes Number of bytes to reserve for label space.
   */
  void Cube::setLabelSize(int labelBytes) {
    openCheck();
    m_labelBytes = labelBytes;
  }


  /**
   * Used prior to the Create method, this will specify the output pixel type.
   * If not invoked, the pixel type will be Real.
   *
   * @param pixelType An enumeration of the pixelType desired in the output cube.
   * See PixelType documentation for more information.
   */
  void Cube::setPixelType(PixelType pixelType) {
    openCheck();
    m_pixelType = pixelType;
  }


  /**
   * This allows the programmer to specify a subset of bands to work with.
   *   This works with both read and write operations, but is typically only
   *   used for reading. This is helpful because users can specify which
   *   bands from an input cube they want to work with. For example, if the
   *   user only wants to work with band 5 out of a 10 band cube, this can be
   *   accommodated.
   *
   * @param[in] vbands A vector of strings containing the virtual bands. The
   *   vector must contain integers in string form (e.g., "5", "10", "1").
   */
  void Cube::setVirtualBands(const QList<QString> &vbands) {
    openCheck();
    if (m_virtualBandList)
      m_virtualBandList->clear();
    else
      m_virtualBandList = new QList<int>;

    if (vbands.size() > 0) {
      QListIterator<QString> it(vbands);
      while (it.hasNext()) {
        m_virtualBandList->append(toInt(it.next()));
      }
    }
    else {
      delete m_virtualBandList;
      m_virtualBandList = NULL;
    }

    if (m_ioHandler) {
      m_ioHandler->setVirtualBands(m_virtualBandList);
    }
  }


  /**
   * This is a deprecated version of setVirtualBands(const QList<QString> &).
   *
   * @param[in] vbands A vector of strings containing the virtual bands. The
   *   vector must contain integers in string form (e.g., "5", "10", "1").
   */
  void Cube::setVirtualBands(const std::vector<QString> &vbands) {
    QList<QString> realVBands;

    for(unsigned int i = 0; i < vbands.size(); i++)
      realVBands << vbands[i];

    setVirtualBands(realVBands);
  }


  /**
   * Relocates the DN data for a cube to an external cube label file
   *
   * @param dnDataFile FileName to relocate the dn data to
   */
  void Cube::relocateDnData(FileName dnDataFile) {
    if (!isOpen()) {
      throw IException(IException::Unknown,
                       QString("Cannot relocate the DN data to [%1] for an external cube label "
                               "file which is not open.")
                         .arg(dnDataFile.original()),
                       _FILEINFO_);
    }


    if (m_storesDnData) {
      throw IException(IException::Unknown,
                       QString("The cube [%1] stores DN data. It cannot be relocated to [%2] - "
                               "this is only supported for external cube label files.")
                         .arg(m_labelFileName->original()).arg(dnDataFile.original()),
                       _FILEINFO_);
    }

    m_label->findObject("IsisCube").findObject("Core").findKeyword("^DnFile")[0] =
        dnDataFile.original();
    reopen(m_labelFile->isWritable()? "rw" : "r");
  }


//   void Cube::relocateDnData(FileName externalLabelFile, FileName dnDataFile) {
//     try {
//       Pvl externalLabelData(externalLabelFile.expanded());
//       externalLabelData.FindObject("IsisCube").FindObject("Core").FindKeyword("^DnFile")[0] =
//           dnDataFile.original();
//     }
//     catch (IException &e) {
//       throw IException(e, IException::Io,
//                        QString("File [%1] does not appear to be an external cube label file")
//                          .arg(externalLabelFile.original().ToQt()),
//                        _FILEINFO_);
//     }
//   }


  /**
   * Returns the number of virtual bands for the cube.
   *
   * @return int The number of bands in the cube.
   */
  int Cube::bandCount() const {
    int numBands = m_bands;
    if (m_virtualBandList)
      numBands = m_virtualBandList->size();
    return numBands;
  }


  /**
   * Returns the base value for converting 8-bit/16-bit pixels to 32-bit.
   * @f[
   * out = in * multiplier + base
   * @f]
   *
   * @return double The base value for converting 8-bit/16-bit pixels to
   *                32-bit.
   */
  double Cube::base() const {
    return m_base;
  }


  /**
   * Returns the byte order/endian-ness of the cube file. Cubes in a native
   *   byte order are quicker to read/write than those who must correct their
   *   byte order.
   *
   * @returns The byte order/endian-ness of the cube file
   */
  ByteOrder Cube::byteOrder() const {
    return m_byteOrder;
  }


  /**
   * Return a camera associated with the cube.  The generation of
   * the camera can throw an exception, so you might want to catch errors
   * if that interests you.
   *
   * @returns A camera based on the open cube
   */
  Camera *Cube::camera() {
    if (m_camera == NULL && isOpen()) {
      m_camera = CameraFactory::Create(*this);
    }
    return m_camera;
  }


  void Cube::attachSpiceFromIsd(nlohmann::json isd) {
    PvlKeyword lkKeyword("LeapSecond");
    PvlKeyword pckKeyword("TargetAttitudeShape");
    PvlKeyword targetSpkKeyword("TargetPosition");
    PvlKeyword ckKeyword("InstrumentPointing");
    PvlKeyword ikKeyword("Instrument");
    PvlKeyword sclkKeyword("SpacecraftClock");
    PvlKeyword spkKeyword("InstrumentPosition");
    PvlKeyword iakKeyword("InstrumentAddendum");
    PvlKeyword demKeyword("ShapeModel");
    PvlKeyword exkKeyword("Extra");

    Spice spice(*this->label(), isd);
    Table ckTable = spice.instrumentRotation()->Cache("InstrumentPointing");
    ckTable.Label() += PvlKeyword("Kernels");

    for (int i = 0; i < ckKeyword.size(); i++)
      ckTable.Label()["Kernels"].addValue(ckKeyword[i]);

    this->write(ckTable);

    Table spkTable = spice.instrumentPosition()->Cache("InstrumentPosition");
    spkTable.Label() += PvlKeyword("Kernels");
    for (int i = 0; i < spkKeyword.size(); i++)
      spkTable.Label()["Kernels"].addValue(spkKeyword[i]);

    this->write(spkTable);

    Table bodyTable = spice.bodyRotation()->Cache("BodyRotation");
    bodyTable.Label() += PvlKeyword("Kernels");
    for (int i = 0; i < targetSpkKeyword.size(); i++)
      bodyTable.Label()["Kernels"].addValue(targetSpkKeyword[i]);

    for (int i = 0; i < pckKeyword.size(); i++)
      bodyTable.Label()["Kernels"].addValue(pckKeyword[i]);

    bodyTable.Label() += PvlKeyword("SolarLongitude",
        toString(spice.solarLongitude().degrees()));
    this->write(bodyTable);

    Table sunTable = spice.sunPosition()->Cache("SunPosition");
    sunTable.Label() += PvlKeyword("Kernels");
    for (int i = 0; i < targetSpkKeyword.size(); i++)
      sunTable.Label()["Kernels"].addValue(targetSpkKeyword[i]);

    this->write(sunTable);

    PvlGroup currentKernels = this->group("Kernels");

    Pvl *label = this->label();
    int i = 0;
    while (i < label->objects()) {
      PvlObject currObj = label->object(i);
      if (currObj.isNamed("NaifKeywords")) {
        label->deleteObject(i);
      }
      else {
        i ++;
      }
    }

    *(this->label()) += spice.getStoredNaifKeywords();
  }


  /**
   * If this is an external cube label file, this will give you the cube dn file that this label
   *   references.
   *
   * @return The cube that this external label file references
   */
  FileName Cube::externalCubeFileName() const {
    if (!isOpen()) {
      throw IException(IException::Unknown,
                       "An external cube label file must be opened in order to use "
                         "Cube::getExternalCubeFileName",
                       _FILEINFO_);
    }

    if (storesDnData()) {
      throw IException(IException::Unknown,
                       "Cube::getExternalCubeFileName can only be called on an external cube label "
                         "file",
                       _FILEINFO_);
    }


   PvlObject &core = m_label->findObject("IsisCube").findObject("Core");

    return core["^DnFile"][0];
  }


  /**
   * Returns the opened cube's filename. This is the name of the file which
   *   contains the labels of the cube and not necessarily the cube data.
   *
   * @returns The opened cube's filename
   */
  QString Cube::fileName() const {
    if (isOpen())
      return m_labelFileName->expanded();
    else
      return "";
  }


  /**
   * @returns the cube's storage format. If no cube is opened yet, then this is
   *   the storage format that will be used if create(...) is called.
   */
  Cube::Format Cube::format() const {
    return m_format;
  }


  /**
   * This method returns a pointer to a Histogram object
   * which allows the program to obtain and use various statistics and
   * histogram information from the cube. Cube does not retain ownership of
   * the returned pointer - please delete it when you are done with it.
   *
   * @param[in] band (Default value is 1) Returns the histogram for the specified
   *          band.If the user specifies 0 for this parameter, the method will loop
   *          through every band in the cube and accumulate a histogram from all of
   *          them
   *
   * @param msg The message to display with the percent process while gathering
   *            histogram data
   *
   * @return (Histogram) A pointer to a Histogram object.
   *
   * @throws IsisProgrammerError Band was less than zero or more than the number
   * of bands in the cube.
   */
  Histogram *Cube::histogram(const int &band, QString msg) {
    return histogram(band, ValidMinimum, ValidMaximum, msg);
  }


  /**
   * This method returns a pointer to a Histogram object
   * which allows the program to obtain and use various statistics and
   * histogram information from the cube. Cube does not retain ownership of
   * the returned pointer - please delete it when you are done with it.
   *
   * @param[in] band Returns the histogram for the specified
   *          band. If the user specifies 0 for this parameter, the method will
   *          loop through every band in the cube and accumulate a histogram from
   *          all of them
   *
   * @param validMin The start of the bin range and valid data range for the
   *                 histogram
   *
   * @param validMax The end of the bin range and valid data range for the
   *                 histogram
   *
   * @param msg The message to display with the percent process while gathering
   *            histogram data
   *
   * @return (Histogram) A pointer to a Histogram object.
   *
   * @throws ProgrammerError Band was less than zero or more than the number
   * of bands in the cube.
   */
  Histogram *Cube::histogram(const int &band, const double &validMin,
                                const double &validMax, QString msg) {
    // Make sure cube is open
    if ( !isOpen() ) {
      QString msg = "Cannot create histogram object for an unopened cube";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    // Make sure band is valid
    if ((band < 0) || (band > bandCount())) {
      QString msg = "Invalid band in [CubeInfo::Histogram]";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    int bandStart = band;
    int bandStop = band;
    int maxSteps = lineCount();
    if (band == 0) {
      bandStart = 1;
      bandStop = bandCount();
      maxSteps = lineCount() * bandCount();
    }

    Progress progress;
    Histogram *hist = new Histogram(*this, band, &progress);
    LineManager line(*this);

    // This range is for throwing out data; the default parameters are OK always
    //hist->SetValidRange(validMin, validMax);

    // We now need to know the binning range - ValidMinimum/Maximum are no longer
    //   acceptable, default to the bin range start/end.
    double binMin = validMin;
    double binMax = validMax;
    if (binMin == ValidMinimum) {
      binMin = hist->BinRangeStart();
    }

    if (binMax == ValidMaximum) {
      binMax = hist->BinRangeEnd();
    }

    //hist->SetBinRange(binMin, binMax);
    hist->SetValidRange(binMin,binMax);

    // Loop and get the histogram
    progress.SetText(msg);
    progress.SetMaximumSteps(maxSteps);
    progress.CheckStatus();

    for(int useBand = bandStart ; useBand <= bandStop ; useBand++) {
      for(int i = 1; i <= lineCount(); i++) {
        line.SetLine(i, useBand);
        read(line);
        hist->AddData(line.DoubleBuffer(), line.size());
        progress.CheckStatus();
      }
    }

    return hist;
  }


  /**
   * Returns a pointer to the IsisLabel object associated with the cube.
   * Modifications made to the label will be written when the file is closed if
   * it was opened read-write or created. Take care not to mangle the Core
   * Object as this can produce unexpected results when a new attempt is made
   * to open the file. This pointer is invalid as soon as the cube is closed.
   *
   * @return Pvl Pointer to the Label object associated with the cube.
   */
  Pvl *Cube::label() const {
    return m_label;
  }


  /**
   * Returns the number of bytes used by the label.
   *
   * @param actual True for consumed size, false for allocated size (i.e. the
   *   number of bytes in the cube set aside for the label).
   * @return int the number of bytes used by the label.
   */
  int Cube::labelSize(bool actual) const {
    int labelSize = m_labelBytes;

    if (actual && m_label) {
      ostringstream s;
      s << *m_label << endl;
      labelSize = s.tellp();
    }
    else if (actual) {
      labelSize = 0;
    }

    return labelSize;
  }


  /**
   * @returns the number of lines (y axis/height) in the cube. If no cube is
   *   open yet, this is the number of lines that will be written if create(...)
   *   is called.
   */
  int Cube::lineCount() const {
    return m_lines;
  }


  /**
    * Returns the multiplier value for converting 8-bit/16-bit pixels to 32-bit.
    * @f[
    * out = in * multiplier + base
    * @f]
    *
    * @return double The multiplier value for converting 8-bit/16-bit pixels
    *                to 32-bit.
    */
  double Cube::multiplier() const {
    return m_multiplier;
  }


  /**
   * @returns the accuracy of pixels in the file. If no cube is opened yet, then
   *   this is the accuracy/number of bytes per pixel that will be used if
   *   create(...) is called.
   */
  PixelType Cube::pixelType() const {
    return m_pixelType;
  }


  /**
   * This method will return the physical band number given a virtual band number.
   * Physical and virtual bands always match unless the programmer made a call
   * to SetVirtualBand prior to opening the cube.
   *
   *
   * @param virtualBand Virtual band to translate to physical band.
   *
   * @return int The physical band number.
   */
  int Cube::physicalBand(const int &virtualBand) const {
    int physicalBand = virtualBand;

    if (m_virtualBandList) {
      if ((virtualBand < 1) ||
          (virtualBand > m_virtualBandList->size())) {
        QString msg = "Out of array bounds [" + toString(virtualBand) + "]";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }
      physicalBand = m_virtualBandList->at(virtualBand - 1);
    }

    return physicalBand;
  }


  /**
   * @return a projection associated with the cube.  The generation of
   * the projection can throw an exception, so you might want to catch errors
   * if that interests you.
   */
  Projection *Cube::projection() {
    if (m_projection == NULL && isOpen()) {
      m_projection =  ProjectionFactory::CreateFromCube(*label());
    }
    return m_projection;
  }


  /**
   * @returns the number of samples (x axis/width) in the cube. If no cube is
   *   open yet, this is the number of samples that will be written if
   *   create(...) is called.
   */
  int Cube::sampleCount() const {
    return m_samples;
  }


  /**
   * This method returns a pointer to a Statistics object
   * which allows the program to obtain and use various statistics
   * from the cube. Cube does not retain ownership of
   * the returned pointer - please delete it when you are done with it.
   *
   * @param[in] band (Default value is 1) Returns the statistics for the specified
   *          band.If the user specifies 0 for this parameter, the method will loop
   *          through every band in the cube and accumulate statistics from each band
   *          seperately
   *
   * @param msg The message to display with the percent process while gathering
   *            statistics
   *
   * @return (Histogram) A pointer to a Statistics object containing details
   *          such as the minimum and maximum pixel values for the input cube on the
   *          band specified, or all bands as the case may be.
   */
  Statistics *Cube::statistics(const int &band, QString msg) {
    return statistics(band, ValidMinimum, ValidMaximum, msg);
  }


  /**
   * This method returns a pointer to a Statistics object
   * which allows the program to obtain and use various statistics
   * from the cube. Cube does not retain ownership of
   * the returned pointer - please delete it when you are done with it.
   *
   * @param band Returns the statistics for the specified
   *          band. If the user specifies 0 for this parameter, the method will
   *          loop through every band in the cube and accumulate statistics from
   *          each band seperately
   * @param validMin
   * @param validMax
   * @param msg
   *
   * @return Statistics*
   */
  Statistics *Cube::statistics(const int &band, const double &validMin,
                                     const double &validMax, QString msg) {
    // Make sure cube is open
    if ( !isOpen() ) {
      QString msg = "Cannot create statistics object for an unopened cube";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    // Make sure band is valid
    if ((band < 0) || (band > bandCount())) {
      string msg = "Invalid band in [CubeInfo::Statistics]";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    // Construct a line buffer manager and a statistics object
    LineManager line(*this);
    Statistics *stats = new Statistics();

    stats->SetValidRange(validMin, validMax);

    int bandStart = band;
    int bandStop = band;
    int maxSteps = lineCount();
    if (band == 0) {
      bandStart = 1;
      bandStop = bandCount();
      maxSteps = lineCount() * bandCount();
    }

    Progress progress;
    progress.SetText(msg);
    progress.SetMaximumSteps(maxSteps);
    progress.CheckStatus();

    // Loop and get the statistics for a good minimum/maximum
    for(int useBand = bandStart ; useBand <= bandStop ; useBand++) {
      for(int i = 1; i <= lineCount(); i++) {
        line.SetLine(i, useBand);
        read(line);
        stats->AddData(line.DoubleBuffer(), line.size());
        progress.CheckStatus();
      }
    }

    return stats;
  }


  /**
   * This method returns a boolean value
   *
   * @return bool
   */
  bool Cube::storesDnData() const {
    return m_storesDnData;
  }


  /**
   * This will add the given caching algorithm to the list of attempted caching
   *   algorithms. The algorithms are tried in the opposite order that they
   *   were added - the first algorithm added is the last algorithm tried.
   *
   * RegionalCachingAlgorithm is the only initial caching algorithm and works
   *   well for most cases. The caching algorithm only apply to the opened Cube
   *   and is reset by any changes to the open status of the Cube.
   *
   * This method takes ownership of algorithm.
   *
   * @param algorithm The caching algorithm to add to the Cube for I/O
   */
  void Cube::addCachingAlgorithm(CubeCachingAlgorithm *algorithm) {

    if (isOpen() && m_ioHandler) {
      m_ioHandler->addCachingAlgorithm(algorithm);
    }
    else if (!isOpen()) {
      QString msg = "Cannot add a caching algorithm until the cube is open";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
  }

  /**
   * This will clear excess RAM used for quicker IO in the cube. This should
   *   only be called if you need hundreds of cubes opened simultaneously. The
   *   IO cache will start growing again on future IO's.
   */
  void Cube::clearIoCache() {
    if (m_ioHandler) {
      QMutexLocker locker(m_mutex);
      m_ioHandler->clearCache();
    }
  }


  /**
   * This method will delete a blob label object from the cube as specified by the
   * Blob type and name. If blob does not exist it will do nothing and return
   * false.
   *
   * @param BlobType type of blob to search for (Polygon, Table, etc)
   * @param BlobName blob to be deleted
   * @return boolean if it found the blob and deleted it.
   */
  bool Cube::deleteBlob(QString BlobType, QString BlobName) {
    for(int i = 0; i < m_label->objects(); i++) {
      PvlObject obj = m_label->object(i);
      if (obj.name().compare(BlobType) == 0) {
        if (obj.findKeyword("Name")[0] == BlobName) {
          m_label->deleteObject(i);
          return true;
        }
      }
    }
    return false;
  }


  /**
   * Deletes a group from the cube labels. If the group does not
   * exist nothing happens; otherwise the group is removed.
   * This will only work on output cubes, therefore, input cubes
   * will not be updated.
   *
   * @param[out] group Name of the group to delete.
   */
  void Cube::deleteGroup(const QString &group) {
    PvlObject &isiscube = label()->findObject("IsisCube");
    if (!isiscube.hasGroup(group)) return;
    isiscube.deleteGroup(group);
  }


  /**
   * Read a group from the cube into a Label. If the group does not exist an
   * exception will be thrown.
   *
   * @param[out] group Name of the group to get
   * @return (PvlGroup) Label which will contain the requested group.
   */
  PvlGroup &Cube::group(const QString &group) const {
    PvlObject &isiscube = label()->findObject("IsisCube");
    return isiscube.findGroup(group);
  }


  /**
   * Return if the cube has a specified group in the labels.
   *
   * @param[out] group Name of the group to check.
   *
   * @return (bool) True if the cube has the specified group, false if not.
   */
  bool Cube::hasGroup(const QString &group) const {
    const PvlObject &isiscube = label()->findObject("IsisCube");
    if (isiscube.hasGroup(group)) return true;
    return false;
  }


  /**
   * Check to see if the cube contains a pvl table by the provided name
   *
   * @param name The name of the pvl table to search for
   *
   * @return bool True if the pvl table was found
   */
  bool Cube::hasTable(const QString &name) {
    for(int o = 0; o < label()->objects(); o++) {
      PvlObject &obj = label()->object(o);
      if (obj.isNamed("Table")) {
        if (obj.hasKeyword("Name")) {
          QString temp = (QString) obj["Name"];
          temp = temp.toUpper();
          QString temp2 = name;
          temp2 = temp2.toUpper();
          if (temp == temp2) return true;
        }
      }
    }
    return false;
  }


  /**
   * Adds a group in a Label to the cube. If the group already
   * exists in the cube it will be completely overwritten.
   * This will only work on output cubes, therefore, input cubes will not be
   * updated.
   *
   * @param[in] group Label containing the group to put.
   */
  void Cube::putGroup(const PvlGroup &group) {
    if (isReadOnly()) {
      QString msg = "Cannot add a group to the label of cube [" + (QString)QFileInfo(fileName()).fileName() +
          "] because it is opened read-only";
      throw IException(IException::Programmer, msg, _FILEINFO_);
      return;
    }

    PvlObject &isiscube = label()->findObject("IsisCube");
    if (isiscube.hasGroup(group.name())) {
      isiscube.findGroup(group.name()) = group;
    }
    else {
      isiscube.addGroup(group);
    }
  }


  /**
   * Applies virtual bands to label
   *
   */
  void Cube::applyVirtualBandsToLabel() {
    PvlObject &core = m_label->findObject("IsisCube").findObject("Core");

    // Prune the band bin group if it exists
    if (m_label->findObject("IsisCube").hasGroup("BandBin")) {
      PvlGroup &bandBin = m_label->findObject("IsisCube").findGroup("BandBin");
      for (int k = 0;k < bandBin.keywords();k++) {
        if (bandBin[k].size() == m_bands && m_virtualBandList) {
          PvlKeyword temp = bandBin[k];
          bandBin[k].clear();
          for (int i = 0;i < m_virtualBandList->size();i++) {
            int physicalBand = m_virtualBandList->at(i) - 1;
            bandBin[k].addValue(temp[physicalBand], temp.unit(physicalBand));
          }
        }
      }
    }

    // Change the number of bands in the labels of the cube
    if (m_virtualBandList && core.hasGroup("Dimensions")) core.findGroup("Dimensions")["Bands"] = toString(m_virtualBandList->size());
  }


  /**
   * This clears all of the allocated memory associated with an open cube.
   *
   * @param removeIt If true, the input cube will be removed from disk
   */
  void Cube::cleanUp(bool removeIt) {
    if (m_ioHandler) {
      delete m_ioHandler;
      m_ioHandler = NULL;
    }

    // Always remove a temporary file
    if (m_tempCube) {
      QFile::remove(m_tempCube->expanded());
      removeIt = false; // dont remove originals

      delete m_tempCube;
      m_tempCube = NULL;
    }

    if (removeIt) {
      QFile::remove(m_labelFileName->expanded());

      if (*m_labelFileName != *m_dataFileName)
        QFile::remove(m_dataFileName->expanded());
    }

    delete m_labelFile;
    m_labelFile = NULL;

    delete m_dataFile;
    m_dataFile = NULL;

    delete m_labelFileName;
    m_labelFileName = NULL;

    delete m_dataFileName;
    m_dataFileName = NULL;

    delete m_label;
    m_label = NULL;

    delete m_virtualBandList;
    m_virtualBandList = NULL;

    initialize();
  }


  /**
   * Initialize members from their initial undefined states
   *
   */
  void Cube::construct() {
    m_labelFile = NULL;
    m_dataFile = NULL;
    m_ioHandler = NULL;
    m_mutex = NULL;

    m_camera = NULL;
    m_projection = NULL;

    m_labelFileName = NULL;
    m_dataFileName = NULL;
    m_tempCube = NULL;
    m_formatTemplateFile = NULL;
    m_label = NULL;

    m_virtualBandList = NULL;

    m_mutex = new QMutex();
    m_formatTemplateFile =
         new FileName("$base/templates/labels/CubeFormatTemplate.pft");

    initialize();
  }


  /**
   * This returns the QFile with cube DN data in it. NULL will be returned
   *   if no files are opened.
   *
   * @returns A file for cube pixel data I/O
   */
  QFile *Cube::dataFile() const {
    if (m_dataFile)
      return m_dataFile;
    else
      return m_labelFile;
  }


  /**
   * This gets the file name of the file which actually contains the DN data. With ecub's, our
   *    data file name could be another ecub or a detached label, so using m_dataFileName is
   *    unreasonable.
   *
   * @return FileName object
   */
  FileName Cube::realDataFileName() const {
    FileName result;

    // Attached, stores DN data - normal cube
    if (m_attached && m_storesDnData) {
      ASSERT(m_labelFileName);
      result = *m_labelFileName;
    }
    // Detached, stores DN data - standard detached cube
    else if (!m_attached && m_storesDnData) {
      ASSERT(m_dataFileName);
      result = *m_dataFileName;
    }
    // External cube - go look at our external file
    else if (!m_storesDnData) {
      ASSERT(m_dataFileName);
      FileName guess = *m_dataFileName;
      QDir dir(guess.toString());

      // If path is relative and there is a labelFileName, start in directory of the ecub, then
      // cd to the directory containing the DnFile, since it is relative to the location of the ecub.
      // We need to turn the relative path into an absolute path.
      if (dir.isRelative() && m_labelFileName) {
        QDir dir2(m_labelFileName->originalPath());
        dir2.cd(guess.path());
        guess = dir2.absolutePath() + "/" + guess.name();
      }
      do {
        Pvl guessLabel(guess.expanded());

        PvlObject &core = guessLabel.findObject("IsisCube").findObject("Core");

        if (core.hasKeyword("^DnFile")) {
          FileName currentGuess = guess;
          guess = core["^DnFile"][0];

          if (!guess.path().startsWith("/")) {
            guess = currentGuess.path() + "/" + guess.original();
          }
        }
        else if (core.hasKeyword("^Core")) {
          result = core["^Core"][0];
        }
        else {
          result = guess;
        }
      }
      while (result.name() == "");
    }

    return result;
  }


  /**
   * This sets Cube to its default state:
   *   Native byte order
   *   Format = Tile
   *   PixelType = Real (4 bytes per pixel)
   *   Attached labels
   *   Label size = 65536 bytes
   *   # samples, lines, bands unset
   *   Base = 0
   *   Multiplier = 1.0
   */
  void Cube::initialize() {
    m_byteOrder = Lsb;
    if (IsBigEndian())
      m_byteOrder = Msb;
    m_format = Tile;
    m_pixelType = Real;

    m_attached = true;
    m_storesDnData = true;
    m_labelBytes = 65536;

    m_samples = 0;
    m_lines = 0;
    m_bands = 0;

    m_base = 0.0;
    m_multiplier = 1.0;
  }


  /**
   * This function initializes the Cube core from a Pvl Label passed as a parameter
   *
   * @param label Pvl label to initialize from
   */
  void Cube::initCoreFromLabel(const Pvl &label) {
    const PvlObject &core = label.findObject("IsisCube").findObject("Core");

    if (!core.hasKeyword("^DnFile")) {
      // Dimensions
      const PvlGroup &dims = core.findGroup("Dimensions");
      m_samples = dims["Samples"];
      m_lines = dims["Lines"];
      m_bands = dims["Bands"];

      // Stored pixel information
      const PvlGroup &pixelsGroup = core.findGroup("Pixels");
      m_byteOrder = ByteOrderEnumeration(pixelsGroup["ByteOrder"]);
      m_base = pixelsGroup["Base"];
      m_multiplier = pixelsGroup["Multiplier"];
      m_pixelType = PixelTypeEnumeration(pixelsGroup["Type"]);

      // Now examine the format to see which type of handler to create
      if ((QString) core["Format"] == "BandSequential") {
        m_format = Bsq;
      }
      else {
        m_format = Tile;
      }
    }
    else {
      FileName temp(core["^DnFile"][0]);
      if (!temp.expanded().startsWith("/")) {
        temp = FileName(m_labelFileName->path() + "/" + temp.original());
      }

      initCoreFromLabel(Pvl(temp.toString()));
    }
  }


  /**
   * This function initializes the Cube label from a file passed as a parameter
   *
   * @param labelFileName FileName to initialize from
   *
   * @param readWrite bool that determines whether the label is an old isis label and needs to be
   *          reformatted
   */
  void Cube::initLabelFromFile(FileName labelFileName, bool readWrite) {
    ASSERT(!m_labelFileName);

    try {
      if (labelFileName.fileExists()) {
        m_label = new Pvl(labelFileName.expanded());
        if (!m_label->objects()) {
          throw IException();
        }
      }
    }
    catch(IException &) {
      if (m_label) {
        delete m_label;
        m_label = NULL;
      }
    }

    try {
      if (!m_label) {
        FileName tmp(labelFileName);
        tmp = tmp.addExtension("cub");
        if (tmp.fileExists()) {
          m_label = new Pvl(tmp.expanded());
          if (!m_label->objects()) {
            throw IException();
          }
          labelFileName = tmp;
        }
      }
    }
    catch(IException &e) {
      if (m_label) {
        delete m_label;
        m_label = NULL;
      }
    }

    try {
      if (!m_label) {
        FileName tmp(labelFileName);
        tmp = tmp.setExtension("lbl");
        if (tmp.fileExists()) {
          m_label = new Pvl(tmp.expanded());
          if (!m_label->objects()) {
            throw IException();
          }
          labelFileName = tmp;
        }
      }
    }
    catch(IException &e) {
      if (m_label) {
        delete m_label;
        m_label = NULL;
      }
    }

    try {
      if (!m_label) {
        FileName tmp(labelFileName);
        tmp = tmp.addExtension("ecub");
        if (tmp.fileExists()) {
          m_label = new Pvl(tmp.expanded());
          if (!m_label->objects()) {
            throw IException();
          }
          labelFileName = tmp;
        }
      }
    }
    catch(IException &e) {
      if (m_label) {
        delete m_label;
        m_label = NULL;
      }
    }

    if (!m_label) {
      QString msg = Message::FileOpen(labelFileName.original());
      throw IException(IException::Io, msg, _FILEINFO_);
    }

    m_labelFileName = new FileName(labelFileName);

    // See if this is an old Isis cube format.  If so then we will
    // need to internalize a new label
    if (m_label->hasKeyword("CCSD3ZF0000100000001NJPL3IF0PDS200000001")) {
      if (!readWrite) {
        reformatOldIsisLabel(m_labelFileName->expanded());
      }
      else {
        QString msg = "Can not open [" + m_labelFileName->original() + "]"
                      " because it is an ISIS2 cube.";
        cleanUp(false);
        throw IException(IException::Io, msg, _FILEINFO_);
      }
    }
    else {
      m_labelFile = new QFile(m_labelFileName->expanded());
    }
  }


  /**
   * Throw an exception if the cube is not open.
   */
  void Cube::openCheck() {
    if (isOpen()) {
      string msg = "Sorry you can't do a SetMethod after the cube is opened";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
  }


  /**
   * Function to read data from a cube label and return it as a PVL object
   *
   * @return Pvl object
   */
  Pvl Cube::realDataFileLabel() const {
    Pvl label = *m_label;
    PvlObject *core = NULL;

    do {
      core = &label.findObject("IsisCube").findObject("Core");

      if (core->hasKeyword("^DnFile")) {

        FileName temp((*core)["^DnFile"][0]);
        if (!temp.expanded().startsWith("/")) {
          temp = realDataFileName();
        }

        label = Pvl(temp.toString());
        core = NULL;
      }
    }
    while (!core);

    return label;
  }


  /**
   * This is a helper, used by open(...), that handles opening Isis 2 cubes as
   *   if they were Isis 3 cubes.
   *
   * @param oldCube The filename of the Isis 2 cube
   */
  void Cube::reformatOldIsisLabel(const QString &oldCube) {
    QString parameters = "from=" + oldCube;
    FileName oldName(oldCube);
    FileName tempCube = FileName::createTempFile("Temporary_" + oldName.name() + ".cub");
    parameters += " to=" + tempCube.expanded();

    if (iApp == NULL) {
      QString command = "$ISISROOT/bin/pds2isis " + parameters;
      ProgramLauncher::RunSystemCommand(command);
    }
    else {
      QString prog = "pds2isis";
      ProgramLauncher::RunIsisProgram(prog, parameters);
    }

    m_tempCube = new FileName(tempCube);
    *m_label = Pvl(m_tempCube->toString());
    m_labelFile = new QFile(m_tempCube->expanded());
  }


/**
 * Returns the latitude and longitude range for the Cube. More accurate than the minimum and
 * maximum latitude and longitude from the mapping group.
 *
 * @param[out] minLatitude minimum latitude present in the cube
 * @param[out] maxLatitude maximum latitude present in the cube
 * @param[out] minLongitude minimum longitude present in the cube
 * @param[out] maxLongitude maximum longitude present in the cube
 */
  void Cube::latLonRange(double &minLatitude, double &maxLatitude, double &minLongitude, double &
                         maxLongitude) {
    Camera *cam;
    TProjection *proj;

    bool isGood = false;
    bool useProj = true;

    if (hasGroup("Instrument")) {
      useProj = false;
    }

    // setup camera or projection
    if (useProj) {
     try {
       proj = (TProjection *) projection();
     }
     catch(IException &e) {
       QString msg = "Cannot calculate lat/lon range without a camera or projection";
       throw IException(e, IException::User, msg, _FILEINFO_);
     }
    }
    else {
      try {
        cam = camera();
      }
      catch(IException &e) {
        QString msg = "Unable to create camera when calculating a lat/lon range.";
        throw IException(e, IException::User, msg, _FILEINFO_);
      }
    }

    // Iterate over all samp/line combos in cube
    minLatitude = 99999;
    minLongitude = 99999;
    maxLatitude = -99999;
    maxLongitude = -99999;

    for (double sample = 0.5; sample < sampleCount() + 0.5; sample++) {
    // Checks to see if the point is in outer space
      for (double line = 0.5; line < lineCount() + 0.5; line++) {
        if (useProj) {
          isGood = proj->SetWorld(sample, line);
        }
        else {
          isGood = cam->SetImage(sample, line);
        }

        double lat, lon;
        if (isGood) {
          if (useProj) {
            lat = proj->UniversalLatitude();
            lon = proj->UniversalLongitude();
          }
          else {
            lat = cam->UniversalLatitude();
            lon = cam->UniversalLongitude();
          }

          // update mix/max lat/lons
          if (lat < minLatitude) {
            minLatitude = lat;
          }
          else if (lat > maxLatitude) {
            maxLatitude = lat;
          }

          if (lon < minLongitude) {
            minLongitude = lon;
          }
          else if (lon > maxLongitude) {
            maxLongitude = lon;
          }
        }
      }
    }
    if ( (minLatitude == 99999) || (minLongitude == 99999) || (maxLatitude == -99999) ||
    (maxLongitude == -99999) ) {
      QString msg = "Unable to calculate a minimum or maximum latitutde or longitude.";
        throw IException(IException::Unknown, msg, _FILEINFO_);
    }
  }

  /**
   * Write the Pvl labels to the cube's label file. Excess data in the attached
   *   labels is set to 0.
   */
  void Cube::writeLabels() {
    if (!isOpen()) {
      string msg = "Cube must be opened first before writing labels";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    // Set the pvl's format template
    m_label->setFormatTemplate(m_formatTemplateFile->original());

    // Write them with attached data
    if (m_attached) {
      QMutexLocker locker(m_mutex);
      QMutexLocker locker2(m_ioHandler->dataFileMutex());

      ostringstream temp;
      temp << *m_label << endl;
      string tempstr = temp.str();
      if ((int) tempstr.length() < m_labelBytes) {
        QByteArray labelArea(m_labelBytes, '\0');
        QByteArray labelUnpaddedContents(tempstr.c_str(), tempstr.length());
        labelArea.replace(0, labelUnpaddedContents.size(), labelUnpaddedContents);
        // Rewrite the label area
        m_labelFile->seek(0);
        m_labelFile->write(labelArea);
      }
      else {
        locker2.unlock();
        QString msg = "Label space is full in [" +
            (QString)FileName(*m_labelFileName).name() +
                     "] unable to write labels";
        cleanUp(false);
        throw IException(IException::Io, msg, _FILEINFO_);
      }
    }

    // or detached label
    else {
      m_label->write(m_labelFileName->expanded());
    }
  }
}
