codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
codeblock readtextfile(ScriptDir+"\_logo.sci");

createvar(menu);

function startup()
{
   sf=T_scene_create;
   sf=root.SC.Universe;
   sss=T_getscene;
   myviewport=T_getviewport;
   myviewport.CameraPos=point(0,0,1);
   myviewport.CameraDir=vector(0,0,-1);
   myviewport.FocalDistance=1;
   myviewport.NearClipPlane=0.1;
   myviewport.FarClipPlane=20;

   screensizey=2*tan(myviewport.aperture/2);
   screensizex=screensizey*myviewport.aspectratio;

   
   logoframe=sf.addsubframe("Logoframe");
   logoframe.transf.translate(vector(0.2*screensizex,-0.3*screensizey,0));
   logoframe.transf.scale(0.06);
   logoframe.transf.rotate(vector(0,1,0),deg2rad(-45));
   createlogo(logoframe);

   menu=T_createmenu;
   root.SC.Universe.MenuFrame.color=color(0.75,0.75,0.75);
   root.SC.Universe.MenuFrame.BlendType=BlendTransparent;
   
   root.mousedampingfactor=0.25;
   root.SC.Universe.MenuFrame.EnabeMouseArrow(point(0.5,0.5),point(0,0),point(1,0.995),0.03);
   root.framerate=60;


   myviewport.EnableUserStop=false;
   root.showcontrols=true;
   menu.sizex=0.3;
   menu.Color=color(1,1,1,1);
   
   
   
   dirstack=list;
   dirstack.add(ScriptDir);
   
   while dirstack.size>0 do {
      curdir=dirstack(dirstack.size-1);dirstack.del(dirstack.size-1);
      curparent=curdir;
      if curparent==ScriptDir then curparent="";
      
      if FileIsPresent(curdir+"\menu.txt") then {
         content=readtextfile(curdir+"\menu.txt");
         while content!="" do {
            line=content.split("~n");
            id=line.split(";");
            name=line;if name=="" then name=id;
            found=false;
            if id=="-" then {
               menu.add(curparent,"-");
               found=true;
            }
            if FileIsPresent(curdir+"\"+id+".SCI") then {
               menu.add(curparent,Translate(name),curdir+"\"+id+".SCI");
               found=true;
            }
            if not(found) and ID.Length>0 then {
               if FileIsPresent(curdir+"\"+id+"\menu.txt") then {
                  dirstack.add(curdir+"\"+id);
                  menu.add(curparent,Translate(name),curdir+"\"+id);
               }
            }
         }
      }    
      
      if false then {
      dirlist=GetFileList(curdir+"\*.*",true);
      foreach dir in dirlist do 
         if (dir!=".") and (dir!="..") and (dir(0)!="_") then {
            dirstack.add(curdir+"\"+dir);
            name=dir;
            if name.find("_")>=0 then name.split("_");
            menu.add(curparent,Translate(name),curdir+"\"+dir);
      }
      filelist=GetFileList(curdir+"\*.SCI",false);
      foreach file in filelist do {
         if file(0)!="_" then {
            name=file;name=name.split(".");
            if name.find("_")>=0 then name.split("_");
            menu.add(curparent,Translate(name),curdir+"\"+file);
         }
      }
      }
      
      
      
      
   }
}

startup;

while true do {
   if menu.WasModified then {
      st=menu.SelectID;
      menu.visible=false;
      root.Viewports.main.FadeColor=Color(0,0,0,1);
      render;
      if FileIsPresent(st) then {
#         try {
            ExecuteScript(st,"");
#         }
      }
      startup;
      menu.SelectID=st;
      output(st);
   }
   render;
}
