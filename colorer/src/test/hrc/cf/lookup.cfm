<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">

<html>
<head>
	<title>Untitled</title>
</head>

<body>
 
<CFQUERY NAME="qdata" datasource="fastTrack_lab">
	select *
	from beans
	where bean_id = #url.productid#
</CFQUERY>

<CFIF qdata.recordcount is 1>
	<CFOUTPUT>
	<SCRIPT LANGUAGE="JavaScript" TYPE="text/javascript">
		parent.appframe.document.forms[0].productname#url.row#.value = '#jsStringformat(qdata.bean_name)#';
	</SCRIPT>
	</CFOUTPUT>
<CFELSE>
	<SCRIPT LANGUAGE="JavaScript" TYPE="text/javascript">
    <!--
		<CFOUTPUT>
    	alert("Not found");
		parent.appframe.document.forms[0].productid#url.row#.value = '';
		parent.appframe.document.forms[0].productid#url.row#.focus();
		</CFOUTPUT>
    //-->
    </SCRIPT>
</CFIF>
	


</body>
</html>
