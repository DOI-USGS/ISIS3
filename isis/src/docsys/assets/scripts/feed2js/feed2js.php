<?php
/*  Feed2JS : RSS feed to JavaScript src file

	VERSION 1.93 (2007 nov 13)
	
	ABOUT
	This PHP script will take an RSS feed as a value of src="...."
	and return a JavaScript file that can be linked 
	remotely from any other web page. Output includes
	site title, link, and description as well as item site, link, and
	description with these outouts contolled by extra parameters.
	
	Developed by Alan Levine initially released 13.may.2004
	http://cogdogblog.com/
	
	PRIMARY SITE:
	http://feed2js.org/
	 
	USAGE:
	See http://eduforge.org/projects/feed2js/
     
	Feed2JS makes use of the Magpie RSS parser from
	 http://magpierss.sourceforge.net/
	
   ------------- small print ---------------------------------------
	GNU General Public License 
	Copyright (C) 2004-2005 Alan Levine
	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details
	http://www.gnu.org/licenses/gpl.html
	------------- small print ---------------------------------------

*/

// ERROR CHECKING FOR NO SOURCE -------------------------------

$script_msg = '';
$src = (isset($_GET['src'])) ? $_GET['src'] : '';

// trap for missing src param for the feed, use a dummy one so it gets displayed.
if (!$src or strpos($src, 'http://')!=0) $src=  'http://' . $_SERVER['SERVER_NAME'] . dirname($_SERVER['PHP_SELF']) . '/nosource.rss';

// test for malicious use of script tages
if (strpos($src, '<script>')) {
	$src = preg_replace("/(\<script)(.*?)(script>)/si", "SCRIPT DELETED", "$src");
	die("Warning! Attempt to inject javascript detected. Aborted and tracking log updated.");
}


// MAGPIE  SETUP ----------------------------------------------------
// access configuration settings
require_once('feed2js_config.php');

//  check for utf encoding type
$utf = (isset($_GET['utf'])) ? $_GET['utf'] : 'n';

if ($utf == 'y') {
	define('MAGPIE_CACHE_DIR', MAGPIE_DIR . 'cache_utf8/');
	// chacrater encoding
	define('MAGPIE_OUTPUT_ENCODING', 'UTF-8');

} else {
	define('MAGPIE_CACHE_DIR', MAGPIE_DIR . 'cache/');
}

// GET VARIABLES ---------------------------------------------
// retrieve values from posted variables

// flag to show channel info
$chan = (isset($_GET['chan'])) ? $_GET['chan'] : 'n';

// variable to limit number of displayed items; default = 0 (show all, 100 is a safe bet to list a big list of feeds)

$num = (isset($_GET['num'])) ? $_GET['num'] : 0;
if ($num==0) $num = 100;

// indicator to show item description,  0 = no; 1=all; n>1 = characters to display
// values of -1 indicate to displa item without the title as a link
// (default=0)
$desc = (isset($_GET['desc'])) ? $_GET['desc'] : 0;

// flag to show date of posts, values: no/yes (default=no)
$date = (isset($_GET['date'])) ? $_GET['date'] : 'n';

// time zone offset for making local time, 
// e.g. +7, =-10.5; 'feed' = print the time string in the RSS w/o conversion
$tz = (isset($_GET['tz'])) ? $_GET['tz'] : 'feed';


// flag to open target window in new window; n = same window, y = new window,
// other = targeted window, 'popup' = call JavaScript function popupfeed() to display
// in new window (default is n)

$targ = (isset($_GET['targ'])) ? $_GET['targ'] : 'n';
if ($targ == 'n') {
	$target_window = ' target="_self"';
} elseif ($targ == 'y' ) {
	$target_window = ' target="_blank"';
} elseif ($targ == 'popup') {
	$target_window = ' onClick="popupfeed(this.href);return false"';
} else {
	$target_window = ' target="' . $targ . '"';
}

// flag to show feed as full html output rather than JavaScript, used for alternative
// views for JavaScript-less users. 
//     y = display html only for non js browsers (NO LONGER USED)
//     n = default (JavaScript view)
//     a = display javascript output but allow HTML 
//     p  = display text only items but convert linefeeds to BR tags

// default setting for no conversion of linebreaks
$html = (isset($_GET['html'])) ? $_GET['html'] : 'n';

$br = ' ';
if ($html == 'a') {
	$desc = 1;
} elseif ($html == 'p') {
	$br = '<br />';
}

// optional parameter to use different class for the CSS container
$rss_box_id = (isset($_GET['css'])) ? '-' . $_GET['css'] : '';

// optional parameter to use different class for the CSS container
$play_podcast = (isset($_GET['pc'])) ? $_GET['pc'] : 'n';


// PARSE FEED and GENERATE OUTPUT -------------------------------
// This is where it all happens!

$rss = @fetch_rss( $src );

// begin javascript output string for channel info
$str= "document.write('<div class=\"rss-box" . $rss_box_id . "\">');\n";


