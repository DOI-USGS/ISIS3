/**
 * @file
 * $Revision: 1.2 $
 * $Date: 2008/06/19 15:56:43 $
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

#ifndef Message_h
#define Message_h

#include <QString>
#include <vector>

namespace Isis {
  /* Documentation for this namespaceis in:
  *    isis/src/docsys/Object/build/isisDoxyDefs.doxydef
  */
  namespace Message {
    /**
     * This error should be used when an Isis object or application is checking
     * array bounds and the legal range has been violated.
     *
     * @param index - Array index which is out of bounds
     *
     * @return QString - A standardized Isis error message with the parameter
     *                  inserted
     */
    QString ArraySubscriptNotInRange(int index);

    /**
     * This error should be used when a supplied keyword for an Isis application
     * parameter does not contain enough characters to distinguish it from
     * another keyword.
     *
     * @param key - The name of the keyword which is ambiguous
     *
     * @return QString - A standardized Isis error message with the parameter
     *                  inserted
     */
    QString KeywordAmbiguous(const QString &key);

    /**
     * This error should be used when a supplied keyword for an Isis application
     * parameter does not match any of the parameters for that application.
     *
     * @param key - The name of the keyword which is unrecognized
     *
     * @return QString - A standardized Isis error message with the parameter
     *                  inserted
     */
    QString KeywordUnrecognized(const QString &key);

    /**
     * This error should be used when a supplied keyword for an Isis application
     * parameter is the same as one previously supplied.
     *
     * @param key - The name of the keyword which has been duplicated
     *
     * @return QString - A standardized Isis error message with the parameter
     *                  inserted
     */
    QString KeywordDuplicated(const QString &key);

    /**
     * This error should be used when a supplied keyword is being used as an
     * array (i.e., with a subscript) but is only a scaler entity.
     *
     * @param key - The name of the keyword which is not an array
     *
     * @return QString - A standardized Isis error message with the parameter
     *                  inserted
     */
    QString KeywordNotArray(const QString &key);

    /**
     * This error should be used when a supplied keyword is not defined in the
     * application.
     *
     * @param key - The name of the keyword which was not found
     *
     * @return QString - A standardized Isis error message with the parameter
     *                  inserted
     */
    QString KeywordNotFound(const QString &key);

    /**
     * This error has been deprecated and should not be used in new code.
     *
     * @param block - The name of the invalid keyword block
     *
     * @return QString - A standardized Isis error message with the parameter
     *                  inserted
     */
    QString KeywordBlockInvalid(const QString &block);

    /**
     * This error has been deprecated and should not be used in new code.
     *
     * @param block - The name of the block with the missing start tag
     * @param found - The QString found in the place of the missing block start
     *
     * @return QString - A standardized Isis error message with the parameter
     *                  inserted
     */
    QString KeywordBlockStartMissing(const QString &block, const QString &found);

    /**
     * This error has been deprecated and should not be used in new code.
     *
     * @param block - The name of the block with the missing end tag
     * @param found - The QString found in the place of the missing block end
     *
     * @return QString - A standardized Isis error message with the parameter
     *                  inserted
     */
    QString KeywordBlockEndMissing(const QString &block, const QString &found);

    /**
     * This error should be used when a supplied keyword does not appear in
     * the list (e.g., an Isis cube label).
     *
     * @param key - The name of the keyword which is bad
     *
     * @return QString - A standardized Isis error message with the parameter
     *                  inserted
     */
    QString KeywordValueBad(const QString &key);

    /**
     * This error should be used when the value of a supplied keyword is
     * incorrect.
     *
     * @param key - The keyword name which is bad
     * @param value - The value of the keyword which is bad
     *
     * @return QString - A standardized Isis error message with the parameter
     *                  inserted
     */
    QString KeywordValueBad(const QString &key, const QString &value);

    /**
     * This error should be used when the value of a supplied keyword was
     * expected but not present.
     *
     * @param key - The keyword name which is bad
     *
     * @return QString - A standardized Isis error message with the parameter
     *                  inserted
     */
    QString KeywordValueExpected(const QString &key);

    /**
     * This error should be used when the value of a supplied keyword is
     * not within a specific range.
     *
     * @param key - The keyword name which has a bad value
     * @param value - The value of the keyword
     * @param range - The minimum and maximum allowed values for the keyword
     *                formatted and inserted into a QString
     *
     * @return QString - A standardized Isis error message with the parameter
     *                  inserted
     */
    QString KeywordValueNotInRange(const QString &key, const QString &value,
                                  const QString &range);

    /**
     * This error should be used when the value of a supplied keyword is
     * not one of a specific list of possible values.
     *
     * @param key - The keyword name which has a bad value
     * @param value - The value of the keyword
     * @param list - A vector of all the legal values for the keyword
     *
     * @return QString - A standardized Isis error message with the parameter
     *                  inserted
     */
    QString KeywordValueNotInList(const QString &key, const QString &value,
                                  const std::vector<QString> &list);

    /**
     * This error should be used when a delimiter is missing.
     *
     * @param delimiter - The delimiter that is missing
     *
     * @return QString - A standardized Isis error message with the parameter
     *                  inserted
     */
    QString MissingDelimiter(const char delimiter);

    /**
     * This error should be used when a delimiter is missing.
     *
     * @param delimiter - The delimiter that is missing
     * @param near - The value the missing delimiter is near
     *
     * @return QString - A standardized Isis error message with the parameter
     *                  inserted
     */
    QString MissingDelimiter(const char delimiter, const QString &near);

    /**
     * This error should be used when a file could not be opened.
     *
     * @param filename - The name of the file
     *
     * @return QString - A standardized Isis error message with the parameter
     *                  inserted
     */
    QString FileOpen(const QString &filename);

    /**
     * This error should be used when a file could not be created.
     *
     * @param filename - The name of the file
     *
     * @return QString - A standardized Isis error message with the parameter
     *                  inserted
     */
    QString FileCreate(const QString &filename);

    /**
     * This error should be used when an error accrues during a read of a file.
     *
     * @param filename - The name of the file
     *
     * @return QString - A standardized Isis error message with the parameter
     *                  inserted
     */
    QString FileRead(const QString &filename);

    /**
     * This error should be used when an error accrues during a write to a file.
     *
     * @param filename - The name of the file
     *
     * @return QString - A standardized Isis error message with the parameter
     *                  inserted
     */
    QString FileWrite(const QString &filename);

    /**
     * This error should be used when an error accrues during a memory
     * allocation such as "new".
     *
     * @return QString - A standardized Isis error message with the parameter
     *                  inserted
     */
    QString MemoryAllocationFailed();
  }
}

#endif
