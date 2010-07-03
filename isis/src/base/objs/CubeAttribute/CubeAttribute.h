/**                                                                       
 * @file                                                                 
 * $Revision: 1.2 $                                                            
 * $Date: 2007/01/30 22:12:22 $                                                                
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

#ifndef CubeAttribute_h
#define CubeAttribute_h
#include <string>
#include "Pvl.h"
#include "iString.h"
#include "PixelType.h"
#include "Endian.h"
#include "iException.h"
#include "CubeFormat.h"

namespace Isis {

  /**
   * @brief Output cube range tracker
   *
   * This enumeration and its functions are for the output range
   * of a cube.
  **/
  enum RangeType {
    PropagateRange, //!< Propagate the range from an input cube
    RangeSet,       //!< The range has been set
  };


  /**
   * Return the string representation of the contents of a
   * variable of type RangeType
   *
   * @param rangeType enum to be converted to a string
   *
   * @return A string representation of the rangeType parameter
   */
  inline std::string RangeTypeName (RangeType rangeType) {
    if (rangeType == PropagateRange) return "Propagate";
    if (rangeType == RangeSet) return "Set";

    std::string msg = "Invalid output range type [" + Isis::iString(rangeType) + "]";
    throw Isis::iException::Message(Isis::iException::Parse,msg, _FILEINFO_);
  }


  /**
   * Return the appropriate RangeType depending on which of
   * the valid values the argument spells
   *
   * @param rangeType A string representation of the rangeType
   *
   * @return The RangeType enum corresponding to the string parameter
   */
  inline RangeType RangeTypeEnumeration (const std::string &rangeType) {
    Isis::iString temp(rangeType);
    temp = temp.UpCase();
    if (temp == "PROPAGATE") return PropagateRange;
    if (temp == "SET") return RangeSet;

    std::string msg = "Invalid output range type string [" + rangeType + "]";
    throw Isis::iException::Message(Isis::iException::Parse,msg, _FILEINFO_);
  }


  /**
   * @brief Input cube label type tracker
   *
   * This enumeration and its functions are for the label
   * type of an input cube. The enum defines the type of labels (i.e.,
   * Both the label and cube are in the same file and the label is in a
   * separate file from the cube.
  **/
  enum LabelAttachment {
    AttachedLabel,  //!< The input label is embedded in the image file
    DetachedLabel   //!< The input label is in a separate data file from the image
  };


  /**
   * Return the string representation of the contents of a
   * variable of type LabelAttachment
   *
   * @param labelType The LabelAttachment enum to be converted to a string
   *
   * @return A string representation of the parameter
   */
  inline std::string LabelAttachmentName (LabelAttachment labelType) {
    if (labelType == AttachedLabel) return "Attached";
    if (labelType == DetachedLabel) return "Detached";

    std::string msg = "Invalid label attachment type [" + Isis::iString(labelType) + "]";
    throw Isis::iException::Message(Isis::iException::Parse,msg, _FILEINFO_);
  }


  /**
   * Return the appropriate LabelType depending on which of
   * the valid values the argument spells
   *
   * @param labelType
   *
   * @return The RangeType enum corresponding to the string parameter
   */
  inline LabelAttachment LabelAttachmentEnumeration (const std::string &labelType) {
    Isis::iString temp(labelType);
    temp = temp.UpCase();
    if (temp == "ATTACHED") return AttachedLabel;
    if (temp == "DETACHED") return DetachedLabel;

    std::string msg = "Invalid label attachment type string [" + labelType + "]";
    throw Isis::iException::Message(Isis::iException::Parse,msg, _FILEINFO_);
  }



