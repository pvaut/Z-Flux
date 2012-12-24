

filename="geostationary";

#tst=list;
#ReadObject(tst,datadir+"\Satellites\"+filename+".bfl");


#filecontent=readtextfile(datadir+"\Satellites\GPS.ssc");
message("read file");
filecontent=readtextfile(datadir+"\Satellites\"+filename+".ssc");
#filecontent=readtextfile(datadir+"\Satellites\eccentrics.ssc");
message("read file completed");

sats=list;

ct=0;
while (filecontent.length>0) do {
   line=filecontent.split("~n");
   if (line(0)!="#") and (line.length>0) then {
   
      sat=map;
      sat.name=line;
      output(str(ct)+" "+sat.name);
   
      line=filecontent.split("~n");if line!="{" then throw("Error");
      
      line="";while (line!="~tEllipticalOrbit") and (filecontent.length>0) do line=filecontent.split("~n");
      line=filecontent.split("~n");if line!="	{" then throw("Error");
      finished=false;
      sat.Period=-9999;sat.SemiMajorAxis=-9999;sat.Eccentricity=-9999;sat.Inclination=-9999;
      sat.AscendingNode=-9999;sat.ArgOfPericenter=-9999;sat.MeanAnomaly=-9999;sat.Epoch=-9999;
      while (not(finished)) and (filecontent.length>0) do {
         line=filecontent.split("~n");
         if line.substring(0,1)=="~t}" then finished=true; else {
            line.replace(" ","~t");
            line.split("~t");line.split("~t");tpe=line.split("	");
            st=line;while st(0)=="~t" do st.split("~t");
            if tpe=="Period" then sat.Period=ToScalar(st);
            if tpe=="SemiMajorAxis" then sat.SemiMajorAxis=ToScalar(st);
            if tpe=="Eccentricity" then sat.Eccentricity=ToScalar(st);
            if tpe=="Inclination" then sat.Inclination=ToScalar(st);
            if tpe=="AscendingNode" then sat.AscendingNode=ToScalar(st);
            if tpe=="ArgOfPericenter" then sat.ArgOfPericenter=ToScalar(st);
            if tpe=="MeanAnomaly" then sat.MeanAnomaly=ToScalar(st);
            if tpe=="Epoch" then sat.Epoch=ToScalar(st);
         }
      }
      line="";while (line!="}") and (filecontent.length>0) do line=filecontent.split("~n");
      
      sats.add(sat);
      ct=ct+1;
   }
}

SaveObject(sats,datadir+"\Satellites\"+filename+".bfl");


