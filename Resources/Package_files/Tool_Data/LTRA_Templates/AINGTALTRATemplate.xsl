<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="2.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format">
	<xsl:output method="html" />
	<xsl:template match="/">
		<html>
	
			<script>
			var itemIdx=1;
			function getItemIdx() 
			{
				return itemIdx++;
			}
			var commentIdx=1;
			function getCommentIdx() 
			{
				return commentIdx++;
			}
			var otherInfoIdx=1;
			function getOtherInfoIdx() 
			{
				return otherInfoIdx++;
			}
			function intendTo(intendMargin)
			{
			 var x = intendMargin*10
			 var y = '<a style="margin-left:'+x+';"> </a>';
			 return y;
			}
			function getTimeInfo(timestamp)
			{
				timestamp = timestamp.split("(");
				return timestamp[0];
			}
			function getCurrentFileName()
			{
				var path = document.URL

				var start = path.lastIndexOf('\\');

				var end = path.lastIndexOf('.');

				return path.substring(start+1, end);
			}	
			function getMoreInfo(Info)
			{
				var title = Info.title;
				title = title.replace(/,/gi,",<br/>");
				var htmlData = '<html>
					<body>'+title+'</body>
				</html>';
				var win = window.open('','MoreInfo','height=500,width=500');
				win.document.body.innerHTML = '';
				win.document.write(htmlData);			
			}
			</script>
			<body>
				<xsl:variable name ="GlobalStatus" select="LogInfo/@Status"/>
				<xsl:variable name="DATE" select="LogInfo/header/REPORTINFO/DATE/." />
				<xsl:variable name="LTRA_TITLE" select="LogInfo/header/REPORTINFO/TITLE/." />
				<xsl:variable name="DESIGNERNAME" select="LogInfo/header/REPORTINFO/DESIGNERNAME/." />
				<xsl:variable name="LTRREF" select="LogInfo/header/REPORTINFO/LTRREF/." />
				<xsl:variable name="executionTime" select="LogInfo/header/REPORTINFO/executionTime/." />
				<table align="center" style="border:solid windowtext 1.0pt;   mso-border-alt:solid windowtext .5pt;padding:0cm 5.4pt 0cm 5.4pt">
					<tr>
						<td style="border-collapse: collapse; border: 1px black; border-right-style: solid ;">
							<table>
								<tr>
									<td>
										<script>
				  var name = getCurrentFileName();

				  var src = name+"_files/Airbus_LTRA_LOGO.png";
					document.write("&lt;img src='"+src+"'/>");
								</script>
							</td>
						</tr>
					</table>
				</td>
				<td style="border-collapse: collapse; border: 1px black;">
					<table align="center" style="border:solid windowtext 1.0pt;   mso-border-alt:solid windowtext .5pt;padding:0cm 5.4pt 0cm 5.4pt">
						<tr>
							<td style="border:solid windowtext 1.0pt;   mso-border-alt:solid windowtext .5pt;padding:0cm 5.4pt 0cm 5.4pt;text-align:center;font-size:8.0pt;font-family:'Arial','sans-serif';mso-ansi-language:EN-GB">
								<b>Date</b>
							</td>
							<td style="border:solid windowtext 1.0pt;   mso-border-alt:solid windowtext .5pt;padding:0cm 5.4pt 0cm 5.4pt;text-align:center;font-size:8.0pt;font-family:'Arial','sans-serif';mso-ansi-language:EN-GB">
								<b>  LTR Title</b>
							</td>
							<td style="border:solid windowtext 1.0pt;   mso-border-alt:solid windowtext .5pt;padding:0cm 5.4pt 0cm 5.4pt;text-align:center;font-size:8.0pt;font-family:'Arial','sans-serif';mso-ansi-language:EN-GB">
								<b>  Test Execution Time</b>
							</td>							
							<td style="border:solid windowtext 1.0pt;   mso-border-alt:solid windowtext .5pt;padding:0cm 5.4pt 0cm 5.4pt;text-align:center;font-size:8.0pt;font-family:'Arial','sans-serif';mso-ansi-language:EN-GB">
								<b>Designer</b>
							</td>
							<td style="border:solid windowtext 1.0pt;   mso-border-alt:solid windowtext .5pt;padding:0cm 5.4pt 0cm 5.4pt;text-align:center;font-size:8.0pt;font-family:'Arial','sans-serif';mso-ansi-language:EN-GB">
								<b>LTR Ref</b>
							</td>
							<td style="border:solid windowtext 1.0pt;   mso-border-alt:solid windowtext .5pt;padding:0cm 5.4pt 0cm 5.4pt;text-align:center;font-size:8.0pt;font-family:'Arial','sans-serif';mso-ansi-language:EN-GB">
								<b>Test Status</b>
							</td>
						</tr>
						<tr>
							<td align="center" style="border:solid windowtext 1.0pt;   mso-border-alt:solid windowtext .5pt;padding:0cm 5.4pt 0cm 5.4pt;text-align:center;font-size:8.0pt;font-family:'Arial','sans-serif';mso-ansi-language:EN-GB">
								<xsl:value-of select="$DATE" />
							</td>
							<td align="center" style="border:solid windowtext 1.0pt;   mso-border-alt:solid windowtext .5pt;padding:0cm 5.4pt 0cm 5.4pt;text-align:center;font-size:8.0pt;font-family:'Arial','sans-serif';mso-ansi-language:EN-GB">
								<xsl:value-of select="$LTRA_TITLE" />
							</td>
							<td align="center" style="border:solid windowtext 1.0pt;   mso-border-alt:solid windowtext .5pt;padding:0cm 5.4pt 0cm 5.4pt;text-align:center;font-size:8.0pt;font-family:'Arial','sans-serif';mso-ansi-language:EN-GB">
								<xsl:value-of select="$executionTime" />
							</td>							
							<td align="center" style="border:solid windowtext 1.0pt;   mso-border-alt:solid windowtext .5pt;padding:0cm 5.4pt 0cm 5.4pt;text-align:center;font-size:8.0pt;font-family:'Arial','sans-serif';mso-ansi-language:EN-GB">
								<xsl:value-of select="$DESIGNERNAME" />
							</td>
							<td align="center" style="border:solid windowtext 1.0pt;   mso-border-alt:solid windowtext .5pt;padding:0cm 5.4pt 0cm 5.4pt;text-align:center;font-size:8.0pt;font-family:'Arial','sans-serif';mso-ansi-language:EN-GB">
								<xsl:value-of select="$LTRREF" />
							</td>
							<td align="center" style="border:solid windowtext 1.0pt;   mso-border-alt:solid windowtext .5pt;padding:0cm 5.4pt 0cm 5.4pt;text-align:center;font-size:8.0pt;font-family:'Arial','sans-serif';mso-ansi-language:EN-GB">
								<xsl:if test="$GlobalStatus = 'OK'">
									<font style="color:green">
										<b>
											<xsl:value-of select="$GlobalStatus" />
										</b>
									</font>
								</xsl:if>
								<xsl:if test="$GlobalStatus = 'KO'">
									<font style="color:red">
										<b>
											<xsl:value-of select="$GlobalStatus" />
										</b>
									</font>
								</xsl:if>					
							</td>				  
						</tr>
					</table>
				</td>
			</tr>
		</table>
		<p />
		<table class="MsoNormalTable" align="center" border="1" cellspacing="0" cellpadding="0" style="width:825pt;border-collapse:collapse; border:none;mso-border-alt:solid windowtext .5pt;mso-yfti-tbllook:1184; mso-padding-alt:0cm 5.4pt 0cm 5.4pt;mso-border-insideh:.5pt solid windowtext; mso-border-insidev:.5pt solid windowtext">
			<tr style="mso-yfti-irow:0;mso-yfti-firstrow:yes">
				<xsl:for-each select="LogInfo/header/TESTDETAIL/Item">
					<td width="55" valign="top" style="width:41.1pt;border:solid windowtext 1.0pt;   mso-border-alt:solid windowtext .5pt;padding:0cm 5.4pt 0cm 5.4pt">
						<p class="MsoNormal" align="center" style="margin-bottom:0cm;margin-bottom:.0001pt;  text-align:center;line-height:10.0pt;mso-line-height-rule:exactly;mso-layout-grid-align:  none;text-autospace:none">
							<b>
								<span lang="EN-GB" style="font-size:8.0pt;  font-family:'Arial','sans-serif';mso-ansi-language:EN-GB">
									<xsl:value-of select="@name" />
								</span>
							</b>
						</p>
					</td>
				</xsl:for-each>
			</tr>
			<tr style="mso-yfti-irow:0;mso-yfti-firstrow:yes">
				<xsl:for-each select="LogInfo/header/TESTDETAIL/Item">
					<td width="55" valign="top" style="width:41.1pt;border:solid windowtext 1.0pt;   mso-border-alt:solid windowtext .5pt;padding:0cm 5.4pt 0cm 5.4pt">
						<p class="MsoNormal" align="center" style="margin-bottom:0cm;margin-bottom:.0001pt;  text-align:center;line-height:10.0pt;mso-line-height-rule:exactly;mso-layout-grid-align:  none;text-autospace:none">
							<b>
								<span lang="EN-GB" style="font-size:8.0pt;  font-family:'Arial','sans-serif';mso-ansi-language:EN-GB">
									<xsl:value-of select="@value" />
								</span>
							</b>
						</p>
					</td>
				</xsl:for-each>
			</tr>
		</table>
		<p />
		<table class="MsoNormalTable" align="center" border="1" cellspacing="0" cellpadding="0" style="width:825pt;border-collapse:collapse; border:none;mso-border-alt:solid windowtext .5pt;mso-yfti-tbllook:1184; mso-padding-alt:0cm 5.4pt 0cm 5.4pt;mso-border-insideh:.5pt solid windowtext; mso-border-insidev:.5pt solid windowtext">
			<tr style="mso-yfti-irow:0;mso-yfti-firstrow:yes">
				<xsl:for-each select="LogInfo/header/SPECIFICATION/Item">
					<td valign="top" style="border:solid windowtext 1.0pt;   mso-border-alt:solid windowtext .5pt;padding:0cm 5.4pt 0cm 5.4pt">
						<p class="MsoNormal" align="center" style="margin-bottom:0cm;margin-bottom:.0001pt;  text-align:center;line-height:10.0pt;mso-line-height-rule:exactly;mso-layout-grid-align:  none;text-autospace:none">
							<b>
								<span lang="EN-GB" style="font-size:8.0pt;  font-family:'Arial','sans-serif';mso-ansi-language:EN-GB">
									<xsl:value-of select="@name" />
								</span>
							</b>
						</p>
					</td>
				</xsl:for-each>
			</tr>
			<tr style="mso-yfti-irow:0;mso-yfti-firstrow:yes">
				<xsl:for-each select="LogInfo/header/SPECIFICATION/Item">
					<td valign="top" style="border:solid windowtext 1.0pt;   mso-border-alt:solid windowtext .5pt;padding:0cm 5.4pt 0cm 5.4pt">
						<p class="MsoNormal" align="center" style="margin-bottom:0cm;margin-bottom:.0001pt;  text-align:center;line-height:10.0pt;mso-line-height-rule:exactly;mso-layout-grid-align:  none;text-autospace:none">
							<b>
								<span lang="EN-GB" style="font-size:8.0pt;  font-family:'Arial','sans-serif';mso-ansi-language:EN-GB">
									<xsl:value-of select="@value" />
								</span>
							</b>
						</p>
					</td>
				</xsl:for-each>
			</tr>
		</table>
		<p />
		<table class="MsoNormalTable" align="center" border="1" cellspacing="0" cellpadding="0" style="width:825pt;border-collapse:collapse; border:none;mso-border-alt:solid windowtext .5pt;mso-yfti-tbllook:1184; mso-padding-alt:0cm 5.4pt 0cm 5.4pt;mso-border-insideh:.5pt solid windowtext; mso-border-insidev:.5pt solid windowtext">
			<tr style="mso-yfti-irow:0;mso-yfti-firstrow:yes">
				<xsl:for-each select="LogInfo/header/OTHERDETAILS/Item">
					<td valign="top" style="border:solid windowtext 1.0pt;   mso-border-alt:solid windowtext .5pt;padding:0cm 5.4pt 0cm 5.4pt">
						<p class="MsoNormal" align="center" style="margin-bottom:0cm;margin-bottom:.0001pt;  text-align:center;line-height:10.0pt;mso-line-height-rule:exactly;mso-layout-grid-align:  none;text-autospace:none">
							<b>
								<span lang="EN-GB" style="font-size:8.0pt;  font-family:'Arial','sans-serif';mso-ansi-language:EN-GB">
									<xsl:value-of select="@name" />
								</span>
							</b>
						</p>
					</td>
				</xsl:for-each>
			</tr>
			<tr style="mso-yfti-irow:0;mso-yfti-firstrow:yes">
				<xsl:for-each select="LogInfo/header/OTHERDETAILS/Item">
					<td valign="top" style="border:solid windowtext 1.0pt;   mso-border-alt:solid windowtext .5pt;padding:0cm 5.4pt 0cm 5.4pt">
						<p class="MsoNormal" align="center" style="margin-bottom:0cm;margin-bottom:.0001pt;  text-align:center;line-height:10.0pt;mso-line-height-rule:exactly;mso-layout-grid-align:  none;text-autospace:none">
							<b>
								<span lang="EN-GB" style="font-size:8.0pt;  font-family:'Arial','sans-serif';mso-ansi-language:EN-GB">
									<xsl:value-of select="@value" />
								</span>
							</b>
						</p>
					</td>
				</xsl:for-each>
			</tr>
		</table>
		<p />		
		<table width="1100" align="center" style="border-collapse: collapse; border: 1px black;border-top-style: solid ;    border-right-style: solid ;     border-left-style: solid ;">
			<tr>
				<td align="center" style="font-size:8.0pt;font-family:'Arial','sans-serif';mso-ansi-language:EN-GB">
					<b>Summary</b>
				</td>
			</tr>
		</table>
		<table width="1100" align="center" style="border-collapse: collapse; border: 1px black; border-bottom-style: solid ; border-top-style: solid ;    border-right-style: solid ;     border-left-style: solid ;">
			<xsl:for-each select="LogInfo/summary/item">
				<xsl:variable name="level1Count" select="position()" />
				<tr>
					<xsl:choose>
						<xsl:when test="@status='OK'">
							<td width="100" style="text-align:right ;border: 1px black;border-right-style: solid ; font-size:8.0pt; font-family:'Arial','sans-serif';color:green;mso-ansi-language:EN-GB">
								<b>
									<xsl:value-of select="@status" />
								</b>
								<xsl:text>     </xsl:text>
							</td>
						</xsl:when>
						<xsl:otherwise>
							<xsl:choose>
								<xsl:when test="@status='KO'">
									<td width="100" style="text-align:right ;border: 1px black;border-right-style: solid ; font-size:8.0pt; font-family:'Arial','sans-serif';color:red;mso-ansi-language:EN-GB">
										<b>
											<xsl:value-of select="@status" />
										</b>
										<xsl:text>     </xsl:text>
									</td>
								</xsl:when>
								<xsl:otherwise>
									<td width="100" style="text-align:right ;border: 1px black;border-right-style: solid ; font-size:8.0pt; font-family:'Arial','sans-serif';color:grey;mso-ansi-language:EN-GB">
										<b>
											<xsl:value-of select="@status" />
										</b>
										<xsl:text>     </xsl:text>
									</td>
								</xsl:otherwise>
							</xsl:choose>				
						</xsl:otherwise>
					</xsl:choose>
					<td style="border: 1px black;border-right-style: solid ;font-size:8.0pt;font-family:'Arial','sans-serif'; color:black;mso-ansi-language:EN-GB">
						<xsl:text>          </xsl:text>
						<xsl:text>&gt;  </xsl:text>
						<b>
							<script>
								var ItemCnt = '<xsl:value-of select="$level1Count" />';
								document.write("&lt;a href='#Item"+ItemCnt+"'>");
						</script>
							Item
						<xsl:value-of select="$level1Count" />
						<script>
								document.write("&lt;/a>");
					</script>
				</b>
				<xsl:text>     </xsl:text>
				<xsl:variable name="summStmt" select="."/>
				<script>
							var sumStmt = '<xsl:value-of select="normalize-space($summStmt)"/>';
							sumStmt = sumStmt.replace("Function Status","Status");
							document.write(sumStmt);
				</script>


			</td>
		</tr>
	</xsl:for-each>
