




<!doctype html public "-//W3C//DTD HTML 4.0 Transitional//EN"
"http://www.w3.org/TR/REC-html40/loose.dtd">
<!-- ViewCVS -- http://viewcvs.sourceforge.net/
by Greg Stein -- mailto:gstein@lyra.org -->
<html>
<head>
<title>SourceForge.net CVS Repository - log - cvs: celestia/celestia/src/celengine/galaxy.cpp</title>
<link rel="stylesheet" href="/viewcvs.py/*docroot*/styles.css" type="text/css">
</head>
<body>
<center>
<iframe SRC="http://ads.osdn.com/?op=iframe&position=1&allpositions=1&site_id=2&section=cvs" width="728" height="90" frameborder="0" border="0" MARGINWIDTH="0" MARGINHEIGHT="0" SCROLLING="no"></iframe>
<!-- image audit code -->
<script LANGUAGE="JAVASCRIPT">
<!--
now = new Date();
tail = now.getTime();
document.write("<IMG SRC='http://images-aud.sourceforge.net/pc.gif?l,");
document.write(tail);
document.write("' WIDTH=1 HEIGHT=1 BORDER=0>");
//-->
</SCRIPT>
<noscript>
<img src="http://images-aud.sourceforge.net/pc.gif?l,81677"
WIDTH=1 HEIGHT=1 BORDER=0>
</noscript>
<!-- end audit code -->
</center>
<div class="vc_navheader">
<table width="100%" border="0" cellpadding="0" cellspacing="0">
<tr>
<td align="left"><a href="/viewcvs.py/#dirlist">[cvs]</a> / <a href="/viewcvs.py/celestia/#dirlist">celestia</a> / <a href="/viewcvs.py/celestia/celestia/#dirlist">celestia</a> / <a href="/viewcvs.py/celestia/celestia/src/#dirlist">src</a> / <a href="/viewcvs.py/celestia/celestia/src/celengine/#dirlist">celengine</a> / galaxy.cpp</h1></td>
<td align="right">

&nbsp;

</td>
</tr>
</table>
</div>
<table width="100%">
<tr><td><h1>cvs: celestia/celestia/src/celengine/galaxy.cpp</h1></td>
<td align=right><a href="http://sourceforge.net"><img src="/sourceforge_whitebg.gif" alt="(logo)" border=0 width=136 height=79></a></td></tr>
</table>

<hr noshade>

