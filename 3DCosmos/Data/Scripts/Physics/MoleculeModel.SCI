codeblock readtextfile(ScriptDir+"\_TOOLS.sci");

sf=T_scene_create;
sss=T_getscene;
sss.ambientlightcolor=color(0.15,0.15,0.15);
refframe=sss.addsubframe("refframe");
vp=root.Viewports.main;
vp.camerapos=point(3,10,20);
vp.cameradir=vecnorm(point(0,0,0)-vp.camerapos);
vp.focaldistance=distance(point(0,0,0),vp.camerapos);
vp.FarClipPlane=60;
vp.NearClipPlane=0.5;


#######################################################################################"
# Prompt for molecule file name
#######################################################################################"
menu=T_createmenu("Select molecule",true);
menu.sizex=0.35;
menu.Color=color(1,1,1,1);
filelist=GetFileList(datadir+"\scripts\Physics\_molecules\*.pdb");

foreach file in filelist do {
   file.invert;file.split(".");file.invert;
   menu.Add("",file);
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
if menu.SelectID=="[Close]" then stop;
filename=menu.SelectID;
T_delmenu;



#######################################################################################"
# Read file
#######################################################################################"
rendermessage("Loading");
atoms=list;connections=list;
filename=datadir+"\Scripts\Physics\_molecules\"+filename+".pdb";
content=readtextfile(filename);
while content!="" do {
   line=content.split("~n");
   tag=line.substring(0,5).split(" ");
   if (tag=="ATOM") or (tag=="HETATM") then {
      atom=map;
      atom.name=line.substring(13,13);atom.name.split(" ");
      output("-"+atom.name+"-");
      atom.posit=point(ToScalar(line.substring(30,37)),
                       ToScalar(line.substring(38,45)),
                       ToScalar(line.substring(46,53)) );
      atoms.add(atom);
   }
   if tag=="CONECT" then {
      connectset=list;
      for i=1 to 12 do {
         nr=toscalar(line.substring(i*5+1,i*5+5));
         if nr>0 then connectset.add(nr);
      }
      connections.add(connectset);
   }
}

#Determine center of gravity of molecule
center=point(0,0,0);
foreach atom in atoms do center=center+atom.posit;
#Re-center atoms
foreach atom in atoms do atom.posit=atom.posit-@vector(center);

#######################################################################################"
# Create atom catalog
#######################################################################################"
atomcatalog=list;
atomcatalog.add(map("name":"",  "sz":1,   "cl":color(1.0,0.0,0.0)      ));
atomcatalog.add(map("name":"C", "sz":1,   "cl":color(0.25,0.5,0)       ));
atomcatalog.add(map("name":"O", "sz":1,   "cl":color(1,0.35,0.35)      ));
atomcatalog.add(map("name":"N", "sz":1,   "cl":color(0.35,0.35,1)      ));
atomcatalog.add(map("name":"P", "sz":1.3, "cl":color(0.75,0.25,0.75)   ));
atomcatalog.add(map("name":"H", "sz":0.5, "cl":color(0.65,0.65,0.65)   ));
atomindex=map;
for i=0 to atomcatalog.size-1 do
   atomindex.AddItem(atomcatalog(i).name:i);
 
atomradiusfactor=1;
if connections.size>0 then atomradiusfactor=0.25;  
   
function GetAtomRadius(atomname)
{
   idxnr=0;if atomindex.IsPresent(atomname) then idxnr=atomindex.get(atomname);
   return(atomradiusfactor*atomcatalog(idxnr).sz);
}

function GetAtomColor(atomname)
{
   idxnr=0;if atomindex.IsPresent(atomname) then idxnr=atomindex.get(atomname);
   return(atomcatalog(idxnr).cl);
}

#######################################################################################"
# Make atom objects
#######################################################################################"
foreach atom in atoms do {
   refframe.add("Sphere","Position":atom.posit,"resolution":12,"CanBuffer":true,
                "Radius":GetAtomRadius(atom.name),
                "Color":GetAtomColor(atom.name),
                "SpecularColor":color(0.3,0.3,0.3),"SpecularValue":25);
}

#######################################################################################"
# Make connections
#######################################################################################"
foreach connectset in connections do {
   idx1=connectset(0);
   ps1=atoms(idx1-1).posit;
   rd1=GetAtomRadius(atoms(idx1-1).name);
   for i=1 to connectset.size-1 do {
      idx2=connectset(i);
      ps2=atoms(idx2-1).posit;
      rd2=GetAtomRadius(atoms(idx2-1).name);
      rd=0.3*min(rd1,rd2);
      refframe.add("Cylinder","Position":ps1,"Axis":(ps2-ps1),
                   "Radius":rd,
                   "color":color(0.3,0.3,0.3),
                   "SpecularColor":color(0.3,0.3,0.3),"SpecularValue":20,
                   "resolution":8,"CanBuffer":true);
   }
}


hiderendermessage;

while true do {
   dr1=-1*vp.cameradir;
   dr2=vecnorm(dr1*vector(0,1,0));
   dr3=vecnorm(dr1*dr2);
   sss.light0pos=point(0,0,0)+500*(dr1-dr2-0.5*dr3);
   render;
}
