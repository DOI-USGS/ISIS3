#ifndef Resource_h
#define Resource_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

// Qt library
#include <QExplicitlySharedDataPointer>
#include <QMap>
#include <QMetaType>
#include <QSharedData>
#include <QString>
#include <QVariant>

#include "GisGeometry.h"  // SharedGisGeometry type
#include "PvlFlatMap.h"

namespace Isis {

  class PvlContainer;
  class PvlKeyword;
  class PvlObject;

  /**
   * This class provides a resource of PVL keywords for Strategy classes. The Resource may have a
   * associated GIS geometry and list of assets. If a Resource has been flagged as discarded
   * (i.e. inactive), then the Strategy using the Resource may be set to not apply algorithms to
   * this Resource.
   *
   * @author 2012-07-15 Kris Becker 
   * @internal 
   *   @history 2012-07-15 Kris Becker - Original version.
   *   @history 2015-03-18 Jeannie Backer - Brought class files closer to ISIS coding standards.
   *   @history 2015-03-10 Ian Humphrey - Fixed bug in toPvl() method. The iterator was never being
   *                           incremented, causing the cnetreader tests to hang.
   *   @history 2015-03-28 Kris Becker - Added the isEqual() method to test for equality or another 
   *                           resource
   *   @history 2015-03-26 Jeannie Backer - Updated documentation.
   *   @history 2015-04-09 Kris Becker - Add Idenitity keyword to Resource upon construction and 
   *                           when callers use setName().
   *   @history 2015-05-06 Kris Becker - Removed some methods that are currently not being called
   *                           to improve code coverage
   *   @history 2015-09-27 Kris Becker - Refactored to store all Resource data (name, keywords,
   *                           assets and geometry) in an explicity shared data structure. The 
   *                           status is maintained outside so that Resources can be created to 
   *                           share data, but the active status can be maintained in separate 
   *                           instances. This is quite handy for creating Assets.
   *   @history 2016-02-29 Ian Humphrey - Added Resource class to ISIS. Added unit test,
   *                           reviewed documentation and coding statndards compliance. Fixes #2406.
   */
  class Resource  {

    public:
      Resource();
      Resource(const QString &name);
      Resource(const QString &name, const PvlFlatMap &profile);
      Resource(const QString &name, const PvlContainer &profile);
      Resource(const Resource &other);
      virtual ~Resource();

      QString name() const;
      void setName(const QString &identity);
      bool isEqual(const Resource &other) const;

      bool exists(const QString &keywordName) const;
      int  count(const QString &keywordName) const;
      bool isNull(const QString &keywordName, const int keywordIndex = 0) const;

      const PvlFlatMap &keys() const;
      QString value(const QString &keywordName, const int &keywordIndex  = 0) const;
      QString value(const QString &keywordName, const QString &defValue,
                    const int &keywordIndex  = 0) const;
      PvlKeyword keyword(const QString &keywordName) const;

      void add(const QString &keywordName, const QString &keywordValue);
      void add(const PvlKeyword &keyword);
      void add(const PvlFlatMap &keys);
      void append(const QString &keywordName, const QString &keywordValue);

      int erase(const QString &keywordName);

      void add(GisGeometry *geom);
      void add(SharedGisGeometry &geom);

      bool hasGeometry() const;
      bool hasValidGeometry() const;
      SharedGisGeometry geometry() const;

      void activate();
      bool isActive() const;
      void discard();
      bool isDiscarded() const;

      bool hasAsset(const QString &assetName) const;
      void addAsset(const QString &assetName, QVariant &assetValue);
      QVariant asset(const QString &assetName) const;
      int removeAsset(const QString &assetName);
      int clearAssets();

      virtual Resource *copy() const;
      virtual Resource *clone(const QString &name, 
                              const bool &withAssets=false) const;

      virtual PvlObject toPvl(const QString &pvlName = "Resource") const;

    protected:
      Resource(const Resource &other, const bool deepcopy);

    private:
      //! Defintion for VariantList, a map between a string and a variant type variable.
      typedef QMap<QString, QVariant> VariantList;

      /** 
       * Shared Resource data pointer
       *  
       * @author 2012-07-15 Kris Becker 
       * @internal 
       *   @history 2012-07-15 Kris Becker - Original version.
       */
      class ResourceData : public QSharedData {
        public:
          ResourceData() : m_name("Resource"), m_keys(), 
                           m_assets(), m_geom(0) { }
          ResourceData(const ResourceData &other) : QSharedData(other), 
                                                    m_name(other.m_name),
                                                    m_keys(other.m_keys),
                                                    m_assets(other.m_assets),
                                                    m_geom(other.m_geom) { }
          ResourceData(const QString &name) : QSharedData(),
                                              m_name(name),
                                              m_keys(),
                                              m_assets(),
                                              m_geom(0) { }
          ResourceData(const QString &name, const PvlFlatMap &profile) : 
                                             QSharedData(),
                                              m_name(name),
                                              m_keys(profile),
                                              m_assets(),
                                              m_geom(0) { }

          QString           m_name;    /**< Name of the Resource. This value is set by the 
                                            constructor or the setName() mutator.*/
          PvlFlatMap        m_keys;    /**< The list of keywords associated with this Resource. 
                                            This is a map between PvlKeyword objects and their 
                                            names. It can be modified by the constructor, add(),
                                            append() or erase() methods.*/
          VariantList       m_assets;  /**< The list of assets associated with this Resource. This
                                            is a map between asset names and values. It can be 
                                            modified by the addAsset() and removeAsset() methods.*/
          SharedGisGeometry m_geom;    /**< A shared pointer to the GIS geometry object associated
                                            with this Resource. It is set using the add() methods.*/ 
      };

      // Member variables
      //! Explicitly managed pointer to Resource data
      QExplicitlySharedDataPointer<ResourceData> m_data; 
      bool                  m_discard; /**< A flag to indicate whether the Resource is inactive. It
                                            is set to false (active) in the constructors and can be
                                            reset using the discard() method.*/
  };
  
  //! Defintion of a SharedResource, a shared pointer to a Resource object. 
  typedef QSharedPointer<Resource> SharedResource;
  
  //! Defintion of a ResourceList, a list of SharedResource types.
  typedef QList<SharedResource> ResourceList;


} // Namespace Isis

// Declarations so they can be stored as QVariants (assets)
Q_DECLARE_METATYPE(Isis::SharedResource);
Q_DECLARE_METATYPE(Isis::ResourceList);

#endif

