#ifndef Blobber_h
#define Blobber_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.4 $                                                             
 * $Date: 2009/12/22 02:09:54 $
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

#include <string>
#include "tnt/tnt_array2d.h"

namespace Isis {

class Cube;
class CubeInfo;
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
   *  @history 2008-05-12 Steven Lambright - Removed references to CubeInfo
   *  @history 2008-06-18 Christopher Austin - Fixed documentation errors
   */
  class Blobber {
    public:
      Blobber();
      Blobber(const std::string &blobname, const std::string &fieldname, 
              const std::string &name = "Blob");
      Blobber(Cube &cube, const std::string &blobname, 
              const std::string &fieldname, const std::string &name = "Blob");
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
      void setName(const std::string &name) { _name = name; }

      /**
       * @brief Sets the name of the ISIS BLOB object that contains the data
       * 
       * This is name of a ISIS Table object that contains one or more fields
       * that make up the BLOB.  This is the value of the [i]Name[/i] keyword
       * in the Table object. 
       *  
       * @param bname Blobber name 
       */
      void setBlobName(const std::string &bname) { _blobname = bname; }

      /**
       * @brief Sets field name in the ISIS BLOB object that contains data
       * 
       * This method sets the name of the field contained within the Table
       * object BLOB from which the data is extracted. 
       *  
       * @param fname Field name 
       */
      void setFieldName(const std::string &fname) { _fieldname = fname; }

      /**
       * Returns the total number of elements (rows * columns) in blob
       * @return The product of rows and columns
       */
      inline int size()    const { return (Lines() * Samples()); }

      /**
       * Number of rows or lines in the BLOB
       * @return The lines or rows
       */
      inline int Lines()   const { return (_buf.dim1()); }
      /**                                       
       * Number of columns or samples in the BLOB
       * @return The number of samples or columns
       */
      inline int Samples() const { return (_buf.dim2()); }

      /**                                       
       * Returns the name of the BLOB given to refer to this instantiation
       * @return Name of BLOB instance
       */
      std::string getName() const { return (_name); }

      /**
       * @brief Retrieves the name of the Table object
       * @return (string) Name of the Table
       */
      std::string getBlobName() const { return (_blobname); }

      /**
       * @brief Retreive the name of the field in the Tabel object BLOB
       * @return (string) Name of the BLOB field
       */
      std::string getFieldName() const { return (_fieldname); }

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
      inline double *operator[](int i) {return (_buf[i]); }
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
      inline double const *operator[](int i) const { return (_buf[i]); }

//  Load options for reading the BLOB from ISIS files
      void load(const std::string &filename);
      void load(Cube &cube);


    protected:
      typedef TNT::Array2D<double> BlobBuf;    //!<  Internal buffer uses TNT

      /**
       * Returns a const reference to the internal buffer for ease of use
       * to derived objects.
       * @return Const reference to BLOB buffer
       */
      inline const BlobBuf &ref() const { return (_buf); }

    private:
      std::string _blobname;    //!<  Name of BLOB to read
      std::string _fieldname;   //!<  Name of field in BLOB to read
      std::string _name;        //!<  Name of this data set
      BlobBuf     _buf;         //!<  Buffer holding data

//  Low/level I/O and conversion methods
      void loadDouble(Table &tbl);
      void loadInteger(Table &tbl);
      double int2ToDouble (int value) const;
   };
};

#endif

