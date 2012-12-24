codeblock readtextfile(ScriptDir+"\_TOOLS.sci");

T_scene_create;
sc=T_getscene;


vp=T_getviewport;
vp.CameraPos=point(0,0,1);
vp.CameraDir=vector(0,0,-1);
vp.FocalDistance=1;
vp.NearClipPlane=0.1;
vp.FarClipPlane=20;


thumbresol=240;

currentfolder=DataDir+"\StereoPictures";
scansubfolders=true;
if isdefined(ScriptArgument) then 
   if ScriptArgument.Length>0 then {
      currentfolder=ScriptArgument;
      scansubfolders=false;
   }


todofolderlist=list(currentfolder);

while todofolderlist.size>0 do {
   currentfolder=todofolderlist(todofolderlist.size-1);todofolderlist.del(todofolderlist.size-1);
   
   #put subfolders in the todo stack
   folderlist=GetFileList(currentfolder+"\*.*",true);
   foreach subfolder in folderlist do 
      if (subfolder!=".") and (subfolder!="..") and scansubfolders then {
         todofolderlist.add(currentfolder+"\"+subfolder);
      }
      
   #process files in this folder
   piclist=GetFileList(currentfolder+"\*.jpg");
   foreach picfile in piclist do {
      rendermessage(currentfolder,picfile);
      picname=picfile;picname.invert;picname.split(".");picname.invert;
      needcreate=true;
      picfilename=currentfolder+"\"+picfile;
      thumbfilename=currentfolder+"\"+picname+".THB";
      if FileIsPresent(thumbfilename) then {
         if FileModifiedTime(picfilename)-FileModifiedTime(thumbfilename)<10 then needcreate=false;
      }
      if needcreate then {
         bmp=LoadBitmap(picfilename);
         fc=round(max(bmp.XRes/2,bmp.Yres)/thumbresol);
         thumbbmp=bmp.Reduce(fc);
         thumbbmp.savejpg(thumbfilename,90);
      }
   }
}

rendermessage("Completed");
for i=1 to 6 do render;
hiderendermessage;

