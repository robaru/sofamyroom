/*********************************************************************//**
 * @file 3D.c
 * @brief Three-dimensional transformation routines.
 **********************************************************************/
#include <math.h>

#include "3D.h"
#include "defs.h"
#include "types.h"

#if 0
/** Compute yaw-pitch-roll transformation matrix.
 */
/* from Mathematica:
	Cos[a] Cos[b]                           -Cos[b] Sin[a]                          Sin[b]
	Cos[c] Sin[a]+Cos[a] Sin[b] Sin[c]      Cos[a] Cos[c]-Sin[a] Sin[b] Sin[c]      -Cos[b] Sin[c]
	-Cos[a] Cos[c] Sin[b]+Sin[a] Sin[c]     Cos[c] Sin[a] Sin[b]+Cos[a] Sin[c]      Cos[b] Cos[c]

*/
static void ComputeYPRT(const YPR *ypr, int sign, YPRT *yprt)
{
    double Cy = cos(ypr->yaw),   Sy = sign * sin(ypr->yaw);
    double Cp = cos(ypr->pitch), Sp = sign * sin(ypr->pitch);
    double Cr = cos(ypr->roll),  Sr = sign * sin(ypr->roll);
    
/* original:    {{ Cy*Cp, -Sy,    -Cy*Sp },{ Sy*Cp*Cr - Sp*Sr, Cy*Cr,           -Sy*Sp*Cr - Cp*Sr}, { Sy*Cp*Sr + Sp*Cr, Cy*Sr,         -Sy*Sp*Sr + Cp*Cr } }

	(*yprt)[0][0] =  Cy*Cp;
    (*yprt)[0][1] = -Sy;
    (*yprt)[0][2] = -Cy*Sp;
    
    (*yprt)[1][0] =  Sy*Cp*Cr - Sp*Sr;
    (*yprt)[1][1] =  Cy*   Cr;
    (*yprt)[1][2] = -Sy*Sp*Cr - Cp*Sr;
    
    (*yprt)[2][0] =  Sy*Cp*Sr + Sp*Cr;
    (*yprt)[2][1] =  Cy*   Sr;
    (*yprt)[2][2] = -Sy*Sp*Sr + Cp*Cr;
*/

	/* mathematica: {{ Cy*Cp, -Cp*Sy, Sp     },{ Cr*Sy + Cy*Sp*Sr, Cy*Cr - Sy*Sp*Sr, -Cp*Sr          }, { -Cy*Cr*Sp+Sy*Sr,  Cr*Sy*Sp+Cy*Sr, Cp*Cr            } } */
	(*yprt)[0][0] = Cy*Cp;
	(*yprt)[0][1] = -Cp*Sy;
	(*yprt)[0][2] = Sp;
	(*yprt)[1][0] = Cr*Sy+Cy*Sp*Sr;
	(*yprt)[1][1] = Cy*Cr-Sy*Sp*Sr;
	(*yprt)[1][2] = -Cp*Sr;
	(*yprt)[2][0] = -Cy*Cr*Sp+Sy*Sr;
	(*yprt)[2][1] = Cr*Sy*Sp+Cy*Sr;
	(*yprt)[2][2] = Cp*Cr;
}
#endif

/** Compute sensor-to-room coordinate transformation matrix.
 *
 *  @param[in]       ypr   Sensor orientation (yaw, pitch, roll) in degrees.
 *  @param[in,out]   yprT  Sensor-to-room coordinate transformation matrix.
 *
 */
void ComputeSensor2RoomYPRT(const YPR *ypr, YPRT *yprt)
{
	/*ComputeYPRT(ypr,1,yprt); */
    double Cy = cos(PI * ypr->yaw   / 180), Sy = sin(PI * ypr->yaw   / 180);
    double Cp = cos(PI * ypr->pitch / 180), Sp = sin(PI * ypr->pitch / 180);
    double Cr = cos(PI * ypr->roll  / 180), Sr = sin(PI * ypr->roll  / 180);

	(*yprt)[0][0] = Cy*Cp;
	(*yprt)[0][1] = -Cr*Sy-Cy*Sp*Sr;
	(*yprt)[0][2] = -Cy*Cr*Sp+Sy*Sr;
	(*yprt)[1][0] = Cp*Sy;
	(*yprt)[1][1] = Cy*Cr-Sy*Sp*Sr;
	(*yprt)[1][2] = -Cr*Sy*Sp-Cy*Sr;
	(*yprt)[2][0] = Sp;
	(*yprt)[2][1] = Cp*Sr;
	(*yprt)[2][2] = Cp*Cr;
}

