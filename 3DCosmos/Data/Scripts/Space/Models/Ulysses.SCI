codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
codeblock readtextfile(ScriptDir+"\_SSYS.sci");
codeblock readtextfile(ScriptDir+"\space\models\_animatemodel.sci");


tr=transformation;
tr.rotate(vector(1,0,0),-0.5*Pi);


animatemodel(datadir+"\Models\Ulysses","ulysses","3DS",tr,0.03);


