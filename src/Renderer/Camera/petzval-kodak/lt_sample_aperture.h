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
        static_cast<float>(+ -4.18825e-05  + 38.5106 *begin_dx + 1.87125e-06 *begin_y + 0.793854 *begin_x + 0.0001189 *begin_x*begin_dy + -2.81858 *begin_dx*lens_ipow(begin_dy, 2) + 0.272773 *lens_ipow(begin_dx, 3) + 0.365348 *begin_y*begin_dx*begin_dy + 0.00884886 *lens_ipow(begin_y, 2)*begin_dx + -0.0413386 *begin_x*lens_ipow(begin_dy, 2) + 0.628179 *begin_x*lens_ipow(begin_dx, 2) + 9.50174e-05 *begin_x*lens_ipow(begin_y, 2) + 0.0258519 *lens_ipow(begin_x, 2)*begin_dx + 0.000194971 *lens_ipow(begin_x, 3) + 0.143863 *begin_dx*lens_ipow(begin_lambda, 3) + 0.0155383 *begin_x*begin_y*begin_dy*begin_lambda + 0.000512147 *lens_ipow(begin_x, 2)*begin_dx*lens_ipow(begin_lambda, 2) + -0.000737467 *lens_ipow(begin_x, 2)*lens_ipow(begin_dx, 3) + 2.42674e-05 *lens_ipow(begin_x, 3)*begin_y*begin_dy + 0.000502016 *begin_x*lens_ipow(begin_y, 2)*lens_ipow(begin_dx, 2)*begin_lambda + 0.00121272 *lens_ipow(begin_x, 3)*lens_ipow(begin_dy, 2)*begin_lambda + 3.94892e-05 *begin_x*lens_ipow(begin_y, 3)*lens_ipow(begin_dy, 3) + -0.00141158 *lens_ipow(begin_x, 2)*begin_y*lens_ipow(begin_dx, 3)*begin_dy + 1.99339 *begin_x*lens_ipow(begin_dy, 4)*lens_ipow(begin_lambda, 3) + -0.4503 *begin_x*lens_ipow(begin_dx, 2)*lens_ipow(begin_dy, 2)*lens_ipow(begin_lambda, 3) + 0.000374371 *begin_x*lens_ipow(begin_y, 2)*lens_ipow(begin_lambda, 5) + -7.90122e-06 *lens_ipow(begin_x, 2)*lens_ipow(begin_y, 3)*begin_dx*lens_ipow(begin_dy, 3) + -6.27188e-13 *lens_ipow(begin_x, 9)),
        static_cast<float>(+ 2.66533e-05  + 38.4607 *begin_dy + 0.000141773 *begin_dx + 0.791788 *begin_y + 1.54974 *lens_ipow(begin_dy, 3) + 1.22591 *lens_ipow(begin_dx, 2)*begin_dy + 0.713683 *begin_y*lens_ipow(begin_dy, 2) + 0.149095 *begin_y*lens_ipow(begin_dx, 2) + 0.0273831 *lens_ipow(begin_y, 2)*begin_dy + 0.000217246 *lens_ipow(begin_y, 3) + 0.55902 *begin_x*begin_dx*begin_dy + 0.0166992 *begin_x*begin_y*begin_dx + 0.0110866 *lens_ipow(begin_x, 2)*begin_dy + 0.00021418 *lens_ipow(begin_x, 2)*begin_y + 0.00160847 *lens_ipow(begin_y, 2)*begin_dy*begin_lambda + 0.189568 *begin_dy*lens_ipow(begin_lambda, 4) + 1.25399e-06 *lens_ipow(begin_x, 4)*begin_dy + 5.45599e-05 *lens_ipow(begin_x, 2)*begin_y*lens_ipow(begin_dy, 2)*begin_lambda + 0.000108438 *begin_x*lens_ipow(begin_y, 3)*begin_dx*lens_ipow(begin_dy, 2) + -2.47276 *lens_ipow(begin_dy, 5)*lens_ipow(begin_lambda, 3) + 4.47355e-05 *lens_ipow(begin_y, 3)*lens_ipow(begin_lambda, 5) + 3.85543e-07 *lens_ipow(begin_y, 5)*lens_ipow(begin_dx, 2)*begin_lambda + -200.169 *lens_ipow(begin_dy, 9) + -445.054 *lens_ipow(begin_dx, 2)*lens_ipow(begin_dy, 5)*lens_ipow(begin_lambda, 2) + -0.0746295 *begin_y*lens_ipow(begin_dx, 2)*lens_ipow(begin_lambda, 6) + 7.86221e-08 *lens_ipow(begin_y, 6)*lens_ipow(begin_dy, 3) + 0.00292727 *begin_x*lens_ipow(begin_y, 2)*lens_ipow(begin_dx, 5)*begin_dy + -3.98504e-12 *lens_ipow(begin_x, 6)*lens_ipow(begin_y, 3))
    };
    const float delta_ap[] = {ap_x - pred_ap[0], ap_y - pred_ap[1]};
    sqr_ap_err = delta_ap[0]*delta_ap[0]+delta_ap[1]*delta_ap[1];
    float dx1_domega0[2][2];
    dx1_domega0[0][0] =  + 38.5106  + -2.81858 *lens_ipow(begin_dy, 2) + 0.81832 *lens_ipow(begin_dx, 2) + 0.365348 *begin_y*begin_dy + 0.00884886 *lens_ipow(begin_y, 2) + 1.25636 *begin_x*begin_dx + 0.0258519 *lens_ipow(begin_x, 2) + 0.143863 *lens_ipow(begin_lambda, 3) + 0.000512147 *lens_ipow(begin_x, 2)*lens_ipow(begin_lambda, 2) + -0.0022124 *lens_ipow(begin_x, 2)*lens_ipow(begin_dx, 2) + 0.00100403 *begin_x*lens_ipow(begin_y, 2)*begin_dx*begin_lambda + -0.00423473 *lens_ipow(begin_x, 2)*begin_y*lens_ipow(begin_dx, 2)*begin_dy + -0.900601 *begin_x*begin_dx*lens_ipow(begin_dy, 2)*lens_ipow(begin_lambda, 3) + -7.90122e-06 *lens_ipow(begin_x, 2)*lens_ipow(begin_y, 3)*lens_ipow(begin_dy, 3)+0.0f;
    dx1_domega0[0][1] =  + 0.0001189 *begin_x + -5.63715 *begin_dx*begin_dy + 0.365348 *begin_y*begin_dx + -0.0826772 *begin_x*begin_dy + 0.0155383 *begin_x*begin_y*begin_lambda + 2.42674e-05 *lens_ipow(begin_x, 3)*begin_y + 0.00242544 *lens_ipow(begin_x, 3)*begin_dy*begin_lambda + 0.000118468 *begin_x*lens_ipow(begin_y, 3)*lens_ipow(begin_dy, 2) + -0.00141158 *lens_ipow(begin_x, 2)*begin_y*lens_ipow(begin_dx, 3) + 7.97356 *begin_x*lens_ipow(begin_dy, 3)*lens_ipow(begin_lambda, 3) + -0.900601 *begin_x*lens_ipow(begin_dx, 2)*begin_dy*lens_ipow(begin_lambda, 3) + -2.37037e-05 *lens_ipow(begin_x, 2)*lens_ipow(begin_y, 3)*begin_dx*lens_ipow(begin_dy, 2)+0.0f;
    dx1_domega0[1][0] =  + 0.000141773  + 2.45182 *begin_dx*begin_dy + 0.298191 *begin_y*begin_dx + 0.55902 *begin_x*begin_dy + 0.0166992 *begin_x*begin_y + 0.000108438 *begin_x*lens_ipow(begin_y, 3)*lens_ipow(begin_dy, 2) + 7.71086e-07 *lens_ipow(begin_y, 5)*begin_dx*begin_lambda + -890.108 *begin_dx*lens_ipow(begin_dy, 5)*lens_ipow(begin_lambda, 2) + -0.149259 *begin_y*begin_dx*lens_ipow(begin_lambda, 6) + 0.0146364 *begin_x*lens_ipow(begin_y, 2)*lens_ipow(begin_dx, 4)*begin_dy+0.0f;
    dx1_domega0[1][1] =  + 38.4607  + 4.64923 *lens_ipow(begin_dy, 2) + 1.22591 *lens_ipow(begin_dx, 2) + 1.42737 *begin_y*begin_dy + 0.0273831 *lens_ipow(begin_y, 2) + 0.55902 *begin_x*begin_dx + 0.0110866 *lens_ipow(begin_x, 2) + 0.00160847 *lens_ipow(begin_y, 2)*begin_lambda + 0.189568 *lens_ipow(begin_lambda, 4) + 1.25399e-06 *lens_ipow(begin_x, 4) + 0.00010912 *lens_ipow(begin_x, 2)*begin_y*begin_dy*begin_lambda + 0.000216876 *begin_x*lens_ipow(begin_y, 3)*begin_dx*begin_dy + -12.3638 *lens_ipow(begin_dy, 4)*lens_ipow(begin_lambda, 3) + -1801.52 *lens_ipow(begin_dy, 8) + -2225.27 *lens_ipow(begin_dx, 2)*lens_ipow(begin_dy, 4)*lens_ipow(begin_lambda, 2) + 2.35866e-07 *lens_ipow(begin_y, 6)*lens_ipow(begin_dy, 2) + 0.00292727 *begin_x*lens_ipow(begin_y, 2)*lens_ipow(begin_dx, 5)+0.0f;
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
    out[0] =  + 0.000102946 *begin_dy + 49.8717 *begin_dx + 0.547648 *begin_x + 5.62711e-05 *begin_x*begin_dy + -22.0716 *begin_dx*lens_ipow(begin_dy, 2) + -21.9223 *lens_ipow(begin_dx, 3) + 0.385359 *begin_y*begin_dx*begin_dy + 0.0164367 *lens_ipow(begin_y, 2)*begin_dx + 0.52518 *begin_x*lens_ipow(begin_dy, 2) + 0.933846 *begin_x*lens_ipow(begin_dx, 2) + 0.0403418 *begin_x*begin_y*begin_dy + 0.000520572 *begin_x*lens_ipow(begin_y, 2) + 0.0563557 *lens_ipow(begin_x, 2)*begin_dx + 0.000511196 *lens_ipow(begin_x, 3) + -0.0186794 *begin_x*lens_ipow(begin_lambda, 3) + 0.253992 *begin_y*begin_dx*lens_ipow(begin_dy, 3) + 0.339805 *begin_y*lens_ipow(begin_dx, 3)*begin_dy + -0.00506257 *lens_ipow(begin_y, 2)*lens_ipow(begin_dx, 5) + -3.84644e-06 *lens_ipow(begin_x, 2)*lens_ipow(begin_y, 3)*begin_dx*begin_dy + 3.0519e-05 *lens_ipow(begin_x, 4)*lens_ipow(begin_dx, 3) + 1.43569e-06 *lens_ipow(begin_x, 5)*lens_ipow(begin_dy, 2) + -0.131399 *begin_dx*lens_ipow(begin_lambda, 8) + -323.266 *lens_ipow(begin_dx, 9) + 7.56974 *begin_y*lens_ipow(begin_dx, 3)*lens_ipow(begin_dy, 3)*lens_ipow(begin_lambda, 2) + 0.046931 *begin_x*lens_ipow(begin_lambda, 8) + 0.232728 *begin_x*lens_ipow(begin_dx, 4)*lens_ipow(begin_lambda, 4) + 7.1928e-08 *begin_x*lens_ipow(begin_y, 5)*lens_ipow(begin_dy, 3) + 2.50009e-10 *lens_ipow(begin_x, 7)*begin_y*begin_dy;
    out[1] =  + -6.90594e-06  + 49.8214 *begin_dy + 0.546129 *begin_y + -5.13183e-05 *begin_y*begin_dy + 0.212412 *begin_dy*lens_ipow(begin_lambda, 2) + -21.6229 *lens_ipow(begin_dy, 3) + -22.0734 *lens_ipow(begin_dx, 2)*begin_dy + 0.960315 *begin_y*lens_ipow(begin_dy, 2) + 0.520915 *begin_y*lens_ipow(begin_dx, 2) + 0.0572416 *lens_ipow(begin_y, 2)*begin_dy + 0.000519256 *lens_ipow(begin_y, 3) + 0.426737 *begin_x*begin_dx*begin_dy + 0.0403476 *begin_x*begin_y*begin_dx + 0.0169644 *lens_ipow(begin_x, 2)*begin_dy + 0.000521259 *lens_ipow(begin_x, 2)*begin_y + -0.0119206 *begin_y*lens_ipow(begin_lambda, 4) + 0.000151004 *lens_ipow(begin_y, 3)*lens_ipow(begin_dx, 2) + 0.0162731 *begin_x*begin_y*begin_dx*lens_ipow(begin_dy, 2) + -0.0086394 *lens_ipow(begin_x, 2)*lens_ipow(begin_dy, 3) + -1.69652 *begin_dy*lens_ipow(begin_lambda, 8) + -151.488 *lens_ipow(begin_dy, 9) + -913.634 *lens_ipow(begin_dx, 2)*lens_ipow(begin_dy, 7) + 3.70036e-11 *lens_ipow(begin_y, 8)*begin_dy + 47.1588 *begin_x*lens_ipow(begin_dx, 5)*lens_ipow(begin_dy, 3) + 1.43412e-10 *lens_ipow(begin_x, 2)*lens_ipow(begin_y, 6)*begin_dy + 1.78745e-10 *lens_ipow(begin_x, 3)*lens_ipow(begin_y, 5)*begin_dx + 5.96238e-08 *lens_ipow(begin_x, 5)*begin_y*lens_ipow(begin_dx, 3) + -8.89699e-09 *lens_ipow(begin_x, 5)*lens_ipow(begin_y, 2)*begin_dx*begin_dy;
    out[2] =  + -8.94061e-07  + -1.40944 *begin_dx + 1.21205e-07 *begin_y + -0.035502 *begin_x + -0.635244 *begin_dx*lens_ipow(begin_dy, 2) + 0.704338 *lens_ipow(begin_dx, 3) + -0.0401341 *begin_y*begin_dx*begin_dy + -0.000432549 *lens_ipow(begin_y, 2)*begin_dx + 0.000200352 *begin_x*lens_ipow(begin_lambda, 2) + -0.0287197 *begin_x*lens_ipow(begin_dy, 2) + -0.00388082 *begin_x*lens_ipow(begin_dx, 2) + -0.00131215 *begin_x*begin_y*begin_dy + -7.99742e-06 *begin_x*lens_ipow(begin_y, 2) + -0.00074833 *lens_ipow(begin_x, 2)*begin_dx + -2.77129e-06 *lens_ipow(begin_x, 3) + 0.00290847 *begin_x*lens_ipow(begin_dx, 2)*lens_ipow(begin_lambda, 2) + -0.000136085 *lens_ipow(begin_x, 2)*begin_dx*lens_ipow(begin_dy, 2) + -0.0744329 *begin_dx*lens_ipow(begin_dy, 2)*lens_ipow(begin_lambda, 4) + 13.4632 *lens_ipow(begin_dx, 5)*lens_ipow(begin_dy, 2) + -0.0171502 *begin_x*lens_ipow(begin_dy, 6) + 1.21745e-07 *lens_ipow(begin_x, 2)*lens_ipow(begin_y, 3)*begin_dx*begin_dy + -6.65853e-05 *lens_ipow(begin_x, 3)*lens_ipow(begin_dy, 4) + 18.0048 *lens_ipow(begin_dx, 9) + 0.0545295 *lens_ipow(begin_y, 2)*lens_ipow(begin_dx, 3)*lens_ipow(begin_dy, 4) + 2.29849e-14 *begin_x*lens_ipow(begin_y, 8) + -4.08395e-06 *lens_ipow(begin_x, 3)*lens_ipow(begin_lambda, 6) + 2.04548e-10 *lens_ipow(begin_x, 6)*begin_y*begin_dx*begin_dy + 5.10085e-11 *lens_ipow(begin_x, 7)*lens_ipow(begin_dx, 2);
    out[3] =  + -1.40741 *begin_dy + -0.0354895 *begin_y + -0.0122666 *begin_dy*lens_ipow(begin_lambda, 2) + 0.747338 *lens_ipow(begin_dy, 3) + 2.08358 *lens_ipow(begin_dx, 2)*begin_dy + 0.00278423 *begin_y*lens_ipow(begin_dx, 2) + -0.000693956 *lens_ipow(begin_y, 2)*begin_dy + -2.448e-06 *lens_ipow(begin_y, 3) + 0.0613949 *begin_x*begin_dx*begin_dy + -0.000130683 *begin_x*begin_y*begin_dx + 0.000379221 *lens_ipow(begin_x, 2)*begin_dy + 1.59958e-06 *lens_ipow(begin_x, 2)*begin_y + -0.000970764 *begin_y*lens_ipow(begin_dy, 2)*begin_lambda + 0.000243998 *begin_y*lens_ipow(begin_lambda, 4) + -6.41461e-06 *lens_ipow(begin_y, 3)*lens_ipow(begin_dx, 2) + -0.125908 *lens_ipow(begin_dx, 2)*begin_dy*lens_ipow(begin_lambda, 3) + 3.70178 *lens_ipow(begin_dx, 6)*begin_dy + 0.00211035 *lens_ipow(begin_y, 2)*lens_ipow(begin_dy, 5) + 0.00174529 *lens_ipow(begin_x, 2)*lens_ipow(begin_dy, 5) + -1.90305e-09 *lens_ipow(begin_x, 2)*lens_ipow(begin_y, 4)*begin_dy + 2.74318e-11 *lens_ipow(begin_x, 4)*lens_ipow(begin_y, 3) + 7.96551e-12 *lens_ipow(begin_x, 6)*begin_y + 19.5306 *lens_ipow(begin_dx, 4)*lens_ipow(begin_dy, 3)*begin_lambda + -5.73774e-10 *lens_ipow(begin_y, 5)*lens_ipow(begin_lambda, 3) + 0.0721489 *begin_dy*lens_ipow(begin_lambda, 8) + -0.926611 *lens_ipow(begin_dy, 5)*lens_ipow(begin_lambda, 4) + 58.9514 *lens_ipow(begin_dx, 2)*lens_ipow(begin_dy, 7) + 3.48745e-14 *lens_ipow(begin_y, 9);
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
    domega2_dx0[0][0] =  + -0.035502  + 0.000200352 *lens_ipow(begin_lambda, 2) + -0.0287197 *lens_ipow(begin_dy, 2) + -0.00388082 *lens_ipow(begin_dx, 2) + -0.00131215 *begin_y*begin_dy + -7.99742e-06 *lens_ipow(begin_y, 2) + -0.00149666 *begin_x*begin_dx + -8.31387e-06 *lens_ipow(begin_x, 2) + 0.00290847 *lens_ipow(begin_dx, 2)*lens_ipow(begin_lambda, 2) + -0.00027217 *begin_x*begin_dx*lens_ipow(begin_dy, 2) + -0.0171502 *lens_ipow(begin_dy, 6) + 2.4349e-07 *begin_x*lens_ipow(begin_y, 3)*begin_dx*begin_dy + -0.000199756 *lens_ipow(begin_x, 2)*lens_ipow(begin_dy, 4) + 2.29849e-14 *lens_ipow(begin_y, 8) + -1.22518e-05 *lens_ipow(begin_x, 2)*lens_ipow(begin_lambda, 6) + 1.22729e-09 *lens_ipow(begin_x, 5)*begin_y*begin_dx*begin_dy + 3.57059e-10 *lens_ipow(begin_x, 6)*lens_ipow(begin_dx, 2)+0.0f;
    domega2_dx0[0][1] =  + 1.21205e-07  + -0.0401341 *begin_dx*begin_dy + -0.000865097 *begin_y*begin_dx + -0.00131215 *begin_x*begin_dy + -1.59948e-05 *begin_x*begin_y + 3.65236e-07 *lens_ipow(begin_x, 2)*lens_ipow(begin_y, 2)*begin_dx*begin_dy + 0.109059 *begin_y*lens_ipow(begin_dx, 3)*lens_ipow(begin_dy, 4) + 1.83879e-13 *begin_x*lens_ipow(begin_y, 7) + 2.04548e-10 *lens_ipow(begin_x, 6)*begin_dx*begin_dy+0.0f;
    domega2_dx0[1][0] =  + 0.0613949 *begin_dx*begin_dy + -0.000130683 *begin_y*begin_dx + 0.000758441 *begin_x*begin_dy + 3.19917e-06 *begin_x*begin_y + 0.00349059 *begin_x*lens_ipow(begin_dy, 5) + -3.80609e-09 *begin_x*lens_ipow(begin_y, 4)*begin_dy + 1.09727e-10 *lens_ipow(begin_x, 3)*lens_ipow(begin_y, 3) + 4.77931e-11 *lens_ipow(begin_x, 5)*begin_y+0.0f;
    domega2_dx0[1][1] =  + -0.0354895  + 0.00278423 *lens_ipow(begin_dx, 2) + -0.00138791 *begin_y*begin_dy + -7.34399e-06 *lens_ipow(begin_y, 2) + -0.000130683 *begin_x*begin_dx + 1.59958e-06 *lens_ipow(begin_x, 2) + -0.000970764 *lens_ipow(begin_dy, 2)*begin_lambda + 0.000243998 *lens_ipow(begin_lambda, 4) + -1.92438e-05 *lens_ipow(begin_y, 2)*lens_ipow(begin_dx, 2) + 0.00422071 *begin_y*lens_ipow(begin_dy, 5) + -7.61218e-09 *lens_ipow(begin_x, 2)*lens_ipow(begin_y, 3)*begin_dy + 8.22953e-11 *lens_ipow(begin_x, 4)*lens_ipow(begin_y, 2) + 7.96551e-12 *lens_ipow(begin_x, 6) + -2.86887e-09 *lens_ipow(begin_y, 4)*lens_ipow(begin_lambda, 3) + 3.13871e-13 *lens_ipow(begin_y, 8)+0.0f;
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
  out[4] =  + 0.673591  + -0.000186789 *begin_dy + 0.000132241 *begin_dx + 0.0641823 *lens_ipow(begin_dy, 2) + 0.0505303 *lens_ipow(begin_dx, 2) + -0.00170725 *begin_y*begin_dy + -9.22804e-05 *lens_ipow(begin_y, 2) + -0.00238155 *begin_x*begin_dx + -7.01377e-05 *lens_ipow(begin_x, 2) + 0.125656 *lens_ipow(begin_lambda, 3) + -1.66983 *lens_ipow(begin_dy, 4) + -1.48929 *lens_ipow(begin_dx, 2)*lens_ipow(begin_dy, 2) + -1.13113 *lens_ipow(begin_dx, 4) + 0.00103378 *lens_ipow(begin_y, 2)*lens_ipow(begin_dy, 2) + 0.00012183 *lens_ipow(begin_y, 2)*lens_ipow(begin_dx, 2)*begin_dy + 0.0040518 *begin_x*begin_y*begin_dx*begin_dy*begin_lambda + -0.246022 *begin_y*lens_ipow(begin_dy, 5) + -0.000289988 *lens_ipow(begin_y, 2)*lens_ipow(begin_dx, 2)*lens_ipow(begin_lambda, 2) + -2.95361e-09 *lens_ipow(begin_y, 6) + 0.00983138 *lens_ipow(begin_x, 2)*lens_ipow(begin_dx, 4) + -1.04882e-08 *lens_ipow(begin_x, 2)*lens_ipow(begin_y, 4) + -8.44813e-09 *lens_ipow(begin_x, 4)*lens_ipow(begin_y, 2) + -3.11318e-09 *lens_ipow(begin_x, 6) + -0.00149673 *lens_ipow(begin_x, 2)*lens_ipow(begin_dy, 2)*lens_ipow(begin_lambda, 3) + -0.00196846 *begin_x*lens_ipow(begin_y, 2)*begin_dx*lens_ipow(begin_dy, 4) + -0.0047658 *begin_x*lens_ipow(begin_y, 2)*lens_ipow(begin_dx, 3)*lens_ipow(begin_dy, 2) + 3.57178e-05 *lens_ipow(begin_x, 4)*lens_ipow(begin_dx, 2)*lens_ipow(begin_dy, 2) + -0.490061 *lens_ipow(begin_lambda, 9);
else
  out[4] = 0.0f;
