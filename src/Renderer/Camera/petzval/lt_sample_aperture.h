//input: scene_[x,y,z] - point in scene, ap_[x,y] - point on aperture
//output: [x,y,dx,dy] point and direction on sensor
#ifndef DEBUG_LOG
#define DEBUG_LOG
#endif
float view[3] =
{
  scene_x,
  scene_y,
  scene_z + lens_outer_pupil_curvature_radius
};
normalise(view);
int error = 0;
if(1 || view[2] >= lens_field_of_view)
{
  const float eps = 1e-8;
  float sqr_err = 1e30, sqr_ap_err = 1e30;
  float prev_sqr_err = 1e32, prev_sqr_ap_err = 1e32;
  for(int k=0;k<100&&(sqr_err>eps||sqr_ap_err>eps)&&error==0;k++)
  {
    prev_sqr_err = sqr_err, prev_sqr_ap_err = sqr_ap_err;
    const float begin_x = x;
    const float begin_y = y;
    const float begin_dx = dx;
    const float begin_dy = dy;
    const float begin_lambda = lambda;
    const float pred_ap[2] = {
       + -1.82916e-05  + -0.00032237 *begin_dy + 50.3474 *begin_dx + -3.41506e-06 *begin_y + 0.791338 *begin_x + 0.00139203 *lens_ipow(begin_dx, 2) + 0.163658 *begin_dx*lens_ipow(begin_dy, 2) + 0.144259 *lens_ipow(begin_dx, 3) + 0.531048 *begin_y*begin_dx*begin_dy + 0.00913662 *lens_ipow(begin_y, 2)*begin_dx + 0.130326 *begin_x*lens_ipow(begin_dy, 2) + 0.659513 *begin_x*lens_ipow(begin_dx, 2) + 0.0139839 *begin_x*begin_y*begin_dy + 0.000170055 *begin_x*lens_ipow(begin_y, 2) + 0.0230085 *lens_ipow(begin_x, 2)*begin_dx + 0.000167466 *lens_ipow(begin_x, 3) + 0.0654764 *begin_x*lens_ipow(begin_lambda, 3) + 4.50379 *begin_dx*lens_ipow(begin_lambda, 4) + -4.93137e-05 *begin_x*lens_ipow(begin_y, 2)*lens_ipow(begin_dx, 2) + 3.25153e-06 *lens_ipow(begin_y, 4)*begin_dx*lens_ipow(begin_dy, 2) + 0.00270115 *begin_x*begin_y*lens_ipow(begin_dy, 5) + 0.0240343 *lens_ipow(begin_x, 2)*begin_dx*lens_ipow(begin_dy, 4) + -11.8302 *begin_dx*lens_ipow(begin_lambda, 8) + -0.190248 *begin_x*lens_ipow(begin_lambda, 8) + 0.443044 *lens_ipow(begin_x, 2)*lens_ipow(begin_dx, 5)*lens_ipow(begin_dy, 2) + 0.130281 *lens_ipow(begin_x, 2)*lens_ipow(begin_dx, 7) + 2.34424e-10 *lens_ipow(begin_x, 5)*lens_ipow(begin_y, 2)*lens_ipow(begin_lambda, 2) + 3.36181e-11 *lens_ipow(begin_x, 7)*lens_ipow(begin_lambda, 2),
       + 2.03179e-05  + 50.3465 *begin_dy + -1.63165e-05 *begin_dx + 0.791355 *begin_y + -3.25724e-07 *begin_x + 0.117111 *lens_ipow(begin_dy, 3) + 0.165445 *lens_ipow(begin_dx, 2)*begin_dy + 0.657974 *begin_y*lens_ipow(begin_dy, 2) + 0.130118 *begin_y*lens_ipow(begin_dx, 2) + 0.0230094 *lens_ipow(begin_y, 2)*begin_dy + 0.000167737 *lens_ipow(begin_y, 3) + 0.530711 *begin_x*begin_dx*begin_dy + 0.013935 *begin_x*begin_y*begin_dx + 0.00912372 *lens_ipow(begin_x, 2)*begin_dy + 0.000168438 *lens_ipow(begin_x, 2)*begin_y + 0.0654946 *begin_y*lens_ipow(begin_lambda, 3) + 4.51724 *begin_dy*lens_ipow(begin_lambda, 4) + 0.000944759 *lens_ipow(begin_x, 2)*lens_ipow(begin_dy, 3) + 0.0441766 *lens_ipow(begin_y, 2)*lens_ipow(begin_dx, 2)*lens_ipow(begin_dy, 3) + 0.00259733 *begin_x*begin_y*lens_ipow(begin_dx, 5) + 9.20227e-06 *lens_ipow(begin_x, 2)*lens_ipow(begin_y, 2)*lens_ipow(begin_dx, 2)*begin_dy + 4.34624e-06 *lens_ipow(begin_x, 4)*lens_ipow(begin_dx, 2)*begin_dy + -11.8986 *begin_dy*lens_ipow(begin_lambda, 8) + -0.191493 *begin_y*lens_ipow(begin_lambda, 8) + -0.00290022 *lens_ipow(begin_y, 3)*lens_ipow(begin_dy, 6) + 4.93185e-11 *lens_ipow(begin_y, 7)*lens_ipow(begin_lambda, 2) + 1.77147e-10 *lens_ipow(begin_x, 2)*lens_ipow(begin_y, 5)*lens_ipow(begin_lambda, 2) + 3.97016e-08 *lens_ipow(begin_x, 4)*begin_y*lens_ipow(begin_lambda, 4)
    };
    const float delta_ap[] = {ap_x - pred_ap[0], ap_y - pred_ap[1]};
    sqr_ap_err = delta_ap[0]*delta_ap[0]+delta_ap[1]*delta_ap[1];
    float dx1_domega0[2][2];
    dx1_domega0[0][0] =  + 50.3474  + 0.00278406 *begin_dx + 0.163658 *lens_ipow(begin_dy, 2) + 0.432777 *lens_ipow(begin_dx, 2) + 0.531048 *begin_y*begin_dy + 0.00913662 *lens_ipow(begin_y, 2) + 1.31903 *begin_x*begin_dx + 0.0230085 *lens_ipow(begin_x, 2) + 4.50379 *lens_ipow(begin_lambda, 4) + -9.86274e-05 *begin_x*lens_ipow(begin_y, 2)*begin_dx + 3.25153e-06 *lens_ipow(begin_y, 4)*lens_ipow(begin_dy, 2) + 0.0240343 *lens_ipow(begin_x, 2)*lens_ipow(begin_dy, 4) + -11.8302 *lens_ipow(begin_lambda, 8) + 2.21522 *lens_ipow(begin_x, 2)*lens_ipow(begin_dx, 4)*lens_ipow(begin_dy, 2) + 0.911967 *lens_ipow(begin_x, 2)*lens_ipow(begin_dx, 6)+0.0f;
    dx1_domega0[0][1] =  + -0.00032237  + 0.327316 *begin_dx*begin_dy + 0.531048 *begin_y*begin_dx + 0.260652 *begin_x*begin_dy + 0.0139839 *begin_x*begin_y + 6.50306e-06 *lens_ipow(begin_y, 4)*begin_dx*begin_dy + 0.0135058 *begin_x*begin_y*lens_ipow(begin_dy, 4) + 0.096137 *lens_ipow(begin_x, 2)*begin_dx*lens_ipow(begin_dy, 3) + 0.886088 *lens_ipow(begin_x, 2)*lens_ipow(begin_dx, 5)*begin_dy+0.0f;
    dx1_domega0[1][0] =  + -1.63165e-05  + 0.33089 *begin_dx*begin_dy + 0.260237 *begin_y*begin_dx + 0.530711 *begin_x*begin_dy + 0.013935 *begin_x*begin_y + 0.0883531 *lens_ipow(begin_y, 2)*begin_dx*lens_ipow(begin_dy, 3) + 0.0129867 *begin_x*begin_y*lens_ipow(begin_dx, 4) + 1.84045e-05 *lens_ipow(begin_x, 2)*lens_ipow(begin_y, 2)*begin_dx*begin_dy + 8.69248e-06 *lens_ipow(begin_x, 4)*begin_dx*begin_dy+0.0f;
    dx1_domega0[1][1] =  + 50.3465  + 0.351332 *lens_ipow(begin_dy, 2) + 0.165445 *lens_ipow(begin_dx, 2) + 1.31595 *begin_y*begin_dy + 0.0230094 *lens_ipow(begin_y, 2) + 0.530711 *begin_x*begin_dx + 0.00912372 *lens_ipow(begin_x, 2) + 4.51724 *lens_ipow(begin_lambda, 4) + 0.00283428 *lens_ipow(begin_x, 2)*lens_ipow(begin_dy, 2) + 0.13253 *lens_ipow(begin_y, 2)*lens_ipow(begin_dx, 2)*lens_ipow(begin_dy, 2) + 9.20227e-06 *lens_ipow(begin_x, 2)*lens_ipow(begin_y, 2)*lens_ipow(begin_dx, 2) + 4.34624e-06 *lens_ipow(begin_x, 4)*lens_ipow(begin_dx, 2) + -11.8986 *lens_ipow(begin_lambda, 8) + -0.0174013 *lens_ipow(begin_y, 3)*lens_ipow(begin_dy, 5)+0.0f;
    float invApJ[2][2];
    const float invdetap = 1.0f/(dx1_domega0[0][0]*dx1_domega0[1][1] - dx1_domega0[0][1]*dx1_domega0[1][0]);
    invApJ[0][0] =  dx1_domega0[1][1]*invdetap;
    invApJ[1][1] =  dx1_domega0[0][0]*invdetap;
    invApJ[0][1] = -dx1_domega0[0][1]*invdetap;
    invApJ[1][0] = -dx1_domega0[1][0]*invdetap;
    for(int i=0;i<2;i++)
    {
      dx += invApJ[0][i]*delta_ap[i];
      dy += invApJ[1][i]*delta_ap[i];
    }
    out[0] =  + 1.11315e-05  + 63.7369 *begin_dx + -1.23959e-06 *begin_y + 0.579071 *begin_x + -42.5137 *begin_dx*lens_ipow(begin_dy, 2) + -41.6967 *lens_ipow(begin_dx, 3) + 0.0103578 *lens_ipow(begin_y, 2)*begin_dx + 0.400105 *begin_x*lens_ipow(begin_dy, 2) + 0.432992 *begin_x*lens_ipow(begin_dx, 2) + 0.0279455 *begin_x*begin_y*begin_dy + 0.000314708 *begin_x*lens_ipow(begin_y, 2) + -6.34641e-06 *lens_ipow(begin_x, 2)*begin_dy + 0.0388507 *lens_ipow(begin_x, 2)*begin_dx + 0.000318243 *lens_ipow(begin_x, 3) + 0.129429 *begin_x*lens_ipow(begin_lambda, 3) + 8.79828 *begin_dx*lens_ipow(begin_lambda, 4) + 0.000188465 *begin_x*lens_ipow(begin_y, 2)*lens_ipow(begin_dx, 2) + -0.000306445 *lens_ipow(begin_x, 2)*begin_y*begin_dx*begin_dy + -1.02735 *begin_x*lens_ipow(begin_dy, 6) + 0.0111276 *lens_ipow(begin_x, 2)*lens_ipow(begin_dx, 5) + -3.75593e-05 *lens_ipow(begin_x, 3)*begin_y*lens_ipow(begin_dy, 3) + 1.11559e-06 *lens_ipow(begin_x, 5)*lens_ipow(begin_dy, 2)*begin_lambda + -23.1103 *begin_dx*lens_ipow(begin_lambda, 8) + -2.45063e-09 *lens_ipow(begin_y, 6)*begin_dx*lens_ipow(begin_lambda, 2) + -0.374188 *begin_x*lens_ipow(begin_lambda, 8) + 0.00621632 *lens_ipow(begin_x, 2)*begin_dx*lens_ipow(begin_dy, 2)*lens_ipow(begin_lambda, 4) + 1.67399 *lens_ipow(begin_x, 2)*lens_ipow(begin_dx, 3)*lens_ipow(begin_dy, 4) + 5.08434e-14 *lens_ipow(begin_x, 9);
    out[1] =  + 5.86587e-05  + 63.7359 *begin_dy + 0.000194373 *begin_dx + 0.578989 *begin_y + -0.0014029 *lens_ipow(begin_dy, 2) + -41.6581 *lens_ipow(begin_dy, 3) + -42.2563 *lens_ipow(begin_dx, 2)*begin_dy + 0.445489 *begin_y*lens_ipow(begin_dy, 2) + 0.406918 *begin_y*lens_ipow(begin_dx, 2) + 0.0391844 *lens_ipow(begin_y, 2)*begin_dy + 0.000320667 *lens_ipow(begin_y, 3) + 0.0281329 *begin_x*begin_y*begin_dx + 0.0102024 *lens_ipow(begin_x, 2)*begin_dy + 0.000314203 *lens_ipow(begin_x, 2)*begin_y + 0.129916 *begin_y*lens_ipow(begin_lambda, 3) + 8.82407 *begin_dy*lens_ipow(begin_lambda, 4) + -0.000817427 *begin_x*begin_dx*begin_dy*lens_ipow(begin_lambda, 2) + -0.000251765 *begin_x*lens_ipow(begin_y, 2)*begin_dx*begin_dy + 9.91573e-10 *begin_x*lens_ipow(begin_y, 4) + 5.05574e-08 *lens_ipow(begin_x, 2)*lens_ipow(begin_y, 3) + 0.0114113 *lens_ipow(begin_y, 2)*lens_ipow(begin_dy, 5) + 2.76012e-07 *lens_ipow(begin_x, 4)*begin_y*lens_ipow(begin_dy, 2) + -2.34659e-12 *lens_ipow(begin_x, 6)*begin_y + 6.447e-07 *lens_ipow(begin_y, 5)*lens_ipow(begin_dx, 2)*begin_lambda + -23.225 *begin_dy*lens_ipow(begin_lambda, 8) + -0.37736 *begin_y*lens_ipow(begin_lambda, 8) + 5.26164e-14 *lens_ipow(begin_y, 9) + -0.000349748 *lens_ipow(begin_x, 2)*begin_dy*lens_ipow(begin_lambda, 6);
    out[2] =  + 5.02364e-07  + -1.62543 *begin_dx + 1.47273e-07 *begin_y + -0.0304649 *begin_x + -1.16093 *begin_dx*lens_ipow(begin_dy, 2) + 0.957655 *lens_ipow(begin_dx, 3) + -0.0478614 *begin_y*begin_dx*begin_dy + -0.000395619 *lens_ipow(begin_y, 2)*begin_dx + -0.000536643 *begin_x*lens_ipow(begin_lambda, 2) + -0.0336881 *begin_x*lens_ipow(begin_dy, 2) + -0.0018729 *begin_x*lens_ipow(begin_dx, 2) + -0.00116021 *begin_x*begin_y*begin_dy + -6.76077e-06 *begin_x*lens_ipow(begin_y, 2) + 1.46978e-07 *lens_ipow(begin_x, 2)*begin_dy + -0.000612358 *lens_ipow(begin_x, 2)*begin_dx + -3.16682e-06 *lens_ipow(begin_x, 3) + 0.0163357 *begin_dx*lens_ipow(begin_lambda, 3) + 2.09194 *lens_ipow(begin_dx, 3)*lens_ipow(begin_dy, 2) + 1.30058e-05 *lens_ipow(begin_x, 2)*begin_y*begin_dx*begin_dy + -3.53207e-09 *lens_ipow(begin_x, 3)*lens_ipow(begin_y, 2)*begin_lambda + 3.43008 *begin_dx*lens_ipow(begin_dy, 6) + -0.000910784 *lens_ipow(begin_y, 2)*lens_ipow(begin_dx, 3)*lens_ipow(begin_dy, 2) + -3.87729e-05 *lens_ipow(begin_x, 3)*lens_ipow(begin_dy, 4) + -2.43883 *lens_ipow(begin_dx, 3)*lens_ipow(begin_dy, 2)*lens_ipow(begin_lambda, 4) + 29.7151 *lens_ipow(begin_dx, 9) + 0.00233673 *begin_x*lens_ipow(begin_lambda, 8) + -0.000218861 *begin_x*lens_ipow(begin_y, 2)*lens_ipow(begin_dx, 6) + -2.50797e-12 *lens_ipow(begin_x, 7)*lens_ipow(begin_dy, 2);
    out[3] =  + -5.85768e-07  + -1.62827 *begin_dy + 4.94653e-06 *begin_dx + -0.0305602 *begin_y + -1.68369e-06 *lens_ipow(begin_dy, 2) + 0.952112 *lens_ipow(begin_dy, 3) + 3.075 *lens_ipow(begin_dx, 2)*begin_dy + -0.00162798 *begin_y*lens_ipow(begin_dy, 2) + 0.00644638 *begin_y*lens_ipow(begin_dx, 2) + -0.0006061 *lens_ipow(begin_y, 2)*begin_dy + -3.18047e-06 *lens_ipow(begin_y, 3) + 0.0734891 *begin_x*begin_dx*begin_dy + 0.000383177 *lens_ipow(begin_x, 2)*begin_dy + 7.68383e-07 *lens_ipow(begin_x, 2)*begin_y + 0.045741 *begin_dy*lens_ipow(begin_lambda, 3) + -0.000203136 *begin_y*lens_ipow(begin_lambda, 3) + -4.22068e-12 *lens_ipow(begin_x, 3)*lens_ipow(begin_y, 3) + 16.7096 *lens_ipow(begin_dx, 2)*lens_ipow(begin_dy, 5) + -7.90988e-09 *lens_ipow(begin_y, 5)*lens_ipow(begin_dx, 2) + -0.142255 *begin_x*lens_ipow(begin_dx, 5)*begin_dy + -0.0728484 *lens_ipow(begin_dx, 2)*begin_dy*lens_ipow(begin_lambda, 5) + -0.157668 *begin_dy*lens_ipow(begin_lambda, 8) + 40.3942 *lens_ipow(begin_dy, 9) + 195.806 *lens_ipow(begin_dx, 4)*lens_ipow(begin_dy, 5) + -1.82833e-13 *lens_ipow(begin_y, 8)*begin_dy + -1.4285e-05 *begin_x*lens_ipow(begin_y, 3)*begin_dx*lens_ipow(begin_dy, 4) + -1.03228e-12 *lens_ipow(begin_x, 4)*lens_ipow(begin_y, 4)*begin_dy + 5.2617e-15 *lens_ipow(begin_x, 8)*begin_y;
    float pred_out_cs[7] = {0.0f};
    lens_sphereToCs(out, out+2, pred_out_cs, pred_out_cs+3, - lens_outer_pupil_curvature_radius, lens_outer_pupil_curvature_radius);
    float view[3] =
    {
      scene_x - pred_out_cs[0],
      scene_y - pred_out_cs[1],
      scene_z - pred_out_cs[2]
    };
    normalise(view);
    float out_new[5];
    lens_csToSphere(pred_out_cs, view, out_new, out_new+2, - lens_outer_pupil_curvature_radius, lens_outer_pupil_curvature_radius);
    const float delta_out[] = {out_new[2] - out[2], out_new[3] - out[3]};
    sqr_err = delta_out[0]*delta_out[0]+delta_out[1]*delta_out[1];
    float domega2_dx0[2][2];
    domega2_dx0[0][0] =  + -0.0304649  + -0.000536643 *lens_ipow(begin_lambda, 2) + -0.0336881 *lens_ipow(begin_dy, 2) + -0.0018729 *lens_ipow(begin_dx, 2) + -0.00116021 *begin_y*begin_dy + -6.76077e-06 *lens_ipow(begin_y, 2) + 2.93957e-07 *begin_x*begin_dy + -0.00122472 *begin_x*begin_dx + -9.50047e-06 *lens_ipow(begin_x, 2) + 2.60116e-05 *begin_x*begin_y*begin_dx*begin_dy + -1.05962e-08 *lens_ipow(begin_x, 2)*lens_ipow(begin_y, 2)*begin_lambda + -0.000116319 *lens_ipow(begin_x, 2)*lens_ipow(begin_dy, 4) + 0.00233673 *lens_ipow(begin_lambda, 8) + -0.000218861 *lens_ipow(begin_y, 2)*lens_ipow(begin_dx, 6) + -1.75558e-11 *lens_ipow(begin_x, 6)*lens_ipow(begin_dy, 2)+0.0f;
    domega2_dx0[0][1] =  + 1.47273e-07  + -0.0478614 *begin_dx*begin_dy + -0.000791237 *begin_y*begin_dx + -0.00116021 *begin_x*begin_dy + -1.35215e-05 *begin_x*begin_y + 1.30058e-05 *lens_ipow(begin_x, 2)*begin_dx*begin_dy + -7.06414e-09 *lens_ipow(begin_x, 3)*begin_y*begin_lambda + -0.00182157 *begin_y*lens_ipow(begin_dx, 3)*lens_ipow(begin_dy, 2) + -0.000437722 *begin_x*begin_y*lens_ipow(begin_dx, 6)+0.0f;
    domega2_dx0[1][0] =  + 0.0734891 *begin_dx*begin_dy + 0.000766354 *begin_x*begin_dy + 1.53677e-06 *begin_x*begin_y + -1.26621e-11 *lens_ipow(begin_x, 2)*lens_ipow(begin_y, 3) + -0.142255 *lens_ipow(begin_dx, 5)*begin_dy + -1.4285e-05 *lens_ipow(begin_y, 3)*begin_dx*lens_ipow(begin_dy, 4) + -4.1291e-12 *lens_ipow(begin_x, 3)*lens_ipow(begin_y, 4)*begin_dy + 4.20936e-14 *lens_ipow(begin_x, 7)*begin_y+0.0f;
    domega2_dx0[1][1] =  + -0.0305602  + -0.00162798 *lens_ipow(begin_dy, 2) + 0.00644638 *lens_ipow(begin_dx, 2) + -0.0012122 *begin_y*begin_dy + -9.5414e-06 *lens_ipow(begin_y, 2) + 7.68383e-07 *lens_ipow(begin_x, 2) + -0.000203136 *lens_ipow(begin_lambda, 3) + -1.26621e-11 *lens_ipow(begin_x, 3)*lens_ipow(begin_y, 2) + -3.95494e-08 *lens_ipow(begin_y, 4)*lens_ipow(begin_dx, 2) + -1.46267e-12 *lens_ipow(begin_y, 7)*begin_dy + -4.28549e-05 *begin_x*lens_ipow(begin_y, 2)*begin_dx*lens_ipow(begin_dy, 4) + -4.1291e-12 *lens_ipow(begin_x, 4)*lens_ipow(begin_y, 3)*begin_dy + 5.2617e-15 *lens_ipow(begin_x, 8)+0.0f;
    float invJ[2][2];
    const float invdet = 1.0f/(domega2_dx0[0][0]*domega2_dx0[1][1] - domega2_dx0[0][1]*domega2_dx0[1][0]);
    invJ[0][0] =  domega2_dx0[1][1]*invdet;
    invJ[1][1] =  domega2_dx0[0][0]*invdet;
    invJ[0][1] = -domega2_dx0[0][1]*invdet;
    invJ[1][0] = -domega2_dx0[1][0]*invdet;
    for(int i=0;i<2;i++)
    {
      x += invJ[0][i]*delta_out[i];
      y += invJ[1][i]*delta_out[i];
    }
    if(sqr_err>prev_sqr_err) error |= 1;
    if(sqr_ap_err>prev_sqr_ap_err) error |= 2;
    if(out[0]!=out[0]) error |= 4;
    if(out[1]!=out[1]) error |= 8;
    DEBUG_LOG;
    // reset error code for first few iterations.
    if(k<10) error = 0;
  }
}
else
  error = 128;
