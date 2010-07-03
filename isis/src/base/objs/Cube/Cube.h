#ifndef Cube_h
#define Cube_h
/**
 * @file
 * $Revision: 1.17 $
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

#include "CubeIoHandler.h"
#include "Blob.h"

class QMutex;

namespace Isis {
  class Camera;
  class Projection;
  class Statistics;
  class Histogram;

  /**
   * @brief I/O Handler for Isis Cubes.
   *
   * This class is used to open, create, read, and write data from Isis cube
   * files.
   *
   * @ingroup LowLevelCubeIO
   *
   * @author 2003-02-14 Jeff Anderson
   *
   * @internal
   *   @todo There are undocumented methods and private variables in the Cube
   *         class. Also, it needs an example.
   *   @history 2003-03-31 Jeff Anderson - updated unitTest truth file.
   *   @history 2003-05-16 Stuart Sides  - modified schema from
   *                                       astrogeology...isis.astrogeology
   *   @history 2003-05-21 Jeff Anderson - added read/write methods to improve
   *                                       speed for IsisLine buffer managers.
   *                                       Also added error checks on reads,
   *                                       writes, and seeks.
   *   @history 2003-05-30 Jeff Anderson - added PhysicalBand method and updated
   *                                       unitTest.
   *   @history 2003-10-06 Jeff Anderson - added PVL to create method to allow for
   *                                       propagation of Pixel specifications.
   *   @history 2003-10-15 Jeff Anderson - fixed bad error check in PhysicalBand
   *                                       method.
   *   @history 2004-01-30 Jeff Anderson - added many Set methods. Major refractor
   *                                       of the class.
   *   @history 2004-02-12 Jeff Anderson - added pruning of the band bin group
   *                                       given the virtual band list.
   *   @history 2004-02-17 Jeff Anderson - modified the number of bands in the
   *                                       label if a virtual band list was given.
   *   @history 2004-03-01 Jeff Anderson - added ability to read Isis 2.1 cube
   *                                       format.
   *   @history 2004-04-01 Jeff Anderson - fixed bug swapping NULL cache tile for
   *                                       non-native cubes in move method.
   *   @history 2005-03-24 Jeff Anderson - Added methods to return a camera or
   *                                       projection associated with the cube.
   *   @history 2005-07-18 Elizabeth Ribelin - Fixed bug in method that returns a
   *                                           projection associated with the cube
   *   @history 2005-10-03 Elizabeth Miller - Added error check to Write(blob)
   *   @history 2006-02-24 Kris Becker - Made the destructor virtual to properly
   *                                     allow the class to be inherited.
   *   @history 2006-04-21 Jacob Danton - Modified the WriteLabel method to use
   *                                      the Pvl's format template abilities.
   *   @history 2006-05-17 Elizabeth Miller - Depricated CameraManger to
   *                                          CameraFactory
   *   @history 2006-07-10 Elizabeth Miller - Added max size preference
   *   @history 2006-11-28 Jeff Anderson - Fixed detached blob bug
   *   @history 2007-01-05 Jeff Anderson - Fixed bug when reading/writing outside
   *                 cube
   *   @history 2007-02-07 Tracie Sucharski - Added ReOpen method
   *   @history 2008-05-09 Steven Lambright - Added Statistics, Histogram, PutGroup,
   *                 GetGroup, DeleteGroup, HasGroup conveinience methods. Removed excess
   *                 references to CubeInfo.
   *   @history 2008-05-27 Jeff Anderson - Fixed bug in open method where
   *                       virtual bands were not handled correctly if a
   *                       BandBin group did not exist
   *   @history 2008-06-09 Christopher Austin - Improved maximum cube size error
   *                                            message.
   *   @history 2008-06-11 Steven Lambright - Changed prototype for SetVirtualBands
   *   @history 2008-06-18 Christopher Austin - Fixed documentation errors
   *   @history 2008-08-11 Steven Lambright - Added another Statistics method
   *                 which accepts a valid range -- also added another Histogram
   *                 method which accepts a bin/valid range (applies both).
   *   @history 2008-08-11 Steven Lambright - Fixed definition of IsisCubeDef,
   *            problem pointed out by "novas0x2a" (Support Board Member)
   *   @history 2008-12-15 Steven Koechle - Added a method to delete blobs from a
   *            cube
   *   @history 2008-12-17 Steven Koechle - BlobDelete method was broken, fixed
   *   @history 2009-06-30 Steven Lambright - Added "HasProjection" for uniform
   *            projection existance test
   *   @history 2010-03-22 Steven Lambright - Added a mutex for reading and writing,
   *            which makes these methods thread safe.
   *
   */
  class Cube {
    public:
      Cube();
      virtual ~Cube();

      void Open(const std::string &cfile, std::string access = "r");
      void ReOpen(std::string access = "r");
      void Create(const std::string &cfile);

      /**
       * Returns if the cube is opened.
       *
       * @return bool True if the cube is opened, false if it is not.
       */
      bool IsOpen() {
        return p_cube.stream.is_open();
      };

      /**
       * Returns if the cube is opened readonly. Default more for Open method.
       *
       * @return bool True if the cube is opened readonly, false if it is not.
       */
      bool IsReadOnly() const {
        return p_cube.access == IsisCubeDef::ReadOnly;
      };

      /**
       * Returns if the cube is opened read/write.
       *
       * @return bool True if the cube is opened read/write, false if it is not.
       */
      bool IsReadWrite() const {
        return p_cube.access == IsisCubeDef::ReadWrite;
      };

      void Close(const bool remove = false);
      void Read(Isis::Buffer &rbuf);
      void Write(Isis::Buffer &wbuf);
      void Read(Isis::Blob &blob);
      void Write(Isis::Blob &blob);
      bool BlobDelete(std::string BlobType, std::string BlobName);

      /**
       * Returns the expanded filename.
       *
       * @return std::string The expanded filename.
       */
      inline std::string Filename() const {
        return p_cube.labelFile;
      };

      /**
       * Returns a pointer to the IsisLabel object associated with the cube.
       * Modifications made to the label will be written when the file is closed if
       * it was opened read-write or created. Take care not to mangle the Core Object
       * as this can produce unexpected results when a new attempt is made to open
       * the file.
       *
       * @return Isis::Pvl Pointer to the Label object associated with the cube.
       */
      inline Isis::Pvl *Label() {
        return &p_cube.label;
      };

      void SetLabelBytes(int labelBytes);
      void SetDimensions(int ns, int nl, int nb);
      void SetPixelType(Isis::PixelType pixelType);
      void SetCubeFormat(Isis::CubeFormat cubeFormat);
      void SetByteOrder(Isis::ByteOrder byteOrder);
      void SetMinMax(double min, double max);
      void SetBaseMultiplier(double base, double mult);
      void SetAttached();
      void SetDetached();

      /**
       * Returns the number of bytes reserved for the label.
       *
       * @return int The number of bytes used for the label.
       */
      inline int LabelBytes() const {
        return p_cube.labelBytes;
      };
      int LabelBytesUsed();

      /**
       * Returns the number of samples in the cube.
       *
       * @return int The number of samples in the cube.
       */
      inline int Samples() const {
        return p_cube.samples;
      };

      /**
       * Returns the number of lines in the cube.
       *
       * @return int The number of lines in the cube.
       */
      inline int Lines() const {
        return p_cube.lines;
      };
      int Bands() const;

      /**
       * Returns an enumeration of the PixelType.
       * @see PixelType.h
       *
       * return Isis::PixelType An enumeration of the PixelType.
       */
      inline Isis::PixelType PixelType() const {
        return p_cube.pixelType;
      };

      /**
       * Returns an enumeration of the cube format (tiled or bsq).
       * @see CubeFormat.h
       *
       * @return Isis::CubeFormat An enumeration of the cube format.
       */
      inline Isis::CubeFormat CubeFormat() const {
        return p_cube.cubeFormat;
      };

      /**
       * Returns an enumeration of the byte order (Isb or Msb).
       * @see Endian.h
       *
       * @return Isis::ByteOrder An enumeration of the byte order.
       */
      inline Isis::ByteOrder ByteOrder() const {
        return p_cube.byteOrder;
      };

      /**
       * Returns if the cube and label data are in the same file.
       *
       * @return bool True if the cube and label data are in the same file,
       *              false if they are not.
       */
      inline bool IsAttached() const {
        return p_cube.attached;
      };

      /**
       * Returns if the cube and label data are in separate files.
       *
       * @return bool Returns true if the cube and label data are in separate
       *              files, false if they are in the same file.
       */
      inline bool IsDetached() const {
        return !p_cube.attached;
      };

      /**
       * Returns the base value for converting 8-bit/16-bit pixels to 32-bit.
       * @f[
       * out = in * multiplier + base
       * @f]
       *
       * @return double The base value for converting 8-bit/16-bit pixels to
       *                32-bit.
       */
      inline double Base() const {
        return p_cube.base;
      };

      /**
       * Returns the multiplier value for converting 8-bit/16-bit pixels to 32-bit.
       * @f[
       * out = in * multiplier + base
       * @f]
       *
       * @return double The multiplier value for converting 8-bit/16-bit pixels
       *                to 32-bit.
       */
      inline double Multiplier() const {
        return p_cube.multiplier;
      };

      void SetVirtualBands(const std::vector<std::string> &vbands);
      int PhysicalBand(const int virtualBand) const;

      bool HasProjection();

      Isis::Camera *Camera();
      Isis::Projection *Projection();
      Isis::Statistics *Statistics(const int band = 1, std::string msg = "Gathering statistics");
      Isis::Statistics *Statistics(const int band, const double validMin, const double validMax, std::string msg = "Gathering statistics");
      Isis::Histogram *Histogram(const int band = 1, std::string msg = "Gathering histogram");
      Isis::Histogram *Histogram(const int band, const double validMin, const double validMax, std::string msg = "Gathering histogram");

      // Change a group in the labels
      void PutGroup(Isis::PvlGroup &group);

      // Return a group in a label
      Isis::PvlGroup &GetGroup(const std::string &group);

      // Delete a group in the labels
      void DeleteGroup(const std::string &group);

      // Check to see if a group is in the labels
      bool HasGroup(const std::string &group);

      bool HasTable(const std::string &name);

    private:
      IsisCubeDef p_cube;
      Isis::CubeIoHandler *p_ioHandler;

      bool p_overwritePreference;
      bool p_historyPreference;
      bool p_attachedPreference;
      BigInt p_maxSizePreference;

      void WriteLabels();
      void ReformatOldIsisLabel(const std::string &oldCube);
      void OpenCheck();

      std::vector<std::string> p_virtualBandList;
      void SetVirtualBands();

      std::string p_tempCube;
      std::string p_formatTemplateFile;

      Isis::Camera *p_camera;
      Isis::Projection *p_projection;

      QMutex *p_mutex;
  };
};

#endif
