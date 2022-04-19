#include "global.h"

void guRotateF(MtxF* m, f32 a, f32 x, f32 y, f32 z) {
    static f32 D_80134D10 = std::numbers::pi_v<float> / 180.0f;
    f32 sine;
    f32 cosine;
    f32 ab;
    f32 bc;
    f32 ca;
    f32 t;
    f32 xs;
    f32 ys;
    f32 zs;

    guNormalize(&x, &y, &z);

    a = a * D_80134D10;

    sine = sinf(a);
    cosine = cosf(a);

    ab = x * y * (1 - cosine);
    bc = y * z * (1 - cosine);
    ca = z * x * (1 - cosine);

    guMtxIdentF(m);

    xs = x * sine;
    ys = y * sine;
    zs = z * sine;

    t = x * x;
    m->mf[0][0] = (1 - t) * cosine + t;
    m->mf[2][1] = bc - xs;
    m->mf[1][2] = bc + xs;
    t = y * y;
    m->mf[1][1] = (1 - t) * cosine + t;
    m->mf[2][0] = ca + ys;
    m->mf[0][2] = ca - ys;
    t = z * z;
    m->mf[2][2] = (1 - t) * cosine + t;
    m->mf[1][0] = ab - zs;
    m->mf[0][1] = ab + zs;
}

void guRotate(Mtx* m, f32 a, f32 x, f32 y, f32 z) {
    MtxF mf;

    guRotateF(&mf, a, x, y, z);
    guMtxF2L(&mf, m);
}
