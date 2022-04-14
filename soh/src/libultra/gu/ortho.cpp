#include "global.h"

void guOrthoF(f32 mf[4][4], f32 left, f32 right, f32 bottom, f32 top, f32 near_, f32 far_, f32 scale) {
    s32 i, j;

    guMtxIdentF(mf);

    mf[0][0] = 2 / (right - left);
    mf[1][1] = 2 / (top - bottom);
    mf[2][2] = -2 / (far_ - near_);
    mf[3][0] = -(right + left) / (right - left);
    mf[3][1] = -(top + bottom) / (top - bottom);
    mf[3][2] = -(far_ + near_) / (far_ - near_);
    mf[3][3] = 1;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            mf[i][j] *= scale;
        }
    }
}

void guOrtho(Mtx* mtx, f32 left, f32 right, f32 bottom, f32 top, f32 near_, f32 far_, f32 scale) {
    f32 mf[4][4];

    guOrthoF(mf, left, right, bottom, top, near_, far_, scale);

    guMtxF2L((MtxF*)mf, mtx);
}
