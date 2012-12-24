codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
codeblock readtextfile(ScriptDir+"\_SSYS.sci");
codeblock readtextfile(ScriptDir+"\space\models\_animatemodel.sci");


tr=transformation;
tr.translate(vector(0,0,0.4));


animatemodel(datadir+"\Models\Cassini","cassini","3DS",tr,3);


