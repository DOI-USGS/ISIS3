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

#include <QMutex>

#include "Application.h"
#include "Camera.h"
#include "CameraFactory.h"
#include "CubeBsqHandler.h"
#include "CubeTileHandler.h"
#include "Endian.h"
#include "Filename.h"
#include "Histogram.h"
#include "iException.h"
#include "LineManager.h"
#include "Message.h"
#include "Preference.h"
#include "ProgramLauncher.h"
#include "Projection.h"
#include "SpecialPixel.h"
#include "Statistics.h"

using namespace std;

namespace Isis {
  //! Constructs a Cube object.
  Cube::Cube() {
    m_labelFile = NULL;
    m_dataFile = NULL;
    m_ioHandler = NULL;
    m_mutex = NULL;

    m_camera = NULL;
    m_projection = NULL;

    m_labelFilename = NULL;
    m_dataFilename = NULL;
    m_tempCube = NULL;
    m_formatTemplateFile = NULL;
    m_label = NULL;

    m_virtualBandList = NULL;

    m_mutex = new QMutex();
    m_formatTemplateFile =
        new iString("$base/templates/labels/CubeFormatTemplate.pft");

    initialize();
  }


  //! Destroys the Cube object.
  Cube::~Cube() {
    close();

    if(m_mutex) {
      delete m_mutex;
      m_mutex = NULL;
    }

    if(m_camera) {
      delete m_camera;
      m_camera = NULL;
    }

    if(m_projection) {
      delete m_projection;
      m_projection = NULL;
    }

    if(m_formatTemplateFile) {
      delete m_formatTemplateFile;
      m_formatTemplateFile = NULL;
    }
  }


  /**
   * Test if a cube file has been opened/created.
   */
  bool Cube::isOpen() const {
    bool open = (m_ioHandler != NULL);

    ASSERT(open == (bool)m_labelFile);
    ASSERT(open == (bool)m_labelFilename);
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
    return getLabel()->FindObject("IsisCube").HasGroup("Mapping");
  }


