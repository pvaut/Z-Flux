
translstrings=list;
translatedstrings=list;
translmap=map;

function addtranslationstring(st)
{
   if not(translmap.IsPresent(st)) then {
      translmap.AddItem(st:(translstrings.size));
      translstrings.add(st);
   }
}

function reset_translations()
{
   translatedstrings.clear;
   foreach st in translstrings do translatedstrings.add("");
}

function addtranslation(txt_orig,txt_transl)
{
   if not(translmap.IsPresent(txt_orig)) then return(0);
   idx=translmap.Get(txt_orig);
   translatedstrings(idx)=txt_transl;
}


startfolderinfo=map;
startfolderinfo.Folder=DataDir+"\Scripts";
startfolderinfo.Name="";
startfolderinfo.Parent="";
startfolderinfo.Level=-1;
todofolderlist=list(startfolderinfo);
while todofolderlist.size>0 do {
   currentfolderinfo=todofolderlist(todofolderlist.size-1);
   todofolderlist.del(todofolderlist.size-1);
   currentfoldername=currentfolderinfo.name;
   if currentfoldername(0)!="_" then {
      if currentfoldername.find("_")>0 then currentfoldername.split("_");
      addtranslationstring(currentfoldername);
   }
   #put subfolders in the todo stack
   folderlist=GetFileList(currentfolderinfo.Folder+"\*.*",true);
   folderlist.Invert;
   foreach subfolder in folderlist do 
      if (subfolder!=".") and (subfolder!="..") then {
         subfolderinfo=map;
         subfolderinfo.Folder=currentfolderinfo.Folder+"\"+subfolder;
         subfolderinfo.Name=Deref(subfolder);
         subfolderinfo.Parent=currentfolderinfo.Folder;
         subfolderinfo.Level=currentfolderinfo.Level+1;
         todofolderlist.add(subfolderinfo);
      }
      
   scriptfilelist=GetFileList(currentfolderinfo.folder+"\*.SCI",false);
   foreach scriptfile in scriptfilelist do {
      if scriptfile(0)!="_" then {
         scriptname=scriptfile;scriptname.invert;scriptname.split(".");scriptname.invert;
        if scriptname.find("_")>0 then scriptname.split("_");
         addtranslationstring(scriptname);
      }
      content=readtextfile(currentfolderinfo.folder+"\"+scriptfile);
      while content.Length>0 do {
         line=content.split("~n");
         if line.find("#")>=0 then line.split("#");
         if line.find("~'")>=0 then {
            line.split("~'");
            st=line.split("~'");
            addtranslationstring(st);
         }
         
      }
   }
}


st="";
foreach s2 in translstrings do {
   st=st+s2+"~n";
}
writetextfile(DataDir+"\Languages\all.txt",st);


#process all languages
languagelist=GetFileList(datadir+"\Languages\*.*",true);
foreach language in languagelist do {
   if (language!=".") and (language!="..") then {
      reset_translations;
      content=readtextfile(datadir+"\Languages\"+language+"\scripts.txt");
      while content.Length>0 do {
         line=content.split("~n");
         if line.length>2 then {
            txt_orig=line.split("~t");
            txt_transl=line;
            addtranslation(txt_orig,txt_transl);
         }
      }
      content="";
      for i=0 to translstrings.size-1 do
         if translatedstrings(i).length>0 then
            content=content+translstrings(i)+"~t"+translatedstrings(i)+"~n";
      content=content+"=================================================";
      for i=0 to translstrings.size-1 do
         if translatedstrings(i).length==0 then
            content=content+translstrings(i)+"~t"+translatedstrings(i)+"~n";
      writetextfile(datadir+"\Languages\"+language+"\scripts.txt",content);

   }
}




