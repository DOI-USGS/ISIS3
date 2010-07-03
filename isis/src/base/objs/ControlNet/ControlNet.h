#ifndef ControlNet_h
#define ControlNet_h

/**
 * @file
 * $Revision: 1.12 $
 * $Date: 2009/09/26 00:52:11 $
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

#include "ControlPoint.h"
#include "Camera.h"
#include "SerialNumberList.h"
#include "Progress.h"

#include <QHash>
#include <QVector>
#include <QString>

namespace Isis {
  /**
   * @brief a control network
   *
   * This class is used to store a network of ControlPoints
   *
   * @ingroup ControlNetwork
   *
   * @author 2005-07-29 Jeff Anderson
   *
   * @see ControlPoint ControlMeasure
   *
   * @internal
   *   @history 2005-07-29 Jeff Anderson Original version
   *   @history 2006-01-11 Jacob Danton Updated unitTest
   *   @history 2006-06-22 Brendan George Updated to conform to changes in
   *                                      SerialNumberList class
   *   @history 2008-04-04 Christopher Austin Added Exists function
   *   @history 2008-04-18 Debbie A. Cook Added Progress reports to loading and SetImages
   *            and calculates the total number of measurements in the control net
   *   @history 2008-06-18 Christopher Austin Fixed documentation errors
   *   @history 2009-01-06 Jeannie Walldren - Fixed typo in
   *            SetImages() exception output. Added
   *            documentation.
   *   @history 2009-02-05 Christopher Austin - when the created date or the
   *            modified date are not set, they default to the time in which
   *            Write() is called.
   *   @history 2009-04-07 Tracie Sucharski - Added NumValidMeasures and
   *                           NumIgnoredMeasures methods.
   *   @history 2009-06-03 Christopher Austin - Added p_invalid functionality
   *            along with forceBuild, as well as other small fixes including
   *            documentation.
   *   @history 2009-07-13 Stacy Alley - The std::vector of
   *            ControlPoints called 'p_points' was replaced with
   *            a QVector of QString 'p_pointIds' in conjunction
   *            with a QHash of <QString, ControlPoint> called
   *            'p_pointsHash'. This was done to speed up the Add
   *            method which was essentially slowing down the
   *            reading or creation of Control Networks.
   *   @history 2009-09-01 Eric Hyer - Added two includes: QVector and QString
   *   @history 2009-09-25 Travis Addair - Changed methods
   *            which return the number of control measures in the
   *            network to compute those values at the time the
   *            method is called, not when the control network is
   *            first initialized
   *                              
   */
  class ControlNet {
    public:
      // Constuctors
      ControlNet ();
      ControlNet(const std::string &ptfile, Progress *progress=0, bool forceBuild=false );

      //! Destroy the control network
      ~ControlNet () {};

     /**
      * Enumeration defining network type 
      */ 
      enum NetworkType { Singleton,    //!<Singleton is a network that identifies unique points such as reseaus, tic-marks, etc
                         ImageToImage, //!<ImageToImage is a network used to tie two or more images together using line/sample coordinates only
                         ImageToGround //!<ImageToGround is a network used to tie one or more images (typically many) between each other and a target (e.g., Mars)
        };

      /** 
       * Set the type of network 
       *  
       * @param type This Control Network's type
       */
      void SetType (const NetworkType &type) { p_type = type; };

      //! Return the type of network
      NetworkType Type () const { return p_type; };

      /** 
       * Set the target name 
       *  
       * @param target The name of the target of this Control Network 
       */
      void SetTarget (const std::string &target) { p_targetName = target; };

      //! Return the target name
      std::string Target() const { return p_targetName; };

      /** 
       * Set the network id 
       *  
       * @param id The Id of this Control Network
       */
      void SetNetworkId(const std::string &id) { p_networkId = id; };

      //! Return the network id
      std::string NetworkId() const { return p_networkId; };

      /** 
       * Set the user name 
       *  
       * @param name The name of the user creating or modifying this Control Net
       */
      void SetUserName(const std::string &name) { p_userName = name; };

      //! Return the user name
      std::string UserName() const { return p_userName; };

      /** 
       * Set the description of the network 
       *  
       * @param desc The description of this Control Network
       */
      void SetDescription(const std::string &desc) { p_description = desc; };

      /** 
       * Return the description of the network 
       *  
       * @return The description of this Control Network 
       */
      std::string Description() const { return p_description; };

      /** 
       * Set the creation time 
       *  
       * @param date The date this Control Network was created 
       */
      void SetCreatedDate(const std::string &date) { p_created = date; };

      /** 
       * Set the last modified date 
       * 
       * @param date The last date this Control Network was modified
       */
      void SetModifiedDate(const std::string &date) { p_modified = date; };

      /** 
       * Return the ith control point 
       *  
       * @param index Control Point index 
       *  
       * @return The Control Point at the provided index
       */
      ControlPoint &operator[](int index) { return p_pointsHash[p_pointIds[index]]; };

      //! Return the number of control points in the network
      int Size() const { return p_pointsHash.size(); };
      int NumValidPoints ();

      //! Return if the control point is invalid
      bool Invalid() const { return p_invalid; }

      int NumMeasures();

      int NumValidMeasures();

      int NumIgnoredMeasures();

      void Add (const ControlPoint &point, bool forceBuild=false);
      void Delete (int index);
      void Delete (const std::string &id);

      void ReadControl(const std::string &ptfile, Progress *progress=0, bool forceBuild=false);
      void Write(const std::string &ptfile);

      ControlPoint *Find(const std::string &id);

      ControlPoint *FindClosest(const std::string &serialNumber,
                                    double sample, double line);

      bool Exists( ControlPoint &point );

      double AverageError();
      double MaximumError();

      void ComputeErrors();
      void ComputeApriori();

      void SetImages (const std::string &imageListFile);
      void SetImages (SerialNumberList &list, Progress *progress=0);

      /**
       * Returns the camera list from the given image number
       * 
       * @param index The image number
       * 
       * @return Isis::Camera* The pointer to the resultant camera list
       */
      Isis::Camera *Camera(int index) { return p_cameraList[index]; };

    private:
      QVector<QString> p_pointIds;  //!< QVector of ControlPoint Ids
      QHash <QString, ControlPoint> p_pointsHash; //!< Hash table of Control Points.
      std::string p_targetName;            //!< Name of the target
      std::string p_networkId;             //!< The Network Id
      std::string p_created;               //!< Creation Date
      std::string p_modified;              //!< Date Last Modified
      std::string p_description;           //!< Textual Description of network
      std::string p_userName;              //!< The user who created the network
      NetworkType p_type;                  //!< The type of network being used
      int p_numIgnoredMeasures;            //!< Number of ignored measures
      std::map<std::string,Isis::Camera *> p_cameraMap;    //!< A map from serialnumber to camera
      std::vector<Isis::Camera *> p_cameraList;            //!< Vector of image number to camera

      bool p_invalid;  //!< If the Control Network is currently invalid

  };
};

#endif

