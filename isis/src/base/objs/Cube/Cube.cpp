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

#include <sstream>

#include <QMutex>

#include "Cube.h"
#include "Preference.h"
#include "Filename.h"
#include "iException.h"
#include "CubeBsqHandler.h"
#include "CubeTileHandler.h"
#include "Endian.h"
#include "SpecialPixel.h"
#include "Message.h"
#include "Application.h"
#include "System.h"
#include "Camera.h"
#include "CameraFactory.h"
#include "Projection.h"
#include "Statistics.h"
#include "Histogram.h"
#include "LineManager.h"

using namespace std;
namespace Isis {
  //! Constructs a Cube object.
  Cube::Cube() {
    // Initialize the user preferences
    p_attachedPreference = true;
    p_overwritePreference = true;
    p_historyPreference = true;

    // Override them
    Isis::PvlGroup &pref = Isis::Preference::Preferences().FindGroup("CubeCustomization");

    Isis::iString temp = (string) pref["Format"];
    p_attachedPreference = temp.UpCase() == "ATTACHED";

    temp = (string) pref["Overwrite"];
    p_overwritePreference = temp.UpCase() == "ALLOW";

    temp = (string) pref["History"];
    p_historyPreference = temp.UpCase() == "ON";

    // convert max size from gigabytes to bytes for later comparison
    p_maxSizePreference = pref["MaximumSize"];
    p_maxSizePreference *= 1073741824;

    // Init the i/o handler pointer
    p_ioHandler = NULL;

    // Init the cube def
    p_cube.labelFile = "";
    p_cube.label.Clear();
    p_cube.labelBytes = 65536;
    p_cube.attached = p_attachedPreference;
    p_cube.history = p_historyPreference;
    p_cube.dataFile = "";
    p_cube.startByte = 0;
    p_cube.access = IsisCubeDef::ReadWrite;
    p_cube.samples = 512;
    p_cube.lines = 512;
    p_cube.bands = 1;
    p_cube.pixelType = Isis::Real;
    p_cube.cubeFormat = Isis::Tile;
    if(Isis::IsBigEndian()) p_cube.byteOrder = Isis::Msb;
    if(Isis::IsLittleEndian()) p_cube.byteOrder = Isis::Lsb;
    p_cube.base = 0.0;
    p_cube.multiplier = 1.0;
    p_cube.virtualBandList.clear();

    p_tempCube = "";
    p_camera = NULL;
    p_projection = NULL;

    p_formatTemplateFile = "$base/templates/labels/CubeFormatTemplate.pft";

    p_mutex = new QMutex();
  }

  //! Destroys the Cube object.
  Cube::~Cube() {
    Close();
    if(p_camera != NULL) delete p_camera;
    if(p_projection != NULL) delete p_projection;
    if(p_mutex != NULL) delete p_mutex;
  }

