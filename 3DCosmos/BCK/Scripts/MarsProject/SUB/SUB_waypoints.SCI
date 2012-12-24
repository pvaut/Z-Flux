
#waypoint object
waypointposits=list;
flagposits=list;

function G_waypointlistfilename()
{
   return(ScriptDir+"\MarsProject\Data\waypointlist.3ob");
}

function G_waypointcount()
{
   return(waypointposits.size);
}

function G_waypoint(nr)
{
   return(waypointposits(nr));
}

function loadwaypoints()
{
  flelist=GetFileList(G_waypointlistfilename);
   if flelist.size>0 then {
      tmplist=list;
      ReadObject(waypointposits,G_waypointlistfilename);
   }
}

function addwaypoint(posit)
{
   waypointposits.add(posit);
   SaveObject(waypointposits,G_waypointlistfilename);
}

function dellastwaypoint()
{
   if waypointposits.size>0 then {
      waypointposits.del(waypointposits.size-1);
      SaveObject(waypointposits,G_waypointlistfilename);
   }
}

function delallwaypoints()
{
   waypointposits.clear;
   SaveObject(waypointposits,G_waypointlistfilename);
}


function G_flaglistfilename()
{
   return(ScriptDir+"\MarsProject\Data\flaglist.3ob");
}

function G_flagcount()
{
   return(flagposits.size);
}

function G_flagposit(nr)
{
   return(flagposits(nr));
}

function loadflags()
{
  flelist=GetFileList(G_flaglistfilename);
   if flelist.size>0 then {
      tmplist=list;
      ReadObject(flagposits,G_flaglistfilename);
   }
}

function addflag(posit)
{
   flagposits.add(posit);
   SaveObject(flagposits,G_flaglistfilename);
}

function dellastflag()
{
   if flagposits.size>0 then {
      flagposits.del(flagposits.size-1);
      SaveObject(flagposits,G_flaglistfilename);
   }
}

function delallflags()
{
   flagposits.clear;
   SaveObject(flagposits,G_flaglistfilename);
}


#display the flags on a globe. flagsframe contains the subframe with the globe; radius= radius of the globe
function dispflags(flagsframe,radius)
{
   flagsframe.clearobjects;
   for wpnr=0 to flagposits.size-1 do {
      flagframe=flagsframe.addsubframe;
      flagframe.transf.origin=@point(radius*@vector(flagposits(wpnr)));
      flagframe.transf.Zaxis=vecnorm(@vector(flagposits(wpnr)));
      flagframe.transf.Xaxis=vecnorm(flagframe.transf.Zaxis*vector(0,0,1));
      flagframe.transf.Yaxis=vecnorm(flagframe.transf.Zaxis*flagframe.transf.Xaxis);
      flagframe.transf.rotate(vector(0,0,1),1+wpnr*wpnr*10);
      
      
      flagframe.add("Cylinder","Radius":(0.0025*radius),"Color":color(0.7,0.7,0.8),
         "IsClosed":true,"Position":point(0,0,0),"Axis":(radius*0.1*vector(0,0,1)) );

      flagframe.add("Cylinder","Radius":(0.002*radius),"Color":color(0.7,0.7,0.8),
         "IsClosed":true,"Position":point(0,0,radius*0.1),"Axis":vector(0,radius*0.08,0) );

      flagframe.add("Sphere","Radius":(0.00253*radius),"Color":color(0.7,0.7,0.8),
         "Position":point(0,0,radius*0.1),"Resolution":10 );
         
      flagframe.add("Rectangle","RenderBack":true,"Position":point(0,0,radius*0.038),"EnableLight":false,"Texture":"Flag",
         "Color":color(0.7,0.7,0.7),"Axis1":vector(0,radius*0.08,0),"Axis2":vector(0,0,radius*0.06));
      
   }
}



#display the waypoints on a globe. wpsframe contains the subframe with the globe; radius= radius of the globe
function dispwaypoints(wpsframe,radius)
{
   wpsframe.clearobjects;
   for wpnr=0 to waypointposits.size-1 do {
      wpframe=wpsframe.addsubframe;

      wpframe.add("Cylinder","Radius":(0.006*radius),"Color":color(1,0,1),
         "IsClosed":true,"Position":@point(radius*@vector(waypointposits(wpnr))),
         "Axis":(radius*0.03*@vector(waypointposits(wpnr))) );
         
      if wpnr>0 then {
         rtobj=wpframe.add("Curve","Color":color(0.5,0,0),
            "CurveRenderType":CurveRenderDot,"Size":2,"Arrow2Size":0.02);
         makepartialcircle(rtobj,
            radius*1.003*@vector(waypointposits(wpnr-1)),
            radius*1.003*@vector(waypointposits(wpnr)));
      }
   }
}