</table>
<h2 />
<!-- space after summary table-->
<table width="1100" align="center" style="border-collapse: collapse; border: 1px black; border-bottom-style: solid ; border-top-style: solid ;    border-right-style: solid ;     border-left-style: solid ;">
	<tr align="center" style="font-size:10.0pt;font-family:'Arial','sans-serif';mso-ansi-language:EN-GB;border-collapse: collapse; border: 1px black; border-bottom-style: solid ; border-top-style: solid ;    border-right-style: solid ;     border-left-style: solid ;">
		<th style="width:39.75pt;font-size:10.0pt;font-family:'Arial','sans-serif';mso-ansi-language:EN-GB;border-collapse: collapse; border: 1px black; border-bottom-style: solid ; border-top-style: solid ;    border-right-style: solid ;     border-left-style: solid ;">Item</th>
		<th style="width:400pt;font-size:10.0pt;font-family:'Arial','sans-serif';mso-ansi-language:EN-GB;border-collapse: collapse; border: 1px black; border-bottom-style: solid ; border-top-style: solid ;    border-right-style: solid ;     border-left-style: solid ;">Test Description</th>
		<th style="width:200pt;font-size:10.0pt;font-family:'Arial','sans-serif';mso-ansi-language:EN-GB;border-collapse: collapse; border: 1px black; border-bottom-style: solid ; border-top-style: solid ;    border-right-style: solid ;     border-left-style: solid ;">Test Result</th>
		<th style="width:200pt;font-size:10.0pt;font-family:'Arial','sans-serif';mso-ansi-language:EN-GB;border-collapse: collapse; border: 1px black; border-bottom-style: solid ; border-top-style: solid ;    border-right-style: solid ;     border-left-style: solid ;">Remarks</th>
	</tr>
	<xsl:for-each select="LogInfo/PROC/title">
		<p>
			<tr>
				<td valign="top" style=" border-collapse: collapse; border: 1px black;border-right-style:solid;border-bottom-style:none;border-top-style: solid ;padding:0cm 5.4pt 0cm 5.4pt">
					<br />
					<p class="MsoNormal" align="center" style="margin-bottom:0cm;margin-bottom:.0001pt;  text-align:center;line-height:10.0pt;mso-line-height-rule:exactly;mso-layout-grid-align:  none;text-autospace:none">
						<b>
							<span lang="DE" style="font-size:8.0pt;font-family:'Arial','sans-serif';mso-ansi-language:  DE">
								<SCRIPT type="text/javascript">
													var x = getItemIdx();
													document.write("&lt;h5 id='item"+x+"'>"+x+"&lt;/h2>");
						</SCRIPT>
					</span>
				</b>
			</p>
		</td>
		<td valign="top" style=" border-collapse: collapse; border: 1px black;border-right-style:solid;border-bottom-style:none;border-top-style: solid ;padding:0cm 5.4pt 0cm 5.4pt">
			<xsl:if test="@Heading!=''">
				<!--
										<p class="MsoNormal" style='margin-top:0.5cm;margin-bottom:.0001pt;line-height:  10.0pt;mso-line-height-rule:exactly;mso-layout-grid-align:none;text-autospace:  none'>
											-->
				<br />
				<b>
					<u>
						<span lang="EN-US" style="font-size:8.0pt;font-family:'Arial','sans-serif';  color:black;mso-ansi-language:EN-US">
							<xsl:value-of select="@Heading" />
							<br />
						</span>
					</u>
				</b>
				<!--
										</p>
												-->
				<xsl:if test="@commandComment!=''">
					<i style="mso-bidi-font-style:normal">
						<span lang="EN-US" style="font-size:8.0pt;font-family:'Arial','sans-serif';  color:#666666;mso-ansi-language:EN-US;mso-bidi-font-weight:bold">
										"<xsl:value-of select="@commandComment" /> " 
						</span>
					</i>
				</xsl:if>
				<br class="MsoNormal" style="margin-bottom:0cm;margin-bottom:.0001pt;line-height:10.0pt;mso-line-height-rule:exactly;mso-layout-grid-align:none;text-autospace:  none">
					<u>
						<span lang="EN-US" style="font-size:8.0pt;font-family:'Arial','sans-serif';  color:black;mso-ansi-language:EN-US;mso-bidi-font-weight:bold">
							<span style="text-decoration:none">   </span>
						</span>
					</u>
				</br>
				<br class="MsoNormal" style="margin-bottom:0cm;margin-bottom:.0001pt;line-height:10.0pt;mso-line-height-rule:exactly;mso-layout-grid-align:none;text-autospace:  none">
					<u>
						<span lang="EN-US" style="font-size:8.0pt;font-family:'Arial','sans-serif';  color:black;mso-ansi-language:EN-US;mso-bidi-font-weight:bold">
							<span style="text-decoration:none">   </span>
						</span>
					</u>
				</br>
			</xsl:if>
		</td>
		<td valign="top" style=" border-collapse: collapse; border: 1px black;border-right-style:solid;border-bottom-style:none;border-top-style: solid ;padding:0cm 5.4pt 0cm 5.4pt" />
		<td valign="top" style=" border-collapse: collapse; border: 1px black;border-bottom-style:none;border-top-style: solid ;padding:0cm 5.4pt 0cm 5.4pt" />
	</tr>
	<xsl:for-each select="*">
		<xsl:variable name="elementName1" select="name()" />
		<xsl:if test="$elementName1='ACTIONS'">
			<tr>
				<td />
				<td valign="top" style="border:solid windowtext 1.0pt;  border-bottom:none; border-top:none;mso-border-top-alt:solid windowtext .5pt;mso-border-alt:solid windowtext .5pt;  padding:0cm 5.4pt 0cm 5.4pt">
					<br class="MsoNormal" style="margin-bottom:0.5cm;line-height:10.0pt;mso-line-height-rule:exactly;mso-layout-grid-align:none;text-autospace:  none">
						<u>
							<span lang="EN-US" style="font-size:8.0pt;font-family:'Arial','sans-serif'; color:black;mso-ansi-language:EN-US;mso-bidi-font-weight:bold">
								<b>ACTION:</b>
							</span>
						</u>
					</br>
				</td>
				<td valign="top" style="border:solid windowtext 1.0pt;   border-bottom:none;border-top:none;mso-border-top-alt:solid windowtext .5pt;mso-border-alt:solid windowtext .5pt;  padding:0cm 5.4pt 0cm 5.4pt" />
				<td valign="top" style="border:solid windowtext 1.0pt;   border-bottom:none;border-top:none;mso-border-top-alt:solid windowtext .5pt;mso-border-alt:solid windowtext .5pt;  padding:0cm 5.4pt 0cm 5.4pt" />
			</tr>
		</xsl:if>
		<xsl:if test="$elementName1='RESULTS'">
			<tr>
				<td valign="top" style="border:solid windowtext 1.0pt;   border-bottom:none;border-top:none;mso-border-top-alt:solid windowtext .5pt;mso-border-alt:solid windowtext .5pt;  padding:0cm 5.4pt 0cm 5.4pt" />
				<td valign="top" style="border:solid windowtext 1.0pt;  border-bottom:none; border-top:none;mso-border-top-alt:solid windowtext .5pt;mso-border-alt:solid windowtext .5pt;  padding:0cm 5.4pt 0cm 5.4pt">
					<br class="MsoNormal" style="margin-bottom:0.5cm;line-height:10.0pt;mso-line-height-rule:exactly;mso-layout-grid-align:none;text-autospace:  none">
						<u>
							<span lang="EN-US" style="font-size:8.0pt;font-family:'Arial','sans-serif'; color:black;mso-ansi-language:EN-US;mso-bidi-font-weight:bold">
								<b>RESULT:</b>
							</span>
						</u>
					</br>
				</td>
				<td valign="top" style="border:solid windowtext 1.0pt;   border-bottom:none;border-top:none;mso-border-top-alt:solid windowtext .5pt;mso-border-alt:solid windowtext .5pt;  padding:0cm 5.4pt 0cm 5.4pt" />
				<td valign="top" style="border:solid windowtext 1.0pt;   border-bottom:none;border-top:none;mso-border-top-alt:solid windowtext .5pt;mso-border-alt:solid windowtext .5pt;  padding:0cm 5.4pt 0cm 5.4pt" />
			</tr>
		</xsl:if>
		<xsl:for-each select="*">
			<xsl:variable name="elementName2" select="name()" />
			<xsl:variable name="commandComment" select="@commandComment" />
			<xsl:variable name="userLogcomment" select="@userLogcomment" />
			<xsl:variable name="actcomp" select="@actcomp" />
			<xsl:variable name="indent" select="@indent" />
			<xsl:variable name="timeOut" select="@timeOut" />
			<xsl:variable name="confirmationTime" select="@confirmationTime" />
			<xsl:variable name="description" select="@description" />
			<xsl:variable name="timestamp" select="timestamp" />
			<xsl:variable name="status" select="status" />
			<xsl:variable name="statement" select="statement" />
			<xsl:variable name="otherDetails" select="otherDetails" />
			<xsl:if test="$statement!=''">
				<tr>
					<td valign="top" style="border:solid windowtext 1.0pt;   border-bottom:none;border-top:none;mso-border-top-alt:solid windowtext .5pt;mso-border-alt:solid windowtext .5pt;  padding:0cm 5.4pt 0cm 5.4pt" />
					<td valign="top" style="border:solid windowtext 1.0pt;   border-bottom:none;border-top:none;mso-border-top-alt:solid windowtext .5pt;mso-border-alt:solid windowtext .5pt;  padding:0cm 5.4pt 0cm 5.4pt">
						<span lang="EN-GB" style="font-size:8.0pt;font-family:  'Arial','sans-serif';color:black;mso-ansi-language:EN-GB;mso-bidi-font-weight:  bold">
							<span lang="EN-GB" style="font-size:8.0pt;font-family:'Arial','sans-serif';color:black;  mso-ansi-language:EN-GB;mso-bidi-font-weight:bold">
														+ 
							</span>
							<xsl:if test="$indent &gt;0">
								<script>
																document.write(intendTo(<xsl:value-of select="@indent" />));
								</script>
							</xsl:if>
							<xsl:if test="$description!=''">
								<xsl:value-of select="$description" /> &#160;
							</xsl:if>
							<script>
											var stmt = '<xsl:value-of select="normalize-space($statement)"/>';
											stmt = stmt.replace("Function Status", "Status");
											stmt = stmt.replace("(Precision:0.0000)","");
											document.write(stmt);
							</script>
							<xsl:if test="($timeOut != '' and $timeOut!='0sec')or ($confirmationTime != '' and $confirmationTime != '0sec')">
										(
								<xsl:if test="$timeOut != '0sec' and $timeOut != '0msec' and $timeOut != ''">
													Time Out:
									<xsl:value-of select="$timeOut"/>
								</xsl:if >
								<xsl:if test="$confirmationTime != '0sec' and $confirmationTime != '0msec' and $confirmationTime != ''">
													Confirmation Time:												
									<xsl:value-of select="$confirmationTime"/>
								</xsl:if >
										)
							</xsl:if >
						</span>
					</td>
					<td valign="top" style="border:solid windowtext 1.0pt;   border-bottom:none;border-top:none;mso-border-top-alt:solid windowtext .5pt;mso-border-alt:solid windowtext .5pt;  padding:0cm 5.4pt 0cm 5.4pt">
						<xsl:if test="$status = 'OK' and $elementName2='ACT'">
							<xsl:if test="$actcomp != 'condition_if' and $actcomp != 'condition_while'">
								<span style="font-size:8.0pt;font-family:'MS Gothic';mso-bidi-font-family:  'MS Gothic';color:green;mso-ansi-language:EN-US;mso-bidi-font-weight:bold">
																		  ✔ 
								</span>
							</xsl:if>
							<xsl:if test="$actcomp = 'condition_if' or $actcomp = 'condition_while'">
								<span style="font-size:8.0pt;font-family:'Arial';mso-bidi-font-family:  'MS Gothic';color:green;mso-ansi-language:EN-US;mso-bidi-font-weight:bold">
																		  <i>
										<b>true</b>
									</i>
								</span>									
							</xsl:if>
						</xsl:if>
						<xsl:if test="$status != 'OK' and $status!='NA' and $elementName2='ACT'">
							<xsl:if test="$actcomp != 'condition_if' and $actcomp != 'condition_while'">
								<span style="font-size:8.0pt;font-family:'Arial';mso-bidi-font-family:  'MS Gothic';color:red;mso-ansi-language:EN-US;mso-bidi-font-weight:bold">
									<i>
										<b>    X     </b>
									</i>
								</span>
							</xsl:if>
							<xsl:if test="$actcomp = 'condition_if' or $actcomp = 'condition_while'">
								<span style="font-size:8.0pt;font-family:'Arial';mso-bidi-font-family:  'MS Gothic';color:red;mso-ansi-language:EN-US;mso-bidi-font-weight:bold">
									<i>
										<b>    false     </b>
									</i>
								</span>									
							</xsl:if>									
						</xsl:if>
						<xsl:if test="$status = 'OK' and $elementName2='CHK'">
							<span style="font-size:8.0pt;font-family:'MS Gothic';mso-bidi-font-family:  'MS Gothic';color:green;mso-ansi-language:EN-US;mso-bidi-font-weight:bold">
								<b>					  OK</b>
							</span>
						</xsl:if>
						<xsl:if test="$status != 'OK' and $status!='NA' and $elementName2='CHK'">
							<span style="font-size:8.0pt;font-family:'Arial';mso-bidi-font-family:  'MS Gothic';color:red;mso-ansi-language:EN-US;mso-bidi-font-weight:bold">
								<i>
									<b>    NOK</b>
								</i>
							</span>
						</xsl:if>
						<i style="mso-bidi-font-style:normal">
							<span lang="EN-US" style="font-size:8.0pt;  font-family:'Arial','sans-serif';color:#595959;mso-ansi-language:EN-US;  mso-bidi-font-weight:bold">
								<span style="mso-spacerun:yes"> 
								</span>
													   <script>
													document.write(getTimeInfo('<xsl:value-of select="$timestamp" />'));
								</script>

							</span>
						</i>
						<!-- 						<i style="mso-bidi-font-style:normal">
							<span lang="EN-US" style="font-size:8.0pt;  font-family:'Arial','sans-serif';color:#595959;mso-ansi-language:EN-US;  mso-bidi-font-weight:bold">
								<span style="mso-spacerun:yes"> 
								</span>
								<xsl:for-each select="otherDetails/info">
									<xsl:variable name="paramName" select="paramName"/>
									<xsl:variable name="paramVal" select="value"/>
									<xsl:if test="$paramName!='' and $paramVal !=''">
										<xsl:if test="$actcomp !='set' or $status !='OK'">
											<xsl:variable name="paramName1" select="concat(normalize-space($paramName), '')" />
											<xsl:variable name="paramvalue1" select="concat(normalize-space($paramVal), '')" />
											<script>
																		var paramName11 = '<xsl:value-of select="$paramName1"/>';
																		var paramvalue11 = '<xsl:value-of select="$paramvalue1"/>';
																		var otherinfo11 = paramName11+"="+paramvalue11;
																		var re = /\"/g;
																		otherinfo11 = otherinfo11.replace(re,"");																		
																		document.write('&lt;a style="cursor:help;" href="#" onClick="getMoreInfo(this)" title="');
																		document.write(otherinfo11);
																		document.write('"&gt;Info&lt;/a&gt;');
											</script>
										</xsl:if>
									</xsl:if>									
								</xsl:for-each>
							</span>
						</i> -->
					</td>
					<td valign="top" style="border:solid windowtext 1.0pt;  border-left:none; border-bottom:none;border-top:none;mso-border-top-alt:solid windowtext .5pt;mso-border-alt:solid windowtext .5pt;  padding:0cm 5.4pt 0cm 5.4pt">
						<i style="mso-bidi-font-style:normal;color:white">
							<span lang="EN-US" style="font-size:8.0pt;  font-family:'Arial','sans-serif';color:#595959;mso-ansi-language:EN-US;  mso-bidi-font-weight:bold">
								<span style="mso-spacerun:yes"> 
								</span>
								<xsl:choose>
									<xsl:when test="$status = 'OK' and $elementName2='CHK'">
										<xsl:for-each select="otherDetails/info">
											<xsl:variable name="paramName" select="paramName"/>
											<xsl:variable name="paramVal" select="value"/>
											<xsl:if test="$paramName!='' and $paramVal !=''">
												<xsl:if test="$actcomp !='set' or $status !='OK'">
												<span style="font-size:8.0pt;font-family:'Arial';mso-bidi-font-family:  'MS Gothic';color:grey;mso-ansi-language:EN-US;mso-bidi-font-weight:bold">
													<script>
														var ParamName = '<xsl:value-of select="$paramName" />';
														var Paramvalue = '<xsl:value-of select="$paramVal" />';
														var temp = "";
											if( Paramvalue.indexOf('Timestamp') >= 0)
											{
												valArray = Paramvalue.split(",");
												for(i=0;i&lt;valArray.length;i++)
												{
													if(valArray[i].indexOf('Timestamp') &lt; 0)
													temp +=valArray[i]+",";
												}
											}
											var stmt = '<xsl:value-of select="normalize-space($statement)"/>';
											stmt = stmt.replace("Function Status", "Status");
											stmt = stmt.replace("(Precision:0.0000)","");
											if(stmt.indexOf('Precision') > 0)
												document.write(ParamName+"="+temp);
													</script>
												</span>
												<br/>
											</xsl:if>
										</xsl:if>
									</xsl:for-each>											
								</xsl:when>
								<xsl:when test="$actcomp = 'Value' and $status !='OK'">
									<xsl:for-each select="otherDetails/info">
										<xsl:variable name="paramName" select="paramName"/>
										<xsl:variable name="paramVal" select="value"/>
										<xsl:if test="$paramName!='' and $paramVal !=''">
											<xsl:if test="$actcomp !='set' or $status !='OK'">
												<xsl:value-of select="$paramName" />
														=
												<span style="font-size:8.0pt;font-family:'Arial';mso-bidi-font-family:  'MS Gothic';color:grey;mso-ansi-language:EN-US;mso-bidi-font-weight:bold">
													<script>
											var Paramvalue = '<xsl:value-of select="$paramVal" />';
											var temp = "";
											if( Paramvalue.indexOf('Timestamp') >= 0)
											{
												valArray = Paramvalue.split(",");
												for(i=0;i&lt;valArray.length;i++)
												{
													if(valArray[i].indexOf('Timestamp') &lt; 0)
													temp +=valArray[i]+",";
												}
											}
											document.write(temp);
												</script>
											</span>
											<br/>
										</xsl:if>
									</xsl:if>
								</xsl:for-each>									  
							</xsl:when>
							<xsl:otherwise>
									<xsl:for-each select="otherDetails/info">
										<xsl:variable name="paramName" select="paramName"/>
										<xsl:variable name="paramVal" select="value"/>
										<xsl:if test="$paramName!='' and $paramVal !=''">
											<xsl:if test="$actcomp !='set' or $status !='OK'">
												<xsl:value-of select="$paramName" />
														=
												<span style="font-size:8.0pt;font-family:'Arial';mso-bidi-font-family:  'MS Gothic';color:grey;mso-ansi-language:EN-US;mso-bidi-font-weight:bold">
													<script>
											var Paramvalue = '<xsl:value-of select="$paramVal" />';
											var temp = "";
											if( Paramvalue.indexOf('Timestamp') >= 0)
											{
												valArray = Paramvalue.split(",");
												for(i=0;i&lt;valArray.length;i++)
												{
													if(valArray[i].indexOf('Timestamp') &lt; 0)
													temp +=valArray[i]+",";
												}
											}
											document.write(temp);
												</script>
											</span>
											<br/>
										</xsl:if>
									</xsl:if>
								</xsl:for-each>							
							</xsl:otherwise>
						</xsl:choose>								

					</span>
				</i>
				<xsl:for-each select="REFERENCES/REFERENCE">
					<xsl:variable name="refType" select="@type"/>
					<xsl:variable name="refPath" select="@source"/>
					<i style="mso-bidi-font-style:normal;color:white">
						<span lang="EN-US" style="font-size:8.0pt;  font-family:'Arial','sans-serif';color:#595959;mso-ansi-language:EN-US;  mso-bidi-font-weight:bold">
							<span style="mso-spacerun:yes"> 
							</span>
							<xsl:if test="$refPath!=''">
								<a target="_blank" href="{$refPath}" >View Image</a>
							</xsl:if>
						</span>
					</i>
				</xsl:for-each>					  
			</td>
		</tr>
	</xsl:if>
	<xsl:if test="$commandComment != ''">
		<tr>
			<td valign="top" style="border:solid windowtext 1.0pt;   border-bottom:none;border-top:none;mso-border-top-alt:solid windowtext .5pt;mso-border-alt:solid windowtext .5pt;  padding:0cm 5.4pt 0cm 5.4pt" />
			<td valign="top" style="border:solid windowtext 1.0pt;  border-bottom:none; border-top:none;mso-border-top-alt:solid windowtext .5pt;mso-border-alt:solid windowtext .5pt;  padding:0cm 5.4pt 0cm 5.4pt">
				<i style="mso-bidi-font-style:normal">
					<span lang="EN-GB" style="font-size:8.0pt;font-family:'Arial','sans-serif';color:grey;  mso-ansi-language:EN-GB;mso-bidi-font-weight:bold">


						<xsl:if test="$indent &gt;0">
							<script>
																document.write(intendTo(<xsl:value-of select="@indent" />));
							</script>
						</xsl:if>	
															" 


						<xsl:value-of select="$commandComment" />
															"
					</span>
				</i>
			</td>
			<td valign="top" style="border:solid windowtext 1.0pt;   border-bottom:none;border-top:none;mso-border-top-alt:solid windowtext .5pt;mso-border-alt:solid windowtext .5pt;  padding:0cm 5.4pt 0cm 5.4pt" />
			<td valign="top" style="border:solid windowtext 1.0pt;   border-bottom:none;border-top:none;mso-border-top-alt:solid windowtext .5pt;mso-border-alt:solid windowtext .5pt;  padding:0cm 5.4pt 0cm 5.4pt" />
		</tr>
	</xsl:if>						
