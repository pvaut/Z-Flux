codeblock readtextfile(ScriptDir+"\_TOOLS.sci");

verbose=true;

function showstatus(msg)
{
   root.SC.Universe.Drop("StatusFrame");
   msgframe=root.SC.Universe.addscreenframe("tatusFrame");
   msgframe.add("Text3D","Name":"MessageBox","Position":point(0.25,0.01,0),"Content":msg,
                "enablelight":false,"color":color(0.7,0.7,0.7),
                "UnitX":vector(0.03,0,0),"UnitY":vector(0,0.03,0));
}

###############################################################################################
# INIT SCENE AND VIEWPORT
###############################################################################################

sf=T_scene_create;
vp=T_getviewport;

###############################################################################################
# PROMPT USER
###############################################################################################

showlist=GetFileList(datadir+"\AnimationSets\*.txt");

menu=T_createmenu("Select animation set",true);
menu.sizex=0.35;

vp.ShowControls=true;

foreach showfilename in showlist do {
   showname=showfilename;showname.Invert;showname.split(".");showname.Invert;
   menu.Add("",showname,showfilename);
}

T_menu_appendclose;

menufinished=false;
while not(menufinished) do {
   if menu.WasModified then {
      menufinished=true;
      currentfolder=menu.SelectID;
   }
   render;
}
slideshowfilename=datadir+"\AnimationSets\"+menu.SelectID;
T_delmenu;

###############################################################################################
# LOAD SHOW
###############################################################################################

showlist=list;
filecontent=readtextfile(slideshowfilename);
while filecontent.length>0 do {
   line=filecontent.split("~n");
   if line.length>0 then
      if line(0)!="#" then {
         name=line.split("~t");
         durat=toscalar(line);
         showlist.add(map("Name":name,"Durat":durat));
      }
}

totdurat=0;
foreach anim in showlist do totdurat=totdurat+anim.durat;

if verbose then {
   showstatus(str(totdurat));
   wait(1);
   hiderendermessage;
}

starttm=CurrentTimeUT;
expectelaps=0;

animationnr=0;
while animationnr<showlist.size do {
   chr=chrono;
   ExecuteScript(ScriptDir+"\"+showlist(animationnr).name+".sci");
   
   if verbose then {
      expectelaps=expectelaps+showlist(animationnr).durat*60;
      elaps=CurrentTimeUT-starttm;
      showstatus(str(round((expectelaps-elaps)/60,1)) + "/" + str(round(chr.elapsed,0)));
      wait(0.25);
   }

   if UIIsKeyDown("SHIFT") then {
      if animationnr>0 then animationnr=animationnr-1;
   }
   else {
      animationnr=animationnr+1;
   }   
   
   fadeout(0.5);
   
#   if verbose then hiderendermessage;
   
   
}