  /**
   * This method will open an isis sube for reading or reading/writing.
   *
   * @param[in] cfile Name of the cube file to open. If the extenstion ".cub" is not
   * given it will be appended (i.e., the extension of .cub is forced).
   * Environment variables in the filename will be automatically expanded as well.
   * @param[in] access (Default value of "r") Defines how the cube will be accessed.
   * Either readonly "r" or read-write "rw".
   */
  void Cube::Open(const std::string &cfile, std::string access) {
    // Already opened?
    if(IsOpen()) {
      string msg = "Cube::Open - You already have [" + p_cube.labelFile +
                   "] opened";
      throw Isis::iException::Message(Isis::iException::Programmer, msg, _FILEINFO_);
    }

    // Figure out the access type
    if(access == "r") {
      p_cube.access = IsisCubeDef::ReadOnly;
    }
    else if(access == "rw") {
      p_cube.access = IsisCubeDef::ReadWrite;
    }
    else {
      string message = "Invalid value for argument [access] must be [r,rw]";
      throw Isis::iException::Message(Isis::iException::Programmer, message, _FILEINFO_);
    }

    // Expand name
    Isis::Filename cubfile(cfile);

    // Read the labels
    try {
      p_cube.label.Clear();
      p_cube.label.Read(cubfile.Expanded());
      if(p_cube.label.Objects() == 0) {
        throw Isis::iException::Message(Isis::iException::Io, "Dummy", _FILEINFO_);
      }
    }
    catch(Isis::iException &e) {
      e.Clear();
      try {
        cubfile.AddExtension("cub");
        p_cube.label.Clear();
        p_cube.label.Read(cubfile.Expanded());
        if(p_cube.label.Objects() == 0) {
          throw Isis::iException::Message(Isis::iException::Io, "Dummy", _FILEINFO_);
        }
      }
      catch(Isis::iException &e) {
        e.Clear();
        try {
          cubfile.RemoveExtension();
          cubfile.AddExtension("lbl");
          p_cube.label.Clear();
          p_cube.label.Read(cubfile.Expanded());
        }
        catch(Isis::iException &e) {
          e.Clear();
          string msg = Isis::Message::FileOpen(cfile);
          throw Isis::iException::Message(Isis::iException::Io, msg, _FILEINFO_);
        }
      }
    }
    p_cube.labelFile = cubfile.Expanded();

    // See if this is an old Isis cube format.  If so then we will
    // need to internalize a new label
    if(p_cube.label.HasKeyword("CCSD3ZF0000100000001NJPL3IF0PDS200000001")) {
      p_tempCube = "";
      if(access == "r") {
        ReformatOldIsisLabel(p_cube.labelFile);
        return;
      }
      else {
        string msg = "Can not open old cube file format with write access [" +
                     cfile + "]";
        throw Isis::iException::Message(Isis::iException::Io, msg, _FILEINFO_);
      }
    }

    // Figure out the name of the data file
    Isis::PvlObject &core = p_cube.label.FindObject("IsisCube").FindObject("Core");
    if(core.HasKeyword("^Core")) {
      Isis::Filename temp(core["^Core"]);
      if(temp.OriginalPath() == ".") {
        p_cube.dataFile = cubfile.Path() + "/" + temp.Name();
      }
      else {
        p_cube.dataFile = temp.Expanded();
      }
      p_cube.attached = false;
    }
    else {
      p_cube.dataFile = cubfile.Expanded();
      p_cube.attached = true;
    }

    // Get location of cube data in the data file
    p_cube.startByte = (Isis::BigInt) core["StartByte"];

    // Get dimensions
    Isis::PvlGroup &dims = core.FindGroup("Dimensions");
    p_cube.samples = dims["Samples"];
    p_cube.lines = dims["Lines"];
    p_cube.bands = dims["Bands"];

    // Get pixel type
    Isis::PvlGroup &ptype = core.FindGroup("Pixels");
    p_cube.pixelType = Isis::PixelTypeEnumeration(ptype["Type"]);

    // Get endianness
    p_cube.byteOrder = Isis::ByteOrderEnumeration(ptype["ByteOrder"]);

    // Get core base and multipler
    p_cube.base = ptype["Base"];
    p_cube.multiplier = ptype["Multiplier"];

    // Determine the number of bytes in the label
    if(p_cube.attached) {
      p_cube.labelBytes = p_cube.label.FindObject("Label")["Bytes"];
    }
    else {
      p_cube.labelBytes = LabelBytesUsed();
    }

    // Now examine the format to see which type of handler to create
    if((string) core["Format"] == "BandSequential") {
      p_cube.cubeFormat = Isis::Bsq;
      p_ioHandler = new Isis::CubeBsqHandler(p_cube);
    }
    else {
      p_cube.cubeFormat = Isis::Tile;
      p_ioHandler = new Isis::CubeTileHandler(p_cube);
    }

    // Open the file
    p_ioHandler->Open();

    // If the virtual band list is empty all bands are assumed
    if(p_virtualBandList.size() == 0) {
      for(int i = 0; i < p_cube.bands; i++) {
        p_cube.virtualBandList.push_back(i + 1);
      }
    }

    // Otherwise use the string version of the VBL and convert them
    // to appropriate bands (map filter names to band numbers or convert to int)
    else {
      SetVirtualBands();

      // Prune the band bin group if it exists
      if(p_cube.label.FindObject("IsisCube").HasGroup("BandBin")) {
        Isis::PvlGroup &bandBin = p_cube.label.FindObject("IsisCube")
                                  .FindGroup("BandBin");
        for(int k = 0; k < bandBin.Keywords(); k++) {
          if(bandBin[k].Size() == p_cube.bands) {
            Isis::PvlKeyword temp = bandBin[k];
            bandBin[k].Clear();
            for(int i = 0; i < (int)p_cube.virtualBandList.size(); i++) {
              int physicalBand = p_cube.virtualBandList[i] - 1;
              bandBin[k].AddValue(temp[physicalBand], temp.Unit(physicalBand));
            }
          }
        }
      }

      // Change the number of bands in the cube
      dims["Bands"] = (int) p_cube.virtualBandList.size();
    }
  }



