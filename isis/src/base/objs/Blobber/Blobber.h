#ifndef Blobber_h
#define Blobber_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QString>

#include <tnt/tnt_array2d.h>

namespace Isis {
  class Cube;
  class Table;

  /**
   * @brief Base class for accessing ISIS blobs
   *
   * This class will read any ISIS BLOB and provide generalized access
   * to its data.  Developers can derive new classes that define access
   * information in the form of names of the Isis Object and field name.
   *
   * The entire contents of the field are read in and internally stored
   * as double floating point values.  All recognized ISIS special pixels
   * are converted when read.
   *
   * Note that this provides read access only. Currently, no output is performed
   * in this implementation although it could be acheived in derived classes.
   *
   * This class does not maintain persistant access to the BLOB.  This means
   * that the entire contents of the BLOB are read and stored and the interface
   * to the ISIS BLOB is then terminated.
   *
   * Below is an example using this class to access a HiRISE BLOB.  It loads
   * the BLOB, reports the number of lines and samples and then computes
   * the mean and standard deviation using the Statistics class:
   *
   * @code
   *   Cube cube("hirise.cub");
   *   Blobber hiblob(cube, "HiRISE Calibration Image", "Calibration",
   *                     "CalibrationImage");
   *   std::cout << "Number of lines:   " << hiblob.Lines() << std::endl;
   *   std::cout << "Number of samples: " << hiblob.Samples() << std::endl;
   *
   *   Statistics stats;
   *   for (int line = 0 ; line < hiblob.Lines() ; line++) {
   *     stats.AddData(hiblob[line], hiblob.Samples());
   *   }
   *
   *   std::cout << "Average: " << stats.Average() << std::endl;
   *   std::cout << "StdDev:  " << stats.StandardDeviation() << std::endl;
   * @endcode
   *
   * In the above example, the name of the ISIS Table (BLOB) is "HiRISE
   * Calibration Image" and the field of interest in that table is "Calibration".
   * Upon instantiation, the BLOB contents are read and converted to double
   * precision floating point values.
   *
   * Note that this class is reuseable and reentrant.  This provides the user
   * to specify a different cube to load the data from.
   *
   * One special thing to note that assigning these objects to other object
   * variables results in a \b reference to the data...it is \b not copied.
   * The following illustrates this concept:
   * @code
   *   Cube cube('hirise.cub');
   *   Blobber hiblob(cube, "HiRISE Calibration Image", "Calibration",
   *                     "CalibrationImage");
   *
   *   Blobber myblob(hiblob);
   *   Blobber blob2 = myblob;
   * @endcode
   * In this example, \b hiblob, \b myblob and \b blob2 refer to the same
   * blob data.
   * Changing a pixel element in one blob results in the change in all
   * instances of the blob.  To get a completely independant copy of the
   * data, use the \b deepcopy() method.
   *
   * @ingroup LowLevelCubeIO
   *
   * @author 2005-12-16 Kris Becker
   *
   * @internal
   *   @history 2008-05-12 Steven Lambright - Removed references to CubeInfo
   *   @history 2008-06-18 Christopher Austin - Fixed documentation errors
   *   @history 2012-10-04 Jeannie Backer Changed references to TableField
   *                           methods to lower camel case. Ordered includes, 
   *                           put third party tnt includes inside angle braces,
   *                           and added padding to control statements to fit
   *                           coding standards. Added padding to control
   *                           statements. References #1169.
   *   @history 2017-08-30 Summer Stapleton - Updated documentation. References #4807.
   *   @history 2018-07-20 Tyler Wilson - Overloaded the int2Double function so it can handle
   *                       special pixel types for 4-byte unsigned integers.  References #971.
   */
  class Blobber {
    public:
      Blobber();
      Blobber(const QString &blobname, const QString &fieldname,
              const QString &name = "Blob");
      Blobber(Cube &cube, const QString &blobname,
              const QString &fieldname, const QString &name = "Blob");
      /** Destructor of this object */
      virtual ~Blobber() { }

      Blobber deepcopy() const;

      /**
       * @brief Specifies the name of this class instantiation
       *
       * This is just an additional name that can be used to identify
       * instantiations of this class.  It provides a way to uniquely
       * identify each object.
       *
       * @param name Unique name
       */
      void setName(const QString &name) {
        _name = name;
      }

      /**
       * @brief Sets the name of the ISIS BLOB object that contains the data
       *
       * This is name of a ISIS Table object that contains one or more fields
       * that make up the BLOB.  This is the value of the [i]Name[/i] keyword
       * in the Table object.
       *
       * @param bname Blobber name
       */
      void setBlobName(const QString &bname) {
        _blobname = bname;
      }

      /**
       * @brief Sets field name in the ISIS BLOB object that contains data
       *
       * This method sets the name of the field contained within the Table
       * object BLOB from which the data is extracted.
       *
       * @param fname Field name
       */
      void setFieldName(const QString &fname) {
        _fieldname = fname;
      }

      /**
       * Returns the total number of elements (rows * columns) in blob
       * @return The product of rows and columns
       */
      inline int size()    const {
        return (Lines() * Samples());
      }

      /**
       * Number of rows or lines in the BLOB
       * @return The lines or rows
       */
      inline int Lines()   const {
        return (_buf.dim1());
      }
      /**
       * Number of columns or samples in the BLOB
       * @return The number of samples or columns
       */
      inline int Samples() const {
        return (_buf.dim2());
      }

      /**
       * Returns the name of the BLOB given to refer to this instantiation
       * @return Name of BLOB instance
       */
      QString getName() const {
        return (_name);
      }

      /**
       * @brief Retrieves the name of the Table object
       * @return (string) Name of the Table
       */
      QString getBlobName() const {
        return (_blobname);
      }

      /**
       * @brief Retreive the name of the field in the Tabel object BLOB
       * @return (string) Name of the BLOB field
       */
      QString getFieldName() const {
        return (_fieldname);
      }

      /**
       * Returns the ith row/line in the BLOB that can be further referenced
       * into samples.  To access the 2nd sample in the 10th line use:
       * @code
       *   double sample = blobber[9][1];
       * @endcode
       *
       * @param i Index
       *
       * @return Pointer to the ith row in the BLOB
       */
      inline double *operator[](int i) {
        return (_buf[i]);
      }
      /**
       * Returns the ith row/line in the BLOB that can be further referenced
       * into samples.  This method provides const access to the data in
       * row matrix form.  To access the 2nd sample in the 10th line use:
       * @code
       *   double sample = blobber[9][1];
       * @endcode
       *
       * @param i Index
       *
       * @return Const pointer to the ith row in the BLOB
       */
      inline double const *operator[](int i) const {
        return (_buf[i]);
      }

//  Load options for reading the BLOB from ISIS files
      void load(const QString &filename);
      void load(Cube &cube);


    protected:
      typedef TNT::Array2D<double> BlobBuf;    //!<  Internal buffer uses TNT

      /**
       * Returns a const reference to the internal buffer for ease of use
       * to derived objects.
       * @return Const reference to BLOB buffer
       */
      inline const BlobBuf &ref() const {
        return (_buf);
      }

    private:
      QString _blobname;    //!<  Name of BLOB to read
      QString _fieldname;   //!<  Name of field in BLOB to read
      QString _name;        //!<  Name of this data set
      BlobBuf     _buf;         //!<  Buffer holding data

//  Low/level I/O and conversion methods
      void loadDouble(Table &tbl);
      void loadInteger(Table &tbl);
      double int2ToDouble(unsigned int value) const;
      double int2ToDouble(int value) const;
  };
};

#endif

