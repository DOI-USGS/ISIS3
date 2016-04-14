#ifndef RunCommandStrategy_h
#define RunCommandStrategy_h
/**
 * @file                                                                  
 * $Revision: 6187 $ 
 * $Date: 2015-05-11 17:31:51 -0700 (Mon, 11 May 2015) $ 
 * $Id: RunCommandStrategy.h 6187 2015-05-12 00:31:51Z kbecker@GS.DOI.NET $
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

// parent class
#include "Strategy.h"

// Qt library
#include <QList>
#include <QString>
#include <QStringList>

// ResourceList and SharedResource typedefs
#include "Resource.h"

namespace Isis {

  class PvlObject;

  /**
   * @brief RunCommandStrategy Executes shell commands for each Resource
   *  
   * @code 
   * Object = Strategy 
   *   Name = RunSomeCommands
   *   Type = RunCommand
   * EndObject
   * @endcode
   *  
   * @author 2015-05-14 Kris Becker 
   * @internal 
   *   @history 2015-05-14 Kris Becker - Original version.
   */
  class RunCommandStrategy : public Strategy {
  
    public:
      RunCommandStrategy();
      RunCommandStrategy(const PvlObject &definition, const ResourceList &globals);
      virtual ~RunCommandStrategy();
  
      virtual int apply(ResourceList &resources, const ResourceList &globals);
      virtual int apply(SharedResource &resource, const ResourceList &globals);
  
    private:
      struct Command {
        Command(const QString &name, const QString &command) : 
                m_name(name), m_command(command), m_status(0) { }
        QString m_name;
        QString m_command;
        int     m_status;
      };

      QList<Command> m_preCommands;
      QList<Command> m_commands;
      QList<Command> m_postCommands;
      QStringList    m_argKeys;
      bool           m_skipAllNoData;
      bool           m_onPreCommandErrorContinue;
      bool           m_onPostCommandErrorContinue;
  
  };

} // Namespace Isis

#endif
