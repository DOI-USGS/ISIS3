#ifndef Buffer_h
#define Buffer_h
/**
 * @file
 * $Revision: 1.5 $
 * $Date: 2010/06/22 17:47:56 $
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

#include "iException.h"
#include "PixelType.h"

namespace Isis {
  /**
   * @brief Buffer for reading and writing cube data
   *
   * This is the base class for I/O on buffers of data from cubes.  The programmer
   * can define an arbitrary shape using this object and then read or write that
   * shape from a cube.  Some example shapes are lines, tiles, columns, cublets,
   * and spectral pencils.  Data which is read or written to a cube will always be
   * contained within an internal buffer of type double.  There are methods for
   * querying the sample/line/band position based upon the buffer index.  Note
   * that this class does not read/write data but is used in conjunction
   * with the class to read/write data. Even more powerful are the BufferManager
   * classes which inherit this object and can step through cubes by line, tile,
   * boxcar, column, etc.
   *
   * If you would like to see Buffer being used in implementation, see circle.cpp
   *
   * @ingroup LowLevelCubeIO
   *
   * @author 2002-04-09 Kris Becker & Jeff Anderson
   *
   * @internal
   *  @history 2003-05-16 Stuart Sides  - modified schema from
   *                                      astrogeology...isis.astrogeology
   *  @history 2005-01-16 Jeff Anderson - moved some private methods/variables
   *                                      into protected space
   *  @history 2007-09-05 Kris Becker - Added default constructor for added
   *           protection;  Added assignment operator for a single value for
   *           convenience.
   *  @history 2008-06-25 Noah Hilt - Added some safety measures to the allocate
   *           and deconstructor methods to prevent accessing buffers that have
   *           not been initialized. Added memory checking to make sure to release
   *           any memory if initialized.
   *  @history 2010-06-22 Steven Lambright - "Copy" now ensures pixel types are
   *           the same to prevent going out of memory bounds
   *  @todo Consider making some of the position methods virtual (Line, Sample,
   *        Band etc.) to increase speed. This means the derived class, like Line,
   *        can make faster computations.
   */
  class Buffer {
    public:
      Buffer();
      Buffer(const int nsamps, const int nlines, const int nbands,
             const Isis::PixelType type);

      ~Buffer();

      Buffer(const Buffer &);

      Buffer &operator=(const double &d);

      /**
       * Returns the number of samples in the shape buffer
       *
       * @return int
       */
      inline int SampleDimension() const {
        return (p_nsamps);
      }

      /**
       * Returns the number of lines in the shape buffer
       *
       * @return int
       */
      inline int LineDimension() const {
        return (p_nlines);
      }

      /**
       * Returns the number of bands in the shape buffer
       *
       * @return int
       */
      inline int BandDimension() const {
        return (p_nbands);
      }

      /**
       * Returns the total number of pixels in the shape buffer
       *
       * @return int
       */
      inline int size() const {
        return (p_npixels);
      }

      // Methods which return absolute coordinates relative to the buffer
      int Sample(const int index = 0) const;
      int Line(const int index = 0) const;
      int Band(const int index = 0) const;
      void Position(const int index, int &i_samp, int &i_line, int &i_band) const;
      int Index(const int i_samp, const int i_line, const int i_band) const
      throw(Isis::iException &);

      // Methods which give info about the buffer or its contents
      double at(const int index) const;

      /**
       * Returns the value in the shape buffer at given index.
       *
       * @param index Index position in buffer. No out of bounds index is checked
       *
       * @return double&
       */
      inline double &operator[](const int index) {
        return (p_buf[index]);
      }

      /**
       * Returns the value in the shape buffer at given index.
       *
       * @param index Index position in buffer. No out of bounds index is checked
       *
       * @return double
       */
      const double &operator[](const int index) const {
        return (p_buf[index]);
      }

      /**
       * Returns the value of the shape buffer
       *
       * @return double* The shape buffer
       */
      inline double *DoubleBuffer() const {
        return (p_buf);
      };
      void Copy(const Buffer &in);

      /**
       * Returns a void pointer to the raw buffer. Cast this void pointer using
       * information from the PixelType() method
       *
       * @return void* Pointer to the raw buffer
       */
      void *RawBuffer() const {
        return p_rawbuf;
      };

      /**
       * Returns the raw buffer pixel type
       *
       * @return Isis::PixelType
       */
      Isis::PixelType PixelType() const {
        return p_pixelType;
      };

    protected:
      void SetBasePosition(const int start_sample, const int start_line,
                           const int start_band);

      /**
       * This method is used to set the base sample position of the shape buffer.
       *
       * @param start_samp Starting sample to set
       */
      inline void SetBaseSample(const int start_samp) {
        p_sample = start_samp;
        return;
      }

      /**
       * This method is used to set the base line position of the shape buffer.
       *
       * @param start_line Starting line to set
       */
      inline void SetBaseLine(const int start_line) {
        p_line = start_line;
        return;
      }

      /**
       * This method is used to set the base band position of the shape buffer.
       *
       * @param start_band Starting band to set
       */
      inline void SetBaseBand(const int start_band) {
        p_band = start_band;
        return;
      }

      int p_sample;   //!< Starting sample to read/write
      int p_nsamps;   //!< Number of samples to read/write

      int p_line;     //!< Starting line to read/write
      int p_nlines;   //!< Number of lines to read/write

      int p_band;     //!< Starting band to read/write
      int p_nbands;   //!< Number of bands to read/write

      int p_npixels;  //!< Number of pixels (nsamps * nlines * nbands)
      double *p_buf;  /**< Shape buffer allocated to the size of npixels for
                           handling reads/writes*/

      const Isis::PixelType p_pixelType;  //!< The pixel type of the raw buffer
      void *p_rawbuf;                     //!< The raw dm read from the disk

      void Allocate();

      /**
       * Copy operator. We will make it private since copies of these buffers
       * do not need to occur.
       *
       * @param rvalue Right hand side
       *
       * @return Buffer&
       */
      Buffer &operator=(const Buffer &rvalue) {
        return const_cast<Buffer &>(rvalue);
      };

  };
};

#endif
