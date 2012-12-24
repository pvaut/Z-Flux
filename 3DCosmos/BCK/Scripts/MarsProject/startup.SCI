

store("Cmd","c");

while true do {
   kbst=Recall("Cmd");
   scriptname="";scriptarg="";
   if kbst=="a" then scriptname="mars";
   if kbst=="b" then scriptname="VallisMarineris";
   if kbst=="c" then scriptname="solarsys";
   if kbst=="d" then scriptname="hohman";
   if kbst=="e" then { scriptname="ShowModel"; scriptarg="ISS"; }
   if kbst=="f" then { scriptname="ShowModel"; scriptarg="MER"; }
   if scriptname=="" then scriptname="solarsys";
   Store("Cmd","");
   ExecuteScript(ScriptDir+"\MarsProject\"+scriptname+".SCI",scriptarg);
#   message("Stopped");
}