<?php

// function to return the path to the correct directory for the RSS2Js files
function whatsmypath() {
	return 'http://' . $_SERVER['SERVER_NAME'] . dirname($_SERVER['PHP_SELF']);
}
?>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
        "http://www.w3.org/TR/1999/REC-html401-19991224/loose.dtd">
<html>
<head>
	<title>No XML Source</title>
<style type="text/css" >
<!--
A:link {color: #002244;}
A:visited {color: #663300;}
body {background-color:#D8D9D5; padding:0; margin:0;}
#main {margin: 5% 10%; padding: 10px 60px; background-color:#FFF; border:2px dotted #444;}
h1 { font-family: Courier, "Courier New", monospaced; font-size: xx-large; margin-bottom:0;}     
p {font-family: Georgia, Bookman, "Times New Roman", Times, serif; line-height:150%;}
.blurb { font-family: Verdana, Arial, Helvetica, sans-serif; font-size: x-small; color:#933; margin:1em 5%;}
-->
</style>
</head>
<body>
<div id="main">
<h1 align="center">XML error</h1>
<p>There is an error in the requested URL as no xml source was provided. A correct request should look like:</p>


<p class="blurb">
&lt;script type="text/javascript" language="Javascript"<br>  src="<?php echo whatsmypath()?>feed2js.php?<strong>src=http://www.somesite.com/myfeed.xml</strong>"&gt;
&lt;/script&gt;
</p>

<p>You should check your source code!</p>

</div>
</body>
</html>
