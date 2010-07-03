<?php
	include 'style/style_pile.php';
	
	// get PHP variables
	
	$flavor = $_REQUEST['flavor'];
	$feed = $_REQUEST['feed'];
	$new_css = $_REQUEST['new_css'];

	$mod = $_REQUEST['mod'];
	
	$my_dir = 'http://' . $_SERVER['SERVER_NAME'] . dirname($_SERVER['PHP_SELF']);
	$rss_str = "feed2js.php?src=" . urlencode($feed) . "&chan=y&num=3&desc=200&date=y&html=n";

	if ($mod) {
		$style_title = "modified / original";
	} else {
		$style_title = $mystyles[$flavor];
	}
?>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
        "http://www.w3.org/TR/html4/loose.dtd">
<html lang="en">
<head>
	<meta http-equiv="content-type" content="text/html; charset=iso-8859-1">
	<title>Styled Feed</title>
	
	
	<?php 
	if ($mod) {
		echo "<style type=\"text/css\">\n" . stripslashes($new_css) . "\n</style>\n";
	} else { 
		echo '<link rel="stylesheet" href="style/' . $flavor . '.css">';
	}
	?>
	
	<style type="text/css">
body {background-color:#fff; margin: 0; padding: 10px 5%;}
h1, h2, h3 { font-family: "American Typewriter", "Courier New", Courier, monospaced; margin-bottom:0;}     
p, li { font-family: Verdana, Arial, Helvetica, sans-serif; font-size: 13px; color:#444; margin:0 0 1em;}
</style>


</head>
<body>

<div id="main">
<h1>Style Preview!</h1>

<p>Here is a preview of the  <strong><?php echo $style_title?></strong> style sheet applied to the RSS Feed <strong><?php echo "$feed"?></strong>. The URL to generate this view is:
<form>
<textarea name="f" rows="4" cols="60"><?php echo $my_dir?>/style_preview.php?feed=<?php echo urlencode($feed)?>&flavor=<?php echo urlencode($flavor)?></textarea>
</form>

<script language="JavaScript" src="<?php echo $rss_str?>"></script>
<noscript>
<a href="<?php echo $rss_str?>">View Feed</a>
</noscript>

<h2>CSS Applied</h2>
<p>Below is the CSS applied here, yours for copy and pasting pleasure. See below for a description of the classes provided and refer to the Feed2JS style selection site for more details on where to use this code.</p>

<p>You may also use the form below to <strong>modify</strong> the CSS to experiment with your own variants.</p>

<form method="post" action="<?php echo $PHP_SELF?>">
<input type="hidden" name="feed" value="<?php echo $feed?>">
<input type="hidden" name="flavor" value="<?php echo $flavor?>">
<textarea name="new_css" rows="20" cols="70"><?php 
		if ($mod) {
			echo stripslashes($new_css);
		} else {
			include "style/$flavor.css";
		}
	?>
</textarea>
<p>Make changes and <input type="submit" name="mod" value="Preview Modified CSS">
or <input type="submit" name="revert" value="Revert to <?php echo $mystyles[$flavor]?>"></p>

</form>

<h3>CSS classes</h3>
<img src="style/css_classes.gif" alt="" width="360" height="223" border="0" align="right">
<p>This sketch represents the CSS classes created by Feed2JS.</p>
<ul>
<li><strong>rss-box</strong> defines the bounding div for the entire display- use to define borders, fill, etc.</li>
<li><strong>rss-title</strong> the title of the feed and link style if displayed. Use with variants of <code>rss-title a:link, rss-title a:hover</code>, etc for rollover styles</li>
<li><strong>rss-items</strong> defines the unordered list <code>&lt;ul&gt;...&lt;/ul&gt;</code> for the feed items- use to define the padding/margins for items.</li>
<li><strong>rss-item</strong> display of each feed item description and title, <code>&lt;li&gt;...&lt;/li&gt;</code> as well as the channel description, if displayed.</li> 
<li><strong>rss-item a:</strong>  variant for the item title and link style</li>
<li><strong>rss-date</strong>  defines the display of item posting dates</li>
</ul>

<br clear="right">

</div>
</body>
</html>
