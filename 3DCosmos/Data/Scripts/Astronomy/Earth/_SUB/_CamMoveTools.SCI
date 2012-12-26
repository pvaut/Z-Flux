function Cam_Init(vp)
{
   vp.custom.cam_rotating=false;
   vp.custom.cam_rotatingpos=false;
}

function Cam_Rotate(vp,newdir,durat)
{
   vp.custom.cam_rotating=true;
   vp.custom.cam_rotate_dir1=deref(vp.CameraDir);
   vp.custom.cam_rotate_angle=vecangle(vp.CameraDir,newdir);
   vp.custom.cam_rotdir=vecnorm(vp.CameraDir*newdir);
   vp.custom.cam_timer=chrono;
   vp.custom.cam_durat=durat;
}

function Cam_RotateHor(vp,ang,durat)
{
   vp.custom.cam_rotating=true;
   vp.custom.cam_rotate_dir1=deref(vp.CameraDir);
   vp.custom.cam_rotate_angle=ang;
   vp.custom.cam_rotdir=vector(0,0,1);
   vp.custom.cam_timer=chrono;
   vp.custom.cam_durat=durat;
}

function Cam_RotateVert(vp,ang,durat)
{
   vp.custom.cam_rotating=true;
   vp.custom.cam_rotate_dir1=deref(vp.CameraDir);
   vp.custom.cam_rotate_angle=ang;
   vp.custom.cam_rotdir=vecnorm(vp.CameraDir*vector(0,0,1));
   vp.custom.cam_timer=chrono;
   vp.custom.cam_durat=durat;
}


function Cam_RotatePos(vp,newpos,durat)
{
   vp.custom.cam_rotatingpos=true;
   vp.custom.cam_rotate_dir1=vecnorm(vp.CameraPos-point(0,0,0));
   vp.custom.cam_dist1=distance(vp.CameraPos,point(0,0,0));
   vp.custom.cam_rotate_angle=vecangle(vp.custom.cam_rotate_dir1,newpos-point(0,0,0));
   vp.custom.cam_rotdir=vecnorm(vp.custom.cam_rotate_dir1*(newpos-point(0,0,0)));
   vp.custom.cam_timer=chrono;
   vp.custom.cam_durat=durat;
   vp.custom.cam_distchange=distance(newpos,point(0,0,0))-distance(vp.CameraPos,point(0,0,0));
}


function Cam_Adjust(vp)
{
   if vp.custom.cam_rotating then {
      fr=vp.custom.cam_timer.Elapsed/(vp.custom.cam_durat);
      if fr>1 then { vp.custom.cam_rotating=false;fr=1; }
      #fr=0.5*(1+sin(fr*pi-pi/2));
      fr=SmoothRamp1(fr,2.0/vp.custom.cam_durat);
      vp.CameraDir=vecrotate(vp.custom.cam_rotate_dir1,vp.custom.cam_rotdir,fr*vp.custom.cam_rotate_angle);
   }
   if vp.custom.cam_rotatingpos then {
      fr=vp.custom.cam_timer.Elapsed/(vp.custom.cam_durat);
      if fr>1 then { vp.custom.cam_rotatingpos=false;fr=1; }
      #fr=0.5*(1+sin(fr*pi-pi/2));
      fr=SmoothRamp1(fr,2.0/vp.custom.cam_durat);
      vp.CameraPos=point(0,0,0)+
         (vp.custom.cam_dist1+fr*vp.custom.cam_distchange)*vecrotate(vp.custom.cam_rotate_dir1,vp.custom.cam_rotdir,fr*vp.custom.cam_rotate_angle);
   }
}
