codeblock readtextfile(ScriptDir+"\_Tools\BrowseFile.sci");

filename=BrowseFile("SCI");
if filename!="" then
   ExecuteScript(filename);