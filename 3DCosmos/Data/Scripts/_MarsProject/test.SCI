

routefilename=ScriptDir+"\MarsProject\Data\route.3ob";
lst=list;

flist=GetFileList(routefilename);
if flist.size>0 then {
   ReadObject(lst,routefilename);
}

lst.add(point(1,1,1));
lst.add(point(1,1,2));
lst.add(point(1,1,3));

SaveObject(lst,routefilename);