/**
 * @brief Parent class for CubeAttributeInput and CubeAttributeOutput.
 *
 * This class provides common functionality for the CubeAttributeInput and
 * CubeAttributeOutput classes. These classes are used to parse and
 * manipulate attribute information attached to the end of a cube filename.
 *
 * @see IsisAml IsisGui
 *
 * @ingroup Parsing
 *
 * @internal
 * @history 2003-07-09 Stuart Sides
 * Original version
 *
 * @history 2003-07-17 Stuart Sides
 * Added input file band attribute capabilities.
 *
 * @history 2003-07-29 Stuart Sides
 * Separated the input and output attributes into two separated class
 * deriving off a base class, instead of one class for all cases.
 *
 * @history 2003-10-03 Stuart Sides
 * Added members HasPixelType. It was needed by the IsisCube so it could do
 * an easy check. Added IsMsb, IsLsb, HasOrder, ByteOrderStr, ByteOrderType,
 * Order and Order. These were needed to allow users to specify a
 * byte order for output cubes.
 *
 * @history 2004-02-03 Stuart Sides
 * Refactor for IsisProcess and cube changes
 *
 * @history 2004-03-03 Stuart Sides
 * Modified IsisCubeAttributeOutput::Write so min and max don't get written
 *      when the pixel type is real.
 *
 */
  class CubeAttribute {
    public:

      //! Constructs an empty CubeAttribute
      CubeAttribute ();


      /**
       * @brief Constructs a CubeAttribute using the argument
       *
       * Constructs a CubeAttribute and initializes it with the
       *  contents of the string parameter. Minimal error checking
       *  is done to see if the string looks like an attribute.
       *
       * @param att    A string containing the file attributes. All characters
       *               before the first "+" are assumed to be the filename
       *               and are ignored.
       */
      CubeAttribute (const Isis::iString &att);


      //! Destroys the object
      virtual ~CubeAttribute ();


      /**
       * Write the attributes to a stream
       *
       * @param ostr   The stream to write the attributes to
       */
      virtual void Write(std::ostream &ostr) const;


      /**
       * Write the attributes to a string
       *
       * @param str   The string to write the attributes to
       */
      virtual void Write(std::string &str) const;


      /**
       * Write the attributes to an Isis::Pvl
       *
       * @param pvl The pvl to write the attributes to
       */
      virtual void Write(Isis::Pvl &pvl) const;

    protected:
      Isis::iString p_attribute; //!< Contains the unparsed attributes with the filename stripped
  };


