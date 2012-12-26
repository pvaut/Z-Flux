function GetPicInfo(currentfolder,picname)
{
   rs=Map;
   rs.tpe='side';
   rs.col=color(1,1,1);
   filecontent='';
   if FileIsPresent(currentfolder+"\"+picname+".txt") then
      filecontent=readtextfile(currentfolder+"\"+picname+".txt");
   if filecontent.length==0 then
      if FileIsPresent(currentfolder+"\_settings.txt") then
         filecontent=readtextfile(currentfolder+"\_settings.txt");
   if filecontent.length>0 then {
      while filecontent.length>0 do {
          line=filecontent.split("~n");
          id=line.split('=');
          if id=='TYPE' then rs.tpe=line;
          if id=='COLOR' then rs.col=color(ToScalar(line.split(',')),ToScalar(line.split(',')),ToScalar(line.split(',')));
      }
   }
   return(rs);
}