  /**
   * Test if the opened cube is read-only, that is write operations will fail
   *   if this is true. A cube must be opened in order to call this method.
   */
  bool Cube::isReadOnly() const {
    bool readOnly = false;

    if(!isOpen()) {
      iString msg = "No cube opened";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    if((m_labelFile->openMode() & QIODevice::ReadWrite) != QIODevice::ReadWrite)
      readOnly = true;

    return readOnly;
  }


  /**
   * Test if the opened cube is read-write, that is read and write operations
   *   should succeed if this is true. A cube must be opened in order to call
   *   this method.
   */
  bool Cube::isReadWrite() const {
    return !isReadOnly();
  }


  /**
   * Test if labels are attached. If a cube is open, then this indicates
   *   whether or not the opened cube's labels are attached. If a cube is not
   *   open, then this indicates whether or not a cube will be created with
   *   attached labels if create(...) is called.
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
    if(isOpen() && isReadWrite())
      writeLabels();

    cleanUp(removeIt);
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
   * @param cubeFilename Name of the cube file to open.  If the extenstion
   *     ".cub" is not given it will be appended (i.e., the extension of .cub
   *      is forced). Environment variables in the filename will be
   *      automatically expanded as well.
   */
  void Cube::create(const iString &cubeFilename) {
    // Already opened?
    if(isOpen()) {
      string msg = "You already have a cube opened";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    if(m_samples < 1 || m_lines < 1 || m_bands < 1) {
      iString msg = "Number of samples [" + iString(m_samples) +
          "], lines [" + iString(m_lines) + "], or bands [" + iString(m_bands) +
          "] cannot be less than 1";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    // Make sure the cube is not going to exceed the maximum size preference
    BigInt size = (BigInt)m_samples * m_lines *
                  (BigInt)m_bands * (BigInt)SizeOf(m_pixelType);

    size = size / 1024; // kb
    size = size / 1024; // mb
    size = size / 1024; // gb

    int maxSizePreference = 0;

    maxSizePreference =
        Preference::Preferences().FindGroup("CubeCustomization")["MaximumSize"];

    if(size > maxSizePreference) {
      string msg;
      msg += "The cube you are attempting to create [" + cubeFilename + "] is ["
             + iString(size) + "GB]. This is larger than the current allowed "
             "size of [" + iString(maxSizePreference) + "GB]. The cube "
             "dimensions were (S,L,B) [" + iString(m_samples) + ", " +
             iString(m_lines) + ", " + iString(m_bands) + "] with [" + 
             iString(SizeOf(m_pixelType)) + "] bytes per pixel. If you still "
             "wish to create this cube, the maximum value can be changed in the"
             " file [~/.Isis/IsisPreferences] within the group "
             "CubeCustomization, keyword MaximumSize.";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    // Expand output name
    Filename cubFile(cubeFilename);
    cubFile.AddExtension("cub");

    // See if we have attached or detached labels
    PvlObject core("Core");
    if(m_attached) {
      // StartByte is 1-based (why!!) so we need to do + 1
      core += PvlKeyword("StartByte", m_labelBytes + 1);
      m_labelFilename = new iString(cubFile.Expanded());
      m_dataFilename = new iString(cubFile.Expanded());
      m_labelFile = new QFile(*m_labelFilename);
    }
    else {
      core += PvlKeyword("StartByte", 1);
      core += PvlKeyword("^Core", cubFile.Name());
      m_dataFilename = new iString(cubFile.Expanded());
      m_dataFile = new QFile(*m_dataFilename);

      Filename labelFilename(cubFile);
      labelFilename.RemoveExtension();
      labelFilename.AddExtension("lbl");
      m_labelFilename = new iString(labelFilename.Expanded());
      m_labelFile = new QFile(*m_labelFilename);
    }

    // Create the size of the core
    PvlGroup dims("Dimensions");
    dims += PvlKeyword("Samples", m_samples);
    dims += PvlKeyword("Lines",   m_lines);
    dims += PvlKeyword("Bands",   m_bands);
    core.AddGroup(dims);

    // Create the pixel type
    PvlGroup ptype("Pixels");
    ptype += PvlKeyword("Type", PixelTypeName(m_pixelType));

    // And the byte ordering
    ptype += PvlKeyword("ByteOrder", ByteOrderName(m_byteOrder));
    ptype += PvlKeyword("Base", m_base);
    ptype += PvlKeyword("Multiplier", m_multiplier);
    core.AddGroup(ptype);

    // Create the Cube
    PvlObject isiscube("IsisCube");
    isiscube.AddObject(core);

    m_label = new Pvl;
    m_label->AddObject(isiscube);

    // Setup storage reserved for the label
    PvlObject lbl("Label");
    lbl += PvlKeyword("Bytes", m_labelBytes);
    m_label->AddObject(lbl);

    const PvlGroup &pref =
        Preference::Preferences().FindGroup("CubeCustomization");
    bool overwrite = pref["Overwrite"][0].UpCase() == "ALLOW";
    if (!overwrite && m_labelFile->exists()) {
      string msg = "Cube file [" + *m_labelFilename + "] exists, " +
                   "user preference does not allow overwrite";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    if(!m_labelFile->open(QIODevice::Truncate | QIODevice::ReadWrite)) {
      cleanUp(false);
      iString msg = "Failed to create [" + m_labelFile->fileName() + "]";
      throw iException::Message(iException::Io, msg, _FILEINFO_);
    }

    if (m_dataFile) {
      if(!m_dataFile->open(QIODevice::Truncate | QIODevice::ReadWrite)) {
        cleanUp(false);
        iString msg = "Failed to create [" + m_dataFile->fileName() + "]";
        throw iException::Message(iException::Io, msg, _FILEINFO_);
      }
    }

    if(m_format == Bsq) {
      m_ioHandler = new CubeBsqHandler(getDataFile(), m_virtualBandList,
          *m_label, false);
    }
    else {
      m_ioHandler = new CubeTileHandler(getDataFile(), m_virtualBandList,
          *m_label, false);
    }

    m_ioHandler->updateLabels(*m_label);

    // Write the labels
    writeLabels();
  }


  /**
   * This method will open an isis sube for reading or reading/writing.
   *
   * @param[in] cubeFilename Name of the cube file to open. Environment
   *     variables in the filename will be automatically expanded.
   * @param[in] access (Default value of "r") Defines how the cube will be
   *     accessed. Either read-only "r" or read-write "rw".
   */
  void Cube::open(const iString &cubeFilename, iString access) {
    // Already opened?
    if(isOpen()) {
      string msg = "You already have a cube opened";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    // Read the labels
    Filename realName(cubeFilename);

    try {
      if(realName.exists()) {
        m_label = new Pvl(realName.Expanded());
        if (!m_label->Objects()) {
          throw iException::Message(iException::Programmer, "", _FILEINFO_);
        }
      }
    }
    catch(iException &e) {
      e.Clear();

      if(m_label) {
        delete m_label;
        m_label = NULL;
      }
    }

    try {
      if(!m_label) {
        Filename tmp(realName);
        tmp.AddExtension("cub");
        if(tmp.exists()) {
          m_label = new Pvl(tmp.Expanded());
          if (!m_label->Objects()) {
            throw iException::Message(iException::Programmer, "", _FILEINFO_);
          }
          realName = tmp;
        }
      }
    }
    catch(iException &e) {
      e.Clear();

      if(m_label) {
        delete m_label;
        m_label = NULL;
      }
    }

    try {
      if(!m_label) {
        Filename tmp(realName);
        tmp.RemoveExtension();
        tmp.AddExtension("lbl");
        if(tmp.exists()) {
          m_label = new Pvl(tmp.Expanded());
          if (!m_label->Objects()) {
            throw iException::Message(iException::Programmer, "", _FILEINFO_);
          }
          realName = tmp;
        }
      }
    }
    catch(iException &e) {
      e.Clear();

      if(m_label) {
        delete m_label;
        m_label = NULL;
      }
    }

    if(!m_label) {
      string msg = Message::FileOpen(cubeFilename);
      throw iException::Message(iException::Io, msg, _FILEINFO_);
    }

    m_labelFilename = new iString(realName.Expanded());

    // See if this is an old Isis cube format.  If so then we will
    // need to internalize a new label
    if(m_label->HasKeyword("CCSD3ZF0000100000001NJPL3IF0PDS200000001")) {
      if(access == "r") {
        reformatOldIsisLabel(*m_labelFilename);
      }
      else {
        cleanUp(false);
        string msg = "Can not open old cube file format with write access [" +
                     cubeFilename + "]";
        throw iException::Message(iException::Io, msg, _FILEINFO_);
      }
    }
    else {
      m_labelFile = new QFile(*m_labelFilename);
    }

    // Figure out the name of the data file
    try {
      PvlObject &core = m_label->FindObject("IsisCube").FindObject("Core");
      if(core.HasKeyword("^Core")) {
        Filename temp(core["^Core"]);

        if(temp.OriginalPath() == ".") {
          m_dataFilename = new iString(realName.Path() + "/" + temp.Name());
        }
        else {
          m_dataFilename = new iString(temp.Expanded());
        }

        m_attached = false;

        m_dataFile = new QFile(*m_dataFilename);
      }
      else {
        m_dataFilename = new iString(realName.Expanded());
        m_attached = true;
      }
    }
    catch (iException &e) {
      cleanUp(false);
      throw;
    }

    if (access == "r") {
      if(!m_labelFile->open(QIODevice::ReadOnly)) {
        iString msg = "Failed to open [" + m_labelFile->fileName() + "] with "
            "read only access";
        cleanUp(false);
        throw iException::Message(iException::Io, msg, _FILEINFO_);
      }

      if (m_dataFile) {
        if(!m_dataFile->open(QIODevice::ReadOnly)) {
          iString msg = "Failed to open [" + m_dataFile->fileName() + "] with "
              "read only access";
          cleanUp(false);
          throw iException::Message(iException::Io, msg, _FILEINFO_);
        }
      }
    }
    else if (access == "rw") {
      if(!m_labelFile->open(QIODevice::ReadWrite)) {
        iString msg = "Failed to open [" + m_labelFile->fileName() + "] with "
            "read/write access";
        cleanUp(false);
        throw iException::Message(iException::Io, msg, _FILEINFO_);
      }

      if (m_dataFile) {
        if(!m_dataFile->open(QIODevice::ReadWrite)) {
          iString msg = "Failed to open [" + m_dataFile->fileName() + "] with "
              "read/write access";
          cleanUp(false);
          throw iException::Message(iException::Io, msg, _FILEINFO_);
        }
      }
    }
    else {
      iString msg = "Unknown value for access [" + access + "]. Expected 'r' "
                    " or 'rw'";
      cleanUp(false);
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    // Get dimensions
    PvlObject &core = m_label->FindObject("IsisCube").FindObject("Core");
    PvlGroup &dims = core.FindGroup("Dimensions");
    m_samples = dims["Samples"];
    m_lines = dims["Lines"];
    m_bands = dims["Bands"];

    // Get pixel type
    PvlGroup &pixelsGroup = core.FindGroup("Pixels");
    // Get endianness
    m_byteOrder = ByteOrderEnumeration(pixelsGroup["ByteOrder"]);
    m_base = pixelsGroup["Base"];
    m_multiplier = pixelsGroup["Multiplier"];
    m_pixelType = PixelTypeEnumeration(pixelsGroup["Type"]);

    // Determine the number of bytes in the label
    if(m_attached) {
      m_labelBytes = m_label->FindObject("Label")["Bytes"];
    }
    else {
      m_labelBytes = getLabelSize(true);
    }

    // Now examine the format to see which type of handler to create
    if((string) core["Format"] == "BandSequential") {
      m_ioHandler = new CubeBsqHandler(getDataFile(), m_virtualBandList,
          *m_label, true);
      m_format = Bsq;
    }
    else {
      m_ioHandler = new CubeTileHandler(getDataFile(), m_virtualBandList,
          *m_label, true);
      m_format = Tile;
    }

    // Prune the band bin group if it exists
    if(m_label->FindObject("IsisCube").HasGroup("BandBin")) {
      PvlGroup &bandBin = m_label->FindObject("IsisCube").FindGroup("BandBin");
      for(int k = 0; k < bandBin.Keywords(); k++) {
        if(bandBin[k].Size() == m_bands && m_virtualBandList) {
          PvlKeyword temp = bandBin[k];
          bandBin[k].Clear();
          for(int i = 0; i < m_virtualBandList->size(); i++) {
            int physicalBand = m_virtualBandList->at(i) - 1;
            bandBin[k].AddValue(temp[physicalBand], temp.Unit(physicalBand));
          }
        }
      }
    }

    // Change the number of bands in the labels of the cube
    if(m_virtualBandList)
      dims["Bands"] = m_virtualBandList->size();
  }


  /**
   * This method will reopen an isis sube for reading or reading/writing.
   *   If access requested is read/write and the open fails, open as read only
   *   and throw error.
   *
   * @param[in]   access  (std::string)  Type of access needed (read or read/write
   *
   */
  void Cube::reopen(iString access) {
    if(!m_labelFile) {
      iString msg = "Cube has not been opened yet. The filename to re-open is "
          "unknown";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    // Preserve filename and virtual bands when re-opening
    iString filename = *m_labelFilename;
    QList<int> virtualBandList;

    if(m_virtualBandList)
      virtualBandList = *m_virtualBandList;

    close();
    open(filename, access);

    if(virtualBandList.size()) {
      if(m_virtualBandList)
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
    if(!isOpen()) {
      string msg = "The cube is not opened so you can't read a blob from it";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    iString cubeFile = *m_labelFilename;
    if (m_tempCube)
      cubeFile = *m_tempCube;

    blob.Read(cubeFile, *getLabel());
  }


  /**
   * This method will read a buffer of data from the cube as specified by the
   * contents of the Buffer object.
   *
   * @param bufferToFill Buffer to be loaded
   */
  void Cube::read(Buffer &bufferToFill) {
    if(!isOpen()) {
      string msg = "Try opening a file before you read it";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
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
    if(!isOpen()) {
      string msg = "The cube is not opened so you can't write a blob to it";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    if (!m_labelFile->isWritable()) { 
      string msg = "The cube must be opened in read/write mode, not readOnly";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    // Write an attached blob
    if(m_attached) {
      // Compute the number of bytes in the cube + label bytes and if the
      // endpos of the file // is not greater than this then seek to that position.
      fstream stream(m_labelFilename->c_str(),
                     ios::in | ios::out | ios::binary);
      stream.seekp(0, ios::end);
      streampos endByte = stream.tellp();
      streampos maxbyte = (streampos) m_labelBytes +
          m_ioHandler->getDataSize();
      if(endByte < maxbyte) stream.seekp(maxbyte, ios::beg);
      blob.Write(*m_label, stream);
    }

    // Write a detached blob
    else {
      Filename blobFilename = getFilename();
      blobFilename.RemoveExtension();
      blobFilename.AddExtension(blob.Type());
      blobFilename.AddExtension(blob.Name());
      string blobFile(blobFilename.Expanded());
      ios::openmode flags = ios::in | ios::binary | ios::out | ios::trunc;
      fstream detachedStream;
      detachedStream.open(blobFile.c_str(), flags);
      if(!detachedStream) {
        string message = "Unable to open data file [" +
                         blobFilename.Expanded() + "]";
        throw iException::Message(iException::Io, message, _FILEINFO_);
      }

// Changed to work with mods to Filename class
//      blob.Write(p_cube.label,detachedStream,blobFilename.Basename()+"."+
//                 blob.Type()+"."+
//                 blobFilename.Extension());
      blob.Write(*m_label, detachedStream, blobFilename.Name());
    }
  }


  /**
   * This method will write a buffer of data from the cube as specified by the
   * contents of the Buffer object.
   *
   * @param bufferToWrite Buffer to be written.
   */
  void Cube::write(Buffer &bufferToWrite) {
    if(!isOpen()) {
      string msg = "Try opening/creating a file before you write it";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    if(isReadOnly()) {
      string msg = "The cube [" + (iString)QFileInfo(getFilename()).fileName() +
          "] is opened read-only ... you can't write to it";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
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
    if(m_pixelType == UnsignedByte) {
      x1 = VALID_MIN1;
      x2 = VALID_MAX1;
      m_multiplier = (max - min) / (x2 - x1);
      m_base = min - m_multiplier * x1;
    }
    else if(m_pixelType == SignedWord) {
      x1 = VALID_MIN2;
      x2 = VALID_MAX2;
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
    if((ns < 1) || (nl < 1) || (nb < 1)) {
      string msg = "SetDimensions:  Invalid number of sample, lines or bands";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }
    m_samples = ns;
    m_lines = nl;
    m_bands = nb;
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
  void Cube::setVirtualBands(const QList<iString> &vbands) {
    openCheck();
    if (m_virtualBandList)
      m_virtualBandList->clear();
    else
      m_virtualBandList = new QList<int>;

    if(vbands.size() > 0) {
      QListIterator<iString> it(vbands);
      while (it.hasNext()) {
        m_virtualBandList->append(it.next());
      }
    }
    else {
      delete m_virtualBandList;
      m_virtualBandList = NULL;
    }

    if(m_ioHandler) {
      m_ioHandler->setVirtualBands(m_virtualBandList);
    }
  }


  /**
   * This is a deprecated version of setVirtualBands(const QList<iString> &).
   *
   * @param[in] vbands A vector of strings containing the virtual bands. The
   *   vector must contain integers in string form (e.g., "5", "10", "1").
   */
  void Cube::setVirtualBands(const std::vector<std::string> &vbands) {
    QList<iString> realVBands;

    for(unsigned int i = 0; i < vbands.size(); i++)
      realVBands << vbands[i];

    setVirtualBands(realVBands);
  }


  /**
   * Returns the number of virtual bands for the cube.
   *
   * @return int The number of bands in the cube.
   */
  int Cube::getBandCount() const {
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
  double Cube::getBase() const {
    return m_base;
  }


  /**
   * Returns the byte order/endian-ness of the cube file. Cubes in a native
   *   byte order are quicker to read/write than those who must correct their
   *   byte order.
   */
  ByteOrder Cube::getByteOrder() const {
    return m_byteOrder;
  }


  /**
   * Return a camera associated with the cube.  The generation of
   * the camera can throw an exception, so you might want to catch errors
   * if that interests you.
   */
  Camera *Cube::getCamera() {
    if(m_camera == NULL) {
      m_camera = CameraFactory::Create(*getLabel());
    }
    return m_camera;
  }


  /**
   * Returns the opened cube's filename. This is the name of the file which
   *   contains the labels of the cube and not necessarily the cube data.
   */
  iString Cube::getFilename() const {
    if(isOpen())
      return *m_labelFilename;
    else
      return "";
  }


  /**
   * Returns the cube's storage format. If no cube is opened yet, then this is
   *   the storage format that will be used if create(...) is called.
   */
  Cube::Format Cube::getFormat() const {
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
  Histogram *Cube::getHistogram(const int &band, iString msg) {
    return getHistogram(band, ValidMinimum, ValidMaximum, msg);
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
  Histogram *Cube::getHistogram(const int &band, const double &validMin,
                             const double &validMax, iString msg) {
    // Make sure band is valid
    if((band < 0) || (band > getBandCount())) {
      string msg = "Invalid band in [CubeInfo::Histogram]";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    int bandStart = band;
    int bandStop = band;
    int maxSteps = getLineCount();
    if(band == 0) {
      bandStart = 1;
      bandStop = getBandCount();
      maxSteps = getLineCount() * getBandCount();
    }

    Progress progress;
    Histogram *hist = new Histogram(*this, band, &progress);
    LineManager line(*this);

    // This range is for throwing out data; the default parameters are OK always
    hist->SetValidRange(validMin, validMax);

    // We now need to know the binning range - ValidMinimum/Maximum are no longer
    //   acceptable, default to the bin range start/end.
    double binMin = validMin;
    double binMax = validMax;
    if(binMin == ValidMinimum) {
      binMin = hist->BinRangeStart();
    }

    if(binMax == ValidMaximum) {
      binMax = hist->BinRangeEnd();
    }

    hist->SetBinRange(binMin, binMax);

    // Loop and get the histogram
    progress.SetText(msg);
    progress.SetMaximumSteps(maxSteps);
    progress.CheckStatus();

    for(int useBand = bandStart ; useBand <= bandStop ; useBand++) {
      for(int i = 1; i <= getLineCount(); i++) {
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
  Pvl *Cube::getLabel() const {
    return m_label;
  }


  /**
   * Returns the number of bytes used by the label.
   *
   * @param actual True for consumed size, false for allocated size (i.e. the
   *   number of bytes in the cube set aside for the label).
   * @return int the number of bytes used by the label.
   */
  int Cube::getLabelSize(bool actual) const {
    int labelSize = m_labelBytes;

    if(actual && m_label) {
      ostringstream s;
      s << *m_label << endl;
      labelSize = s.tellp();
    }
    else if(actual) {
      labelSize = 0;
    }

    return labelSize;
  }


  /**
   * Returns the number of lines (y axis/height) in the cube. If no cube is
   *   open yet, this is the number of lines that will be written if create(...)
   *   is called.
   */
  int Cube::getLineCount() const {
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
  double Cube::getMultiplier() const {
    return m_multiplier;
  }


  /**
   * This is the accuracy of pixels in the file. If no cube is opened yet, then
   *   this is the accuracy/number of bytes per pixel that will be used if
   *   create(...) is called.
   */
  PixelType Cube::getPixelType() const {
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
  int Cube::getPhysicalBand(const int &virtualBand) const {
    int physicalBand = virtualBand;

    if (m_virtualBandList) {
      if((virtualBand < 1) ||
          (virtualBand > m_virtualBandList->size())) {
        string msg = "Out of array bounds [" + iString(virtualBand) + "]";
        throw iException::Message(iException::Programmer, msg, _FILEINFO_);
      }
      physicalBand = m_virtualBandList->at(virtualBand - 1);
    }

    return physicalBand;
  }


  /**
   * Return a projection associated with the cube.  The generation of
   * the projection can throw an exception, so you might want to catch errors
   * if that interests you.
   */
  Projection *Cube::getProjection() {
    if(m_projection == NULL) {
      m_projection =  ProjectionFactory::CreateFromCube(*getLabel());
    }
    return m_projection;
  }


  /**
   * Returns the number of samples (x axis/width) in the cube. If no cube is
   *   open yet, this is the number of samples that will be written if
   *   create(...) is called.
   */
  int Cube::getSampleCount() const {
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
  Statistics *Cube::getStatistics(const int &band, iString msg) {
    return getStatistics(band, ValidMinimum, ValidMaximum, msg);
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
  Statistics *Cube::getStatistics(const int &band, const double &validMin,
                                     const double &validMax, iString msg) {
    // Make sure band is valid
    if((band < 0) || (band > getBandCount())) {
      string msg = "Invalid band in [CubeInfo::Statistics]";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    // Construct a line buffer manager and a statistics object
    LineManager line(*this);
    Statistics *stats = new Statistics();

    stats->SetValidRange(validMin, validMax);

    int bandStart = band;
    int bandStop = band;
    int maxSteps = getLineCount();
    if(band == 0) {
      bandStart = 1;
      bandStop = getBandCount();
      maxSteps = getLineCount() * getBandCount();
    }

    Progress progress;
    progress.SetText(msg);
    progress.SetMaximumSteps(maxSteps);
    progress.CheckStatus();

    // Loop and get the statistics for a good minimum/maximum
    for(int useBand = bandStart ; useBand <= bandStop ; useBand++) {
      for(int i = 1; i <= getLineCount(); i++) {
        line.SetLine(i, useBand);
        read(line);
        stats->AddData(line.DoubleBuffer(), line.size());
        progress.CheckStatus();
      }
    }

    return stats;
  }


  /**
   * This will clear excess RAM used for quicker I/O in the cube. This should
   *   only be called if you need hundreds of cubes opened simultaneously. The
   *   I/O cache will start growing again on future I/O's.
   */
  void Cube::clearIoCache() {
    if(m_ioHandler) {
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
  bool Cube::deleteBlob(iString BlobType, iString BlobName) {
    for(int i = 0; i < m_label->Objects(); i++) {
      PvlObject obj = m_label->Object(i);
      if(obj.Name().compare(BlobType) == 0) {
        if(obj.FindKeyword("Name")[0] == BlobName) {
          m_label->DeleteObject(i);
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
  void Cube::deleteGroup(const iString &group) {
    PvlObject &isiscube = getLabel()->FindObject("IsisCube");
    if(!isiscube.HasGroup(group)) return;
    isiscube.DeleteGroup(group);
  }


  /**
   * Read a group from the cube into a Label. If the group does not exist an
   * exception will be thrown.
   *
   * @param[out] group Name of the group to get
   * @return (PvlGroup) Label which will contain the requested group.
   */
  PvlGroup &Cube::getGroup(const iString &group) const {
    PvlObject &isiscube = getLabel()->FindObject("IsisCube");
    return isiscube.FindGroup(group);
  }


  /**
   * Return if the cube has a specified group in the labels.
   *
   * @param[out] group Name of the group to check.
   *
   * @return (bool) True if the cube has the specified group, false if not.
   */
  bool Cube::hasGroup(const iString &group) const {
    const PvlObject &isiscube = getLabel()->FindObject("IsisCube");
    if(isiscube.HasGroup(group)) return true;
    return false;
  }


  /**
   * Check to see if the cube contains a pvl table by the provided name
   *
   * @param name The name of the pvl table to search for
   *
   * @return bool True if the pvl table was found
   */
  bool Cube::hasTable(const iString &name) {
    for(int o = 0; o < getLabel()->Objects(); o++) {
      PvlObject &obj = getLabel()->Object(o);
      if(obj.IsNamed("Table")) {
        if(obj.HasKeyword("Name")) {
          iString temp = (string) obj["Name"];
          temp.UpCase();
          iString temp2 = name;
          temp2.UpCase();
          if(temp == temp2) return true;
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
  void Cube::putGroup(PvlGroup &group) {
    PvlObject &isiscube = getLabel()->FindObject("IsisCube");
    if(isiscube.HasGroup(group.Name())) {
      isiscube.FindGroup(group.Name()) = group;
    }
    else {
      isiscube.AddGroup(group);
    }
  }

  /**
   * This clears all of the allocated memory associated with an open cube.
   *
   * @param removeIt If true, the input cube will be removed from disk
   */
  void Cube::cleanUp(bool removeIt) {
    if(m_ioHandler) {
      delete m_ioHandler;
      m_ioHandler = NULL;
    }

    // Always remove a temporary file
    if(m_tempCube) {
      remove(m_tempCube->c_str());
      removeIt = false; // dont remove originals

      delete m_tempCube;
      m_tempCube = NULL;
    }

    if(removeIt) {
      remove(m_labelFilename->c_str());

      if(*m_labelFilename != *m_dataFilename)
        remove(m_dataFilename->c_str());
    }

    if (m_labelFile) {
      delete m_labelFile;
      m_labelFile = NULL;
    }

    if (m_dataFile) {
      delete m_dataFile;
      m_dataFile = NULL;
    }

    if (m_labelFilename) {
      delete m_labelFilename;
      m_labelFilename = NULL;
    }

    if (m_dataFilename) {
      delete m_dataFilename;
      m_dataFilename = NULL;
    }

    if (m_label) {
      delete m_label;
      m_label = NULL;
    }

    if (m_virtualBandList) {
      delete m_virtualBandList;
      m_virtualBandList = NULL;
    }

    initialize();
  }


  /**
   * This returns the QFile with cube DN data in it. NULL will be returned
   *   if no files are opened.
   */
  QFile * Cube::getDataFile() const {
    if (m_dataFile)
      return m_dataFile;
    else
      return m_labelFile;
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
    m_labelBytes = 65536;

    m_samples = 0;
    m_lines = 0;
    m_bands = 0;

    m_base = 0.0;
    m_multiplier = 1.0;
  }


  /**
   * Throw an exception if the cube is not open.
   */
  void Cube::openCheck() {
    if(isOpen()) {
      string msg = "Sorry you can't do a SetMethod after the cube is opened";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }
  }


  /**
   * This is a helper, used by open(...), that handles opening Isis 2 cubes as
   *   if they were Isis 3 cubes.
   *
   * @param oldCube The filename of the Isis 2 cube
   */
  void Cube::reformatOldIsisLabel(const iString &oldCube) {
    string parameters = "from=" + oldCube;
    Filename oldName(oldCube);
    Filename tempCube("Temporary_" + oldName.Name(), "cub");
    parameters += " to=" + tempCube.Expanded();

    if(iApp == NULL) {
      iString command = "$ISISROOT/bin/pds2isis " + parameters;
      ProgramLauncher::RunSystemCommand(command);
    }
    else {
      iString prog = "pds2isis";
      ProgramLauncher::RunIsisProgram(prog, parameters);
    }

    m_tempCube = new iString(tempCube.Expanded());
    *m_label = Pvl(*m_tempCube);
    m_labelFile = new QFile(*m_tempCube);
  }


  /**
   * Write the Pvl labels to the cube's label file. Excess data in the attached
   *   labels is set to 0.
   */
  void Cube::writeLabels() {
    if (!isOpen()) {
      string msg = "Cube must be opened first before writing labels";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    // Set the pvl's format template
    m_label->SetFormatTemplate(*m_formatTemplateFile);

    // Write them with attached data
    if(m_attached) {
      ostringstream temp;
      temp << *m_label << endl;
      string tempstr = temp.str();
      if((int) tempstr.length() < m_labelBytes) {
        // Clear out the label area
        m_labelFile->seek(0);
        m_labelFile->write(QByteArray(m_labelBytes, '\0'));

        m_labelFile->seek(0);
        m_labelFile->write(tempstr.c_str(), tempstr.length());
      }
      else {
        string msg = "Label space is full in [" +
            (iString)Filename(*m_labelFilename).fileName() +
                     "] unable to write labels";
        cleanUp(false);
        throw iException::Message(iException::Io, msg, _FILEINFO_);
      }
    }

    // or detached label
    else {
      m_label->Write(*m_labelFilename);
    }
  }
}
