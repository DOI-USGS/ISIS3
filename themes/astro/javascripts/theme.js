 if (window.jQuery) { 
  $(document).ready(function(){
    if (window.devicePixelRatio > 1) {
      var images = findImagesByRegexp('contacts_thumbnail', document);

      for(var i = 0; i < images.length; i++) {
        var lowres = images[i].src;
        old_size = lowres.match(/\/(\d*)$/)[1]
        var highres = lowres.replace(/\/(\d*)$/, "/" + String(old_size*2));
        images[i].src = highres;
      }

      var images = findImagesByRegexp('gravatar.com/avatar', document);

      for(var i = 0; i < images.length; i++) {
        var lowres = images[i].src;
        old_size = lowres.match(/size=(\d+)x(\d+)/)[1]
        var highres = lowres.replace(/size=(\d+)x(\d+)/, "size=" + String(old_size*2));
        images[i].src = highres;
      }    
    }
    $('#q').attr('placeholder','Search');
    loadFooter();
    $('#header').append('<a href="http://astrogeology.usgs.gov"><div id="astrolink"> </div></a>');
    $('.login-without-cas').hide();
  });
} else {
  document.observe("dom:loaded", function() {
    if (window.devicePixelRatio > 1) {
      var images = findImagesByRegexp('thumbnail', document);

      for(var i = 0; i < images.length; i++) {
        var lowres = images[i].src;
        old_size = lowres.match(/size=(\d*)$/)[1]
        var highres = lowres.replace(/size=(\d*)$/, "size=" + String(old_size*2));
        images[i].src = highres;
      }

      var images = findImagesByRegexp('gravatar.com/avatar', document);

      for(var i = 0; i < images.length; i++) {
        var lowres = images[i].src;
        old_size = lowres.match(/size=(\d+)x(\d+)/)[1]
        var highres = lowres.replace(/size=(\d+)x(\d+)/, "size=" + String(old_size*2));
        images[i].src = highres;
      }    
    }

  });
}

function findImagesByRegexp(regexp, parentNode) {
   var images = Array.prototype.slice.call((parentNode || document).getElementsByTagName('img'));
   var length = images.length;
   var ret = [];
   for(var i = 0; i < length; ++i) {
      if(images[i].src.search(regexp) != -1) {
         ret.push(images[i]);
      }
   }
   return ret;
};


function loadFooter() {

var footerHTML = '<div class="icons projects black scroll-wrapper">' +
        '<div class="scroll">' +
	'<a title="Integrated Software for Imagers and Spectrometers" href="http://isis.astrogeology.usgs.gov" class="icon">' +
	'<img alt="ISIS Logo" height="112" width="112" src="themes/astro/images/logos/isis_2x.jpg"/>' +
	'<span class="label">ISIS</span>' +
	'</a>' +
	'<a title="Gazetteer of Planetary Nomenclature" href="http://planetarynames.wr.usgs.gov" class="icon">' +
	'<img alt="Nomenclature Logo" height="112" width="112" src="themes/astro/images/logos/nomenclature_2x.jpg"/>' +
	'<span class="label">Planetary Nomenclature</span>' +
	'</a>' +
	'<a title="Map-a-Planet" href="http://www.mapaplanet.org" class="icon">' +
	'<img alt="Map-a-Planet Logo" height="112" width="112" src="themes/astro/images/logos/map_a_planet_2x.jpg"/>' +
	'<span class="label">Map-a-Planet</span>' +
	'</a>' +
	'<a title="PDS Imaging Node" href="http://astrogeology.usgs.gov/facilities/imaging-node-of-nasa-planetary-data-system-pds" class="icon">' +
	'<img alt="PDS Logo" height="112" src="themes/astro/images/logos/pds_logo-black-web.png"/>' +
	'<span class="label">PDS Imaging Node</span>' +
	'</a>' +
	'<a title="Regional Planetary Image Facility" href="http://astrogeology.usgs.gov/rpif" class="icon">' +
	'<img alt="RPIF Logo" height="112" width="112" src="themes/astro/images/logos/rpif_2x.jpg"/>' +
	'<span class="label">RPIF</span>' +
	'</a>' +
	'<a title="Photogrammetry Guest Facility" href="http://astrogeology.usgs.gov/facilities/photogrammetry-guest-facility" class="icon">' +
	'<img alt="Photogrammetry Guest Faciltiy Logo" height="112" width="112" src="themes/astro/images/logos/photogrammetry_2x.jpg"/>' +
	'<span class="label">Photogrammetry Guest Facility</span>' +
	'</a>' +
	'<a title="Planetary Image Locator Tool" href="http://pilot.wr.usgs.gov" class="icon">' +
	'<img alt="Pilot Logo" height="112" width="112" src="themes/astro/images/logos/pilot_2x.jpg"/>' +
	'<span class="label">PILOT</span>' +
	'</a>' +
	'<a title="Mapping, Remote-sensing, Cartography, Technology and Research GIS Lab" href="http://astrogeology.usgs.gov/facilities/mrctr" class="icon">' +
	'<img alt="MRCTR GIS Lab Logo" height="112" width="112" src="themes/astro/images/logos/mrctr_2x.jpg"/>' +
	'<span class="label">MRCTR GIS Lab</span>' +
	'</a>' +
   '</div></div>';

$('#footer').before(footerHTML);


};
