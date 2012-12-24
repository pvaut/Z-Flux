codeblock readtextfile(ScriptDir+"\_TOOLS.sci");


function BrowseFile(extension)
{
   T_scene_create;
   sc=T_getscene;
   myviewport=T_getviewport;
   myviewport.CameraPos=point(0,0,1);
   myviewport.CameraDir=vector(0,0,-1);
   myviewport.FocalDistance=1;
   
   refframe=sc.addsubframe("refframe");
   
   
   cframe=refframe.addscreenframe("Controls Frame");
   cframe.enablelight=false;
   cframe.BlendType=BlendTranslucent;
   cframe.DepthMask=DepthMaskDisable;
   cframe.color=color(1,1,1,0.5);
   
   root.mousedampingfactor=0.25;
   cframe.EnabeMouseArrow(point(0.5,0.5),point(0,0),point(1,0.995),0.03);
   
   
   px=0.07;py=0.12;
   px2=px+0.4;
   
   textsize=0.025;
   county=30;
   
   currentdir="";
   if not(isdefined(Recall("BrowseDir"))) then
      currentdir=DataDir;
   else
      currentdir=Recall("BrowseDir");
   currentdir.Replace("/","\");
   
   curdirctrl=cframe.add("TextControl","Size":textsize,"Position":point(px,py+(county+0.5)*textsize));
   
   btok=cframe.add("ButtonControl","Content":'Open',"IsDefault":true,"Position":point(px,py-2*textsize),"Size":textsize,"SizeX":0.2);
   btcancel=cframe.add("ButtonControl","Content":'Cancel',"Position":point(px+0.3,py-2*textsize),"Size":textsize,"SizeX":0.2);
   
   filectrl=cframe.add("ListControl","Size":textsize,"SizeX":0.5,"CountY":county);
   filectrl.position=point(px,py);
   
   function LoadFileList()
   {
      curdirctrl.Content=currentdir;
      lst=list;
      dirmask="";
      if currentdir.length>0 then {
         lst.add("[..] (Up)");
         dirmask=currentdir+"\*.*";
      }
      
      dirlist=GetFileList(dirmask,true);
      sortidx=dirlist.sort;
      foreach idx in sortidx do {
         diritem=dirlist(idx);
         if (diritem!=".") and (diritem!="..") then
            lst.add("["+diritem+"]");
      }
            
      if currentdir.length>0 then {
         filelist=GetFileList(currentdir+"\*."+extension,false);
         sortidx=filelist.sort;
         foreach idx in sortidx do
            lst.add("    "+filelist(idx));
         if lst.size==0 then
            lst.add("[..] (Up)");
      }
      filectrl.List=lst;
      filectrl.SelectIdx=0;
   }
   
   
   LoadFileList;
   
   py=py-3*textsize;
   
   
   finished=false;
   while not(finished) do {
      i=5;
      if btok.wasmodified then {
         selstr=filectrl.Selected;
         if selstr=="[..] (Up)" then {
            currentdir.Invert;
            currentdir.Split("\");
            currentdir.Invert;
            LoadFileList;
         } else {
            if (selstr(0)=="[") and (selstr(selstr.length-1)=="]") then {
               if currentdir.length>0 then
                  currentdir=currentdir+"\";
               currentdir=currentdir+selstr.substring(1,selstr.length-2);
               LoadFileList;
            } else {
               Store("BrowseDir",currentdir);
               return(currentdir+"\"+selstr.substring(4,9999));
            }
         }
      }
      if btcancel.wasmodified then return("");
      render;
   }
}

#filename=BrowseFile("SCI");
#ExecuteScript(filename);
#stop;
