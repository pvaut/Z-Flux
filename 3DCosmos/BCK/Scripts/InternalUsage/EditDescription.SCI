


ID=GetHighlightFuncID;

#EditText(ID);
#stop;

descr=GetRawFunctionDescription(ID);
descr=ID+"~n"+descr;
if EditText(descr) then {
   testID=descr.split("~n");
   if testID!=ID then {
      message("Invalid editing");
      stop;
   }
   SetRawFunctionDescription(ID,descr);
}
stop;