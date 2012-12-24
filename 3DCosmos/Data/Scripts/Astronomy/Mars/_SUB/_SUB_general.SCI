

function makepartialcircle(obj,vc1,vc2)
{
   vc2=vc2-0.007*vecnorm(vc2-vc1);
   ang=vecangle(vc1,vc2);
   resol=ang*20+2;
   for i=0 to resol-1 do {
      obj.addpoint(@point(vecrotate(vc1,vecnorm(vc1*vc2),ang*i/resol)));
   }
   obj.addpoint(@point(vc2));
}

function G_MarsRadius()
{
   return(3396.2);
}

function SUB_init()
{
   #create "loading data" frame
   loadingframe=T_getscene.Universe.addscreenframe("Loading");
   loadingframe.add("Text3D",
      "UnitX":vector(0.1,0,0),"UnitY":vector(0,0.1,0),"Position":point(0.3,0.4,0),
      "Content":'Loading data',"Color":color(1,0,0),"EnableLight":false,"DepthTest":DepthTestDisable);
   loadingframe.visible=false;
}

Monthnames=list('Januari','Februari','March','April','May','June','July','August','September','October','November','December');

function datestring(tm)
{
   rs=str(tm.day);if rs.length<2 then rs="0"+rs;
   rs=rs+" "+monthnames(tm.month-1)+" "+str(tm.year);
   return(rs);
}

function longit2string(longit)
{
   longit=longit/Pi*180;
   while (longit<-180) do longit=longit+360;
   while (longit> 180) do longit=longit-360;
   if longit>=0 then return(str(round(longit,1))+"° "+'East');
   else return(str(round(-1*longit,1))+"° "+'West');
}

function lattit2string(lattit)
{
   lattit=lattit/Pi*180;
   if lattit>=0 then return(str(round(lattit,1))+"° "+'North');
   else return(str(round(-1*lattit,1))+"° "+'South');
}

function SUB_showloading(status)
{
   T_getscene.Universe.Loading.visible=status;
   render;
}



function SUB_createleftpanel()
{
   #create offset to make space for left panel
   #T_getviewport.xoffsetfrac=-0.07;

   PanelLeft=root.SC.Universe.addscreenframe("PanelLeft");
   return(PanelLeft);
}

function SUB_G_leftpanel()
{
   return(root.SC.Universe.PanelLeft);
}

function SUB_G_leftpaneloffsett()
{
   return(0.01);
}

function SUB_G_sidepanelcolor()
{
   return(color(0.65,0.65,0.65));
}

createvar(linespanelframe);
createvar(linespanelypos);

function PanelLines_init(ipanelframe,ipanelypos)
{
   linespanelframe=ipanelframe;
   linespanelypos=ipanelypos;
}

function PanelLines_add(content)
{
   retobj=linespanelframe.add("Text3D","Content":content,"EnableLight":false,"UnitX":vector(0.02,0,0),"UnitY":vector(0,0.02,0),
      "Position":point(SUB_G_leftpaneloffsett+0.01,linespanelypos,0),"Color":SUB_G_sidepanelcolor);
   linespanelypos=linespanelypos-0.028;
   return(retobj);
}

function PanelLines_skip()
{
   linespanelypos=linespanelypos-0.028;
}


function SUB_testcmd()
{
   kbst=UIGetKeyPressed;
   if (kbst.Length==1) then {
      if (kbst>="a") and (kbst<="g") then {
         Store("Cmd",kbst);
         render;
         stop;
      }
      
      if kbst=="0" then SetMonitorStatus(false);
      if kbst=="1" then SetMonitorStatus(true);
      
      if kbst=="9" then ShutdownComputer;
      
   }
}