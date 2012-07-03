#ifndef CubeAttribute_h
#define CubeAttribute_h
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

#include <string>
#include <typeinfo>

#include <QDebug>

#include "Cube.h"
#include "Endian.h"
#include "FileName.h"
#include "IException.h"
#include "iString.h"
#include "PixelType.h"

class QStringList;

namespace Isis {
  /**
   * @brief Input cube label type tracker
   *
   * This enumeration and its functions are for the label
   * type of an input cube. The enum defines the type of labels (i.e.,
   * Both the label and cube are in the same file and the label is in a
   * separate file from the cube.
   */
  enum LabelAttachment {
    AttachedLabel,  //!< The input label is embedded in the image file
    DetachedLabel,  //!< The input label is in a separate data file from the image
    /**
     * The label is pointing to an external DN file - the label is also external to the data.
     *
     * This format implies that the output is a cube that contains everything except DN data
     *   (more similar to attached than detached).
     */
    ExternalLabel
  };


  /**
   * Return the string representation of the contents of a
   * variable of type LabelAttachment
   *
   * @param labelType The LabelAttachment enum to be converted to a string
   *
   * @return A string representation of the parameter
   */
  inline std::string LabelAttachmentName(LabelAttachment labelType) {
    if(labelType == AttachedLabel) return "Attached";
    if(labelType == DetachedLabel) return "Detached";
    if(labelType == ExternalLabel) return "External";

    std::string msg = "Invalid label attachment type [" + iString(labelType) + "]";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }


