/***********************************
 * Script:  homepage.js
 * Purpose: write little HTML snippets on Isis homepage,
 *          avoiding XML/XSLT weirdness
 * Author:  Deborah Lee Soltesz
 * Date:    2006-11-13
 *
 ***********************************/

function writeAnnouncements() {
    isWeb = (document.URL.lastIndexOf("isis.astrogeology.usgs.gov") > 0) ||
            (document.URL.lastIndexOf("isis-dev.wr.usgs.gov") > 0) ||
            (document.URL.lastIndexOf("blackflag") > 0) ;
    if (isWeb) {
      document.write (
                     "<div><script language=\"JavaScript\" type=\"text/javascript\" src=\"http://isis.astrogeology.usgs.gov/IsisSupport/topics_anywhere.php?mode=show&f=1&n=3&a=y&t=_top&rt=n&so=d&b=dis&lpd=3&af=p3R0pyCnbHB0pw%3D%3D&l=y\"></script></div>"   );
    } else {
      document.write ( "Visit the <a href=\"http://isis.astrogeology.usgs.gov/IsisSupport/viewforum.php?f=1\" target=\"_top\">Isis Support Center</a> for the latest news and announcements") ;
    }
    if (isWeb) {
      document.write (
                     "<p class=\"caption\" style=\"text-align:right;\">"  +
                     "  <a href=\"http://isis.astrogeology.usgs.gov/IsisSupport/viewforum.php?f=1\" target=\"_top\">"  +
                     "    More...</a>"  +
                     "</p>" ) ;
    }
}

function writeWhatsUpAtTheSupportCenter () {
    isWeb = (document.URL.lastIndexOf("isis.astrogeology.usgs.gov") > 0) ||
            (document.URL.lastIndexOf("isis-dev.wr.usgs.gov") > 0) ||
            (document.URL.lastIndexOf("blackflag") > 0) ;
    if (isWeb) {
      document.write (
                     "<!- - Isis Support - ->                                                                                                                                                                                                                                                                 " +
                     "<h2>What's Up at the Isis Support Center?</h2>                                                                                                                                                                                                                                        " +
                     "<div style=\"text-align:center;\">                                                                                                                                                                                                                                                    " +
                     "<div style=\"padding:10px; text-align: left;\">                                                                                                                                                                                                                                       "
                     );
    }
    if (isWeb) {
      document.write (
                     "<script language=\"JavaScript\" type=\"text/javascript\" src=\"http://isis.astrogeology.usgs.gov/IsisSupport/topics_anywhere.php?mode=show&f=a&n=10&jlp=y&ct=caption&sfn=y&fnl=y&r=y&a=y&s=y&l=y&h=mpl&so=d&b=lpi&lpb=0&lpd=3&lpi=y&bl=y&ct=caption&t=_top\"></script>"   );
    }
    if (isWeb) {
      document.write (
                     "<p class=\"caption\" style=\"text-align:right;\">                                                                                                                                                                                                                                                       " +
                     "  <a href=\"http://isis.astrogeology.usgs.gov/IsisSupport\" target=\"_top\">                                                                                                                                                                                                          " +
                     "    More...</a>                                                                                                                                                                                                                                                                       " +
                     "</p>                                                                                                                                                                                                                                                                                  " +
                     "</div>                                                                                                                                                                                                                                                                                " +
                     "</div>                                                                                                                                                                                                                                                                                "
                     );
    }
}