Default branch: MAIN
<br>
Bookmark a link to HEAD:
(<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?view=markup">view</a>)
(<a href="/viewcvs.py/*checkout*/celestia/celestia/src/celengine/galaxy.cpp">download</a>)


<br>



 



<hr size=1 noshade>


<a name="rev1.15"></a>
<a name="HEAD"></a>


Revision <b>1.15</b>
 -
(<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?rev=1.15&view=markup">view</a>)
(<a href="/viewcvs.py/*checkout*/celestia/celestia/src/celengine/galaxy.cpp?rev=1.15">download</a>)


(<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?annotate=1.15">annotate</a>)


- <a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?r1=1.15">[select for diffs]</a>




<br>

<i>Mon Aug 15 15:39:21 2005 UTC</i> (2 months, 3 weeks ago) by <i>cjlaurel</i>

<br>Branch:

<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?only_with_tag=MAIN"><b>MAIN</b></a>



<br>CVS Tags:

<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?only_with_tag=HEAD"><b>HEAD</b></a>






<br>Changes since <b>1.14: +11 -5 lines</b>







<br>Diff to <a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?r1=1.14&amp;r2=1.15">previous 1.14</a>










<pre>Fixed bugs with elliptical galaxy shader; model of star distribution still needs some work before this method of rendering galaxies can be enabled.
</pre>

<hr size=1 noshade>


<a name="rev1.14"></a>


Revision <b>1.14</b>
 -
(<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?rev=1.14&view=markup">view</a>)
(<a href="/viewcvs.py/*checkout*/celestia/celestia/src/celengine/galaxy.cpp?rev=1.14">download</a>)


(<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?annotate=1.14">annotate</a>)


- <a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?r1=1.14">[select for diffs]</a>




<br>

<i>Wed Jul 13 08:29:51 2005 UTC</i> (3 months, 3 weeks ago) by <i>cjlaurel</i>

<br>Branch:

<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?only_with_tag=MAIN"><b>MAIN</b></a>







<br>Changes since <b>1.13: +224 -81 lines</b>







<br>Diff to <a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?r1=1.13&amp;r2=1.14">previous 1.13</a>










<pre>Integrated new galaxy rendering code based on Toti and Fridger's work, plus special handling of elliptical galaxies. Integration of more recent galaxy code is still to be done.
</pre>

<hr size=1 noshade>


<a name="rev1.13"></a>


Revision <b>1.13</b>
 -
(<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?rev=1.13&view=markup">view</a>)
(<a href="/viewcvs.py/*checkout*/celestia/celestia/src/celengine/galaxy.cpp?rev=1.13">download</a>)


(<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?annotate=1.13">annotate</a>)


- <a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?r1=1.13">[select for diffs]</a>




<br>

<i>Fri Feb 11 05:09:36 2005 UTC</i> (8 months, 3 weeks ago) by <i>cjlaurel</i>

<br>Branch:

<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?only_with_tag=MAIN"><b>MAIN</b></a>







<br>Changes since <b>1.12: +13 -0 lines</b>







<br>Diff to <a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?r1=1.12&amp;r2=1.13">previous 1.12</a>










<pre>Added separate render and label flags for the various types of deep sky objects.  This will break some scripts and cel:// URLs that relied on the 'show galaxies' flag to also show nebulae, but this is the inescapable cost of moving to a more flexible set of render settings.
</pre>

<hr size=1 noshade>


<a name="rev1.12"></a>
<a name="ver1_3_2"></a>


Revision <b>1.12</b>
 -
(<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?rev=1.12&view=markup">view</a>)
(<a href="/viewcvs.py/*checkout*/celestia/celestia/src/celengine/galaxy.cpp?rev=1.12">download</a>)


(<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?annotate=1.12">annotate</a>)


- <a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?r1=1.12">[select for diffs]</a>




<br>

<i>Tue Jan 27 16:51:43 2004 UTC</i> (21 months, 1 week ago) by <i>cjlaurel</i>

<br>Branch:

<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?only_with_tag=MAIN"><b>MAIN</b></a>



<br>CVS Tags:

<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?only_with_tag=ver1_3_2"><b>ver1_3_2</b></a>






<br>Changes since <b>1.11: +1 -0 lines</b>







<br>Diff to <a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?r1=1.11&amp;r2=1.12">previous 1.11</a>










<pre>Added a missing GL texture enable that caused stars and galaxies rendered after a nebula to appear square.
</pre>

<hr size=1 noshade>


<a name="rev1.11"></a>
<a name="ver_1_3_1"></a>


Revision <b>1.11</b>
 -
(<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?rev=1.11&view=markup">view</a>)
(<a href="/viewcvs.py/*checkout*/celestia/celestia/src/celengine/galaxy.cpp?rev=1.11">download</a>)


(<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?annotate=1.11">annotate</a>)


- <a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?r1=1.11">[select for diffs]</a>




<br>

<i>Wed Jul 30 04:34:04 2003 UTC</i> (2 years, 3 months ago) by <i>cjlaurel</i>

<br>Branch:

<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?only_with_tag=MAIN"><b>MAIN</b></a>



<br>CVS Tags:

<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?only_with_tag=ver_1_3_1"><b>ver_1_3_1</b></a>






<br>Changes since <b>1.10: +0 -1 lines</b>







<br>Diff to <a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?r1=1.10&amp;r2=1.11">previous 1.10</a>










<pre>Complete rewrite of texture handling code for cleanup and in order to support virtual textures.  What was the texture class is now split between Texture and Image.  There are several subclasses of Texture, including ImageTexture, TiledTexture, CubeMap, and VirtualTexture.
</pre>

<hr size=1 noshade>


<a name="rev1.10"></a>
<a name="ver_1_3_0"></a>


Revision <b>1.10</b>
 -
(<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?rev=1.10&view=markup">view</a>)
(<a href="/viewcvs.py/*checkout*/celestia/celestia/src/celengine/galaxy.cpp?rev=1.10">download</a>)


(<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?annotate=1.10">annotate</a>)


- <a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?r1=1.10">[select for diffs]</a>




<br>

<i>Mon Mar 24 07:55:54 2003 UTC</i> (2 years, 7 months ago) by <i>cjlaurel</i>

<br>Branch:

<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?only_with_tag=MAIN"><b>MAIN</b></a>



<br>CVS Tags:

<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?only_with_tag=ver_1_3_0"><b>ver_1_3_0</b></a>






<br>Changes since <b>1.9: +2 -2 lines</b>







<br>Diff to <a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?r1=1.9&amp;r2=1.10">previous 1.9</a>










<pre>Support local resource directories for deep sky objects and 3DS meshes.
</pre>

<hr size=1 noshade>


<a name="rev1.9"></a>


Revision <b>1.9</b>
 -
(<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?rev=1.9&view=markup">view</a>)
(<a href="/viewcvs.py/*checkout*/celestia/celestia/src/celengine/galaxy.cpp?rev=1.9">download</a>)


(<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?annotate=1.9">annotate</a>)


- <a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?r1=1.9">[select for diffs]</a>




<br>

<i>Tue Feb 25 07:26:18 2003 UTC</i> (2 years, 8 months ago) by <i>cjlaurel</i>

<br>Branch:

<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?only_with_tag=MAIN"><b>MAIN</b></a>







<br>Changes since <b>1.8: +29 -0 lines</b>







<br>Diff to <a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?r1=1.8&amp;r2=1.9">previous 1.8</a>










<pre>Fixed rendering problems when mixing nebulae and galaxies.
</pre>

<hr size=1 noshade>


<a name="rev1.8"></a>


Revision <b>1.8</b>
 -
(<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?rev=1.8&view=markup">view</a>)
(<a href="/viewcvs.py/*checkout*/celestia/celestia/src/celengine/galaxy.cpp?rev=1.8">download</a>)


(<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?annotate=1.8">annotate</a>)


- <a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?r1=1.8">[select for diffs]</a>




<br>

<i>Wed Jan 29 05:03:41 2003 UTC</i> (2 years, 9 months ago) by <i>cjlaurel</i>

<br>Branch:

<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?only_with_tag=MAIN"><b>MAIN</b></a>







<br>Changes since <b>1.7: +1 -3 lines</b>







<br>Diff to <a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?r1=1.7&amp;r2=1.8">previous 1.7</a>










<pre>Removed extraneous debugging output.
</pre>

<hr size=1 noshade>


<a name="rev1.7"></a>


Revision <b>1.7</b>
 -
(<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?rev=1.7&view=markup">view</a>)
(<a href="/viewcvs.py/*checkout*/celestia/celestia/src/celengine/galaxy.cpp?rev=1.7">download</a>)


(<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?annotate=1.7">annotate</a>)


- <a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?r1=1.7">[select for diffs]</a>




<br>

<i>Wed Jan 29 04:43:58 2003 UTC</i> (2 years, 9 months ago) by <i>cjlaurel</i>

<br>Branch:

<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?only_with_tag=MAIN"><b>MAIN</b></a>







<br>Changes since <b>1.6: +92 -131 lines</b>







<br>Diff to <a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?r1=1.6&amp;r2=1.7">previous 1.6</a>










<pre>Added new DeepSkyObject base class for Galaxy and Nebula
</pre>

<hr size=1 noshade>


<a name="rev1.6"></a>


Revision <b>1.6</b>
 -
(<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?rev=1.6&view=markup">view</a>)
(<a href="/viewcvs.py/*checkout*/celestia/celestia/src/celengine/galaxy.cpp?rev=1.6">download</a>)


(<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?annotate=1.6">annotate</a>)


- <a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?r1=1.6">[select for diffs]</a>




<br>

<i>Fri Dec 13 18:43:41 2002 UTC</i> (2 years, 10 months ago) by <i>christey</i>

<br>Branch:

<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?only_with_tag=MAIN"><b>MAIN</b></a>







<br>Changes since <b>1.5: +1 -1 lines</b>







<br>Diff to <a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?r1=1.5&amp;r2=1.6">previous 1.5</a>










<pre>constness correstions
</pre>

<hr size=1 noshade>


<a name="rev1.5"></a>
<a name="v124"></a>


Revision <b>1.5</b>
 -
(<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?rev=1.5&view=markup">view</a>)
(<a href="/viewcvs.py/*checkout*/celestia/celestia/src/celengine/galaxy.cpp?rev=1.5">download</a>)


(<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?annotate=1.5">annotate</a>)


- <a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?r1=1.5">[select for diffs]</a>




<br>

<i>Wed Feb  6 20:00:19 2002 UTC</i> (3 years, 8 months ago) by <i>cjlaurel</i>

<br>Branch:

<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?only_with_tag=MAIN"><b>MAIN</b></a>



<br>CVS Tags:

<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?only_with_tag=v124"><b>v124</b></a>






<br>Changes since <b>1.4: +3 -3 lines</b>







<br>Diff to <a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?r1=1.4&amp;r2=1.5">previous 1.4</a>










<pre>Added debug level parameter to DPRINTFs
</pre>

<hr size=1 noshade>


<a name="rev1.4"></a>


Revision <b>1.4</b>
 -
(<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?rev=1.4&view=markup">view</a>)
(<a href="/viewcvs.py/*checkout*/celestia/celestia/src/celengine/galaxy.cpp?rev=1.4">download</a>)


(<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?annotate=1.4">annotate</a>)


- <a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?r1=1.4">[select for diffs]</a>




<br>

<i>Tue Feb  5 20:00:58 2002 UTC</i> (3 years, 9 months ago) by <i>dramsey</i>

<br>Branch:

<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?only_with_tag=MAIN"><b>MAIN</b></a>







<br>Changes since <b>1.3: +1 -1 lines</b>







<br>Diff to <a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?r1=1.3&amp;r2=1.4">previous 1.3</a>










<pre>Verbose option for better recognition of errors
</pre>

<hr size=1 noshade>


<a name="rev1.3"></a>


Revision <b>1.3</b>
 -
(<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?rev=1.3&view=markup">view</a>)
(<a href="/viewcvs.py/*checkout*/celestia/celestia/src/celengine/galaxy.cpp?rev=1.3">download</a>)


(<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?annotate=1.3">annotate</a>)


- <a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?r1=1.3">[select for diffs]</a>




<br>

<i>Thu Jan  3 20:01:11 2002 UTC</i> (3 years, 10 months ago) by <i>dramsey</i>

<br>Branch:

<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?only_with_tag=MAIN"><b>MAIN</b></a>







<br>Changes since <b>1.2: +4 -4 lines</b>







<br>Diff to <a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?r1=1.2&amp;r2=1.3">previous 1.2</a>










<pre>Put #includes back the way they were, and fix compile problems under win
</pre>

<hr size=1 noshade>


<a name="rev1.2"></a>


Revision <b>1.2</b>
 -
(<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?rev=1.2&view=markup">view</a>)
(<a href="/viewcvs.py/*checkout*/celestia/celestia/src/celengine/galaxy.cpp?rev=1.2">download</a>)


(<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?annotate=1.2">annotate</a>)


- <a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?r1=1.2">[select for diffs]</a>




<br>

<i>Tue Dec 18 22:00:25 2001 UTC</i> (3 years, 10 months ago) by <i>dramsey</i>

<br>Branch:

<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?only_with_tag=MAIN"><b>MAIN</b></a>







<br>Changes since <b>1.1: +6 -4 lines</b>







<br>Diff to <a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?r1=1.1&amp;r2=1.2">previous 1.1</a>










<pre>Autoconf distribute extension &amp; bugfixes
</pre>

<hr size=1 noshade>


<a name="rev1.1"></a>


Revision <b>1.1</b>
 -
(<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?rev=1.1&view=markup">view</a>)
(<a href="/viewcvs.py/*checkout*/celestia/celestia/src/celengine/galaxy.cpp?rev=1.1">download</a>)


(<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?annotate=1.1">annotate</a>)


- <a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?r1=1.1">[select for diffs]</a>




<br>

<i>Wed Nov 28 00:48:38 2001 UTC</i> (3 years, 11 months ago) by <i>cjlaurel</i>

<br>Branch:

<a href="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp?only_with_tag=MAIN"><b>MAIN</b></a>
















<pre>Reorganized source tree into multiple directories.
</pre>

 



<a name=diff></a>
<hr noshade>
This form allows you to request diffs between any two revisions of
a file. You may select a symbolic revision name using the selection
box or you may type in a numeric name using the type-in text box.
<p>
<form method=get action="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp" name=diff_select>

Diffs between
<select name="r1">
<option value="text" selected>Use Text Field</option>

<option value="1.11:ver_1_3_1">ver_1_3_1</option>

<option value="1.10:ver_1_3_0">ver_1_3_0</option>

<option value="1.12:ver1_3_2">ver1_3_2</option>

<option value="1.5:v124">v124</option>

<option value="1.15:MAIN">MAIN</option>

<option value="1.15:HEAD">HEAD</option>

</select>
<input type="TEXT" size="12" name="tr1" value="1.1"
onChange="document.diff_select.r1.selectedIndex=0">
and
<select name="r2">
<option value="text" selected>Use Text Field</option>

<option value="1.11:ver_1_3_1">ver_1_3_1</option>

<option value="1.10:ver_1_3_0">ver_1_3_0</option>

<option value="1.12:ver1_3_2">ver1_3_2</option>

<option value="1.5:v124">v124</option>

<option value="1.15:MAIN">MAIN</option>

<option value="1.15:HEAD">HEAD</option>

</select>
<input type="TEXT" size="12" name="tr2" value="1.15"
onChange="document.diff_select.r1.selectedIndex=0">
<br>Type of Diff should be a
<select name="diff_format" onchange="submit()">
<option value="h" selected>Colored Diff</option>
<option value="l" >Long Colored Diff</option>
<option value="u" >Unidiff</option>
<option value="c" >Context Diff</option>
<option value="s" >Side by Side</option>
</select>
<input type=submit value=" Get Diffs "></form>



<hr noshade>
<a name=branch></a>
<form method=GET action="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp">

View only Branch:
<select name="only_with_tag" onchange="submit()">
<option value="" selected>Show all branches</option>

<option value="MAIN" >MAIN</option>

</select>
<input type=submit value=" View Branch ">
</form>


<hr noshade>
<a name=logsort></a>
<form method=get action="/viewcvs.py/celestia/celestia/src/celengine/galaxy.cpp">

Sort log by:
<select name="logsort" onchange="submit()">
<option value="cvs" >Not sorted</option>
<option value="date" selected>Commit date</option>
<option value="rev" >Revision</option>
</select>
<input type=submit value=" Sort ">
</form>


<hr noshade>
<table width="100%" border="0" cellpadding="0" cellspacing="0">
<tr>
<td align="left">
<address><a href="http://sourceforge.net/">Back to SourceForge.net</a></address><br />
Powered by <a href="http://viewcvs.sourceforge.net/">ViewCVS 1.0-dev</a>
</td>
<td align="right">
<h3><a target="_blank" href="/viewcvs.py/*docroot*/help_log.html">ViewCVS and CVS Help</a></h3>
</td>
</tr>
</table>
</body>
</html>

