<?require("../php/paths.php");?>
<?header("Content-type: text/html; charset=\"UTF-8\"");?>
<html>
<head>
<title>Colorer-take5 be5 design</title>
<link rel="stylesheet" type="text/css" href="../styles/styles.css"/>
<link rel="stylesheet" type="text/css" href="styles.css">
<link rel="icon" href="../images/logo-small.png"/>
<style>
.codeinc{
  border: solid 1pt #d8d8d8;
  background:#fbfbfb;
  margin: 10pt 10pt 10pt 10pt;
  padding: 3pt 3pt 3pt 8pt;
}
</style>
</head>
<body>
<div class='back'><a href='../index.html'><img alt='back' src='../images/back.png' width='40' height='18'/></a></div>

<div>
<a href="http://sourceforge.net"><img align=right src="http://sourceforge.net/sflogo.php?group_id=34855&amp;type=2" width="125" height="37" border="0" alt="SourceForge.net Logo"/></a>
</div>

<h1>Colorer-take5 be5 design usecases</h1>


<h2>Prologue/Epilogue inherit feature</h2>
<p>
Minor improvement over inheritance usability.
Allows to shortcut HRC code development.
Outlines most common inheritance usecase.
</p>
<?colorerInclude("prologue-epilogue.hrc")?>

<h2>Inheritance behaviour change</h2>
<p>
<strong>Old behaviour:</strong>
When a level of inherit substitution is applied,
it becomes disabled for new schemes, invoked because of &lt;virtual&gt; rules.

<br/>
<strong>New behaviour:</strong>
Only lower levels of inheritance are disabled when applying a particular &lt;virtual&gt; rule.
This level's &lt;virtual&gt; rules (including the applied one) are still valid and active.
</p>
<?colorerInclude("inherit-behaviour-change.hrc")?>


<h2>Region inheritance feature</h2>
<p>
Allows HRC regions parametrization. Allows heavier flexibility in highlighting and implementing multiple color styles.
</p>
<?colorerInclude("region-vars.hrc")?>