  /**
   * Return the appropriate LabelType depending on which of
   * the valid values the argument spells
   *
   * @param labelType
   *
   * @return The RangeType enum corresponding to the string parameter
   */
  inline LabelAttachment LabelAttachmentEnumeration(const std::string &labelType) {
    iString temp(labelType);
    temp = temp.UpCase();
    if(temp == "ATTACHED") return AttachedLabel;
    if(temp == "DETACHED") return DetachedLabel;
    if(temp == "External") return ExternalLabel;

    std::string msg = "Invalid label attachment type string [" + labelType + "]";
    throw IException(IException::Unknown, msg, _FILEINFO_);
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
   * @author 2003-07-09 Stuart Sides
   *
   * @internal
   *   @history 2003-07-17 Stuart Sides - Added input file band attribute
   *                           capabilities.
   *   @history 2003-07-29 Stuart Sides - Separated the input and output
   *                           attributes into two separated class deriving off
   *                           a base class, instead of one class for all cases.
   *   @history 2003-10-03 Stuart Sides - Added members HasPixelType. It was
   *                           needed by the IsisCube so it could do an easy
   *                           check. Added IsMsb, IsLsb, HasOrder,
   *                           ByteOrderStr, ByteOrderType, Order and Order.
   *                           These were needed to allow users to specify a
   *                           byte order for output cubes.
   *   @history 2004-02-03 Stuart Sides - Refactor for IsisProcess and cube
   *                           changes
   *   @history 2004-03-03 Stuart Sides - Modified
   *                           IsisCubeAttributeOutput::Write so min and max
   *                           don't get written when the pixel type is real.
   *   @history 2012-07-02 Steven Lambright and Stuart Sides - Refactored to minimize
   *                           code duplication. Updated to match current coding standards.
   *                           Added safety check capabilities for unrecognized attributes.
   *                           References #961.
   */
  template<typename ChildClass> class CubeAttribute {
    public:

      //! Constructs an empty CubeAttribute
      CubeAttribute(QList< bool (ChildClass::*)(iString) const > testers) {
        m_attributeTypeTesters = testers;
      }


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
      CubeAttribute(QList< bool (ChildClass::*)(iString) const > testers,
                    const FileName &fileName) {
        m_attributeTypeTesters = testers;
        setAttributes(fileName);
      }


      //! Destroys the object
      virtual ~CubeAttribute() {
      }


      /**
       * Return a string-representation of this cube attributes. This will typically be exactl
       *   the string passed in if you used setAttributes(...). This can be an empty string ("") or
       *   if there are attributes then it will be +att1+att2+... The result of this method could
       *   be used to re-create this instance.
       *
       * @return The cube attributes in string form
       */
      iString toString() const {
        iString result;

        if (!m_attributes.isEmpty())
          result = "+" + m_attributes.join("+");

        return result;
      }


      /**
       * Add a single attribute to these attributes. This attribute should NOT have a '+' in it.
       *   For example, if you wanted to add BandSequential to the CubeAttributeOutput class, you
       *   could call addAttribute("BSQ") or addAttribute("BandSequential") or any valid deviation
       *   of that string. This will preserve existing attributes unless it's of the same type (if
       *   Tile was already set, then this will overwrite Tile). If the attribute is unrecognized
       *   or ambiguous, then an exception will be thrown.
       *
       * @param attribute The attribute we're adding to the current cube attributes
       */
      void addAttribute(iString attribute) {
        iString upcaseAtt = iString(attribute).UpCase();

        if (attribute.ToQt().contains("+")) {
          throw IException(IException::Unknown,
                           "Individual attributes (for example, BSQ) cannot contain the '+' "
                           "character because that is used to denote the separation of individual "
                           "attributes",
                           _FILEINFO_);
        }

        // Verify this attribute is legal
        bool legal = false;
        bool (ChildClass::*tester)(iString) const;
        foreach (tester, m_attributeTypeTesters) {
          if ( (static_cast<const ChildClass *>(this)->*tester)(upcaseAtt) ) {
            if (legal) {
              throw IException(IException::Unknown,
                               QObject::tr("Attribute [%1] is ambiguous").arg(attribute.ToQt()),
                               _FILEINFO_);
            }

            legal = true;
          }
        }

        if (!legal) {
          throw IException(IException::Unknown,
                           QObject::tr("Attribute [%1] is not recognized").arg(attribute.ToQt()),
                           _FILEINFO_);
        }

        m_attributes.append(attribute.ToQt());
      }


      /**
       * Append the attributes found in the filename to these cube attributes. This will call
       *   addAttribute() for every attribute found in the file name.
       *
       * @see FileName::attributes()
       * @param fileNameWithAtts A filename with attributes appended, for example
       *                         FileName("out.cub+Bsq")
       */
      void addAttributes(const FileName &fileNameWithAtts) {
        addAttributes(fileNameWithAtts.attributes());
      }


      /**
       * Append the attributes in the string to these cube attributes. This will call
       *   addAttribute() for every attribute in the string. The initial "+" is not expected but
       *   allowed. This should NOT be called with a file name.
       *
       * @param attributesString A string of recognizable attributes, for example
       *                         "+Bsq+Real" or "Bsq+Real"
       */
      void addAttributes(const char *attributesString) {
        addAttributes(iString(attributesString));
      }


      /**
       * Append the attributes in the string to these cube attributes.
       *
       * @see addAttributes(const char *)
       *
       * @param attributesString A string of recognizable attributes, for example
       *                         "+Bsq+Real" or "Bsq+Real"
       */
      void addAttributes(const iString &attributesString) {
        setAttributes(toString() + "+" + attributesString);
      }


      /**
       * Replaces the current attributes with the attributes in the given file name. This will call
       *   addAttribute() for every attribute in the file name.
       *
       * @see FileName::attributes()
       * @see addAttributes(const char *)
       * @param fileName A file name with (or without) attributes on the end, for example
       *                 FileName("out.cub+Bsq")
       */
      void setAttributes(const FileName &fileName) {
        QStringList attributes = fileName.attributes().ToQt().split("+", QString::SkipEmptyParts);

        m_attributes.clear();
        foreach (QString attribute, attributes)
          addAttribute(attribute);
      }


    protected:
      /**
       * Get a list of attributes that the tester returns true on. This is helpful for accessing the
       *   values of existing attributes. The strings will always be the UPPER CASE version of the
       *   attribute, i.e. not Bsq but BSQ. The returned attributes do not contain delimiters.
       *
       * @param tester A method that determines whether the attribute should be returned/is relevant
       * @return A list of attributes for which the tester returns true on.
       */
      QStringList attributeList(bool (ChildClass::*tester)(iString) const) const {
        QStringList relevantAttributes;

        foreach (QString attribute, m_attributes) {
          iString upcaseAtt = iString(attribute).UpCase();
          if ( (static_cast<const ChildClass *>(this)->*tester)(upcaseAtt) ) {
            relevantAttributes.append(upcaseAtt);
          }
        }

        return relevantAttributes;
      }


      /**
       * Set the attribute(s) for which tester returns true to newValue. If multiple attributes
       *   match (tester returns true on them), only the first one is preserved and it's value
       *   becomes newValue. Subsequent matching attributes are removed/deleted. This is done to
       *   simplify the resulting attribute string to be minimal with this particular attribute.
       *
       * @param newValue The string to set the attribute to... tester(newValue) really ought to
       *                 return true.
       * @param tester A method that determines if an attribute is of the same type of newValue, so
       *               that existing attributes can be overwritten.
       */
      void setAttribute(iString newValue, bool (ChildClass::*tester)(iString) const) {
        QMutableListIterator<QString> it(m_attributes);

        bool found = false;
        while (it.hasNext()) {
          QString &attribute = it.next();

          iString upcaseAtt = iString(attribute).UpCase();
          if ( (static_cast<const ChildClass *>(this)->*tester)(upcaseAtt) ) {
            if (found || newValue == "") {
              // already found one (remove the duplicate) or just deleting it
              it.remove();
            }
            else {
              // modify existing attribute value
              attribute = newValue.ToQt();
            }

            found = true;
          }
        }

        // Attribute doesn't exist, add it
        if (!found && newValue != "") {
          m_attributes.append(newValue);
        }
      }

    private:
      /**
       * These are the attributes that this cube attribute stores. These attributes do not contain
       *   any delimiters, are not formatted and often are exactly what a user has typed in.
       *   Everything in this list will return true when given to exactly one of the testers.
       */
      QStringList m_attributes;

      /**
       * These testers determine if an attribute looks like a particular option. For example,
       *   "Bsq" looks like a cube format so that tester would return true. However, the pixel type
       *   tester would return false. This is used to validate that every attribute looks like one
       *   and only one data type (is unambiguous and is known). This list will not change after
       *   this class is instantiated.
       */
      QList< bool (ChildClass::*)(iString) const > m_attributeTypeTesters;
  };


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
   * @author 2003-07-29 Stuart Sides
   *
   * @internal
   *   @history 2003-07-29 Stuart Sides - Separated the input and output
   *                           attributes into two seprated class deriving off a
   *                           base class, instead of one class for all cases.
   *   @history 2004-02-03 Stuart Sides - Refactor for IsisProcess and cube
   *                           changes
   *   @history 2006-01-05 Stuart Sides - Fixed bug when the input attribute was
   *                           "+7-10". In this case the Write members were not
   *                           putting the "+" at the beginning.
   *   @history 2012-07-02 Steven Lambright and Stuart Sides - Uses a refactored
   *                           CubeAttribute parent class now. Updated to match current
   *                           coding standards. Added safety checks for
   *                           unrecognized attributes. References #961.
   */
  class CubeAttributeInput : public CubeAttribute<CubeAttributeInput> {

    public:

      //! Constructs an empty CubeAttributeInput
      CubeAttributeInput();


      /**
       *
       * Constructs a CubeAttributeInput and initialized it with the
       * contents of the string parameter. The string is parsed to
       * obtain any band specifiers. Any attribute information that
       * is not valid for an input cube will throw an error.
       *
       * @param att The attribute string to be parsed.
      **/
      CubeAttributeInput(const FileName &fileName);


      //! Destroys the object
      ~CubeAttributeInput();


      //! Return a vector of the input bands specified
      std::vector<std::string> bands() const;

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
      iString bandsString() const;

      //! Set the band attribute according to the list of bands
      void setBands(const std::vector<std::string> &bands);

      using CubeAttribute<CubeAttributeInput>::toString;

    private:
      bool isBandRange(iString attribute) const;

      static iString toString(const std::vector<std::string> &bands);
      static QList<bool (CubeAttributeInput::*)(iString) const> testers();

    private:
      std::vector<std::string> m_bands; //!< A list of the specified bands
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
   * @author 2003-07-29 Stuart Sides
   *
   * @internal
   *   @history 2003-07-29 Stuart Sides - Separated the input and output
   *                           attributes into two separated class deriving off
   *                           a base class, instead of one class for all cases.
   *   @history 2003-10-03 Stuart Sides - Added members HasPixelType. It was
   *                           needed by the IsisCube so it could do an easy
   *                           check. Added IsMsb, IsLsb, HasOrder,
   *                           ByteOrderStr, ByteOrderType, Order and Order.
   *                           These were needed to allow users to specify a
   *                           byte order for output cubes.
   *   @history 2004-02-03 Stuart Sides - Refactor for IsisProcess and cube
   *                           changes
   *   @history 2004-03-03 Stuart Sides - Modified
   *                           IsisCubeAttributeOutput::Write so min and max
   *                           don't get written when the pixel type is real.
   *   @history 2012-07-02 Steven Lambright and Stuart Sides - Uses a refactored
   *                           CubeAttribute parent class now. Updated to match current
   *                           coding standards. Added the "+External+ attribute. Added safety
   *                           checks for unrecognized attributes. References #961.
   */
  class CubeAttributeOutput : public CubeAttribute<CubeAttributeOutput> {

    public:

      //! Constructs an empty CubeAttributeOutput
      CubeAttributeOutput();

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
       */
      CubeAttributeOutput(const FileName &fileName);


      //! Destroys the object
      ~CubeAttributeOutput();


      //! Return true if the pixel type is to be propagated from an input cube
      bool propagatePixelType() const;

      //! Return true if the min/max are to be propagated from an input cube
      bool propagateMinimumMaximum() const;

      //! Return the file format an Cube::Format
      Cube::Format fileFormat() const;

      //! Return the file format as a string
      iString fileFormatString() const;

      //! Set the format to the fmt parameter
      void setFileFormat(Cube::Format fmt);

      //! Return the byte order as an Isis::ByteOrder
      ByteOrder byteOrder() const;

      //! Return the byte order as a string
      iString byteOrderString() const;

      //! Set the order according to the parameter order
      void setByteOrder(ByteOrder order);

      //! Return the output cube attribute minimum
      double minimum() const;

      //! Return the output cube attribute maximum
      double maximum() const;

      //! Set the output cube attribute minimum
      void setMinimum(double min);

      //! Set the output cube attribute maximum
      void setMaximum(double max);

      //! Return the pixel type as an Isis::PixelType
      PixelType pixelType() const;

      //! Set the pixel type to that given by the parameter
      void setPixelType(PixelType type);

      //! Set the label attachment type to the parameter value
      void setLabelAttachment(LabelAttachment attachment);

      LabelAttachment labelAttachment() const;

      using CubeAttribute<CubeAttributeOutput>::toString;


    private:
      bool isByteOrder(iString attribute) const;
      bool isFileFormat(iString attribute) const;
      bool isLabelAttachment(iString attribute) const;
      bool isPixelType(iString attribute) const;
      bool isRange(iString attribute) const;

      static iString toString(Cube::Format);

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

      static QList<bool (CubeAttributeOutput::*)(iString) const> testers();
  };
};

#endif