  /**
   * This method will reopen an isis sube for reading or reading/writing.
   *   If access requested is read/write and the open fails, open as read only
   *   and throw error.
   *
   * @param[in]   access  (std::string)  Type of access needed (read or read/write
   *
   */
  void Cube::ReOpen(std::string access) {
    // Make sure it is Already opened
    if(!IsOpen()) {
      string msg = "Cube::ReOpen - [" + p_cube.labelFile +
                   "] has not been opened yet";
      throw Isis::iException::Message(Isis::iException::Programmer, msg, _FILEINFO_);
    }

    // If current access is correct, simply return
    if((p_cube.access == IsisCubeDef::ReadOnly && access == "r") ||
        (p_cube.access == IsisCubeDef::ReadWrite && access == "rw")) return;

    // First close cube
    Close();

    // Figure out the access type
    if(access == "r") {
      p_cube.access = IsisCubeDef::ReadOnly;
    }
    else if(access == "rw") {
      p_cube.access = IsisCubeDef::ReadWrite;
    }
    else {
      string msg = "Invalid value for argument [access] must be [r,rw]";
      throw Isis::iException::Message(Isis::iException::Programmer, msg, _FILEINFO_);
    }

    //  Reset the virtual band list
    SetVirtualBands();

    // Now examine the format to see which type of handler to create
    if(p_cube.cubeFormat == Isis::Bsq) {
      p_ioHandler = new Isis::CubeBsqHandler(p_cube);
    }
    else {
      p_ioHandler = new Isis::CubeTileHandler(p_cube);
    }

    // Open the file
    try {
      p_ioHandler->Open();
    }
    catch(Isis::iException &e) {
      //  If access requested is read/write, try opening read, but throw error.
      if(access == "rw") {
        e.Clear();
        p_cube.access = IsisCubeDef::ReadOnly;
        p_ioHandler->Open();
        string msg = "Cannot open read/write [" + p_cube.labelFile +
                     "] will be opened read only";
        throw Isis::iException::Message(Isis::iException::Programmer, msg, _FILEINFO_);
      }
    }

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
   * @param cfile Name of the cube file to open.  If the extenstion ".cub" is not
   * given it will be appended (i.e., the extension of .cub is forced).
   * Environment variables in the filename will be automatically expanded as well.
   */
  void Cube::Create(const std::string &cfile) {
    // Already opened?
    if(IsOpen()) {
      string msg = "Cube::Create - You already have [" + p_cube.labelFile +
                   "] opened";
      throw Isis::iException::Message(Isis::iException::Programmer, msg, _FILEINFO_);
    }

    // Make sure the cube is not going to exceed the maximum size preference
    BigInt size = (BigInt)p_cube.samples * (BigInt)p_cube.lines *
                  (BigInt)p_cube.bands * (BigInt)Isis::SizeOf(p_cube.pixelType);
    if(size > p_maxSizePreference) {
      string msg;
      msg += "The cube you are attempting to create [" + cfile + "] is [";
      msg += (Isis::iString)(size / 1073741824) + "GB] ";
      msg += "This is larger than the current allowed size of [";
      msg += (Isis::iString)(p_maxSizePreference / 1073741824) + "GB]. The cube ";
      msg += "dimensions were (S,L,B) [" + (Isis::iString)p_cube.samples + ", ";
      msg += (iString)p_cube.lines + ", " + (Isis::iString)p_cube.bands;
      msg += "] with [" + (Isis::iString)(Isis::SizeOf(p_cube.pixelType));
      msg += "] bytes per pixel.";
      msg += " If you still wish to create this cube, the maximum value can";
      msg += " be changed in the file [~/.Isis/IsisPreferences] within the group";
      msg += " CubeCustomization, keyword MaximumSize.";
      throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
    }

    // Set the access
    p_cube.access = IsisCubeDef::ReadWrite;

    // Expand output name
    Isis::Filename cubfile(cfile);
    cubfile.AddExtension("cub");

    // See if we have attached or detached labels
    Isis::PvlObject core("Core");
    if(p_cube.attached) {
      p_cube.startByte = p_cube.labelBytes + 1;
      core += Isis::PvlKeyword("StartByte", p_cube.startByte);
      p_cube.labelFile = cubfile.Expanded();
      p_cube.dataFile = cubfile.Expanded();
    }
    else {
      p_cube.startByte = 1;
      core += Isis::PvlKeyword("StartByte", p_cube.startByte);
      core += Isis::PvlKeyword("^Core", cubfile.Name());
      p_cube.dataFile = cubfile.Expanded();
      cubfile.RemoveExtension();
      cubfile.AddExtension("lbl");
      p_cube.labelFile = cubfile.Expanded();
    }

    // Create the size of the core
    Isis::PvlGroup dims("Dimensions");
    dims += Isis::PvlKeyword("Samples", p_cube.samples);
    dims += Isis::PvlKeyword("Lines", p_cube.lines);
    dims += Isis::PvlKeyword("Bands", p_cube.bands);
    core.AddGroup(dims);

    // Create the pixel type
    Isis::PvlGroup ptype("Pixels");
    ptype += Isis::PvlKeyword("Type", Isis::PixelTypeName(p_cube.pixelType));



    // And the byte ordering
    ptype += Isis::PvlKeyword("ByteOrder", Isis::ByteOrderName(p_cube.byteOrder));

    // Set the pixel base and multiplier (real pix = base + mult * disk pix)
    if(p_cube.pixelType == Isis::Real) {
      p_cube.base = 0.0;
      p_cube.multiplier = 1.0;
    }
    ptype += Isis::PvlKeyword("Base", p_cube.base);
    ptype += Isis::PvlKeyword("Multiplier", p_cube.multiplier);
    core.AddGroup(ptype);

    // Create the Cube
    Isis::PvlObject isiscube("IsisCube");
    isiscube.AddObject(core);

    p_cube.label.Clear();
    p_cube.label.AddObject(isiscube);

    // Setup storage reserved for the label
    Isis::PvlObject lbl("Label");
    lbl += Isis::PvlKeyword("Bytes", p_cube.labelBytes);
    p_cube.label.AddObject(lbl);

    // Create the appropriate handler
    if(p_cube.cubeFormat == Isis::Bsq) {
      p_ioHandler = new Isis::CubeBsqHandler(p_cube);
    }
    else {
      p_ioHandler = new Isis::CubeTileHandler(p_cube);
    }

    // Set up the virtual band list (all bands always for output)
    for(int i = 0; i < p_cube.bands; i++) {
      p_cube.virtualBandList.push_back(i + 1);
    }

    // Create the output file
    p_ioHandler->Create(p_overwritePreference);

    // Write the labels
    WriteLabels();

  }

  /**
   * This method will read a buffer of data from the cube as specified by the
   * contents of the Buffer object.
   *
   * @param rbuf Buffer to be loaded
   */
  void Cube::Read(Isis::Buffer &rbuf) {
    if(!IsOpen()) {
      string msg = "Cube::Read - Try opening a file before you read it";
      throw Isis::iException::Message(Isis::iException::Programmer, msg, _FILEINFO_);
    }

    p_mutex->lock();

    try {
      p_ioHandler->Read(rbuf);
    }
    catch(...) {
      p_mutex->unlock();
      throw;
    }

    p_mutex->unlock();

    p_ioHandler->ToDouble(rbuf);
  }

  /**
   * This method will write a buffer of data from the cube as specified by the
   * contents of the Buffer object.
   *
   *
   * @param wbuf Buffer to be written.
   */
  void Cube::Write(Isis::Buffer &wbuf) {
    if(!IsOpen()) {
      string msg = "Cube::Write - Try opening/creating a file before you write it";
      throw Isis::iException::Message(Isis::iException::Programmer, msg, _FILEINFO_);
    }

    if(IsReadOnly()) {
      string msg = "The cube [" + Filename() + "] is opened read-only ... ";
      msg += "you can't write to it";
      throw Isis::iException::Message(Isis::iException::Programmer, msg, _FILEINFO_);
    }

    p_ioHandler->ToRaw(wbuf);

    p_mutex->lock();

    try {
      p_ioHandler->Write(wbuf);
    }
    catch(...) {
      p_mutex->unlock();
      throw;
    }

    p_mutex->unlock();
  }

  /**
   * Closes the cube and updates the labels. Optionally, it deletes the cube if
   * requested.
   *
   * @param removeIt (Default value = false) Indicates if the file should be
   * removed/deleted.
   */
  void Cube::Close(const bool removeIt) {
    // Ignore if the stream is closed
    if(p_ioHandler == NULL) return;

    if(p_cube.stream.is_open()) {

      // Write the labels
      if(p_cube.access == IsisCubeDef::ReadWrite) {
        if(!removeIt) WriteLabels();
      }

      // Close the data file
      p_ioHandler->Close(removeIt);
    }

    // Always remove a temporary file
    if(p_tempCube != "") {
      remove(p_tempCube.c_str());
      p_tempCube = "";
    }

    delete p_ioHandler;
    p_cube.virtualBandList.clear();
    p_ioHandler = NULL;
  }

  void Cube::ReformatOldIsisLabel(const std::string &oldCube) {
    string parameters = "from=" + oldCube;
    Isis::Filename oldName(oldCube);
    Isis::Filename tempCube("Temporary_" + oldName.Name(), "cub");
    parameters += " to=" + tempCube.Expanded();

    if(Isis::iApp == NULL) {
      string command = "$ISISROOT/bin/pds2isis " + parameters;
      System(command);
    }
    else {
      std::string prog = "pds2isis";
      Isis::iApp->Exec(prog, parameters);
    }

    p_tempCube = tempCube.Expanded();
    Open(tempCube.Expanded(), "r");
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
  int Cube::PhysicalBand(const int virtualBand) const {
    if((virtualBand < 1) ||
        (virtualBand > (int)p_cube.virtualBandList.size())) {
      string msg = "Out of array bounds [" + Isis::iString(virtualBand) + "]";
      throw Isis::iException::Message(Isis::iException::Programmer, msg, _FILEINFO_);
    }
    return p_cube.virtualBandList[virtualBand-1];
  }

  void Cube::WriteLabels() {
    // Set the pvl's format template
    p_cube.label.SetFormatTemplate(p_formatTemplateFile);
    // Write them with attached data
    if(p_cube.attached) {
      ostringstream temp;
      temp << p_cube.label << endl;
      string tempstr = temp.str();
      if((int) tempstr.length() < p_cube.labelBytes) {
        // Clear out the label area
        char *tempbuf = new char[p_cube.labelBytes];
        memset(tempbuf, 0, p_cube.labelBytes);
        p_cube.stream.seekp(0, std::ios::beg);
        p_cube.stream.write(tempbuf, p_cube.labelBytes);
        delete [] tempbuf;

        p_cube.stream.seekp(0, std::ios::beg);
        p_cube.stream.write(tempstr.c_str(), tempstr.length());
      }
      else {
        p_cube.stream.close();
        string msg = "Label space is full in [" + p_cube.labelFile +
                     "] unable to write labels";
        throw Isis::iException::Message(Isis::iException::Io, msg, _FILEINFO_);
      }
    }

    // or detached label
    else {
      p_cube.label.Write(p_cube.labelFile);
    }
  }

  /**
   * Used prior to the Create method, this will allocate a specific number of
   * bytes in the label area for attached files. If not invoked, 65536 bytes will
   * be reserved by default.
   *
   * @param[in] labelBytes Number of bytes to reserve for label space.
   */
  void Cube::SetLabelBytes(int labelBytes) {
    OpenCheck();
    p_cube.labelBytes = labelBytes;
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
  void Cube::SetDimensions(int ns, int nl, int nb) {
    OpenCheck();
    if((ns < 1) || (nl < 1) || (nb < 1)) {
      string msg = "SetDimensions:  Invalid number of sample, lines or bands";
      throw Isis::iException::Message(Isis::iException::Programmer, msg, _FILEINFO_);
    }
    p_cube.samples = ns;
    p_cube.lines = nl;
    p_cube.bands = nb;
  }

  /**
   * Used prior to the Create method, this will specify the output pixel type.
   * If not invoked, the pixel type will be Real.
   *
   * @param pixelType An enumeration of the pixelType desired in the output cube.
   * See PixelType documentation for more information.
   */
  void Cube::SetPixelType(Isis::PixelType pixelType) {
    OpenCheck();
    p_cube.pixelType = pixelType;
  }

  /**
   * Used prior to the Create method, this will specify the format of the cube,
   * either band, sequential or tiled.
   * If not invoked, a tiled file will be created.
   *
   * @param cubeFormat An enumeration of either Isis::Bsq or Isis::Tile.
   */
  void Cube::SetCubeFormat(Isis::CubeFormat cubeFormat) {
    OpenCheck();
    p_cube.cubeFormat = cubeFormat;
  }

  /**
   * Used prior to the Create method, this will specify the byte order of pixels,
   * either least or most significant byte.
   *
   * @param byteOrder An enumeration of either Isis::Msb or Isis::Lsb.
   */
  void Cube::SetByteOrder(Isis::ByteOrder byteOrder) {
    OpenCheck();
    p_cube.byteOrder = byteOrder;
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
  void Cube::SetMinMax(double min, double max) {
    OpenCheck();

    double base = 0.0;
    double multiplier = 1.0;
    double x1, x2;

    if(p_cube.pixelType == Isis::UnsignedByte) {
      x1 = Isis::VALID_MIN1;
      x2 = Isis::VALID_MAX1;
      multiplier = (max - min) / (x2 - x1);
      base = min - multiplier * x1;
    }
    else if(p_cube.pixelType == Isis::SignedWord) {
      x1 = Isis::VALID_MIN2;
      x2 = Isis::VALID_MAX2;
      multiplier = (max - min) / (x2 - x1);
      base = min - multiplier * x1;
    }

    p_cube.base = base;
    p_cube.multiplier = multiplier;
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
  void Cube::SetBaseMultiplier(double base, double mult) {
    OpenCheck();
    p_cube.base = base;
    p_cube.multiplier = mult;
  }

  /**
   * Used prior to the Create method, this will specify that the labels and data
   * will be in one file.
   */
  void Cube::SetAttached() {
    OpenCheck();
    p_cube.attached = true;
  }

  /**
   * Used prior to the Create method, this will specify that the labels and data
   * will be in separate files. In particular, filename.lbl and filename.cub will
   * contain the labels and cube, respectively.
   */
  void Cube::SetDetached() {
    OpenCheck();
    p_cube.attached = false;
  }

  /**
   * Returns the number of bands in the cube. Note that this is the number of
   * virtual bands if the Open method was used.
   *
   * @return int The number of bands in the cube.
   */
  int Cube::Bands() const {
    return p_cube.virtualBandList.size();
  }

  /**
   * Used prior to the Open method, this allows the programmer to specify a subset
   * of bands to work with. For example, if the programmer only wants to work with
   * band 5 out of a 10 band cube, this can be accommodated. In the future, this
   * method will accept BandBin FilterName as well as band numbers.
   *
   * @param[in] vbands A vector of strings containing the virtual bands. The bands
   * will be verified when the cube is opened. For now, the vector must contain
   * integers (e.g., "5", "10", "1").
   */
  void Cube::SetVirtualBands(const std::vector<std::string> &vbands) {
    OpenCheck();
    p_cube.virtualBandList.clear();
    p_virtualBandList = vbands;
  }

  /**
   * Used prior to the Open method this allows the programmer to specify a subset
   * of bands to work with.
   */
  void Cube::SetVirtualBands() {
    // For now convert to integers ... later convert filter names to band numbers
    int i;
    try {
      for(i = 0; i < (int)p_virtualBandList.size(); i++) {
        Isis::iString sband = p_virtualBandList[i];
        int band = sband.ToInteger();
        if((band < 1) || (band > p_cube.bands)) {
          string msg = "Invalid virtual band list";
          throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
        }
        p_cube.virtualBandList.push_back(band);
      }
    }
    catch(Isis::iException &e) {
      string msg = "Invalid virtual band [" + p_virtualBandList[i] + "]";
      throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
    }
  }

  void Cube::OpenCheck() {
    if(IsOpen()) {
      string msg = "Sorry you can't do a SetMethod after the cube is opened";
      throw Isis::iException::Message(Isis::iException::Programmer, msg, _FILEINFO_);
    }
  }

  /**
   * Returns the number of bytes used by the label.
   *
   * @return int the number of bytes used by the label.
   */
  int Cube::LabelBytesUsed() {
    ostringstream s;
    s << p_cube.label << endl;
    string temp = s.str();
    return temp.size();
  }

  /**
   * This method will read data from the specified Blob object.
   *
   * @param[in] blob The Blob data to be loaded
   *
   * @return (type)return description
   */
  void Cube::Read(Isis::Blob &blob) {
    if(!p_cube.stream.is_open()) {
      string msg = "The cube is not opened so you can't read a blob from it";
      throw Isis::iException::Message(Isis::iException::Programmer, msg, _FILEINFO_);
    }
    blob.Read(p_cube.label, p_cube.stream);
  }

  /**
   * This method will write a blob of data (e.g. History, Table, etc)
   * to the cube as specified by the contents of the Blob object.
   *
   * @param blob data to be written
   */
  void Cube::Write(Isis::Blob &blob) {
    if(!p_cube.stream.is_open()) {
      string msg = "The cube is not opened so you can't write a blob to it";
      throw Isis::iException::Message(Isis::iException::Programmer, msg, _FILEINFO_);
    }

    if(p_cube.access == IsisCubeDef::ReadOnly) {
      string msg = "The cube must be opened in read/write mode, not readOnly";
      throw Isis::iException::Message(Isis::iException::Programmer, msg, _FILEINFO_);
    }

    // Write an attached blob
    if(p_cube.attached) {
      // Compute the number of bytes in the cube + label bytes and if the
      // endpos of the file // is not greater than this then seek to that position.
      p_cube.stream.seekp(0, std::ios::end);
      streampos sbyte = p_cube.stream.tellp();
      streampos maxbyte = (streampos) p_cube.labelBytes + p_cube.dataBytes;
      if(sbyte < maxbyte) p_cube.stream.seekp(maxbyte, std::ios::beg);
      blob.Write(p_cube.label, p_cube.stream);
    }

    // Write a detached blob
    else {
      Isis::Filename blobFilename = Filename();
      blobFilename.RemoveExtension();
      blobFilename.AddExtension(blob.Type());
      blobFilename.AddExtension(blob.Name());
      string blobFile(blobFilename.Expanded());
      ios::openmode flags = std::ios::in | std::ios::binary | std::ios::out |
                            std::ios::trunc;
      fstream detachedStream;
      detachedStream.open(blobFile.c_str(), flags);
      if(!detachedStream) {
        string message = "Unable to open data file [" +
                         blobFilename.Expanded() + "]";
        throw Isis::iException::Message(Isis::iException::Io, message, _FILEINFO_);
      }

// Changed to work with mods to Filename class
//      blob.Write(p_cube.label,detachedStream,blobFilename.Basename()+"."+
//                 blob.Type()+"."+
//                 blobFilename.Extension());
      blob.Write(p_cube.label, detachedStream, blobFilename.Name());
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
  bool Cube::BlobDelete(std::string BlobType, std::string BlobName) {
    for(int i = 0; i < p_cube.label.Objects(); i++) {
      Isis::PvlObject obj = p_cube.label.Object(i);
      if(obj.Name().compare(BlobType) == 0) {
        if(obj.FindKeyword("Name")[0] == BlobName) {
          p_cube.label.DeleteObject(i);
          return true;
        }
      }
    }
    return false;
  }

  /**
   * Return a camera associated with the cube.  The generation of
   * the camera can throw an exception, so you might want to catch errors
   * if that interests you.
   */
  Isis::Camera *Cube::Camera() {
    if(p_camera == NULL) {
      p_camera = CameraFactory::Create(*Label());
    }
    return p_camera;
  }

  /**
   * Returns true if the labels of the cube appear to have a valid mapping
   * group. This returning true does not guarantee that the cube can project or
   * that the Projection() method will succeed.
   *
   *
   * @return bool True if the file should have a valid projection
   */
  bool Cube::HasProjection() {
    return Label()->FindObject("IsisCube").HasGroup("Mapping");
  }

  /**
   * Return a projection associated with the cube.  The generation of
   * the projection can throw an exception, so you might want to catch errors
   * if that interests you.
   */
  Isis::Projection *Cube::Projection() {
    if(p_projection == NULL) {
      p_projection =  ProjectionFactory::CreateFromCube(*Label());
    }
    return p_projection;
  }

  /**
   * Check to see if the cube contains a pvl table by the provided name
   *
   * @param name The name of the pvl table to search for
   *
   * @return bool True if the pvl table was found
   */
  bool Cube::HasTable(const std::string &name) {
    for(int o = 0; o < Label()->Objects(); o++) {
      Isis::PvlObject &obj = Label()->Object(o);
      if(obj.IsNamed("Table")) {
        if(obj.HasKeyword("Name")) {
          Isis::iString temp = (string) obj["Name"];
          temp.UpCase();
          Isis::iString temp2 = name;
          temp2.UpCase();
          if(temp == temp2) return true;
        }
      }
    }
    return false;
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
   * @return (Isis::Histogram) A pointer to a Statistics object containing details
   *          such as the minimum and maximum pixel values for the input cube on the
   *          band specified, or all bands as the case may be.
   */
  Isis::Statistics *Cube::Statistics(const int band, std::string msg) {
    return Statistics(band, Isis::ValidMinimum, Isis::ValidMaximum, msg);
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
   * @return Isis::Statistics*
   */
  Isis::Statistics *Cube::Statistics(const int band, const double validMin,
                                     const double validMax, std::string msg) {
    // Make sure band is valid
    if((band < 0) || (band > Bands())) {
      string msg = "Invalid band in [CubeInfo::Statistics]";
      throw Isis::iException::Message(Isis::iException::Programmer, msg, _FILEINFO_);
    }

    // Construct a line buffer manager and a statistics object
    Isis::LineManager line(*this);
    Isis::Statistics *stats = new Isis::Statistics();

    stats->SetValidRange(validMin, validMax);

    int bandStart = band;
    int bandStop = band;
    int maxSteps = Lines();
    if(band == 0) {
      bandStart = 1;
      bandStop = Bands();
      maxSteps = Lines() * Bands();
    }

    Isis::Progress progress;
    progress.SetText(msg);
    progress.SetMaximumSteps(maxSteps);
    progress.CheckStatus();

    // Loop and get the statistics for a good minimum/maximum
    for(int useBand = bandStart ; useBand <= bandStop ; useBand++) {
      for(int i = 1; i <= Lines(); i++) {
        line.SetLine(i, useBand);
        Read(line);
        stats->AddData(line.DoubleBuffer(), line.size());
        progress.CheckStatus();
      }
    }

    return stats;
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
   * @return (Isis::Histogram) A pointer to a Histogram object.
   *
   * @throws IsisProgrammerError Band was less than zero or more than the number
   * of bands in the cube.
   */
  Isis::Histogram *Cube::Histogram(const int band, std::string msg) {
    return Histogram(band, Isis::ValidMinimum, Isis::ValidMaximum, msg);
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
   * @return (Isis::Histogram) A pointer to a Histogram object.
   *
   * @throws ProgrammerError Band was less than zero or more than the number
   * of bands in the cube.
   */
  Isis::Histogram *Cube::Histogram(const int band, const double validMin, const double validMax, std::string msg) {
    // Make sure band is valid
    if((band < 0) || (band > Bands())) {
      string msg = "Invalid band in [CubeInfo::Histogram]";
      throw Isis::iException::Message(Isis::iException::Programmer, msg, _FILEINFO_);
    }

    int bandStart = band;
    int bandStop = band;
    int maxSteps = Lines();
    if(band == 0) {
      bandStart = 1;
      bandStop = Bands();
      maxSteps = Lines() * Bands();
    }

    Isis::Progress progress;
    Isis::Histogram *hist = new Isis::Histogram(*this, band, &progress);
    Isis::LineManager line(*this);

    // This range is for throwing out data; the default parameters are OK always
    hist->SetValidRange(validMin, validMax);

    // We now need to know the binning range - Isis::ValidMinimum/Maximum are no longer
    //   acceptable, default to the bin range start/end.
    double binMin = validMin;
    double binMax = validMax;
    if(binMin == Isis::ValidMinimum) {
      binMin = hist->BinRangeStart();
    }

    if(binMax == Isis::ValidMaximum) {
      binMax = hist->BinRangeEnd();
    }

    hist->SetBinRange(binMin, binMax);

    // Loop and get the histogram
    progress.SetText(msg);
    progress.SetMaximumSteps(maxSteps);
    progress.CheckStatus();

    for(int useBand = bandStart ; useBand <= bandStop ; useBand++) {
      for(int i = 1; i <= Lines(); i++) {
        line.SetLine(i, useBand);
        Read(line);
        hist->AddData(line.DoubleBuffer(), line.size());
        progress.CheckStatus();
      }
    }

    return hist;
  }

  /**
   * Adds a group in a Label to the cube. If the group already
   * exists in the cube it will be completely overwritten.
   * This will only work on output cubes, therefore, input cubes will not be
   * updated.
   *
   * @throws IsisProgrammerError The programmer attempted to overwrite the Cube
   * group.
   *
   * @param[in] group Label containing the group to put.
   */
  void Cube::PutGroup(Isis::PvlGroup &group) {
    Isis::PvlObject &isiscube = Label()->FindObject("IsisCube");
    if(isiscube.HasGroup(group.Name())) {
      isiscube.FindGroup(group.Name()) = group;
    }
    else {
      isiscube.AddGroup(group);
    }
  }

  /**
   * Read a group from the cube into a Label. If the group does not exist an
   * exception will be thrown.
   *
   * @param[out] group Name of the group to get
   * @return (Isis::PvlGroup) Label which will contain the requested group.
   */
  Isis::PvlGroup &Cube::GetGroup(const std::string &group) {
    Isis::PvlObject &isiscube = Label()->FindObject("IsisCube");
    return isiscube.FindGroup(group);
  }

  /**
   * Deletes a group from the cube labels. If the group does not
   * exist nothing happens; otherwise the group is removed.
   * This will only work on output cubes, therefore, input cubes
   * will not be updated.
   *
   * @param[out] group Name of the group to delete.
   */
  void Cube::DeleteGroup(const std::string &group) {
    Isis::PvlObject &isiscube = Label()->FindObject("IsisCube");
    if(!isiscube.HasGroup(group)) return;
    isiscube.DeleteGroup(group);
  }

  /**
   * Return if the cube has a specified group in the labels.
   *
   * @param[out] group Name of the group to check.
   *
   * @return (bool) True if the cube has the specified group, false if not.
   */
  bool Cube::HasGroup(const std::string &group) {
    Isis::PvlObject &isiscube = Label()->FindObject("IsisCube");
    if(isiscube.HasGroup(group)) return true;
    return false;
  }
}
