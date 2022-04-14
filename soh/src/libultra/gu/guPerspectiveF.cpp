#include "global.h"

void guPerspectiveF(f32 mf[4][4], u16* perspNorm, f32 fovy, f32 aspect, f32 near_, f32 far_, f32 scale) {
    f32 yscale;
    s32 row;
    s32 col;

    guMtxIdentF(mf);

    fovy *= GU_PI / 180.0;
    yscale = cosf(fovy / 2) / sinf(fovy / 2);
    mf[0][0] = yscale / aspect;
    mf[1][1] = yscale;
    mf[2][2] = (near_ + far_) / (near_ - far_);
    mf[2][3] = -1;
    mf[3][2] = 2 * near_ * far_ / (near_ - far_);
    mf[3][3] = 0.0f;

    for (row = 0; row < 4; row++) {
        for (col = 0; col < 4; col++) {
            mf[row][col] *= scale;
        }
    }

    if (perspNorm != NULL) {
        if (near_ + far_ <= 2.0) {
            *perspNorm = 65535;
        } else {
            *perspNorm = (f64)(1 << 17) / (near_ + far_);
            if (*perspNorm <= 0) {
                *perspNorm = 1;
            }
        }
    }
}
void guPerspective(Mtx* m, u16* perspNorm, f32 fovy, f32 aspect, f32 near_, f32 far_, f32 scale) {
    f32 mf[4][4];

    guPerspectiveF(mf, perspNorm, fovy, aspect, near_, far_, scale);
    guMtxF2L((MtxF*)mf, m);

}