</xsl:for-each>
</xsl:for-each>
</p>
</xsl:for-each>
</table>
<h2 />
<table width="1100" align="center" style="border-collapse: collapse; border: 1px black;border-top-style: solid ;    border-right-style: solid ; border-bottom-style: none ;    border-left-style: solid ;">
	<tr>
		<td align="center" style="font-size:8.0pt;font-family:'Arial','sans-serif';mso-ansi-language:EN-GB">
			<i>
				<b>Attachments</b>
			</i>
		</td>
	</tr>
</table>
<table width="1100" align="center" style="border-collapse: collapse; border: 1px black;border-top-style: solid ;    border-right-style: solid ; border-bottom-style: solid ;    border-left-style: solid ;">
	<tr>
		<td align="center" style="width:250pt;font-size:8.0pt;font-family:'Arial','sans-serif';mso-ansi-language:EN-GB; border: 1px black;border-top-style: solid ;    border-right-style: solid ; border-bottom-style: solid ;    border-left-style: solid ;">
					Name
		</td>
		<td align="center" style="font-size:8.0pt;font-family:'Arial','sans-serif';mso-ansi-language:EN-GB;border: 1px black;border-top-style: solid ;    border-right-style: solid ; border-bottom-style: solid ;    border-left-style: solid ;">
					Path
		</td>
	</tr>
	<xsl:for-each select="LogInfo/ATTACHEMENTS/FILE">
		<tr>
			<td align="left" style="font-size:8.0pt;font-family:'Arial','sans-serif';mso-ansi-language:EN-GB;border: 1px black;border-top-style: solid ;    border-right-style: solid ; border-bottom-style: solid ;    border-left-style: solid ;">
				<xsl:value-of select="@name" />
			</td>
			<td align="left" style="font-size:8.0pt;font-family:'Arial','sans-serif';mso-ansi-language:EN-GB;border: 1px black;border-top-style: solid ;    border-right-style: solid ; border-bottom-style: solid ;    border-left-style: solid ;">
				<script>
				document.write('&lt;a target="_blank" title="<xsl:value-of select="@name" />" href="<xsl:value-of select="@path" />"&gt;');
				document.write("<xsl:value-of select="@path" />&lt;/a&gt;");
				</script>
			</td>
		</tr>
	</xsl:for-each>
