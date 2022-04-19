#include "global.h"

void guOrthoF(MtxF* mf, f32 left, f32 right, f32 bottom, f32 top, f32 near_, f32 far_, f32 scale) {
    s32 i, j;

    guMtxIdentF(mf);

    mf->mf[0][0] = 2 / (right - left);
    mf->mf[1][1] = 2 / (top - bottom);
    mf->mf[2][2] = -2 / (far_ - near_);
    mf->mf[3][0] = -(right + left) / (right - left);
    mf->mf[3][1] = -(top + bottom) / (top - bottom);
    mf->mf[3][2] = -(far_ + near_) / (far_ - near_);
    mf->mf[3][3] = 1;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            mf->mf[i][j] *= scale;
        }
    }
}

void guOrtho(Mtx* mtx, f32 left, f32 right, f32 bottom, f32 top, f32 near_, f32 far_, f32 scale) {
    MtxF mf;

    guOrthoF(&mf, left, right, bottom, top, near_, far_, scale);

    guMtxF2L(&mf, mtx);
}