// no feed found by magpie, return error statement
if  (!$rss) {
	$str.= "document.write('<p class=\"rss-item\">$script_msg<em>Error:</em> Feed failed! Causes may be (1) No data  found for RSS feed $src; (2) There are no items are available for this feed; (3) The RSS feed does not validate.<br /><br /> Please verify that the URL <a href=\"$src\">$src</a> works first in your browser and that the feed passes a <a href=\"http://feedvalidator.org/check.cgi?url=" . urlencode($src) . "\">validator test</a>.</p></div>');\n";


} else {


	// Create CONNECTION CONFIRM
	// create output string for local javascript variable to let 
	// browser know that the server has been contacted
	$feedcheck_str = "feed2js_ck = true;\n\n";

	// we have a feed, so let's process
	if ($chan == 'y') {
	
		// output channel title and description	
		$str.= "document.write('<p class=\"rss-title\"><a class=\"rss-title\" href=\"" . trim($rss->channel['link']) . '"' . $target_window . ">" . addslashes(strip_returns($rss->channel['title'])) . "</a><br /><span class=\"rss-item\">" . addslashes(strip_returns(strip_tags($rss->channel['description']))) . "</span></p>');\n";
	
	} elseif ($chan == 'title') {
		// output title only
		$str.= "document.write('<p class=\"rss-title\"><a class=\"rss-title\" href=\"" . trim($rss->channel['link']) . '"' . $target_window . ">" . addslashes(strip_returns($rss->channel['title'])) . "</a></p>');\n";
	
	}	
	
	// begin item listing
	$str.= "document.write('<ul class=\"rss-items\">');\n";
		
	// Walk the items and process each one
	$all_items = array_slice($rss->items, 0, $num);
	
	foreach ( $all_items as $item ) {
		
		// set defaults thanks RPFK
		if (!isset($item['summary'])) $item['summary'] = ''; 
		$more_link = '';
		
		if ($item['link']) {
			// link url
			$my_url = addslashes($item['link']);
		} elseif  ($item['guid']) {
			//  feeds lacking item -> link
			$my_url = ($item['guid']);
		}
		
		
		if ($desc < 0) {
			$str.= "document.write('<li class=\"rss-item\">');\n";
			
		} elseif ($item['title']) {
			// format item title
			$my_title = addslashes(strip_returns($item['title']));
						
			// create a title attribute. thanks Seb!
			$title_str = substr(addslashes(strip_returns(strip_tags((htmlspecialchars($item['summary']))))), 0, 255) . '...'; 

			// write the title strng
			$str.= "document.write('<li class=\"rss-item\"><a class=\"rss-item\" href=\"" . trim($my_url) . "\" title=\"$title_str\"". $target_window . '>' . $my_title . "</a><br />');\n";

		} else {
			// if no title, build a link to tag on the description
			$str.= "document.write('<li class=\"rss-item\">');\n";
			$more_link = " <a class=\"rss-item\" href=\"" . trim($my_url) . '"' . $target_window . ">&laquo;details&raquo;</a>";
		}
	
		// print out date if option indicated

		if ($date == 'y') {
					
			if ($tz == 'feed') {
			//   echo the date/time stamp reported in the feed

				if ($item['pubdate'] != '') {
					// RSS 2.0 is alreayd formatted, so just use it
					$pretty_date = $item['pubdate'];
				} elseif ($item['published'] != "") {
					// ATOM 1.0 format, remove the "T" and "Z" and the time zone offset
					$pretty_date = str_replace("T", " ", $item['published']);
					$pretty_date= str_replace("Z", " ", $pretty_date);
	
				} elseif ($item['issued'] != "") {
					// ATOM 0.3 format, remove the "T" and "Z" and the time zone offset
					$pretty_date = str_replace("T", " ", $item['issued']);
					$pretty_date= str_replace("Z", " ", $pretty_date);
				} elseif ( $item['dc']['date'] != "") {
					// RSS 1.0, remove the "T" and the time zone offset
					$pretty_date = str_replace("T", " ", $item['dc']['date']);
					$pretty_date = substr($pretty_date, 0,-6);
				} else {
				
					// no time/date stamp, 
					$pretty_date =  'n/a';
				}

			} else {
				// convert to local time via conversion to GMT + offset
				
				// adjust local server time to GMT and then adjust time according to user
				// entered offset.
				
				$pretty_date = date($date_format, $item['date_timestamp'] - $tz_offset + $tz * 3600);
			
			}
	
			$str.= "document.write('<span class=\"rss-date\">$pretty_date</span><br />');\n"; 
		}

		// link to podcast media if availavle
		
		if ($play_podcast == 'y' and is_array($item['enclosure'])) {
			$str.= "document.write('<div class=\"pod-play-box\">');\n";
			for ($i = 0; $i < count($item['enclosure']); $i++) {
			
				// display only if enclosure is a valid URL
				//if (strpos($item['enclosure'][$i]['url'], 'http://')!=0) {
					$str.= "document.write('<a class=\"pod-play\" href=\"" . trim($item['enclosure'][$i]['url']) . "\" title=\"Play Now\" target=\"_blank\"><em>Play</em> <span> " .  substr(trim($item['enclosure'][$i]['url']), -3)  . "</span></a> ');\n";
				//}
			
			}
			
			$str.= "document.write('</div>');\n";
		
		}

	
		// output description of item if desired
		if ($desc) {
		
		 // Atom/encocded content support (thanks David Carter-Tod)

			if ($item['content']['encoded']) {
				$my_blurb = html_entity_decode ( $item['content']['encoded'], ENT_NOQUOTES);
				
	
			} else {   
				$my_blurb = $item['summary'];
			}
			
			// strip html
			if ($html != 'a') $my_blurb = strip_tags($my_blurb);
			
			// trim descriptions
			if ($desc > 1) {
			
				// display specified substring numbers of chars;
				//   html is stripped to prevent cut off tags
				$my_blurb = substr($my_blurb, 0, $desc) . '...';
			}
	
		
			$str.= "document.write('" . addslashes(strip_returns($my_blurb, $br)) . "');\n"; 
			
		}
			
		$str.= "document.write('$more_link</li>');\n";	
	}


	$str .= "document.write('</ul></div>');\n";

}

// Render as JavaScript
// START OUTPUT
// headers to tell browser this is a JS file
if ($rss) header("Content-type: application/x-javascript"); 

// Spit out the results as the series of JS statements
echo $feedcheck_str . $str;


?>
