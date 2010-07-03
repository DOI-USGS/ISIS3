function popupfeed(url) {
/*
	Use this function to generate a pop up window for item links generated
	by the Feed2JS service. The name of this function must exactly be
			popupfeed
	
	and you can use the code below to specify specific window features
	See http://jade.mcli.dist.maricopa.edu/feed/index.php?s=mod
*/

	// string to specify window features

	var myfeatures = "toolbar=no,location=no,directories=no,menubar=no,scrollbars=yes,status=yes,resizable=no,width=800,height=400";
	
	thefeed = window.open( url, 'feed2jspop', myfeatures);
	if (window.focus) {thefeed.focus()}
}
