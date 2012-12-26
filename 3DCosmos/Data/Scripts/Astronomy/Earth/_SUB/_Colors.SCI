

function GetColor(name,opacity)
{
   cl=color(1,1,1);
   if name=="Red" then cl=color(0.8,0.15,0.4);
   if name=="Clock" then cl=color(0.8,0.7,0.3);
   if isvardefined("opacity") then
      cl.a=opacity;
   return(cl);
}