if(out[0]*out[0]+out[1]*out[1] > lens_outer_pupil_radius*lens_outer_pupil_radius) error |= 16;
const float begin_x = x;
const float begin_y = y;
const float begin_dx = dx;
const float begin_dy = dy;
const float begin_lambda = lambda;
if(error==0)
  out[4] =  + 0.694197  + 0.142573 *begin_lambda + 1.58324e-05 *begin_dx + 1.52442e-07 *begin_y + -0.00814384 *lens_ipow(begin_dy, 2) + -0.00366396 *lens_ipow(begin_dx, 2) + -1.345e-05 *lens_ipow(begin_y, 2) + 8.87887e-07 *begin_x*begin_dy + -1.47621e-05 *lens_ipow(begin_x, 2) + -0.127889 *lens_ipow(begin_lambda, 3) + -0.000771522 *begin_y*begin_dy*begin_lambda + -0.00106236 *begin_x*begin_dx*begin_lambda + 0.000126026 *lens_ipow(begin_y, 2)*lens_ipow(begin_dy, 2) + -7.19374e-05 *lens_ipow(begin_y, 2)*lens_ipow(begin_dx, 2) + 0.000211294 *begin_x*begin_y*begin_dx*begin_dy + -0.885044 *lens_ipow(begin_dx, 2)*lens_ipow(begin_dy, 4) + 1.89161 *lens_ipow(begin_dx, 6) + -2.78915e-10 *lens_ipow(begin_y, 6) + -1.05702e-09 *lens_ipow(begin_x, 2)*lens_ipow(begin_y, 4) + -7.59323e-10 *lens_ipow(begin_x, 4)*lens_ipow(begin_y, 2) + -2.08956e-10 *lens_ipow(begin_x, 6) + -0.0925794 *lens_ipow(begin_dx, 2)*lens_ipow(begin_lambda, 5) + -1.88877e-05 *lens_ipow(begin_y, 2)*lens_ipow(begin_lambda, 5) + 0.00692053 *begin_x*begin_y*lens_ipow(begin_dx, 3)*begin_dy*begin_lambda + 5.5186 *lens_ipow(begin_dy, 8) + 2.08778e-05 *begin_x*lens_ipow(begin_y, 3)*begin_dx*lens_ipow(begin_dy, 3) + 0.151361 *lens_ipow(begin_lambda, 9) + -0.000651781 *lens_ipow(begin_x, 2)*lens_ipow(begin_dy, 2)*lens_ipow(begin_lambda, 5);
else
  out[4] = 0.0f;
