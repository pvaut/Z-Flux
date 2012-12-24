codeblock readtextfile(ScriptDir+"\_TOOLS.sci");
sf=T_scene_create;
sss=T_getscene;
myviewport=T_getviewport;

menu=T_createmenu;

menu.add("","Item 1","1",true);
menu.add("","Item 2","2",true);
menu.add("","Item 3","3");
menu.add("","Item 4","4");

menu.add("4","Sub 4 1");
menu.add("4","Sub 4 2");

menu.add("Sub 4 1","SubSub A","A");
menu.add("Sub 4 1","SubSub B","B");
menu.add("Sub 4 1","SubSub C","C");
menu.add("Sub 4 1","SubSub D","D");

menu.checked("2")=true;

while true do {
   if menu.WasModified then {
      st=menu.SelectID;
      if menu.Checked(menu.SelectID) then st=st+" checked";
      message(st);
   }
   render;
}
