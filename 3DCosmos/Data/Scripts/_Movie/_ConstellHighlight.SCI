###########################################################################################################

createvar(VarConstellHighlight_starglobeframe);
createvar(VarConstellHighlight_starglobe);
VarConstellHighlight_colangle=0;
#VarConstellHighlight_activelist=list;

function ConstellHighlight_Init(istarglobeframe,istarglobe)
{
   VarConstellHighlight_starglobeframe=istarglobeframe.AddSubFrame("ConstellHighLight");
   VarConstellHighlight_starglobe=istarglobe;
}



function equat2eclipt_2(alpha,delta,ecl1,ecl2)
{
   coseps=0.9174369381;
   sineps=0.3978812030;
   r1=coseps*sin(delta)-sineps*cos(delta)*sin(alpha);
   ecl2=asin(r1);
   r1=cos(alpha);
   r2=sin(alpha)*coseps+tan(delta)*sineps;
   ecl1=angle(r1,r2);
}


function PointHighlight_Start(name,icol,RA,DEC)#both in degrees
{
   e1=0;e2=0;
   equat2eclipt_2(deg2rad(RA),deg2rad(DEC),Ref(e1),Ref(e2));
   rd=VarConstellHighlight_starglobe.Radius;
   pt=point(rd*cos(e1)*cos(e2),rd*sin(e1)*cos(e2),rd*sin(e2));

   sf=VarConstellHighlight_starglobeframe.AddSubFrame(name);
   sf.custom.frac=0;
   sf.custom.active=true;
   sf.add("Sphere","position":pt,"radius":(rd/50),"enablelight":false,"BlendType":BlendTransparent,"Color":icol);
}

function ArcHighlight_Start(name,icol,RA1,DEC1,RA2,DEC2,showarrow)
{
   ea1=0;ea2=0;eb1=0;eb2=0;
   equat2eclipt_2(deg2rad(RA1),deg2rad(DEC1),Ref(ea1),Ref(ea2));
   equat2eclipt_2(deg2rad(RA2),deg2rad(DEC2),Ref(eb1),Ref(eb2));
   rd=VarConstellHighlight_starglobe.Radius;
   dir1=vector(cos(ea1)*cos(ea2),sin(ea1)*cos(ea2),sin(ea2));
   dir2=vector(cos(eb1)*cos(eb2),sin(eb1)*cos(eb2),sin(eb2));

   sf=VarConstellHighlight_starglobeframe.AddSubFrame(name);
   sf.custom.frac=0;
   sf.custom.active=true;
   line=sf.add("Curve","Size":4,"enablelight":false,"BlendType":BlendTranslucent,"Color":icol);
   line.makearc(point(0,0,0),dir1,dir2,rd,50);
   if showarrow then
      line.Arrow2Size=0.1*rd;
}

function ConstellHighlight_Start(iconstelid,icol)
{
   lineslist=VarConstellHighlight_starglobe.GetConLines(iconstelid);
   sf=VarConstellHighlight_starglobeframe.AddSubFrame(iconstelid);
   foreach line in lineslist do {
      ln=sf.add("Curve",
           "Size":6,
           #"CurveRenderType":CurveRenderDot,
           "EnableLight":false,"RenderBack":true,
           "Color":icol,
           "BlendType":BlendTransparent);
      ln.makeline(line.Point1,line.Point2);
   }
   sf.custom.frac=0;
   sf.custom.active=true;
}

function ConstellHighlight_Stop(iconstelid)
{
   sf=VarConstellHighlight_starglobeframe.get(iconstelid);
#   DelObject(sf);
   sf.custom.active=false;
}

function ConstellHighlight_Update()
{
   VarConstellHighlight_colangle=VarConstellHighlight_colangle+0.25;
   lst=VarConstellHighlight_starglobeframe.getmembers;
   foreach sf in lst do {
      if sf.custom.active then {
         sf.custom.frac=sf.custom.frac+0.03;
         if sf.custom.frac>1 then sf.custom.frac=1;
      } else {
         sf.custom.frac=sf.custom.frac-0.03;
      }
      sf.Color.a=sf.custom.frac*(0.5+0.4*sin(VarConstellHighlight_colangle));
      if sf.custom.frac<0 then delobject(sf);
   }
}
