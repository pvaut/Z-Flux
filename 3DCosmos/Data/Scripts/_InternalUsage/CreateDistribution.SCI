

function copyfileset(sourcepath,destpath,wildcard,copysubdirs,denyfolderlist,denyfilelist)
{
   dirstack=list;
   dirstack.add("");
   
   denyfoldermap=map;
   if isvardefined("denyfolderlist") then
      foreach folder in denyfolderlist do denyfoldermap.AddItem(folder:1);
   denyfilemap=map;
   if isvardefined("denyfilelist") then
      foreach file in denyfilelist do denyfilemap.AddItem(file:1);
   
   while dirstack.size>0 do {
      curdir=dirstack(dirstack.size-1);dirstack.del(dirstack.size-1);
      CreateDirectory(destpath+curdir);
      if copysubdirs then {
         dirlist=GetFileList(sourcepath+curdir+"\*.*",true);
         foreach dir in dirlist do 
            if (dir!=".") and (dir!="..") and not(denyfoldermap.IsPresent(dir)) then
               dirstack.add(curdir+"\"+dir);
      }
      filelist=GetFileList(sourcepath+curdir+"\"+wildcard,false);
      foreach file in filelist do
         if not(denyfilemap.IsPresent(file)) then
            copyfile(sourcepath+curdir+"\"+file,destpath+curdir+"\"+file);
   }
}


sourcepath=ExecDir;
destpath="D:\Z-Flux\DistributionFull";



CreateDirectory(destpath);
CreateDirectory(destpath+"\data");
CreateDirectory(destpath+"\data\AnimationSets");
CreateDirectory(destpath+"\data\Models");
CreateDirectory(destpath+"\data\SlideShows");
CreateDirectory(destpath+"\data\StereoPictures");
CopyFile(sourcepath+"\z-flux.exe",destpath+"\z-flux.exe");
CopyFile(sourcepath+"\scriptlang.flf",destpath+"\scriptlang.flf");
CopyFile(sourcepath+"\documentation\manual.pdf",destpath+"\manual.pdf");
CopyFile(sourcepath+"\data\basichelp.txt",destpath+"\data\basichelp.txt");


writetextfile(destpath+"\data\openscripts.txt","0");
writetextfile(destpath+"\data\settings.txt","FrameRate=60");


copyfileset(sourcepath+"\data\AstroData",destpath+"\data\AstroData","*.txt",false);
copyfileset(sourcepath+"\data\AstroData",destpath+"\data\AstroData","*.tab",false);

copyfileset(sourcepath+"\data\Languages",destpath+"\data\Languages","*.txt",true);

copyfileset(sourcepath+"\data\Paths",destpath+"\data\Paths","*.pth",false);
copyfileset(sourcepath+"\data\Satellites",destpath+"\data\Satellites","*.*",false);

copyfileset(sourcepath+"\data\Models",destpath+"\data\Models","*.*",true);

CreateDirectory(destpath+"\data\StereoPictures\Samples");
copyfileset(sourcepath+"\data\StereoPictures\Samples",destpath+"\data\StereoPictures\Samples","*.*",true);


copyfileset(sourcepath+"\data\Textures",destpath+"\data\Textures","*.*",false);

copyfileset(sourcepath+"\data\Scripts",destpath+"\data\Scripts","*.*",true,
   list("_InternalUsage",
        "_MarsProject",
        "_Movie"),
   list("except.dmp"
   ));

copyfileset(sourcepath+"\data\UserMenu_Standard",destpath+"\data\UserMenu_Standard","*.*",true,list,list);


message("completed");