#ifndef DOXY_INTERNAL

 /**
  * @brief Manipulate and parse attributes of input cube filenames.
  *
  * This class provides parsing and manipulation of attributes associated
  * with input cube filenames. Input cube filenames can have an attribute
  * of "band(s) specification"
  *
  * @see IsisAml IsisGui
  *
  * @ingroup Parsing
  *
  * @internal
  * @history 2003-07-29 Stuart Sides
  * Separated the input and output attributes into two seprated class
  * deriving off a base class, instead of one class for all cases.
  *
  * @history 2004-02-03 Stuart Sides
  * Refactor for IsisProcess and cube changes
  *
  * @history 2006-01-05 Stuart Sides
  * Fixed bug when the input attribute was "+7-10". In this case the Write
  * members were not putting the "+" at the beginning.
  *
  */
  class CubeAttributeInput : public CubeAttribute {

    public:

      //! Constructs an empty CubeAttributeInput
      CubeAttributeInput ();


      /**
       *
       * Constructs a CubeAttributeInput and initialized it with the
       * contents of the string parameter. The string is parased to
       * obtain any band specifiers. Any attribute information that
       * is not valie for an input cube will throw an error.
       *
       * @param att The attribute string to be parsed.
      **/
      CubeAttributeInput (const Isis::iString &att);


      //! Destroys the object
      ~CubeAttributeInput ();


      /**
       * Set the input attributes according to the argument. Note:
       * the attributes are not initialized prior to parsing the argument. This
       * means that multipal invocations will be cumulative.
       *
       *  @param att    A string containing the file attributes. All characters
       *                before the first "+" are assumed to be the filename
       *                and are ignored.
      **/
      void Set (const std::string &att);


      //! Set the input attributes to the default state (i.e., empty)
      void Reset ();


       //! Return an STL vector of the input bands specified
      std::vector<std::string> Bands () const;

      /**
       * @brief Return a string representation of all the bands
       *
       * @internal
       * @history Stuart Sides 2005-01-2005 ???
       *
       * Combines all the specified bands numbers into a single
       * string with commas between each band number
       *
       * @return A comma delimited string of all bands from the input attribute
       */
      std::string BandsStr() const;

      //! Set the band attribute according to the list of bands
      void Bands (const std::vector<std::string> &bands);

      //! Set the band attribute according the string parameter
      void Bands (const std::string &bands);

      //! Write the attributes to a stream
      void Write(std::ostream &ostr) const;

      //! Write the attributes to a string
      void Write(std::string &str) const;

      //! Write the attributes to a Pvl
      void Write(Isis::Pvl &pvl) const;

    private:
      std::vector<std::string> p_bands; //!< A list of the specified bands

      /**
       *
       * Parse the string parameter and populate the private
       * variable accordinly.
       *
       * @param att    A string containing the file attributes. All characters
       *               before the first "+" are assumed to be the filename
       *               and are ignored.
       */
      void Parse (const std::string &att);
  };


 /**
  * @brief Manipulate and parse attributes of output cube filenames.
  *
  * This class provides parsing and manipulation of attributes associated
  * with output cube filenames. Output cube filenames can have an attributes
  * of "minimum:maximum", "pixel type", "file format", "byte order", and
  * "label placement"
  *
  * @see IsisAml IsisGui
  *
  * @ingroup Parsing
  *
  * @internal
  * @history 2003-07-29 Stuart Sides
  * Separated the input and output attributes into two separated class
  * deriving off a base class, instead of one class for all cases.
  *
  * @history 2003-10-03 Stuart Sides
  * Added members HasPixelType. It was needed by the IsisCube so it could do
  * an easy check. Added IsMsb, IsLsb, HasOrder, ByteOrderStr, ByteOrderType,
  * Order and Order. These were needed to allow users to specify a
  * byte order for output cubes.
  *
  * @history 2004-02-03 Stuart Sides
  * Refactor for IsisProcess and cube changes
  *
  * @history 2004-03-03 Stuart Sides
  * Modified IsisCubeAttributeOutput::Write so min and max don't get written
  * when the pixel type is real.
  *
  */
  class CubeAttributeOutput : public CubeAttribute {
    public:

      //! Constructs an empty CubeAttributeOutput
      CubeAttributeOutput ();

      /**
       *
       * Constructs a CubeAttributeOutput and initialized it with the
       * contents of the string parameter. The string is parased to
       * obtain any min/max, pixel type, byte order, file format or
       * label placement. Any attribute information that
       * is not valie for an output cube will throw an error.
       *
       * @param att    A string containing the file attributes. All characters
       *               before the first "+" are assumed to be the filename
       *               and are ignored.
      **/
      CubeAttributeOutput (const Isis::iString &att);


      //! Destroys the object
      ~CubeAttributeOutput ();


      //! Return true if the pixel type is to be propagated from an input cube
      inline bool PropagatePixelType () const {
        if (p_pixelTypeDef == "PROPAGATE") {
          return true;
        }
        return false;
      };

      //! Return true if the min/max are to be propagated from an input cube
      inline bool PropagateMinimumMaximum () const {
        if (p_rangeType == Isis::PropagateRange) {
          return true;
        }
        return false;
      };

      /**
       * Set the output attributes according to the argument. Note:
       * the attributes are not initialized prior to parsing the argument. This
       * means that multipal invocations will be cumulative.
       *
       *  @param att    A string containing the file attributes. All characters
       *                before the first "+" are assumed to be the filename
       *                and are ignored.
      **/
      void Set (const std::string &att);

      /**
       * Set the output attributes to the default state
       *
       * @see Initialize
      **/
      void Reset ();

      //! Return the file format as a string
      std::string FileFormatStr() const;

      //! Return the file format an Isis::CubeFormat
      Isis::CubeFormat FileFormat() const;

      //! Set the format to the fmt parameter
      void Format (const Isis::CubeFormat fmt);

      //! Return the byte order as a string
      std::string ByteOrderStr() const;

      //! Return the byte order as an Isis::ByteOrder
      Isis::ByteOrder ByteOrder() const;

      //! Set the order according to the parameter order
      void Order (const Isis::ByteOrder order);

      //! Return the output cube attribute minimum
      double Minimum() const;

      //! Return the output cube attribute maximum
      double Maximum() const;

      //! Set the output cube attribute minimum
      void Minimum (const double min);

      //! Set the output cube attribute maximum
      void Maximum (const double max);

      //! Return the pixel type as an Isis::PixelType
      Isis::PixelType PixelType() const;

      //! Set the label attachment type to the parameter value
      void Label(Isis::LabelAttachment attachment) { p_labelAttachment = attachment; };

      //! Return true if the attachement type is "Attached"
      bool AttachedLabel() const { return p_labelAttachment == Isis::AttachedLabel; };

      //! Return true if the attachement type is "Detached"
      bool DetachedLabel() const { return p_labelAttachment == Isis::DetachedLabel; };

      //! Set the pixel type to that given by the parameter
      void PixelType (const Isis::PixelType type);

      //! Write the output attributes to a stream
      void Write(std::ostream &ostr) const;

      //! Write the output attributes to a string
      void Write(std::string &str) const;

      //! Write the output attributes to a Pvl
      void Write(Isis::Pvl &pvl) const;


    private:
      Isis::PixelType p_pixelType; //!< Stores the pixel type
      /**
       *
       * Stores weather the p_pixelType has been set or should be
       * propagated from an input cube.
       */
      std::string p_pixelTypeDef;
      /**
       *
       * Stores weather the pixel range has been set or should be
       * propagated from an input cube
       */
      Isis::RangeType p_rangeType;

      double p_minimum; //!< Stores the minimum for the output cube attribute
      double p_maximum; //!< Stores the maximum for the output cube attribute
      Isis::CubeFormat p_format; //!< Store the cube format
      Isis::ByteOrder p_order; //!< Store the byte order for the cube attribute
      Isis::LabelAttachment p_labelAttachment; //!< Store the type of label attachment

      /**
       *
       * Parse the string parameter and populate the private member variables
       * accordingly
       * @param att
       */
      void Parse (const std::string &att);

      //! Initialize the output cube attribute to default values
      void Initialize ();

  };
  #endif //DOXY_INTERNAL
};

#endif
