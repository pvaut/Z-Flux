
vp=root.Viewports.main;
vp.framesize=0;

ct=2;

createvar(rsbm);

for i1=0 to ct-1 do {
   for i2=0 to ct-1 do {
      
      vp.ZoomX1=i1/ct;
      vp.ZoomY1=i2/ct;
      vp.ZoomX2=(i1+1)/ct;
      vp.ZoomY2=(i2+1)/ct;
      render;
      render;
      bmp=vp.CaptureFrame;
#      bmp2=bmp.Crop(0,0,bmp.XRes-1,bmp.YRes-1);
      bmp2=bmp;
      bmp2.SaveJpg("d:\text\ima"+Str(i1)+Str(i2)+".jpg",99);
      if (i1==0) and (i2==0) then
         rsbm=CreateBitmap(ct*bmp2.XRes,ct*bmp2.YRes);
      rsbm.Insert(bmp2,i1*(bmp2.XRes),i2*(bmp2.YRes));
      delay(0.1);
   }
}

rsbm.SaveJpg("d:\text\ima.jpg",99);
Message("ok");