codeblock readtextfile(ScriptDir+"\_Tools\BrowseFile.sci");

filename=BrowseFile("JPG");
if filename=="" then stop;

filename.Invert;
filename.split("\");
filename.Invert;
ExecuteScript(scriptdir+"\Stereo pictures\99_Create Thumbnails.SCI",filename);
ExecuteScript(scriptdir+"\Stereo pictures\01_Stereo pictures.SCI",filename);


