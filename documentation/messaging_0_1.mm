<map version="freeplane 1.3.0">
<!--To view this file, download free mind mapping software Freeplane from http://freeplane.sourceforge.net -->
<node TEXT="messaging functionality" ID="ID_1723255651" CREATED="1283093380553" MODIFIED="1539094376915"><hook NAME="MapStyle">

<map_styles>
<stylenode LOCALIZED_TEXT="styles.root_node">
<stylenode LOCALIZED_TEXT="styles.predefined" POSITION="right">
<stylenode LOCALIZED_TEXT="default" MAX_WIDTH="600" COLOR="#000000" STYLE="as_parent">
<font NAME="SansSerif" SIZE="10" BOLD="false" ITALIC="false"/>
</stylenode>
<stylenode LOCALIZED_TEXT="defaultstyle.details"/>
<stylenode LOCALIZED_TEXT="defaultstyle.note"/>
<stylenode LOCALIZED_TEXT="defaultstyle.floating">
<edge STYLE="hide_edge"/>
<cloud COLOR="#f0f0f0" SHAPE="ROUND_RECT"/>
</stylenode>
</stylenode>
<stylenode LOCALIZED_TEXT="styles.user-defined" POSITION="right">
<stylenode LOCALIZED_TEXT="styles.topic" COLOR="#18898b" STYLE="fork">
<font NAME="Liberation Sans" SIZE="10" BOLD="true"/>
</stylenode>
<stylenode LOCALIZED_TEXT="styles.subtopic" COLOR="#cc3300" STYLE="fork">
<font NAME="Liberation Sans" SIZE="10" BOLD="true"/>
</stylenode>
<stylenode LOCALIZED_TEXT="styles.subsubtopic" COLOR="#669900">
<font NAME="Liberation Sans" SIZE="10" BOLD="true"/>
</stylenode>
<stylenode LOCALIZED_TEXT="styles.important">
<icon BUILTIN="yes"/>
</stylenode>
</stylenode>
<stylenode LOCALIZED_TEXT="styles.AutomaticLayout" POSITION="right">
<stylenode LOCALIZED_TEXT="AutomaticLayout.level.root" COLOR="#000000">
<font SIZE="18"/>
</stylenode>
<stylenode LOCALIZED_TEXT="AutomaticLayout.level,1" COLOR="#0033ff">
<font SIZE="16"/>
</stylenode>
<stylenode LOCALIZED_TEXT="AutomaticLayout.level,2" COLOR="#00b439">
<font SIZE="14"/>
</stylenode>
<stylenode LOCALIZED_TEXT="AutomaticLayout.level,3" COLOR="#990000">
<font SIZE="12"/>
</stylenode>
<stylenode LOCALIZED_TEXT="AutomaticLayout.level,4" COLOR="#111111">
<font SIZE="10"/>
</stylenode>
</stylenode>
</stylenode>
</map_styles>
</hook>
<hook NAME="AutomaticEdgeColor" COUNTER="12"/>
<node TEXT="performance" POSITION="right" ID="ID_1772077988" CREATED="1539094391487" MODIFIED="1539094395799">
<edge COLOR="#00ff00"/>
<node TEXT="&quot;realtime&quot; on robot" ID="ID_837104830" CREATED="1539094654281" MODIFIED="1539094696636"/>
<node TEXT="fast to cockpit" ID="ID_207334655" CREATED="1539094698870" MODIFIED="1539094709164"/>
</node>
<node TEXT="reliabiility" POSITION="right" ID="ID_1188482489" CREATED="1539094396631" MODIFIED="1539094403042">
<edge COLOR="#ff00ff"/>
<node TEXT="all messages arrive completely" ID="ID_1377580205" CREATED="1539094717348" MODIFIED="1539094730154"/>
<node TEXT="if not can request retransmission" ID="ID_851007441" CREATED="1539094731189" MODIFIED="1539094744570"/>
</node>
<node TEXT="PC Cockpit" POSITION="left" ID="ID_679202860" CREATED="1539094795175" MODIFIED="1539095451980">
<edge COLOR="#007c00"/>
<node TEXT="displays information" ID="ID_776400783" CREATED="1539094818107" MODIFIED="1539094832228"/>
<node TEXT="&quot;relative&quot; realtime" ID="ID_1932191313" CREATED="1539094832878" MODIFIED="1539094847436"/>
<node TEXT="logging = rosbag" ID="ID_274625828" CREATED="1539094854369" MODIFIED="1539094861557"/>
<node TEXT="enables data analysis / testing" ID="ID_1682641344" CREATED="1539095468743" MODIFIED="1539095493313"/>
</node>
<node TEXT="different platforms" POSITION="right" ID="ID_1050201394" CREATED="1539094379651" MODIFIED="1539094386305">
<edge COLOR="#ff0000"/>
<node TEXT="PC Cockpit" ID="ID_1999665823" CREATED="1539094497865" MODIFIED="1539094512335"/>
<node TEXT="RPI" ID="ID_1324288044" CREATED="1539094514058" MODIFIED="1539094518642"/>
<node TEXT="Arduino" ID="ID_1493908411" CREATED="1539094519589" MODIFIED="1539094524686"/>
<node TEXT="Jetson" ID="ID_30848662" CREATED="1539094525303" MODIFIED="1539094527206"/>
<node TEXT="Other" ID="ID_1347280590" CREATED="1539094527967" MODIFIED="1539094540822"/>
</node>
<node TEXT="message types" POSITION="right" ID="ID_1721370860" CREATED="1539094386825" MODIFIED="1539094390887">
<edge COLOR="#0000ff"/>
<node TEXT="network = tcp" ID="ID_1383260836" CREATED="1539094548926" MODIFIED="1539094571244"/>
<node TEXT="between embedded and PC" ID="ID_486429073" CREATED="1539094572198" MODIFIED="1539094582644"/>
<node TEXT="between sensors and connected" ID="ID_485360638" CREATED="1539094583701" MODIFIED="1539094624400"/>
<node TEXT="infrastruture (i.e. snapshot timing)" ID="ID_214295864" CREATED="1539094627249" MODIFIED="1539094648215"/>
</node>
<node TEXT="messages" POSITION="left" ID="ID_26264656" CREATED="1539095618783" MODIFIED="1539095622734">
<edge COLOR="#007c7c"/>
<node TEXT="content" ID="ID_1689162687" CREATED="1539095628526" MODIFIED="1539095658964">
<node TEXT="header (data about the message)" ID="ID_963884807" CREATED="1539095717435" MODIFIED="1539095762318"/>
<node TEXT="metadata (data about the data)" ID="ID_1074719666" CREATED="1539095734952" MODIFIED="1539095746679"/>
<node TEXT="data" ID="ID_1512802688" CREATED="1539095720930" MODIFIED="1539095722816"/>
</node>
<node TEXT="size" ID="ID_1615481041" CREATED="1539095660748" MODIFIED="1539095662504">
<node TEXT="short messages" ID="ID_873974496" CREATED="1539095689596" MODIFIED="1539095693891"/>
<node TEXT="long messages" ID="ID_1755992991" CREATED="1539095694540" MODIFIED="1539095697539"/>
</node>
</node>
<node TEXT="sender / receiver" POSITION="left" ID="ID_1692444748" CREATED="1539095591400" MODIFIED="1539095602578">
<edge COLOR="#7c007c"/>
</node>
</node>
</map>
