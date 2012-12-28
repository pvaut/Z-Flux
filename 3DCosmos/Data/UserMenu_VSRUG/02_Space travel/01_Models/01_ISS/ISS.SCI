codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
codeblock readtextfile(ScriptDir+"\_SSYS.sci");
codeblock readtextfile(ScriptDir+"\space\models\_animatemodel.sci");


tr=transformation;
tr.rotate(vector(1,0,0),-0.5*Pi);
tr.translate(vector(-4,-2,0));


animatemodel(datadir+"\Models\ISS_Building","isscomplete","3DS",tr,3.1,true);


