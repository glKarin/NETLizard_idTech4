#ifndef _KARIN_MATHALGO_H
#define _KARIN_MATHALGO_H

#include "netlizard.h"

#define NL_VEC3_X(v) (v).xyz[0]
#define NL_VEC3_Y(v) (v).xyz[1]
#define NL_VEC3_Z(v) (v).xyz[2]
#define NL_VEC3(x, y, z) { {x, y, z} }
#define NL_VEC3V(v) { {v[0], v[1], v[2]} }

#define NL_TRI_A(v) (v).abc[0]
#define NL_TRI_B(v) (v).abc[1]
#define NL_TRI_C(v) (v).abc[2]
#define NL_TRI(a, b, c) { {a, b, c} }

#define NL_BOUND_MIN(v) (v).bv[0]
#define NL_BOUND_MAX(v) (v).bv[1]

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct NLvec3_e
    {
        NLfloat xyz[3];
    } NLvec3;

    typedef struct NLtri_e
    {
        NLvec3 abc[3];
    } NLtri;

    typedef struct NLbound_e
    {
        NLvec3 bv[2];
    } NLbound;

void vector3_addv(NLvec3 *r, const NLvec3 *a, const NLvec3 *b);
void vector3_subtractv(NLvec3 *r, const NLvec3 *a, const NLvec3 *b);
NLboolean triangle_cale_normal(const NLtri *tri, NLvec3 *normal);
void vector3_crossv(NLvec3 *r, const NLvec3 *a, const NLvec3 *b);
void vector3_normalizev(NLvec3 *a);
float vector3_length(const NLvec3 *a);
void vector3_scalev(NLvec3 *a, float n);
void bound_invalid(NLbound *bv);
void bound_add_vec3(NLbound *bv, const NLvec3 *o);

#ifdef __cplusplus
}
#endif

#endif
