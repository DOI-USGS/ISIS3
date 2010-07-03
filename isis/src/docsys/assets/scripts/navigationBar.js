
  // SCRIPT:   Navigation Bar
  // Filename: navigationBar.js
  // Purpose:  behavior for the horizontal navigation bar
  // Author:   Deborah Lee Soltesz, USGS, 10/2001
  // History:  12/2005 dls implemented writing of navbar in this script
  //                       to allow changing the menu in one place for
  //                       the entire site



    navWidth    = 600 ;
    navHeight   = 19  ;
    navSubImgName  = "bsubtopics" ;

    site = "http://astrogeology.usgs.gov" ;

    // array indices
    navFilenameCool = 0 ;
    navFilenameHot  = 1 ;
    navImgName      = 2 ;
    navSubFilename  = 3 ;
    navMap          = 4 ;

    imageNumChoice = 0 ; // current image highlighted

    defaultBar = 0 ;     // the bar to be displayed when none is highlighted

    // customize: edit path
    navBaseURL  = "/assets/navigation/menubar/" ;

    // customize: name of blank submenu bars
    navSubBlank = "blank_menu_bar.gif" ;
    navSubBase  = "starfield_menu_bar.gif" ;

    Timeout = 5000 ;
    TimeoutID = 0 ;

    // customize: list images to be used in the script
    navArr = new Array (
        new Array ("", "", ""), // no 0
        new Array ("solarsystem_menu_button.gif", "solarsystem_menu_button_hot.gif", "bsolarsystem", "solarsystem_menu_bar.gif" , "SolarSystemMap"),
        new Array ("missions_menu_button.gif",    "missions_menu_button_hot.gif",    "bmissions"   , "missions_menu_bar.gif"    , "MissionsMap"   ),
        new Array ("technology_menu_button.gif",  "technology_menu_button_hot.gif",  "btechnology" , "technology_menu_bar.gif"  , "TechnologyMap" ),
        new Array ("datainfo_menu_button.gif",    "datainfo_menu_button_hot.gif",    "bdatainfo"   , "datainfo_menu_bar.gif"    , "DataAndInformationMap"),
        new Array ("research_menu_button.gif",    "research_menu_button_hot.gif",    "bresearch"   , "research_menu_bar.gif"    , ""),
        new Array ("hottopics_menu_button.gif",   "hottopics_menu_button_hot.gif",   "bhottopics"  , navSubBlank                , ""),
        new Array ("gallery_menu_button.gif",     "gallery_menu_button_hot.gif",     "bgallery"    , "gallery_menu_bar.gif"     , ""),
        new Array ("about_menu_button.gif",       "about_menu_button_hot.gif",       "babout"      , "about_menu_bar.gif"       , ""),
        new Array ("search_menu_button.gif",      "search_menu_button_hot.gif",      "bsearch"     , navSubBlank                , ""),
        new Array ("kidszone_menu_button.gif",    "kidszone_menu_button_hot.gif",    "bkids"       , navSubBlank                , "")
    ) ;
    numNavImages = navArr.length ;

    // preloading images needs to be looked into -- it seems to clear up
    // some problems in Netscape4.7, but there's still a lag on changing
    // images on mouseover the first time in all browsers, suggesting the
    // preload may not be working?
    preloadImage    = new Array (numNavImages) ;
    preloadImageSub = new Array (numNavImages) ;
    for (loop = 1 ; loop < numNavImages ; loop++) {
      preloadImage[loop]    = new Image () ;
      preloadImageSub[loop] = new Image () ;
      preloadImage[loop].src    = site + navBaseURL + navArr [loop][navFilenameHot] ;
      preloadImageSub[loop].src = site + navBaseURL + navArr [loop][navSubFilename] ;
    }

    // customize: list links for altering submenu image map
    //            Up to 12 links, array corresponds to same order above
    //            First element is path to subtopics
    navLinkArr = new Array (
        new Array ("",                     "/", "/", "/", "/", "/", "/", "/", "/", "/", "/", "/", "/"), // no 0
        new Array ("/SolarSystem/",        "Sun/", "Mercury/", "Venus/", "Earth/", "Mars/", "Jupiter/", "Saturn/", "Uranus/", "Neptune/", "Pluto/", "OtherObjects/", "Beyond/"),
        new Array ("/Missions/",           "Cassini/", "Clementine/", "Galileo/", "LunarOrbiter/", "LunarOrbiter/", "Magellan/", "Mariner/", "MarsPathfinder/", "MarsGlobalSurveyor/", "Viking/", "Voyager/", "More/"),
        new Array ("/Technology/",         "Software/", "Software/", "ImageProcessing/", "ImageProcessing/", "LabsAndFacilities/", "LabsAndFacilities/", "OtherTechnology/", "OtherTechnology/", "TechnicalInformationLinks/", "TechnicalInformationLinks/", "TechnicalInformationLinks/", "TechnicalInformationLinks/"),
        new Array ("/DataAndInformation/", "Databases/", "Databases/", "Databases/", "Databases/", "Databases/", "Databases/", "ImagesAndMaps/", "ImagesAndMaps/", "ImagesAndMaps/", "ImagesAndMaps/", "ImagesAndMaps/", "ImagesAndMaps/"),
        new Array ("/Research/",           "Geology/", "Monitoring/", "Monitoring/", "RemoteSensing/", "RemoteSensing/", "IceAndPolar/", "IceAndPolar/", "InTheLab/", "OtherResearch/", "OtherResearch/", "Organizations/", "Organizations/"),
        new Array ("/HotTopics/",          "", "", "", "", "", "", "", "", "", "", "", ""),
        new Array ("/Gallery/",            "ImageGallery/", "ImageGallery/", "DesktopWallpaper/", "DesktopWallpaper/", "DesktopWallpaper/", "MoviesAndAnimations/", "MoviesAndAnimations/", "MoviesAndAnimations/", "Posters/", "Posters/", "ScreenSavers/", "ScreenSavers/"),
        new Array ("/About/",              "AstroHistory/", "AstroToday/", "AstroFuture/", "People/", "Teams/", "Contact/", "Contact/", "Visitors/", "Visitors/", "Careers/", "Crediting/", "Crediting/"),
        new Array ("/Search/",             "", "", "", "", "", "", "", "", "", "", "", ""),
        new Array ("/Kids/",               "", "", "", "", "", "", "", "", "", "", "", "")
    ) ;
    numLinks = navLinkArr.length ;

    // customize: list corresponding alt/title tags for submenu image map
    navLinkAltArr = new Array (
        new Array ("", "", "", "", "", "", "", "", "", "", "", "", ""),  // no 0
        new Array ("", "Sun", "Mercury", "Venus", "Earth", "Mars", "Jupiter", "Saturn", "Uranus", "Neptune", "Pluto", "Other Objects", "Beyond the Solar System"),
        new Array ("", "Cassini", "Clementine", "Galileo", "Lunar Orbiter", "Lunar Orbiter", "Magellan", "Mariner", "Mars Pathfinder", "Mars Global Surveyor", "Viking", "Voyager", "More Missions"),
        new Array ("", "Software", "Software", "Image Processing", "Image Processing", "Labs and Facilities", "Labs and Facilities", "Other Technology", "Other Technology", "Technical Information Links", "Technical Information Links", "Technical Information Links", "TechnicalInformationLinks"),
        new Array ("", "", "", "", "", "Databases", "Databases", "ImagesAndMaps", "ImagesAndMaps", "", "", "", ""),
        new Array ("", "Geology", "Monitoring", "Monitoring", "Remote Sensing", "Remote Sensing", "Ice and Polar", "Ice and Polar", "In the Lab", "Other Research", "Other Research", "Organizations", "Organizations"),
        new Array ("", "", "", "", "", "", "", "", "", "", "", "", ""),
        new Array ("", "Image Gallery", "Image Gallery", "Desktop Wallpaper", "Desktop Wallpaper", "Desktop Wallpaper", "Movies and Animations", "Movies and Animations", "Movies and Animations", "Posters", "Posters", "Screen Savers", "Screen Savers"),
        new Array ("", "History", "Now", "Future", "People", "Teams", "Contact Us", "Contact Us", "Visitor Information", "Visitors Information", "Careers", "Using Our Images", "Using Our Images"),
        new Array ("", "", "", "", "", "", "", "", "", "", "", "", ""),
        new Array ("", "", "", "", "", "", "", "", "", "", "", "", "")
    ) ;

    // 'enumed' types for referring to each topic corresponding to arrays
    noBar        = 0 ;
    solarBar     = 1 ;
    missionBar   = 2 ;
    techBar      = 3 ;
    datainfoBar  = 4 ;
    researchBar  = 5 ;
    hottopicsBar = 6 ;
    galleryBar   = 7 ;
    aboutBar     = 8 ;
    searchBar    = 9 ;
    kidsBar      = 10 ;

    gonnaReheat  = 0 ; // lets cool function know it's been called from the heat function

    // writes the image width
    function writeNavWidth () {
      document.write(navWidth) ;
    }

    // writes the image height
    function writeNavHeight () {
      document.write(navHeight) ;
    }

    // write out the path and filename to the chosen image
    function writeNavPathAndFilename (imageNum) {
      document.write(site + navBaseURL + navArr[imageNum][navFilename]) ;
    }

    // change specified image tag named 'imgName' to chosen image
    function heatNavButton (imageNum) {
      gonnaReheat = 1 ;
      coolAllNavButtons () ;
      window.clearTimeout(TimeoutID) ;
      gonnaReheat = 0 ;
      setImageNum (imageNum) ;
      document.images[navArr[imageNum][navImgName]].src = site + navBaseURL + navArr[imageNum][navFilenameHot] ;
      document.images[navSubImgName].src = site + navBaseURL + navArr[imageNum][navSubFilename] ;
      TimeoutID = window.setTimeout("coolAllNavButtons()", Timeout);
    }


    // functions for changing the image map area hrefs and titles
    function setImageNum (imageNum) {
      imageNumChoice = imageNum ;
    }

    // set which is the default bar to highlight for the page
    function setDefaultBarTo (imgNum) {
      defaultBar = imgNum ;
    }

    function getMapAreaHref (i) {
      return site + navLinkArr[imageNumChoice][0] + navLinkArr[imageNumChoice][i] ;
    }

    function getMapAreaAlt (i) {
      return navLinkAltArr[imageNumChoice][i] ;
    }



    // change specified image tag named 'imgName' to cool state
    function coolNavButton (imageNum) {
      document.images[navArr[imageNum][navImgName]].src = site + navBaseURL + navArr[imageNum][navFilenameCool] ;
      document.images[navSubImgName].src = site + navBaseURL + navSubBase ;
    }

    // change all buttons to cool state
    function coolAllNavButtons () {
      document.images[navSubImgName].src = site + navBaseURL + navSubBase ;
      for (i = 1 ; i < numNavImages ; i++) {
        document.images[navArr[i][navImgName]].src = site + navBaseURL + navArr[i][navFilenameCool] ;
      }

      if (gonnaReheat != 1) {
        imageNumChoice = defaultBar ;
        if (imageNumChoice) {
          heatNavButton (imageNumChoice) ;
        }
      }
    }


    function writeNavigationBar () {
      document.write("       <table cellpadding=\"0\" cellspacing=\"0\" border=\"0\"  width=\"600\" bgcolor=\"white\">") ;
      document.write("         <tr valign=top align=left>") ;

      document.write("           <!-- Solar System -->") ;
      document.write("           <td>") ;
      document.write("             <a href=\"" + site + "/SolarSystem/\" onMouseOver=\"heatNavButton(1);\" accesskey=\"1\">") ;
      document.write("             <img src=\"" + site + "/assets/navigation/menubar/solarsystem_menu_button.gif\" alt=\"[Solar System]\" name=\"bsolarsystem\" width=\"70\" height=\"19\" border=0></a></td>") ;

      document.write("           <!-- Missions -->") ;
      document.write("           <td>") ;

      document.write("             <a href=\"" + site + "/Missions/\" onMouseOver=\"heatNavButton(2);\" accesskey=\"2\">") ;
      document.write("             <img src=\"" + site + "/assets/navigation/menubar/missions_menu_button.gif\" alt=\"[Missions]\" name=\"bmissions\" width=\"53\" height=\"19\" border=0></a></td>") ;

      document.write("           <!-- Technology -->") ;
      document.write("           <td>") ;
      document.write("             <a href=\"" + site + "/Technology/\" onMouseOver=\"heatNavButton(3);\" accesskey=\"3\">") ;
      document.write("             <img src=\"" + site + "/assets/navigation/menubar/technology_menu_button.gif\" alt=\"[Technology]\" name=\"btechnology\" width=\"66\" height=\"19\" border=0 hspace=0 vspace=0></a></td>") ;

      document.write("           <!-- Data and Information -->") ;
      document.write("           <td>") ;

      document.write("             <a href=\"" + site + "/DataAndInformation/\" target=_top onMouseOver=\"heatNavButton(4);\" accesskey=\"4\">") ;
      document.write("             <img src=\"" + site + "/assets/navigation/menubar/datainfo_menu_button.gif\" alt=\"[Data &amp; Information]\" name=\"bdatainfo\" width=\"102\" height=\"19\" border=0 hspace=0 vspace=0></a></td>") ;

      document.write("           <!-- Research -->") ;
      document.write("           <td>") ;
      document.write("             <a href=\"" + site + "/Research/\" target=_top onMouseOver=\"heatNavButton(5);\" accesskey=\"5\">") ;
      document.write("             <img src=\"" + site + "/assets/navigation/menubar/research_menu_button.gif\" alt=\"[Research]\" name=\"bresearch\" width=\"54\" height=\"19\" border=0 hspace=0 vspace=0></a></td>") ;

      document.write("           <!-- Hot Topics -->") ;
      document.write("           <td>") ;

      document.write("             <a href=\"" + site + "/HotTopics/\" target=_top onMouseOver=\"heatNavButton(6);\" accesskey=\"6\">") ;
      document.write("             <img src=\"" + site + "/assets/navigation/menubar/hottopics_menu_button.gif\" alt=\"[Hot Topics]\" name=\"bhottopics\" width=\"60\" height=\"19\" border=0 hspace=0 vspace=0></a></td>") ;

      document.write("           <!-- Gallery -->") ;
      document.write("           <td>") ;
      document.write("             <a href=\"" + site + "/Gallery/\" target=_top onMouseOver=\"heatNavButton(7);\" accesskey=\"7\">") ;
      document.write("             <img src=\"" + site + "/assets/navigation/menubar/gallery_menu_button.gif\" alt=\"[Gallery]\" name=\"bgallery\" width=\"46\" height=\"19\" border=0 hspace=0 vspace=0></a></td>") ;

      document.write("           <!-- About Us -->") ;
      document.write("           <td>") ;

      document.write("             <a href=\"" + site + "/About/\" target=_top onMouseOver=\"heatNavButton(8);\" accesskey=\"8\">") ;
      document.write("             <img src=\"" + site + "/assets/navigation/menubar/about_menu_button.gif\" alt=\"[About Us]\" name=\"babout\" width=\"53\" height=\"19\" border=0 hspace=0 vspace=0></a></td>") ;

      document.write("           <!-- Search -->") ;
      document.write("           <td>") ;
      document.write("             <a href=\"" + site + "/Search/\" target=_top onMouseOver=\"heatNavButton(9);\" accesskey=\"9\">") ;
      document.write("             <img src=\"" + site + "/assets/navigation/menubar/search_menu_button.gif\" alt=\"[Search]\" name=\"bsearch\" width=\"43\" height=\"19\" border=0 hspace=0 vspace=0></a></td>") ;

      document.write("           <!-- Kids' Zone -->") ;
      document.write("           <td>") ;

      document.write("             <a href=\"" + site + "/Kids/\" title=\"\" onMouseOver=\"heatNavButton(10);\" accesskey=\"0\">") ;
      document.write("             <img src=\"" + site + "/assets/navigation/menubar/kidszone_menu_button.gif\" alt=\"[Kids' Zone]\" name=\"bkids\" width=\"52\" height=\"19\" border=0></a></td>") ;
      document.write("         </tr>") ;

      document.write("         <tr>") ;
      document.write("           <td colspan=\"10\">") ;
      document.write("             <img src=\"" + site + "/assets/navigation/menubar/starfield_menu_bar.gif\" alt=\" [end navigation bar] \" name=\"bsubtopics\" width=600 height=21 border=0 usemap=\"#menumap\" hspace=\"0\" vspace=\"0\"></td>") ;
      document.write("         </tr>") ;

      document.write("       </table>") ;

      document.write(" <map name=\"menumap\">") ;

      document.write("   <area shape=\"rect\" coords=\"0,0,50,21\"    href=\"#top\" title=\"\" target=\"_top\" alt=\"\" name=\"link1\"  onmouseover=\"this.href=getMapAreaHref(1);  this.alt=getMapAreaAlt(1);  this.title=getMapAreaAlt(1); \">") ;
      document.write("   <area shape=\"rect\" coords=\"50,0,100,21\"  href=\"#top\" title=\"\" target=\"_top\" alt=\"\" name=\"link2\"  onmouseover=\"this.href=getMapAreaHref(2);  this.alt=getMapAreaAlt(2);  this.title=getMapAreaAlt(2); \">") ;
      document.write("   <area shape=\"rect\" coords=\"100,0,150,21\" href=\"#top\" title=\"\" target=\"_top\" alt=\"\" name=\"link3\"  onmouseover=\"this.href=getMapAreaHref(3);  this.alt=getMapAreaAlt(3);  this.title=getMapAreaAlt(3); \">") ;
      document.write("   <area shape=\"rect\" coords=\"150,0,200,21\" href=\"#top\" title=\"\" target=\"_top\" alt=\"\" name=\"link4\"  onmouseover=\"this.href=getMapAreaHref(4);  this.alt=getMapAreaAlt(4);  this.title=getMapAreaAlt(4); \">") ;
      document.write("   <area shape=\"rect\" coords=\"200,0,250,21\" href=\"#top\" title=\"\" target=\"_top\" alt=\"\" name=\"link5\"  onmouseover=\"this.href=getMapAreaHref(5);  this.alt=getMapAreaAlt(5);  this.title=getMapAreaAlt(5); \">") ;
      document.write("   <area shape=\"rect\" coords=\"250,0,300,21\" href=\"#top\" title=\"\" target=\"_top\" alt=\"\" name=\"link6\"  onmouseover=\"this.href=getMapAreaHref(6);  this.alt=getMapAreaAlt(6);  this.title=getMapAreaAlt(6); \">") ;
      document.write("   <area shape=\"rect\" coords=\"300,0,350,21\" href=\"#top\" title=\"\" target=\"_top\" alt=\"\" name=\"link7\"  onmouseover=\"this.href=getMapAreaHref(7);  this.alt=getMapAreaAlt(7);  this.title=getMapAreaAlt(7); \">") ;
      document.write("   <area shape=\"rect\" coords=\"350,0,400,21\" href=\"#top\" title=\"\" target=\"_top\" alt=\"\" name=\"link8\"  onmouseover=\"this.href=getMapAreaHref(8);  this.alt=getMapAreaAlt(8);  this.title=getMapAreaAlt(8); \">") ;
      document.write("   <area shape=\"rect\" coords=\"400,0,450,21\" href=\"#top\" title=\"\" target=\"_top\" alt=\"\" name=\"link9\"  onmouseover=\"this.href=getMapAreaHref(9);  this.alt=getMapAreaAlt(9);  this.title=getMapAreaAlt(9); \">") ;

      document.write("   <area shape=\"rect\" coords=\"450,0,500,21\" href=\"#top\" title=\"\" target=\"_top\" alt=\"\" name=\"link10\" onmouseover=\"this.href=getMapAreaHref(10); this.alt=getMapAreaAlt(10); this.title=getMapAreaAlt(10);\">") ;
      document.write("   <area shape=\"rect\" coords=\"500,0,550,21\" href=\"#top\" title=\"\" target=\"_top\" alt=\"\" name=\"link11\" onmouseover=\"this.href=getMapAreaHref(11); this.alt=getMapAreaAlt(11); this.title=getMapAreaAlt(11);\">") ;
      document.write("   <area shape=\"rect\" coords=\"550,0,600,21\" href=\"#top\" title=\"\" target=\"_top\" alt=\"\" name=\"link12\" onmouseover=\"this.href=getMapAreaHref(12); this.alt=getMapAreaAlt(12); this.title=getMapAreaAlt(12);\">") ;
      document.write("   <area shape=\"default\" nohref alt=\"\">") ;
      document.write(" </map>") ;
      document.write(" <!-- END MAIN TOPIC NAVIGATION -->") ;
    }