</table>
<p />
<table width="1100" align="center" style="border-collapse: collapse; border: 1px black;border-top-style: solid ;    border-bottom-style: solid ; border-right-style: solid ;  border-left-style: solid ;">
	<tr>
		<td align="center" style="font-size:8.0pt;font-family:'Arial','sans-serif';mso-ansi-language:EN-GB">
			<i>
				<b>Parameters Used</b>
			</i>
		</td>
	</tr>
</table>
<table width="1100" align="center" style="border-collapse: collapse; border: 1px black;border-top-style: none ;    border-bottom-style: solid ; border-right-style: solid ;  border-left-style: solid ;">
	<xsl:for-each select="LogInfo/PARAMETERLIST/parameter">
		<tr>
			<td align="left" style="font-size:8.0pt;font-family:'Arial','sans-serif';mso-ansi-language:EN-GB;border: 1px black;border-top-style: solid ;    border-right-style: solid ; border-bottom-style: solid ;    border-left-style: solid ;">
<script>				
				document.write('&lt;a target="_blank" title="ParameterList" href="<xsl:value-of select="."/>"&gt;');
				document.write("<xsl:value-of select="."/>&lt;/a&gt;");
</script>				
			</td>
		</tr>				
	</xsl:for-each>
</table>
</body>
</html>
</xsl:template>
</xsl:stylesheet>