/** Compute room-to-sensor coordinate transformation matrix.
 *
 *  @param[in]       ypr   Sensor orientation (yaw, pitch, roll) in degrees.
 *  @param[in,out]   yprT  Room-to-sensor coordinate transformation matrix.
 *
 */
void ComputeRoom2SensorYPRT(const YPR *ypr, YPRT *yprt)
{
	/*ComputeYPRT(ypr,-1,yprt); */
    double Cy = cos(PI * ypr->yaw   / 180), Sy = sin(PI * ypr->yaw   / 180);
    double Cp = cos(PI * ypr->pitch / 180), Sp = sin(PI * ypr->pitch / 180);
    double Cr = cos(PI * ypr->roll  / 180), Sr = sin(PI * ypr->roll  / 180);

	(*yprt)[0][0] = Cy*Cp;
	(*yprt)[0][1] = Cp*Sy;
	(*yprt)[0][2] = Sp;
	(*yprt)[1][0] = -Cr*Sy-Cy*Sp*Sr;
	(*yprt)[1][1] = Cy*Cr-Sy*Sp*Sr;
	(*yprt)[1][2] = Cp*Sr;
	(*yprt)[2][0] = -Cy*Cr*Sp+Sy*Sr;
	(*yprt)[2][1] = -Cr*Sy*Sp-Cy*Sr;
	(*yprt)[2][2] = Cp*Cr;
}

/** Apply yaw-pitch-roll coordinate transformation. 
 */
void YawPitchRoll(const XYZ *xyz, const YPRT *yprt, XYZ *xyzhat)
{
    xyzhat->x = ((*yprt)[0][0])*(xyz->x) + ((*yprt)[0][1])*(xyz->y) + ((*yprt)[0][2])*(xyz->z);
    xyzhat->y = ((*yprt)[1][0])*(xyz->x) + ((*yprt)[1][1])*(xyz->y) + ((*yprt)[1][2])*(xyz->z);
    xyzhat->z = ((*yprt)[2][0])*(xyz->x) + ((*yprt)[2][1])*(xyz->y) + ((*yprt)[2][2])*(xyz->z);
}

/** Apply yaw-pitch-roll coordinate transformation (in place). 
 */
void YawPitchRoll_InPlace(XYZ *xyz, const YPRT *yprt)
{
	XYZ tmp;
    tmp.x = ((*yprt)[0][0])*(xyz->x) + ((*yprt)[0][1])*(xyz->y) + ((*yprt)[0][2])*(xyz->z);
    tmp.y = ((*yprt)[1][0])*(xyz->x) + ((*yprt)[1][1])*(xyz->y) + ((*yprt)[1][2])*(xyz->z);
    tmp.z = ((*yprt)[2][0])*(xyz->x) + ((*yprt)[2][1])*(xyz->y) + ((*yprt)[2][2])*(xyz->z);
	*xyz = tmp;
}

/** Convert (x,y,z) coordinates to vertical polar azimuth/elevation coordinates. 
 *
 *  @param[in]		xyz		pointer to XYZ structure containing (x,y,z) coordinates.
 *  @param[in,out]	vp		pointer to AZEL structure for vertical polar azimuth/elevation.
 *
 */
void azel_vp(const XYZ *xyz, AZEL *vp)
{
    /* az = atan2(x(2,:),x(1,:)); */
    /* el = atan(x(3,:)./(eps+sqrt(x(1,:).^2+x(2,:).^2))); */

    vp->az = atan2(xyz->y,xyz->x);
    vp->el = atan(xyz->z / (EPS0 + sqrt( (xyz->x)*(xyz->x) + (xyz->y)*(xyz->y) ) ) );
}
