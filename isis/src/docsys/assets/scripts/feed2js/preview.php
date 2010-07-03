<?php

// Get variables from input 

	
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


$rss_str = "feed2js.php?src=" . urlencode($src) . $options . $html_options;

$noscript_rss_str = "feed2js.php?src=" . urlencode($src) . $options .  '&html=y';
?>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
        "http://www.w3.org/TR/html4/loose.dtd">
<html lang="en">
<head>
	<?php 
			if ($utf== 'y') {
				echo '<meta http-equiv="content-type" content="text/html; charset=utf-8">';
			} else {
				echo '<meta http-equiv="content-type" content="text/html; charset=iso-8859-1">';
			}
	?>
	<title>Feed Sneak Preview</title>
	<link rel="stylesheet" href="style/basic1.css">

	<style type="text/css">
body {background-color:#fff; margin: 12px;}
h1  { font-family: Verdana, Arial, Helvetica, sans-serif;
margin-bottom:0;}     
p, li { font-family: Verdana, Arial, Helvetica, sans-serif; font-size: 13px; color:#444; margin:0 0 1em;}
</style>

<script src="popup.js" type="text/javascript" language="Javascript"></script>

</head>
<body>

<div id="main">
<h1>Look At Your Feed</h1>
<p>Below is a preview of your feed using a basic style (learn how to customize this via the Feed2JS style pages). If this looks like the correct content and display, close this window and use the <strong>Generate JavaScript</strong> button to create your own web page code.</p>

<script language="JavaScript" src="<?php echo $rss_str?>"></script>
<noscript>
<a href="<?php echo $noscript_rss_str?>">View Feed</a>
</noscript>

<div align="center">
<form>
<input type="button" value="Close Window" onClick="self.close()">

</form>
</div>

</div>
</body>
</html>

