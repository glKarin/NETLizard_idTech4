#include "mathalgo.h"

#include <float.h>

#include "priv_local.h"

NLboolean triangle_cale_normal(const NLtri *tri, NLvec3 *normal)
{
    NLvec3 first, second;
    vector3_subtractv(&first, &(NL_TRI_B(*tri)), &(NL_TRI_A(*tri)));
    vector3_subtractv(&second, &(NL_TRI_C(*tri)), &(NL_TRI_A(*tri)));

    //vector3_crossv(normal, &second, &first); // org
    vector3_crossv(normal, &first, &second);
    vector3_normalizev(normal);

    return NL_TRUE;
}

void vector3_addv(NLvec3 *r, const NLvec3 *a, const NLvec3 *b)
{
    NL_VEC3_X(*r) = NL_VEC3_X(*a) + NL_VEC3_X(*b);
    NL_VEC3_Y(*r) = NL_VEC3_Y(*a) + NL_VEC3_Y(*b);
    NL_VEC3_Z(*r) = NL_VEC3_Z(*a) + NL_VEC3_Z(*b);
}

void vector3_subtractv(NLvec3 *r, const NLvec3 *a, const NLvec3 *b)
{
    NL_VEC3_X(*r) = NL_VEC3_X(*a) - NL_VEC3_X(*b);
    NL_VEC3_Y(*r) = NL_VEC3_Y(*a) - NL_VEC3_Y(*b);
    NL_VEC3_Z(*r) = NL_VEC3_Z(*a) - NL_VEC3_Z(*b);
}

void vector3_crossv(NLvec3 *r, const NLvec3 *a, const NLvec3 *b)
{
    NL_VEC3_X(*r) = NL_VEC3_Y(*a) * NL_VEC3_Z(*b) - NL_VEC3_Z(*a) * NL_VEC3_Y(*b);
    NL_VEC3_Y(*r) = NL_VEC3_Z(*a) * NL_VEC3_X(*b) - NL_VEC3_X(*a) * NL_VEC3_Z(*b);
    NL_VEC3_Z(*r) = NL_VEC3_X(*a) * NL_VEC3_Y(*b) - NL_VEC3_Y(*a) * NL_VEC3_X(*b);
}

void vector3_normalizev(NLvec3 *a)
{
    float m;

    m = vector3_length(a);
    if(m != 0.0)
    {
        m = 1.0 / m;
        vector3_scalev(a, m);
    }
}

float vector3_length(const NLvec3 *a)
{
#define POW2(n) ((n) * (n))
    return sqrt(POW2(NL_VEC3_X(*a)) + POW2(NL_VEC3_Y(*a)) + POW2(NL_VEC3_Z(*a)));
#undef POW2
}

void vector3_scalev(NLvec3 *a, float n)
{
    NL_VEC3_X(*a) *= n;
    NL_VEC3_Y(*a) *= n;
    NL_VEC3_Z(*a) *= n;
}

void bound_invalid(NLbound *bv)
{
    NL_VEC3_X(NL_BOUND_MIN(*bv)) = FLT_MAX;
    NL_VEC3_Y(NL_BOUND_MIN(*bv)) = FLT_MAX;
    NL_VEC3_Z(NL_BOUND_MIN(*bv)) = FLT_MAX;

    NL_VEC3_X(NL_BOUND_MAX(*bv)) = -FLT_MAX;
    NL_VEC3_Y(NL_BOUND_MAX(*bv)) = -FLT_MAX;
    NL_VEC3_Z(NL_BOUND_MAX(*bv)) = -FLT_MAX;
}

#define IF_LESS_ASSIGN(a, b) \
    if((b) < (a)) { \
        (a) = (b); \
    }
#define IF_GREATER_ASSIGN(a, b) \
    if((b) > (a)) { \
        (a) = (b); \
    }
void bound_add_vec3(NLbound *bv, const NLvec3 *o)
{
    IF_LESS_ASSIGN(NL_VEC3_X(NL_BOUND_MIN(*bv)), NL_VEC3_X(*o))
    IF_LESS_ASSIGN(NL_VEC3_Y(NL_BOUND_MIN(*bv)), NL_VEC3_Y(*o))
    IF_LESS_ASSIGN(NL_VEC3_Z(NL_BOUND_MIN(*bv)), NL_VEC3_Z(*o))

    IF_GREATER_ASSIGN(NL_VEC3_X(NL_BOUND_MAX(*bv)), NL_VEC3_X(*o))
    IF_GREATER_ASSIGN(NL_VEC3_Y(NL_BOUND_MAX(*bv)), NL_VEC3_Y(*o))
    IF_GREATER_ASSIGN(NL_VEC3_Z(NL_BOUND_MAX(*bv)), NL_VEC3_Z(*o))
}
#undef IF_LESS_ASSIGN
#undef IF_GREATER_ASSIGN