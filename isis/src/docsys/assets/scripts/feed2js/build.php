<?php
/*  Feed2JS : RSS feed to JavaScript
	build.php
	
	ABOUT
	This script can be used to create a form that is useful
	for creating the JavaScript strings and testing the output
		
	Developed by Alan Levine
	http://cogdogblog.com/
	
	MORE:
	Part of the Feed2JS package
	See http://feed2js.org/

*/


	
// GET VARIABLES ---------------------------------------------
// Get variables from input form and set default values

	
	
	$src = (isset($_GET['src'])) ? $_GET['src'] : '';
	$chan = (isset($_GET['chan'])) ? $_GET['chan'] : 'y';
	$num = (isset($_GET['num'])) ? $_GET['num'] : 0;
	$desc = (isset($_GET['desc'])) ? $_GET['desc'] : 1;
	$date = (isset($_GET['date'])) ? $_GET['date'] : 'n';
	$tz = (isset($_GET['tz'])) ? $_GET['tz'] : 'feed';
	$targ = (isset($_GET['targ'])) ? $_GET['targ'] : 'n';
	$html = (isset($_GET['html'])) ? $_GET['html'] : 'n';
	$utf = (isset($_GET['utf'])) ? $_GET['utf'] : 'n';
	$rss_box_id = (isset($_GET['rss_box_id'])) ? $_GET['rss_box_id'] : '';
	$pc = (isset($_GET['pc'])) ? $_GET['pc'] : 'n';


// test for malicious use of script tages
if (strpos($src, '<script>')) {
	$src = preg_replace("/(\<script)(.*?)(script>)/si", "SCRIPT DELETED", "$src");
	die("Warning! Attempt to inject javascript detected. Aborted and tracking log updated.");
}
	

// check for status of submit buttons	
	$generate = (isset($_GET['generate'])) ? $_GET['generate'] : '';
	if (isset($generate)) $generate = $_GET['generate'];

// update to full descriptions for html turned on	
	if ($html=='a') $desc = 0;

// build parameter string for the feed2js url
	$options = '';	
	if ($chan != 'n') $options .= "&chan=$chan";
	if ($num != 0) $options .= "&num=$num";
	if ($desc != 0) $options .= "&desc=$desc";
	if ($date != 'n') $options .= "&date=$date";
	if ($tz != 'feed') $options .= "&tz=$tz";
	if ($targ != 'n') $options .= "&targ=$targ";
	if ($html != 'n') $html_options = "&html=$html";
	if ($utf == 'y') $options .= '&utf=y';
	if ($rss_box_id != '') $options .= "&css=$rss_box_id";
	if ($pc == 'y') $options .= '&pc=y';


	
if ($generate) {
	// URLs for a preview or a generated feed link
	
		$my_dir = 'http://' . $_SERVER['SERVER_NAME'] . dirname($_SERVER['PHP_SELF']);
		
		$rss_str = "$my_dir/feed2js.php?src=" . urlencode($src) . htmlentities($options . $html_options);

		$noscript_rss_str = "$my_dir/feed2js.php?src=" . urlencode($src) . htmlentities($options .  '&html=y');

}

?>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
        "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
	<title>Cut n' Paste JavaScript RSS Feed</title>
	<link rel="stylesheet" href="style/main.css" media="all" />
<script type="text/javascript" language="Javascript">
<!--
function query_str(form) {

	// builds a proper query string by extracting Javascript form variables
	// so we can open a preview in a new window
	options = encodeURIComponent(form.src.value);
		
	if (form.chan[2].checked) {
		options += '&chan=n';
	} else if (form.chan[1].checked) {
		options += '&chan=title';
	}
	
	if (form.num.value != 0) options += '&num=' + form.num.value;
	if (form.desc.value != 1 && !form.html[0].checked) options += '&desc=' + form.desc.value;

	if (form.date[0].checked) options += '&date=y';
	if (form.tz.value != 'feed') options += '&tz=' + form.tz.value;
	
	if (form.html[0].checked) {
		options += '&html=a';
	} else if (form.html[2].checked) {
		options += '&html=p';
	}


	options += '&targ=' + form.targ.value;
	
	if (form.utf.checked) options += '&utf=y';
	options += '&css=' + form.rss_box_id.value;
	
	if (form.pc[0].checked) options += '&pc=y';
	
	return(options);


}
//-->
</script>

	<script src="popup.js" type="text/javascript" language="Javascript">
</script>

</head>
<body>
<div id="content">
<h1>Feed2JS Build JavaScript and Preview</h1>
<p class="first">The tool below will help you format a feed's display with the information you want to use on your web site. All you need to enter is the URL for the RSS source, and select the desired options below. </p>

<p>First, be sure to <strong>preview</strong> the feed to verify the content and format. Once the content is displayed how you like, just use the <strong>generate javascript</strong> button to get your code. Once the content looks okay, move on to our <a href="style.php">style tool</a> to make it pretty.</p>

<?php if ($generate):?>


<h2>Get Your Code Here</h2>
<p class="first">Below is the code you need to copy and paste to your own web page to include this RSS feed. The NOSCRIPT tag provides a link to a HTML display of the feed for users who may not have JavaScript enabled. </p>
<form>
<span class="caption">cut and paste javascript:</span><br><textarea name="t" rows="8" cols="70">
&lt;script language="JavaScript" src="<?php echo htmlentities($rss_str)?>" type="text/javascript"&gt;&lt;/script&gt;

