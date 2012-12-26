codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
codeblock readtextfile(ScriptFilePath+"\_StereoPicTools.sci");

T_scene_create;
sc=T_getscene;


vp=T_getviewport;
vp.CameraPos=point(0,0,1);
vp.CameraDir=vector(0,0,-1);
vp.FocalDistance=1;
vp.NearClipPlane=0.1;
vp.FarClipPlane=20;


thumbresol=240;

currentfolder=DataDir+"\StereoPictures\sun";
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
      if picname.substring(picname.length-2,picname.length)=='_R' then {
         picname=picname.substring(0,picname.length-3);
         mergefilename=currentfolder+"\"+picname+".jpg";
         if not(FileIsPresent(mergefilename)) then {
            bmpL=LoadBitmap(currentfolder+"\"+picname+"_L.jpg");
            bmpR=LoadBitmap(currentfolder+"\"+picname+"_R.jpg");
            bmp=CreateBitmap(2*bmpL.XRes,bmpL.YRes);
            bmp.Insert(bmpL,0,0);
            bmp.Insert(bmpR,bmpL.XRes,0);
            bmp.SaveJpg(mergefilename,95);
         }
      }
   }
}

rendermessage("Completed");
for i=1 to 6 do render;
hiderendermessage;

