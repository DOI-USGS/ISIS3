#ifndef PvlContainer_h
#define PvlContainer_h
/**
 * @file
 * $Revision: 1.12 $
 * $Date: 2010/04/14 00:21:53 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include "PvlKeyword.h"

template<typename T> class QList;

namespace Isis {
  /**
   * @brief Contains more than one keyword-value pair.
   *
   * This is the container for PvlKeywords. It holds information about more than
   * one set of PvlKeywords.
   *
   * @ingroup Parsing
   *
   * @author 2002-10-11 Jeff Anderson
   *
   * @internal
   *  @history 2005-04-04 Leah Dahmer wrote class documentation.
   *  @history 2006-04-21 Jacob Danton Added format templates abilities.
   *  @history 2006-05-17 Jacob Danton Added DeleteKeyword by index method
   *  @history 2006-09-11 Stuart Sides Added formatting ability
   *  @history 2008-07-02 Steven Lambright Added const functionality
   *  @history 2008-07-10 Steven Lambright PvlContainer is no longer a PvlKeyword,
   *           but rather has a set of pvl keywords
   *  @history 2008-09-30 Christopher Austin Replaced all std::endl in the <<
   *           operator with PvlFormat.FormatEOL()
   *  @history 2008-10-30 Steven Lambright Moved Find methods' implementations to
   *           the cpp file from the header file, added <algorithm> include,
   *           problem pointed out by "novas0x2a" (Support Forum Member)
   *  @history 2009-06-01 Kris Becker - Added a new AddKeyword method that
   *           provides insert capabilities at iterator positions.
   *  @history 2010-01-06 Christopher Austin - Added CleanDuplicateKeywords()
   *  @history 2010-04-13 Eric Hyer - Added Copy constructor
   *                                - Added Assignment operator
   *  @history 2010-09-27 Sharmila Prasad - Validate all the Keywords in a Container and
   *                                        verify the 'Repeat' Option also
   *  @history 2010-10-18 Sharmila Prasad - Added more options for the keyword validation
   *  @todo 2005-04-04 Need coded example.
   */
  class PvlContainer {
    public:
      PvlContainer(const QString &type);
      PvlContainer(const QString &type, const QString &name);
      PvlContainer(const PvlContainer &other);

      //! Set the name of the container.
      void SetName(const QString &name) {
        p_name.SetValue(name);
      };
      /**
       * Returns the container name.
       * @return The container name.
       */
      inline QString Name() const {
        return (QString) p_name;
      };
      /**
       * Returns whether the given string is equal to the container name or not.
       * @param match The string to compare to the name.
       * @return True if the name and string are the same, false if they are
       * not.
       */
      bool IsNamed(const QString &match) const {
        return PvlKeyword::StringEqual(match, (QString)p_name);
      }
      /**
       * Returns the container type.
       * @return The container type.
       */
      inline QString Type() const {
        return p_name.Name();
      };
      /**
       * Returns the number of keywords contained in the PvlContainer.
       * @return The number of keywords.
       */
      inline int Keywords() const {
        return p_keywords.size();
      };

      //! Clears PvlKeywords
      void Clear() {
        p_keywords.clear();
      };
      //! Contains both modes: Append or Replace.
      enum InsertMode { Append, Replace };
      /**
       * Add a keyword to the container.
       * @param keyword The PvlKeyword object to append.
       * @param mode Using the InsertMode value of Append.
       */
      void AddKeyword(const PvlKeyword &keyword,
                      const InsertMode mode = Append);


      /**
       * When you use the += operator with a PvlKeyword, it will call the
       * AddKeyword() method.
       * @param keyword The PvlKeyword to be added.
       */
      void operator+= (const PvlKeyword &keyword) {
        AddKeyword(keyword);
      };

      PvlKeyword &FindKeyword(const QString &name);
      /**
       * When you use the [] operator with a (string) name, it will call the
       * FindKeyword() method.
       * @param name The name of the keyword to find.
       */
      PvlKeyword &operator[](const QString &name) {
        return FindKeyword(name);
      };
      PvlKeyword &operator[](const int index);

      /**
       * When you use the [] operator with a (char) name, it will call the
       * FindKeyword() method.
       * @param name The name of the keyword to find.
       */
      PvlKeyword &operator[](const char *name) {
        return operator[](QString(name));
      };

      const PvlKeyword &FindKeyword(const QString &name) const;
      /**
       * When you use the [] operator with a (string) name, it will call the
       * FindKeyword() method.
       * @param name The name of the keyword to find.
       */

      const PvlKeyword &operator[](const QString &name) const {
        return FindKeyword(name);
      };
      const PvlKeyword &operator[](const int index) const;

      /**
       * When you use the [] operator with a (char) name, it will call the
       * FindKeyword() method.
       * @param name The name of the keyword to find.
       */
      PvlKeyword operator[](const char *name) const {
        return operator[](QString(name));
      };

      bool HasKeyword(const QString &name) const;
      //! The keyword iterator.
      typedef QList<PvlKeyword>::iterator PvlKeywordIterator;

      //! The const keyword iterator
      typedef QList<PvlKeyword>::const_iterator ConstPvlKeywordIterator;


      PvlKeywordIterator FindKeyword(const QString &name,
                                     PvlKeywordIterator beg,
                                     PvlKeywordIterator end);

      ConstPvlKeywordIterator FindKeyword(const QString &name,
                                          ConstPvlKeywordIterator beg,
                                          ConstPvlKeywordIterator end) const;

      PvlKeywordIterator AddKeyword(const PvlKeyword &keyword,
                                    PvlKeywordIterator pos);

      /**
       * Return the beginning iterator.
       * @return The beginning iterator.
       */
      PvlKeywordIterator Begin() {
        return p_keywords.begin();
      };

      /**
       * Return the const beginning iterator.
       * @return The const beginning iterator.
       */
      ConstPvlKeywordIterator Begin() const {
        return p_keywords.begin();
      };

      /**
       * Return the ending iterator.
       * @return The ending iterator.
       */
      PvlKeywordIterator End() {
        return p_keywords.end();
      };

      /**
       * Return the const ending iterator.
       * @return The const ending iterator.
       */
      ConstPvlKeywordIterator End() const {
        return p_keywords.end();
      };

      void DeleteKeyword(const QString &name);
      void DeleteKeyword(const int index);

      bool CleanDuplicateKeywords();

      /**
       * When you use the -= operator with a (string) name, it will call the
       * DeleteKeyword() method.
       * @param name The name of the keyword to remove.
       */
      void operator-= (const QString &name) {
        DeleteKeyword(name);
      };
      /**
       * When you use the -= operator with a PvlKeyword object, it will call the
       * DeleteKeyword() method.
       * @param key The PvlKeyword object to remove.
       */
      void operator-= (const PvlKeyword &key) {
        DeleteKeyword(key.Name());
      };
      /**
       * Returns the filename used to initialise the Pvl object. If the object
       * was not initialized using a file, this string is empty.
       * @return The filename.
       */
      QString FileName() const {
        return p_filename;
      };

      void SetFormatTemplate(PvlContainer &ref) {
        p_formatTemplate = &ref;
      };

      bool HasFormatTemplate() {
        return p_formatTemplate != NULL;
      };

      PvlContainer *FormatTemplate() {
        return p_formatTemplate;
      };

      PvlFormat *GetFormat() {
        return p_name.GetFormat();
      }
      void SetFormat(PvlFormat *format) {
        p_name.SetFormat(format);
      }

      int Indent() {
        return p_name.Indent();
      }
      void SetIndent(int indent) {
        p_name.SetIndent(indent);
      }

      inline int Comments() const {
        return p_name.Comments();
      };
      QString Comment(const int index) const {
        return p_name.Comment(index);
      }

      void AddComment(const QString &comment) {
        p_name.AddComment(comment);
      }

      PvlKeyword &GetNameKeyword() {
        return p_name;
      }
      const PvlKeyword &GetNameKeyword() const {
        return p_name;
      }

      const PvlContainer &operator=(const PvlContainer &other);

    protected:
      QString p_filename;                   /**<This contains the filename
                                                    used to initialize
                                                    the pvl object. If the
                                                    object was not
                                                    initialized using a filename
                                                    the string is empty.*/
      PvlKeyword p_name;                   //!< This is the name keyword
      QList<PvlKeyword> p_keywords; /**<This is the vector of
                                                    PvlKeywords the container is
                                                    holding. */

      void Init();

      /**
       * Sets the filename to the specified string.
       *
       * @param filename The new filename to use.
       */
      void SetFileName(const QString &filename) {
        p_filename = filename;
      }

      PvlContainer *p_formatTemplate;
      
      //! Validate All the Keywords in a Container comparing with the Template
      void ValidateAllKeywords(PvlContainer &pPvlCont);
      
      //! Validate the Repeat Option for a Keyword
      void ValidateRepeatOption(PvlKeyword & pPvlTmplKwrd, PvlContainer & pPvlCont);
  };

  std::ostream &operator<<(std::ostream &os, PvlContainer &container);
};

#endif