&lt;noscript&gt;
&lt;a href="<?php echo htmlentities($noscript_rss_str)?>"&gt;View RSS feed&lt;/a&gt;
&lt;/noscript&gt;
</textarea>
</form>


<?php endif?>

<form method="get" action="build.php"  name="builder">

<p><strong>URL</strong> Enter the web address of the RSS Feed (must be in http:// format, not feed://)<br>

<input type="text" name="src" size="50" value="<?php echo $src?>"> <br>
<span style="font-size:x-small">Note: Please verify the URL of your feed (make sure it presents raw RSS) and <a href="http://feedvalidator.org/" onClick="window.open('http://feedvalidator.org/check.cgi?url=' + encodeURIComponent(document.builder.src.value), 'check'); return false;">check that it is valid</a>  before using this form.</span>
</p>

<div id="badge" style="width:250px; padding:0;">
<h3 class="badge-header">Show n' Tell!</h3>
	<div align="center">
	<input type="button" name="preview" value="Preview Feed" onClick="pr=window.open('preview.php?src=' + query_str(document.builder), 'prev', 'scrollbars,resizable,left=20,screenX=20,top=40,screenY=40,height=580,width=700'); pr.focus();"
	/> <br />
	<input type="submit" name="generate" value="Generate JavaScript" />
	</div>
</div>


<p><strong>Show channel?</strong> (yes/no/title) Display information about the publisher of the feed (yes=show the title and description; title= display title only, no=do not display anything) <br>
<input type="radio" name="chan" value="y" <?php if ($chan=='y') echo 'checked="checked"'?> /> yes <input type="radio" name="chan" value="title" <?php if ($chan=='title') echo 'checked="checked"'?>/> title <input type="radio" name="chan" value="n" <?php if ($chan=='n') echo 'checked="checked"'?>/> no</p>

<p><strong>Number of items to display.</strong> Enter the number of items to be displayed (enter 0 to show all available)<br>
<input type="text" name="num" size="10" value="<?php echo $num?>"></p>

<p><strong>Show/Hide item descriptions? How much?</strong> (0=no descriptions; 1=show full description text; n>1 = display first n characters of description; n=-1 do not link item title, just display item contents)<br>
<input type="text" name="desc" size="10" value="<?php echo $desc?>"></p>

<p><strong>Use HTML in item display? </strong> ("yes" = use HTML from feed and the full item descriptions will be used, ignoring any character limit set above; "no" = output is text-only formatted by CSS; "preserve paragraphs" = no HTML but convert all RETURN/linefeeds to &lt;br&gt; to preserve paragraph breaks)<br>
<input type="radio" name="html" value="a" <?php if ($html=='a') echo 'checked="checked"'?>/> yes <input type="radio" name="html" value="n" <?php if ($html=='n') echo 'checked="checked"'?> /> no <input type="radio" name="html" value="p" <?php if ($html=='p') echo 'checked="checked"'?> /> preserve paragraphs only</p>

<p><strong>Show item posting date?</strong> (yes/no) Display the time and date for each item.<br>
<input type="radio" name="date" value="y" <?php if ($date=='y') echo 'checked="checked"'?>/> yes <input type="radio" name="date" value="n" <?php if ($date!='y') echo 'checked="checked"'?> /> no</p>

<p><strong>Time Zone Offset</strong> (+n/-n/'feed') Date and timer are converted to GMT time; to have display in local time, you must enter an offset from your current local time to <strong><?php echo gmdate("r")?> (GMT)</strong>.  If your local time is 5 hours before GMT, enter <code>-5</code>. If your local time is 8 hours past GMT, enter <code>+8</code>. Fractional offsets such as +10:30 must be entered as decimal <code>+10.5</code>. If you prefer to just display the date is recorded in the RSS, use a value = <code>feed</code><br>
<input type="text" name="tz" size="10" value="<?php echo $tz?>"></p>

<p><strong>Target links in the new window?</strong> (n="no, links open the same page", y="yes, open links in a new window", "xxxx" = open links in a frame named 'xxxx', 'popup' = use a <a href="popup.js">JavaScript function</a> <code>popupfeed()</code> to open in new window) <br>
<input type="text" name="targ" size="10" value="<?php echo $targ?>"></p>

<p><strong>UTF-8 Character Encoding</strong><br>  Required for many non-western language web pages and also may help if you see strange characters replacing quotes in your output (see <a href="http://jade.mcli.dist.maricopa.edu/feed/index.php?s=help#chars">help pages</a> for more information).<br />
<input type="checkbox" name="utf" value="y" <?php if ($utf=='y') echo 'checked="checked"'?> /> use UTF-8 character encoding
</p>

<p><strong>Podcast enclosures</strong><br> For RSS 2.0 feeds with enclosures, display link to media files<br />
<input type="radio" name="pc" value="y" <?php if ($pc=='y') echo 'checked="checked"'?> /> yes
<input type="radio" name="pc" value="n" <?php if ($pc!='y') echo 'checked="checked"'?> /> no
</p>

<p><strong>Custom CSS Class (advanced users)</strong> <br> Use to create different styles for multiple feeds per page. Specify class for content as <code>rss-box-XXXX</code> where XXXX is the value entered below. Style sheets must be created in accordance with <a href="style.php#2style">Feed2JS guidelines</a>.<br>
<input type="text" name="rss_box_id" size="10" value="<?php echo $rss_box_id?>"></p>

</form>
</div>

<?php include 'footer'?>

</body>
</html>
