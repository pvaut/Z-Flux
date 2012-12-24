

codeblock readtextfile(ScriptDir+"\TOOLS.sci");

T_scene_create;

menu=T_createmenu;

menu.add("","Seizoenen op Mars","MarsSeasons");
menu.add("","Start Astro3D","Astro3D");
menu.add("","Programma afsluiten","CloseApp");
menu.add("","Computer afsluiten","CloseComp");

menu.position=point(0.4,0.8,0);
menu.sizex=0.4;

while true do {
   if menu.WasModified then {
   
      if menu.SelectID=="MarsSeasons" then {
         ExecuteScript(ScriptDir+"\MarsProject\seasons.SCI","");
      }
   
      if menu.SelectID=="Astro3D" then {
         Execute("c:\astro3d\stereo.exe","c:\astro3d",false);
         ExitProgram;
      }
      if menu.SelectID=="CloseApp" then ExitProgram;
      if menu.SelectID=="CloseComp" then ShutdownComputer;
   }
   render;
}