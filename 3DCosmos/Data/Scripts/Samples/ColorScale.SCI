codeblock readtextfile(ScriptDir+"\_TOOLS.sci");

function HSIColor(H,S,I)
{
   H=H*2*Pi;
   r=0;g=0;b=0;
   if ((H>=0) and (H<=2*Pi/3)) then
   {
      b=(1-S)/3;
      r=(1.0+S*cos(H)/cos(2*Pi/6-H))/3;
      g=1.0-r-b;
   }
   if ((H>=2*Pi/3) and (H<=4*Pi/3)) then
   {
      r=(1-S)/3;
      g=(1.0+S*cos(H-2*Pi/3)/cos(2*Pi/6-H+2*Pi/3))/3;
      b=1.0-r-g;
   }
   if ((H>=4*Pi/3) and (H<=6*Pi/3)) then
   {
      g=(1-S)/3;
      b=(1.0+S*cos(H-4*Pi/3)/cos(2*Pi/6-H+4*Pi/3))/3;
      r=1.0-g-b;
   }
   r=r*I*3;
   g=g*I*3;
   b=b*I*3;
   if (r<0) then r=0; if (r>1) then r=1;
   if (g<0) then g=0; if (g>1) then g=1;
   if (b<0) then b=0; if (b>1) then b=1;
   return(color(r,g,b));
}


sf=T_scene_create;
sss=T_getscene;
myviewport=T_getviewport;

sss.ambientlightcolor=color(0.05,0.05,0.05);


rootframe=sss.Universe.addsubframe("Root");

for iH=0 to 20 do {
   H=iH/20;
   for iS=0 to 10 do {
      S=iS/10;
      rootframe.add("sphere","radius":0.1,"position":point(6*H-3,4*S-2,0),"resolution":10,
         "color":HSIColor(H,S,0.75));
   }
}




while true do {
   render;
}
