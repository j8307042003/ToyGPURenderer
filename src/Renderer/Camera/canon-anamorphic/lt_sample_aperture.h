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
        static_cast<float>(+ -2.8249e-05  + 43.07 *begin_dx + 0.789807 *begin_x + -6.60329e-05 *begin_x*begin_dx + -4.08213 *begin_dx*lens_ipow(begin_dy, 2) + -3.77168 *lens_ipow(begin_dx, 3) + 0.096252 *begin_y*begin_dx*begin_dy + 0.000394718 *lens_ipow(begin_y, 2)*begin_dx + -0.302459 *begin_x*lens_ipow(begin_dy, 2) + -0.0281843 *begin_x*lens_ipow(begin_dx, 2) + -0.00939815 *begin_x*begin_y*begin_dy + -0.000135064 *begin_x*lens_ipow(begin_y, 2) + -0.00348141 *lens_ipow(begin_x, 2)*begin_dx + -0.000121576 *lens_ipow(begin_x, 3) + 1.47374 *begin_dx*lens_ipow(begin_lambda, 3) + 0.0134401 *begin_x*lens_ipow(begin_lambda, 3) + -5.24576e-05 *begin_x*lens_ipow(begin_y, 2)*lens_ipow(begin_dx, 2) + 0.000152707 *lens_ipow(begin_x, 3)*lens_ipow(begin_dx, 2) + 8.83929e-06 *begin_x*lens_ipow(begin_y, 2)*lens_ipow(begin_lambda, 4) + -3.20556e-11 *begin_x*lens_ipow(begin_y, 6) + -0.37882 *lens_ipow(begin_x, 2)*lens_ipow(begin_dx, 5) + 2.57224e-06 *lens_ipow(begin_x, 4)*begin_y*begin_dx*begin_dy + 1.85111e-08 *lens_ipow(begin_x, 4)*lens_ipow(begin_y, 2)*begin_dx + -8.58586e-07 *lens_ipow(begin_x, 5)*lens_ipow(begin_dy, 2) + -4.25433 *begin_dx*lens_ipow(begin_lambda, 8) + -0.040636 *begin_x*lens_ipow(begin_lambda, 8) + -1.07368e-12 *lens_ipow(begin_x, 5)*lens_ipow(begin_y, 4) + -5.70665e-13 *lens_ipow(begin_x, 9)),
        static_cast<float>(+ -8.73022e-06  + 46.6881 *begin_dy + 0.907564 *begin_y + -0.00366551 *begin_dx*begin_dy + 0.478591 *lens_ipow(begin_dy, 3) + -0.324864 *lens_ipow(begin_dx, 2)*begin_dy + 0.0112152 *begin_y*lens_ipow(begin_dy, 2) + -0.143935 *begin_y*lens_ipow(begin_dx, 2) + -0.00566694 *lens_ipow(begin_y, 2)*begin_dy + -0.000106649 *lens_ipow(begin_y, 3) + 0.242198 *begin_x*begin_dx*begin_dy + -0.0063857 *begin_x*begin_y*begin_dx + 0.00360194 *lens_ipow(begin_x, 2)*begin_dy + -0.000106403 *lens_ipow(begin_x, 2)*begin_y + 0.0205903 *begin_y*lens_ipow(begin_lambda, 3) + 3.00014 *begin_dy*lens_ipow(begin_lambda, 4) + -0.0165559 *begin_x*begin_y*begin_dx*lens_ipow(begin_dy, 2) + 8.21189e-07 *begin_x*lens_ipow(begin_y, 3)*begin_dx + -0.00880398 *lens_ipow(begin_x, 2)*lens_ipow(begin_dx, 2)*begin_dy + 0.00080362 *begin_x*begin_y*begin_dx*lens_ipow(begin_lambda, 5) + -7.8079 *begin_dy*lens_ipow(begin_lambda, 8) + -0.0613272 *begin_y*lens_ipow(begin_lambda, 8) + 1.83569e-08 *lens_ipow(begin_y, 5)*lens_ipow(begin_lambda, 4) + -6.31658e-14 *lens_ipow(begin_y, 9) + 9.33712e-08 *lens_ipow(begin_x, 2)*lens_ipow(begin_y, 3)*lens_ipow(begin_lambda, 4) + -4.23266e-13 *lens_ipow(begin_x, 2)*lens_ipow(begin_y, 7) + 1.85794e-07 *lens_ipow(begin_x, 6)*lens_ipow(begin_dy, 3) + -7.73144e-13 *lens_ipow(begin_x, 6)*lens_ipow(begin_y, 3))
    };
    const float delta_ap[] = {ap_x - pred_ap[0], ap_y - pred_ap[1]};
    sqr_ap_err = delta_ap[0]*delta_ap[0]+delta_ap[1]*delta_ap[1];
    float dx1_domega0[2][2];
    dx1_domega0[0][0] =  + 43.07  + -6.60329e-05 *begin_x + -4.08213 *lens_ipow(begin_dy, 2) + -11.315 *lens_ipow(begin_dx, 2) + 0.096252 *begin_y*begin_dy + 0.000394718 *lens_ipow(begin_y, 2) + -0.0563686 *begin_x*begin_dx + -0.00348141 *lens_ipow(begin_x, 2) + 1.47374 *lens_ipow(begin_lambda, 3) + -0.000104915 *begin_x*lens_ipow(begin_y, 2)*begin_dx + 0.000305414 *lens_ipow(begin_x, 3)*begin_dx + -1.8941 *lens_ipow(begin_x, 2)*lens_ipow(begin_dx, 4) + 2.57224e-06 *lens_ipow(begin_x, 4)*begin_y*begin_dy + 1.85111e-08 *lens_ipow(begin_x, 4)*lens_ipow(begin_y, 2) + -4.25433 *lens_ipow(begin_lambda, 8)+0.0f;
    dx1_domega0[0][1] =  + -8.16426 *begin_dx*begin_dy + 0.096252 *begin_y*begin_dx + -0.604918 *begin_x*begin_dy + -0.00939815 *begin_x*begin_y + 2.57224e-06 *lens_ipow(begin_x, 4)*begin_y*begin_dx + -1.71717e-06 *lens_ipow(begin_x, 5)*begin_dy+0.0f;
    dx1_domega0[1][0] =  + -0.00366551 *begin_dy + -0.649728 *begin_dx*begin_dy + -0.287871 *begin_y*begin_dx + 0.242198 *begin_x*begin_dy + -0.0063857 *begin_x*begin_y + -0.0165559 *begin_x*begin_y*lens_ipow(begin_dy, 2) + 8.21189e-07 *begin_x*lens_ipow(begin_y, 3) + -0.017608 *lens_ipow(begin_x, 2)*begin_dx*begin_dy + 0.00080362 *begin_x*begin_y*lens_ipow(begin_lambda, 5)+0.0f;
    dx1_domega0[1][1] =  + 46.6881  + -0.00366551 *begin_dx + 1.43577 *lens_ipow(begin_dy, 2) + -0.324864 *lens_ipow(begin_dx, 2) + 0.0224305 *begin_y*begin_dy + -0.00566694 *lens_ipow(begin_y, 2) + 0.242198 *begin_x*begin_dx + 0.00360194 *lens_ipow(begin_x, 2) + 3.00014 *lens_ipow(begin_lambda, 4) + -0.0331118 *begin_x*begin_y*begin_dx*begin_dy + -0.00880398 *lens_ipow(begin_x, 2)*lens_ipow(begin_dx, 2) + -7.8079 *lens_ipow(begin_lambda, 8) + 5.57382e-07 *lens_ipow(begin_x, 6)*lens_ipow(begin_dy, 2)+0.0f;
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
    out[0] =  + -0.000559373  + -0.00369856 *begin_dy + 63.5158 *begin_dx + -0.77325 *begin_x + 97.6866 *begin_dx*lens_ipow(begin_dy, 2) + 37.5444 *lens_ipow(begin_dx, 3) + 0.633891 *begin_y*begin_dx*begin_dy + 0.0011821 *lens_ipow(begin_y, 2)*begin_dx + -1.43819 *begin_x*lens_ipow(begin_dy, 2) + -1.92031 *begin_x*lens_ipow(begin_dx, 2) + 0.00146817 *begin_x*begin_y*begin_dy + -0.00020038 *begin_x*lens_ipow(begin_y, 2) + 0.0172462 *lens_ipow(begin_x, 2)*begin_dx + -0.000268229 *lens_ipow(begin_x, 3) + 45.9277 *begin_dx*lens_ipow(begin_lambda, 3) + -0.136052 *begin_x*lens_ipow(begin_dx, 3) + 0.161732 *begin_x*lens_ipow(begin_lambda, 4) + -195.539 *begin_x*lens_ipow(begin_dx, 6) + 1.23226e-07 *lens_ipow(begin_x, 2)*lens_ipow(begin_y, 4)*begin_dx + 1.30366e-07 *lens_ipow(begin_x, 3)*lens_ipow(begin_y, 3)*begin_dy + -4.50682e-06 *lens_ipow(begin_x, 4)*begin_y*begin_dx*begin_dy + 1.4194e-07 *lens_ipow(begin_x, 6)*begin_dx + 3931.87 *lens_ipow(begin_dx, 3)*lens_ipow(begin_dy, 2)*lens_ipow(begin_lambda, 3) + 1.71046 *begin_y*begin_dx*begin_dy*lens_ipow(begin_lambda, 5) + -134.121 *begin_dx*lens_ipow(begin_lambda, 8) + 2902.14 *lens_ipow(begin_dx, 5)*lens_ipow(begin_lambda, 4) + -0.423753 *begin_x*lens_ipow(begin_lambda, 8) + 3.85761e-10 *lens_ipow(begin_x, 7)*begin_y*begin_dy;
    out[1] =  + -0.000111012  + 79.6288 *begin_dy + -0.228085 *begin_y + -4.27497e-05 *begin_x + -0.108885 *begin_dx*begin_dy + 0.000857922 *begin_x*begin_dy + 150.579 *lens_ipow(begin_dy, 3) + 84.9587 *lens_ipow(begin_dx, 2)*begin_dy + 0.850797 *begin_y*lens_ipow(begin_dy, 2) + 0.00850817 *lens_ipow(begin_y, 2)*begin_dy + -0.000150397 *lens_ipow(begin_y, 3) + -1.34274 *begin_x*begin_dx*begin_dy + 0.00218923 *begin_x*begin_y*begin_dx + 0.00447283 *lens_ipow(begin_x, 2)*begin_dy + -0.000253048 *lens_ipow(begin_x, 2)*begin_y + 56.1992 *begin_dy*lens_ipow(begin_lambda, 3) + 0.170187 *begin_y*lens_ipow(begin_dx, 2)*begin_lambda + 0.649229 *begin_y*lens_ipow(begin_lambda, 4) + -0.000956375 *begin_x*lens_ipow(begin_y, 2)*begin_dx*begin_dy + 0.0552712 *lens_ipow(begin_x, 2)*lens_ipow(begin_dy, 3) + 1.19927 *begin_y*lens_ipow(begin_dy, 2)*lens_ipow(begin_lambda, 4) + 1.10582e-07 *lens_ipow(begin_x, 2)*lens_ipow(begin_y, 4)*begin_dy + 8.48522e-08 *lens_ipow(begin_x, 5)*begin_y*begin_dx + 1.55696 *begin_x*begin_dx*begin_dy*lens_ipow(begin_lambda, 5) + -159.672 *begin_dy*lens_ipow(begin_lambda, 8) + -1.70819 *begin_y*lens_ipow(begin_lambda, 8) + -9.4026e-09 *lens_ipow(begin_y, 7)*lens_ipow(begin_dy, 2) + 5.28202e-10 *lens_ipow(begin_x, 6)*lens_ipow(begin_y, 2)*begin_dy;
    out[2] =  + -4.37489e-07  + -4.21202e-05 *begin_dy + -0.259245 *begin_dx + -0.0124343 *begin_x + -0.000843824 *begin_dx*begin_dy + -0.000940051 *lens_ipow(begin_dx, 2) + 3.07259 *begin_dx*lens_ipow(begin_dy, 2) + 2.03845 *lens_ipow(begin_dx, 3) + 0.0225453 *begin_y*begin_dx*begin_dy + -1.10586e-05 *lens_ipow(begin_y, 2)*begin_dx + -0.00955115 *begin_x*lens_ipow(begin_dy, 2) + -0.025243 *begin_x*lens_ipow(begin_dx, 2) + -0.000143628 *begin_x*begin_y*begin_dy + 8.48119e-07 *begin_x*lens_ipow(begin_y, 2) + -0.000133205 *lens_ipow(begin_x, 2)*begin_dx + 2.18779e-06 *lens_ipow(begin_x, 3) + 0.599197 *begin_dx*lens_ipow(begin_lambda, 3) + -0.00159841 *begin_x*lens_ipow(begin_dx, 3) + 0.000355605 *begin_x*lens_ipow(begin_lambda, 4) + -2.22124e-05 *lens_ipow(begin_x, 2)*begin_y*begin_dx*begin_dy + -2.11442e-05 *lens_ipow(begin_x, 3)*lens_ipow(begin_dx, 2) + 0.0100816 *begin_x*lens_ipow(begin_dy, 2)*lens_ipow(begin_lambda, 3) + 0.0414884 *begin_x*lens_ipow(begin_dx, 2)*lens_ipow(begin_lambda, 3) + 4.14707e-06 *lens_ipow(begin_y, 4)*begin_dx*lens_ipow(begin_dy, 2) + -0.0528377 *lens_ipow(begin_x, 2)*lens_ipow(begin_dx, 5) + 0.0329061 *begin_y*begin_dx*begin_dy*lens_ipow(begin_lambda, 5) + -1.71071 *begin_dx*lens_ipow(begin_lambda, 8) + -0.000171512 *begin_x*lens_ipow(begin_y, 2)*lens_ipow(begin_dx, 2)*lens_ipow(begin_lambda, 4);
    out[3] =  + -8.05852e-07  + -0.238115 *begin_dy + -0.0116464 *begin_y + -6.6141e-07 *begin_x + -0.00199569 *begin_dx*begin_dy + -1.06344e-05 *begin_y*begin_dx + 3.83305 *lens_ipow(begin_dy, 3) + 2.53126 *lens_ipow(begin_dx, 2)*begin_dy + 0.0402076 *begin_y*lens_ipow(begin_dy, 2) + 0.00858833 *begin_y*lens_ipow(begin_dx, 2) + 5.76231e-06 *lens_ipow(begin_y, 2)*begin_dy + 5.56565e-07 *lens_ipow(begin_y, 3) + -0.018506 *begin_x*begin_dx*begin_dy + -0.000111056 *begin_x*begin_y*begin_dx + -0.000114095 *lens_ipow(begin_x, 2)*begin_dy + 1.24151e-06 *lens_ipow(begin_x, 2)*begin_y + 0.720445 *begin_dy*lens_ipow(begin_lambda, 3) + 0.0076245 *begin_y*lens_ipow(begin_lambda, 4) + -2.27038e-05 *lens_ipow(begin_x, 2)*begin_y*lens_ipow(begin_dy, 2) + 0.0491605 *begin_y*lens_ipow(begin_dy, 2)*lens_ipow(begin_lambda, 4) + 0.0121162 *begin_y*lens_ipow(begin_dx, 2)*lens_ipow(begin_lambda, 4) + 0.0318132 *begin_x*begin_dx*begin_dy*lens_ipow(begin_lambda, 4) + 5.09222e-12 *lens_ipow(begin_x, 2)*lens_ipow(begin_y, 5) + -7.23122e-08 *lens_ipow(begin_x, 5)*begin_dx*begin_dy + -2.05414 *begin_dy*lens_ipow(begin_lambda, 8) + -0.0203676 *begin_y*lens_ipow(begin_lambda, 8) + 1.39095e-06 *lens_ipow(begin_y, 4)*begin_dy*lens_ipow(begin_lambda, 4) + 7.15784e-15 *lens_ipow(begin_y, 9);
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
    domega2_dx0[0][0] =  + -0.0124343  + -0.00955115 *lens_ipow(begin_dy, 2) + -0.025243 *lens_ipow(begin_dx, 2) + -0.000143628 *begin_y*begin_dy + 8.48119e-07 *lens_ipow(begin_y, 2) + -0.00026641 *begin_x*begin_dx + 6.56338e-06 *lens_ipow(begin_x, 2) + -0.00159841 *lens_ipow(begin_dx, 3) + 0.000355605 *lens_ipow(begin_lambda, 4) + -4.44248e-05 *begin_x*begin_y*begin_dx*begin_dy + -6.34326e-05 *lens_ipow(begin_x, 2)*lens_ipow(begin_dx, 2) + 0.0100816 *lens_ipow(begin_dy, 2)*lens_ipow(begin_lambda, 3) + 0.0414884 *lens_ipow(begin_dx, 2)*lens_ipow(begin_lambda, 3) + -0.105675 *begin_x*lens_ipow(begin_dx, 5) + -0.000171512 *lens_ipow(begin_y, 2)*lens_ipow(begin_dx, 2)*lens_ipow(begin_lambda, 4)+0.0f;
    domega2_dx0[0][1] =  + 0.0225453 *begin_dx*begin_dy + -2.21172e-05 *begin_y*begin_dx + -0.000143628 *begin_x*begin_dy + 1.69624e-06 *begin_x*begin_y + -2.22124e-05 *lens_ipow(begin_x, 2)*begin_dx*begin_dy + 1.65883e-05 *lens_ipow(begin_y, 3)*begin_dx*lens_ipow(begin_dy, 2) + 0.0329061 *begin_dx*begin_dy*lens_ipow(begin_lambda, 5) + -0.000343023 *begin_x*begin_y*lens_ipow(begin_dx, 2)*lens_ipow(begin_lambda, 4)+0.0f;
    domega2_dx0[1][0] =  + -6.6141e-07  + -0.018506 *begin_dx*begin_dy + -0.000111056 *begin_y*begin_dx + -0.00022819 *begin_x*begin_dy + 2.48302e-06 *begin_x*begin_y + -4.54076e-05 *begin_x*begin_y*lens_ipow(begin_dy, 2) + 0.0318132 *begin_dx*begin_dy*lens_ipow(begin_lambda, 4) + 1.01844e-11 *begin_x*lens_ipow(begin_y, 5) + -3.61561e-07 *lens_ipow(begin_x, 4)*begin_dx*begin_dy+0.0f;
    domega2_dx0[1][1] =  + -0.0116464  + -1.06344e-05 *begin_dx + 0.0402076 *lens_ipow(begin_dy, 2) + 0.00858833 *lens_ipow(begin_dx, 2) + 1.15246e-05 *begin_y*begin_dy + 1.6697e-06 *lens_ipow(begin_y, 2) + -0.000111056 *begin_x*begin_dx + 1.24151e-06 *lens_ipow(begin_x, 2) + 0.0076245 *lens_ipow(begin_lambda, 4) + -2.27038e-05 *lens_ipow(begin_x, 2)*lens_ipow(begin_dy, 2) + 0.0491605 *lens_ipow(begin_dy, 2)*lens_ipow(begin_lambda, 4) + 0.0121162 *lens_ipow(begin_dx, 2)*lens_ipow(begin_lambda, 4) + 2.54611e-11 *lens_ipow(begin_x, 2)*lens_ipow(begin_y, 4) + -0.0203676 *lens_ipow(begin_lambda, 8) + 5.56378e-06 *lens_ipow(begin_y, 3)*begin_dy*lens_ipow(begin_lambda, 4) + 6.44206e-14 *lens_ipow(begin_y, 8)+0.0f;
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
  out[4] =  + 0.0742279  + 0.237429 *begin_lambda + -1.02132e-05 *begin_dx + 2.5632e-07 *begin_y + -0.0173411 *lens_ipow(begin_dy, 2) + -0.00755505 *lens_ipow(begin_dx, 2) + -5.92563e-05 *begin_y*begin_dy + -6.60204e-07 *lens_ipow(begin_y, 2) + -2.37729e-06 *begin_x*begin_dy + 1.39742e-05 *begin_x*begin_dx + -2.36726e-06 *lens_ipow(begin_x, 2) + -0.205642 *lens_ipow(begin_lambda, 3) + 3.38108e-06 *begin_x*lens_ipow(begin_dx, 2) + -2.30251 *lens_ipow(begin_dx, 2)*lens_ipow(begin_dy, 2) + -0.0346063 *begin_y*lens_ipow(begin_dy, 3) + -0.026213 *begin_y*lens_ipow(begin_dx, 2)*begin_dy + -0.000265425 *lens_ipow(begin_y, 2)*lens_ipow(begin_dy, 2) + -7.08049e-05 *lens_ipow(begin_y, 2)*lens_ipow(begin_dx, 2) + -4.36657e-07 *lens_ipow(begin_y, 3)*begin_dy + -0.00212413 *begin_x*begin_dx*lens_ipow(begin_dy, 2) + -1.50463e-05 *begin_x*begin_y*begin_dx*begin_dy + 9.60923e-07 *lens_ipow(begin_x, 2)*begin_y*begin_dy + -1.63307e-06 *lens_ipow(begin_x, 3)*begin_dx + -40.5667 *lens_ipow(begin_dy, 6) + -19.7352 *lens_ipow(begin_dx, 6) + 4.68306e-09 *lens_ipow(begin_x, 3)*lens_ipow(begin_y, 2)*begin_dx + -1.65328e-10 *lens_ipow(begin_x, 6) + 0.224671 *lens_ipow(begin_lambda, 9);
else
  out[4] = 0.0f;
