
function CreateClock(showdate)
{
   clockcolor=GetColor("Clock");
   thesize=0.12;
   clockframe=root.SC.Universe.addscreenframe("ClockFrame");
   root.SC.Universe.moveobject("ClockFrame",-1000);
   clockframe.CastVolumeShadow=false;
   #clockframe.
   clockpos=point(thesize+0.02,thesize+0.02,0);
   if showdate then {
      info_clock=clockframe.add("Clock","Type":ClockTypeCalendar,"Position":point(0.02,0.02,0),"Size":(thesize*0.35),
         "Color":clockcolor,"EnableLight":false);
      clockpos=clockpos+vector(0,thesize*0.4,0);
   }
   clockframe.add("cylinder","position":(clockpos-vector(0,0,0.001)),"radius":thesize,"axis":vector(0,0,0.0001),"resolution":40,
      "color":color(0.7,0.7,0.7,0.5),"BlendType":BlendTranslucent,"DepthMask":DepthMaskDisable);
   info_clock=clockframe.add("Clock","Type":ClockTypeAnalog,"Position":clockpos,"Size":(thesize*0.93),
      "Color":clockcolor,"EnableLight":false);
   info_clock.DepthTest=DepthTestDisable;
   info_clock.DepthMask=DepthMaskDisable;

}


function Animate(elapse)
{
   chr=Chrono;chr.start;
   while chr.Elapsed<elapse do {
      AnimateStep;
   }
}


function FadeViewportIn(vp,durat)
{
   chr=Chrono;chr.start;
   while chr.Elapsed<=durat do {
      fadecol=color(0,0,0,max(0,1-chr.Elapsed/durat));
      vp.FadeColor=fadecol;
      AnimateStep;
   }
}

function FadeViewportOut(vp,durat)
{
   chr=Chrono;chr.start;
   while chr.Elapsed<=durat do {
      fadecol=color(0,0,0,min(1,chr.Elapsed/durat));
      vp.FadeColor=fadecol;
      AnimateStep;
   }
}


function FadeViewportsIn()
{
   viewportlist=root.Viewports.getmembers;
   for i=0 to 60 do {
      fadecol=color(0,0,0,max(0,1-i/60));
      foreach vp in viewportlist do vp.FadeColor=fadecol;
      AnimateStep;
   }
}

function FadeViewportsOut()
{
   viewportlist=root.Viewports.getmembers;
   for i=0 to 60 do {
      fadecol=color(0,0,0,min(1,i/60));
      foreach vp in viewportlist do vp.FadeColor=fadecol;
      AnimateStep;
   }
}

function FlashIntro(subframe)
{
   subframe.visible=true;
   subframe.BlinkPeriod=0.25;
   subframe.MaxBlinkCount=10;
}

function CreateFigure(figframe,figsize)
{
   bl=Blob;
   r2=0.3;
   bl.AddSphere(point(0,0,0.95),0.2,0.3);
   bl.AddSegment(point(0,0,0),point(0,0,1),0.23,r2+0.03);
   bl.AddSegment(point(0.22,0,-1),point(0.15,0,0.5),0.2,r2);
   bl.AddSegment(point(-0.22,0,-1),point(-0.15,0,0.5),0.2,r2);
   bl.AddSegment(point(0.26,0,0.55),point(0.66,-0.4,0.94),0.185,r2);
   bl.AddSegment(point(-0.26,0,0.55),point(-0.66,-0.4,0.94),0.185,r2);
   sh=bl.CreateSolidShape(0.06);
   ttf=transformation;ttf.translate(vector(0,0,0.65*figsize));ttf.scale(0.6*figsize);
   ttf.rotate(vector(0,0,1),Pi/2);
   sh.transform(ttf);
   ob=figframe.add("SolidObject");
   ob.CreateShape(sh);
   ob.color=color(0.7,0.6,0.5);
   ob.SpecularColor=color(0.6,0.6,0.0);
   ob.SpecularValue=30;
   return(ob);
}


