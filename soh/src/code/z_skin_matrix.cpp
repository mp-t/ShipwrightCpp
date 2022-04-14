#include "global.h"
#include "vt.h"

// clang-format off
MtxF sMtxFClear = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
};
// clang-format on

/**
 * Multiplies the matrix mf by a 4 components column vector [ src , 1 ] and writes the resulting 4 components to xyzDest
 * and wDest.
 *
 * \f[ \begin{bmatrix} \texttt{xyzDest} \\ \texttt{wDest} \\ \end{bmatrix}
 *      = [\texttt{mf}] \cdot
 *        \begin{bmatrix} \texttt{src} \\ 1 \\ \end{bmatrix}
 * \f]
 */
void SkinMatrix_Vec3fMtxFMultXYZW(MtxF* mf, Vec3f* src, Vec3f* xyzDest, f32* wDest) {
    xyzDest->x = mf->mf_raw.xw + ((src->x * mf->mf_raw.xx) + (src->y * mf->mf_raw.xy) + (src->z * mf->mf_raw.xz));
    xyzDest->y = mf->mf_raw.yw + ((src->x * mf->mf_raw.yx) + (src->y * mf->mf_raw.yy) + (src->z * mf->mf_raw.yz));
    xyzDest->z = mf->mf_raw.zw + ((src->x * mf->mf_raw.zx) + (src->y * mf->mf_raw.zy) + (src->z * mf->mf_raw.zz));
    *wDest = mf->mf_raw.ww + ((src->x * mf->mf_raw.wx) + (src->y * mf->mf_raw.wy) + (src->z * mf->mf_raw.wz));
}

/**
 * Multiplies the matrix mf by a 4 components column vector [ src , 1 ] and writes the resulting xyz components to dest.
 *
 * \f[ \begin{bmatrix} \texttt{dest} \\ - \\ \end{bmatrix}
 *      = [\texttt{mf}] \cdot
 *        \begin{bmatrix} \texttt{src} \\ 1 \\ \end{bmatrix}
 * \f]
 */
void SkinMatrix_Vec3fMtxFMultXYZ(MtxF* mf, Vec3f* src, Vec3f* dest) {
    f32 mx = mf->mf_raw.xx;
    f32 my = mf->mf_raw.xy;
    f32 mz = mf->mf_raw.xz;
    f32 mw = mf->mf_raw.xw;

    dest->x = mw + ((src->x * mx) + (src->y * my) + (src->z * mz));
    mx = mf->mf_raw.yx;
    my = mf->mf_raw.yy;
    mz = mf->mf_raw.yz;
    mw = mf->mf_raw.yw;
    dest->y = mw + ((src->x * mx) + (src->y * my) + (src->z * mz));
    mx = mf->mf_raw.zx;
    my = mf->mf_raw.zy;
    mz = mf->mf_raw.zz;
    mw = mf->mf_raw.zw;
    dest->z = mw + ((src->x * mx) + (src->y * my) + (src->z * mz));
}

/**
 * Matrix multiplication, dest = mfA * mfB.
 * mfB and dest should not be the same matrix.
 */
void SkinMatrix_MtxFMtxFMult(MtxF* mfA, MtxF* mfB, MtxF* dest) {
    f32 cx;
    f32 cy;
    f32 cz;
    f32 cw;
    //---ROW1---
    f32 rx = mfA->mf_raw.xx;
    f32 ry = mfA->mf_raw.xy;
    f32 rz = mfA->mf_raw.xz;
    f32 rw = mfA->mf_raw.xw;
    //--------

    cx = mfB->mf_raw.xx;
    cy = mfB->mf_raw.yx;
    cz = mfB->mf_raw.zx;
    cw = mfB->mf_raw.wx;
    dest->mf_raw.xx = (rx * cx) + (ry * cy) + (rz * cz) + (rw * cw);

    cx = mfB->mf_raw.xy;
    cy = mfB->mf_raw.yy;
    cz = mfB->mf_raw.zy;
    cw = mfB->mf_raw.wy;
    dest->mf_raw.xy = (rx * cx) + (ry * cy) + (rz * cz) + (rw * cw);

    cx = mfB->mf_raw.xz;
    cy = mfB->mf_raw.yz;
    cz = mfB->mf_raw.zz;
    cw = mfB->mf_raw.wz;
    dest->mf_raw.xz = (rx * cx) + (ry * cy) + (rz * cz) + (rw * cw);

    cx = mfB->mf_raw.xw;
    cy = mfB->mf_raw.yw;
    cz = mfB->mf_raw.zw;
    cw = mfB->mf_raw.ww;
    dest->mf_raw.xw = (rx * cx) + (ry * cy) + (rz * cz) + (rw * cw);

    //---ROW2---
    rx = mfA->mf_raw.yx;
    ry = mfA->mf_raw.yy;
    rz = mfA->mf_raw.yz;
    rw = mfA->mf_raw.yw;
    //--------
    cx = mfB->mf_raw.xx;
    cy = mfB->mf_raw.yx;
    cz = mfB->mf_raw.zx;
    cw = mfB->mf_raw.wx;
    dest->mf_raw.yx = (rx * cx) + (ry * cy) + (rz * cz) + (rw * cw);

    cx = mfB->mf_raw.xy;
    cy = mfB->mf_raw.yy;
    cz = mfB->mf_raw.zy;
    cw = mfB->mf_raw.wy;
    dest->mf_raw.yy = (rx * cx) + (ry * cy) + (rz * cz) + (rw * cw);

    cx = mfB->mf_raw.xz;
    cy = mfB->mf_raw.yz;
    cz = mfB->mf_raw.zz;
    cw = mfB->mf_raw.wz;
    dest->mf_raw.yz = (rx * cx) + (ry * cy) + (rz * cz) + (rw * cw);

    cx = mfB->mf_raw.xw;
    cy = mfB->mf_raw.yw;
    cz = mfB->mf_raw.zw;
    cw = mfB->mf_raw.ww;
    dest->mf_raw.yw = (rx * cx) + (ry * cy) + (rz * cz) + (rw * cw);

    //---ROW3---
    rx = mfA->mf_raw.zx;
    ry = mfA->mf_raw.zy;
    rz = mfA->mf_raw.zz;
    rw = mfA->mf_raw.zw;
    //--------
    cx = mfB->mf_raw.xx;
    cy = mfB->mf_raw.yx;
    cz = mfB->mf_raw.zx;
    cw = mfB->mf_raw.wx;
    dest->mf_raw.zx = (rx * cx) + (ry * cy) + (rz * cz) + (rw * cw);

    cx = mfB->mf_raw.xy;
    cy = mfB->mf_raw.yy;
    cz = mfB->mf_raw.zy;
    cw = mfB->mf_raw.wy;
    dest->mf_raw.zy = (rx * cx) + (ry * cy) + (rz * cz) + (rw * cw);

    cx = mfB->mf_raw.xz;
    cy = mfB->mf_raw.yz;
    cz = mfB->mf_raw.zz;
    cw = mfB->mf_raw.wz;
    dest->mf_raw.zz = (rx * cx) + (ry * cy) + (rz * cz) + (rw * cw);

    cx = mfB->mf_raw.xw;
    cy = mfB->mf_raw.yw;
    cz = mfB->mf_raw.zw;
    cw = mfB->mf_raw.ww;
    dest->mf_raw.zw = (rx * cx) + (ry * cy) + (rz * cz) + (rw * cw);

    //---ROW4---
    rx = mfA->mf_raw.wx;
    ry = mfA->mf_raw.wy;
    rz = mfA->mf_raw.wz;
    rw = mfA->mf_raw.ww;
    //--------
    cx = mfB->mf_raw.xx;
    cy = mfB->mf_raw.yx;
    cz = mfB->mf_raw.zx;
    cw = mfB->mf_raw.wx;
    dest->mf_raw.wx = (rx * cx) + (ry * cy) + (rz * cz) + (rw * cw);

    cx = mfB->mf_raw.xy;
    cy = mfB->mf_raw.yy;
    cz = mfB->mf_raw.zy;
    cw = mfB->mf_raw.wy;
    dest->mf_raw.wy = (rx * cx) + (ry * cy) + (rz * cz) + (rw * cw);

    cx = mfB->mf_raw.xz;
    cy = mfB->mf_raw.yz;
    cz = mfB->mf_raw.zz;
    cw = mfB->mf_raw.wz;
    dest->mf_raw.wz = (rx * cx) + (ry * cy) + (rz * cz) + (rw * cw);

    cx = mfB->mf_raw.xw;
    cy = mfB->mf_raw.yw;
    cz = mfB->mf_raw.zw;
    cw = mfB->mf_raw.ww;
    dest->mf_raw.ww = (rx * cx) + (ry * cy) + (rz * cz) + (rw * cw);
}

/**
 * "Clear" in thisv file means the identity matrix.
 */
void SkinMatrix_GetClear(MtxF** mfp) {
    *mfp = &sMtxFClear;
}

void SkinMatrix_Clear(MtxF* mf) {
    mf->mf_raw.xx = 1.0f;
    mf->mf_raw.yy = 1.0f;
    mf->mf_raw.zz = 1.0f;
    mf->mf_raw.ww = 1.0f;
    mf->mf_raw.yx = 0.0f;
    mf->mf_raw.zx = 0.0f;
    mf->mf_raw.wx = 0.0f;
    mf->mf_raw.xy = 0.0f;
    mf->mf_raw.zy = 0.0f;
    mf->mf_raw.wy = 0.0f;
    mf->mf_raw.xz = 0.0f;
    mf->mf_raw.yz = 0.0f;
    mf->mf_raw.wz = 0.0f;
    mf->mf_raw.xw = 0.0f;
    mf->mf_raw.yw = 0.0f;
    mf->mf_raw.zw = 0.0f;
}

void SkinMatrix_MtxFCopy(MtxF* src, MtxF* dest) {
    dest->mf_raw.xx = src->mf_raw.xx;
    dest->mf_raw.yx = src->mf_raw.yx;
    dest->mf_raw.zx = src->mf_raw.zx;
    dest->mf_raw.wx = src->mf_raw.wx;
    dest->mf_raw.xy = src->mf_raw.xy;
    dest->mf_raw.yy = src->mf_raw.yy;
    dest->mf_raw.zy = src->mf_raw.zy;
    dest->mf_raw.wy = src->mf_raw.wy;
    dest->mf_raw.xz = src->mf_raw.xz;
    dest->mf_raw.yz = src->mf_raw.yz;
    dest->mf_raw.zz = src->mf_raw.zz;
    dest->mf_raw.wz = src->mf_raw.wz;
    dest->mf_raw.xw = src->mf_raw.xw;
    dest->mf_raw.yw = src->mf_raw.yw;
    dest->mf_raw.zw = src->mf_raw.zw;
    dest->mf_raw.ww = src->mf_raw.ww;
}

/**
 * Inverts a matrix using the Gauss-Jordan method.
 * returns 0 if successfully inverted
 * returns 2 if matrix non-invertible (0 determinant)
 */
s32 SkinMatrix_Invert(MtxF* src, MtxF* dest) {
    MtxF mfCopy;
    s32 i;
    s32 pad;
    f32 temp2;
    f32 temp1;
    s32 thisCol;
    s32 thisRow;

    SkinMatrix_MtxFCopy(src, &mfCopy);
    SkinMatrix_Clear(dest);
    for (thisCol = 0; thisCol < 4; thisCol++) {
        thisRow = thisCol;
        while ((thisRow < 4) && (fabsf(mfCopy.mf[thisCol][thisRow]) < 0.0005f)) {
            thisRow++;
        }
        if (thisRow == 4) {
            // Reaching row = 4 means the column is either all 0 or a duplicate column.
            // Therefore src is a singular matrix (0 determinant).

            osSyncPrintf(VT_COL(YELLOW, BLACK));
            osSyncPrintf("Skin_Matrix_InverseMatrix():逆行列つくれません\n");
            osSyncPrintf(VT_RST);
            return 2;
        }

        if (thisRow != thisCol) {
            // Diagonal element mf[thisCol][thisCol] is zero.
            // Swap the rows thisCol and thisRow.
            for (i = 0; i < 4; i++) {
                temp1 = mfCopy.mf[i][thisRow];
                mfCopy.mf[i][thisRow] = mfCopy.mf[i][thisCol];
                mfCopy.mf[i][thisCol] = temp1;

                temp2 = dest->mf[i][thisRow];
                dest->mf[i][thisRow] = dest->mf[i][thisCol];
                dest->mf[i][thisCol] = temp2;
            }
        }

        // Scale thisv whole row such that the diagonal element is 1.
        temp1 = mfCopy.mf[thisCol][thisCol];
        for (i = 0; i < 4; i++) {
            mfCopy.mf[i][thisCol] /= temp1;
            dest->mf[i][thisCol] /= temp1;
        }

        for (thisRow = 0; thisRow < 4; thisRow++) {
            if (thisRow != thisCol) {
                temp1 = mfCopy.mf[thisCol][thisRow];
                for (i = 0; i < 4; i++) {
                    mfCopy.mf[i][thisRow] -= mfCopy.mf[i][thisCol] * temp1;
                    dest->mf[i][thisRow] -= dest->mf[i][thisCol] * temp1;
                }
            }
        }
    }
    return 0;
}

/**
 * Produces a matrix which scales x,y,z components of vectors or x,y,z rows of matrices (when applied on LHS)
 */
void SkinMatrix_SetScale(MtxF* mf, f32 x, f32 y, f32 z) {
    mf->mf_raw.yx = 0.0f;
    mf->mf_raw.zx = 0.0f;
    mf->mf_raw.wx = 0.0f;
    mf->mf_raw.xy = 0.0f;
    mf->mf_raw.zy = 0.0f;
    mf->mf_raw.wy = 0.0f;
    mf->mf_raw.xz = 0.0f;
    mf->mf_raw.yz = 0.0f;
    mf->mf_raw.wz = 0.0f;
    mf->mf_raw.xw = 0.0f;
    mf->mf_raw.yw = 0.0f;
    mf->mf_raw.zw = 0.0f;
    mf->mf_raw.ww = 1.0f;
    mf->mf_raw.xx = x;
    mf->mf_raw.yy = y;
    mf->mf_raw.zz = z;
}

/**
 * Produces a rotation matrix using ZYX Tait-Bryan angles.
 */
void SkinMatrix_SetRotateZYX(MtxF* mf, s16 x, s16 y, s16 z) {
    f32 cos;
    f32 sinZ = Math_SinS(z);
    f32 cosZ = Math_CosS(z);
    f32 xy;
    f32 sin;
    f32 xz;
    f32 yy;
    f32 yz;

    mf->mf_raw.yy = cosZ;
    mf->mf_raw.xy = -sinZ;
    mf->mf_raw.wx = mf->mf_raw.wy = mf->mf_raw.wz = 0;
    mf->mf_raw.xw = mf->mf_raw.yw = mf->mf_raw.zw = 0;
    mf->mf_raw.ww = 1;

    if (y != 0) {
        sin = Math_SinS(y);
        cos = Math_CosS(y);

        mf->mf_raw.xx = cosZ * cos;
        mf->mf_raw.xz = cosZ * sin;

        mf->mf_raw.yx = sinZ * cos;
        mf->mf_raw.yz = sinZ * sin;
        mf->mf_raw.zx = -sin;
        mf->mf_raw.zz = cos;

    } else {
        mf->mf_raw.xx = cosZ;
        if (1) {}
        if (1) {}
        xz = sinZ; // required to match
        mf->mf_raw.yx = sinZ;
        mf->mf_raw.zx = mf->mf_raw.xz = mf->mf_raw.yz = 0;
        mf->mf_raw.zz = 1;
    }

    if (x != 0) {
        sin = Math_SinS(x);
        cos = Math_CosS(x);

        xy = mf->mf_raw.xy;
        xz = mf->mf_raw.xz;
        mf->mf_raw.xy = (xy * cos) + (xz * sin);
        mf->mf_raw.xz = (xz * cos) - (xy * sin);

        if (1) {}
        yz = mf->mf_raw.yz;
        yy = mf->mf_raw.yy;
        mf->mf_raw.yy = (yy * cos) + (yz * sin);
        mf->mf_raw.yz = (yz * cos) - (yy * sin);

        if (cos) {}
        mf->mf_raw.zy = mf->mf_raw.zz * sin;
        mf->mf_raw.zz = mf->mf_raw.zz * cos;
    } else {
        mf->mf_raw.zy = 0;
    }
}

/**
 * Produces a rotation matrix using YXZ Tait-Bryan angles.
 */
void SkinMatrix_SetRotateYXZ(MtxF* mf, s16 x, s16 y, s16 z) {
    f32 cos;
    f32 sinY = Math_SinS(y);
    f32 cosY = Math_CosS(y);
    f32 zx;
    f32 sin;
    f32 zy;
    f32 xx;
    f32 xy;

    mf->mf_raw.xx = cosY;
    mf->mf_raw.zx = -sinY;
    mf->mf_raw.wz = 0;
    mf->mf_raw.wy = 0;
    mf->mf_raw.wx = 0;
    mf->mf_raw.zw = 0;
    mf->mf_raw.yw = 0;
    mf->mf_raw.xw = 0;
    mf->mf_raw.ww = 1;

    if (x != 0) {
        sin = Math_SinS(x);
        cos = Math_CosS(x);

        mf->mf_raw.zz = cosY * cos;
        mf->mf_raw.zy = cosY * sin;

        mf->mf_raw.xz = sinY * cos;
        mf->mf_raw.xy = sinY * sin;
        mf->mf_raw.yz = -sin;
        mf->mf_raw.yy = cos;

    } else {
        mf->mf_raw.zz = cosY;
        if (1) {}
        if (1) {}
        xy = sinY; // required to match
        mf->mf_raw.xz = sinY;
        mf->mf_raw.xy = mf->mf_raw.zy = mf->mf_raw.yz = 0;
        mf->mf_raw.yy = 1;
    }

    if (z != 0) {
        sin = Math_SinS(z);
        cos = Math_CosS(z);
        xx = mf->mf_raw.xx;
        xy = mf->mf_raw.xy;
        mf->mf_raw.xx = (xx * cos) + (xy * sin);
        mf->mf_raw.xy = xy * cos - (xx * sin);
        if (1) {}
        zy = mf->mf_raw.zy;
        zx = mf->mf_raw.zx;
        mf->mf_raw.zx = (zx * cos) + (zy * sin);
        mf->mf_raw.zy = (zy * cos) - (zx * sin);
        if (cos) {}
        mf->mf_raw.yx = mf->mf_raw.yy * sin;
        mf->mf_raw.yy = mf->mf_raw.yy * cos;
    } else {
        mf->mf_raw.yx = 0;
    }
}

/**
 * Produces a matrix which translates a vector by amounts in the x, y and z directions
 */
void SkinMatrix_SetTranslate(MtxF* mf, f32 x, f32 y, f32 z) {
    mf->mf_raw.yx = 0.0f;
    mf->mf_raw.zx = 0.0f;
    mf->mf_raw.wx = 0.0f;
    mf->mf_raw.xy = 0.0f;
    mf->mf_raw.zy = 0.0f;
    mf->mf_raw.wy = 0.0f;
    mf->mf_raw.xz = 0.0f;
    mf->mf_raw.yz = 0.0f;
    mf->mf_raw.wz = 0.0f;
    mf->mf_raw.xx = 1.0f;
    mf->mf_raw.yy = 1.0f;
    mf->mf_raw.zz = 1.0f;
    mf->mf_raw.ww = 1.0f;
    mf->mf_raw.xw = x;
    mf->mf_raw.yw = y;
    mf->mf_raw.zw = z;
}

/**
 * Produces a matrix which scales, then rotates (using ZYX Tait-Bryan angles), then translates.
 */
void SkinMatrix_SetTranslateRotateZYXScale(MtxF* dest, f32 scaleX, f32 scaleY, f32 scaleZ, s16 rotX, s16 rotY, s16 rotZ,
                                           f32 translateX, f32 translateY, f32 translateZ) {
    MtxF mft1;
    MtxF mft2;

    SkinMatrix_SetTranslate(dest, translateX, translateY, translateZ);
    SkinMatrix_SetRotateZYX(&mft1, rotX, rotY, rotZ);
    SkinMatrix_MtxFMtxFMult(dest, &mft1, &mft2);
    SkinMatrix_SetScale(&mft1, scaleX, scaleY, scaleZ);
    SkinMatrix_MtxFMtxFMult(&mft2, &mft1, dest);
}

/**
 * Produces a matrix which scales, then rotates (using YXZ Tait-Bryan angles), then translates.
 */
void SkinMatrix_SetTranslateRotateYXZScale(MtxF* dest, f32 scaleX, f32 scaleY, f32 scaleZ, s16 rotX, s16 rotY, s16 rotZ,
                                           f32 translateX, f32 translateY, f32 translateZ) {
    MtxF mft1;
    MtxF mft2;

    SkinMatrix_SetTranslate(dest, translateX, translateY, translateZ);
    SkinMatrix_SetRotateYXZ(&mft1, rotX, rotY, rotZ);
    SkinMatrix_MtxFMtxFMult(dest, &mft1, &mft2);
    SkinMatrix_SetScale(&mft1, scaleX, scaleY, scaleZ);
    SkinMatrix_MtxFMtxFMult(&mft2, &mft1, dest);
}

/**
 * Produces a matrix which rotates (using ZYX Tait-Bryan angles), then translates.
 */
void SkinMatrix_SetTranslateRotateZYX(MtxF* dest, s16 rotX, s16 rotY, s16 rotZ, f32 translateX, f32 translateY,
                                      f32 translateZ) {
    MtxF rotation;
    MtxF translation;

    SkinMatrix_SetTranslate(&translation, translateX, translateY, translateZ);
    SkinMatrix_SetRotateZYX(&rotation, rotX, rotY, rotZ);
    SkinMatrix_MtxFMtxFMult(&translation, &rotation, dest);
}

void SkinMatrix_Vec3fToVec3s(Vec3f* src, Vec3s* dest) {
    dest->x = src->x;
    dest->y = src->y;
    dest->z = src->z;
}

void SkinMatrix_Vec3sToVec3f(Vec3s* src, Vec3f* dest) {
    dest->x = src->x;
    dest->y = src->y;
    dest->z = src->z;
}

void SkinMatrix_MtxFToMtx(MtxF* src, Mtx* dest) {
    guMtxF2L(src, dest);
}

Mtx* SkinMatrix_MtxFToNewMtx(GraphicsContext* gfxCtx, MtxF* src) {
    Mtx* mtx = static_cast<Mtx*>( Graph_Alloc(gfxCtx, sizeof(Mtx)) );

    if (mtx == NULL) {
        osSyncPrintf("Skin_Matrix_to_Mtx_new() 確保失敗:NULLを返して終了\n", mtx);
        return NULL;
    }
    SkinMatrix_MtxFToMtx(src, mtx);
    return mtx;
}

/**
 * Produces a matrix which rotates by binary angle `angle` around a unit vector (`axisX`,`axisY`,`axisZ`).
 * NB: the rotation axis is assumed to be a unit vector.
 */
void SkinMatrix_SetRotateAxis(MtxF* mf, s16 angle, f32 axisX, f32 axisY, f32 axisZ) {
    f32 sinA;
    f32 cosA;
    f32 xx;
    f32 yy;
    f32 zz;
    f32 xy;
    f32 yz;
    f32 xz;
    f32 pad;

    sinA = Math_SinS(angle);
    cosA = Math_CosS(angle);

    xx = axisX * axisX;
    yy = axisY * axisY;
    zz = axisZ * axisZ;
    xy = axisX * axisY;
    yz = axisY * axisZ;
    xz = axisX * axisZ;

    mf->mf_raw.xx = (1.0f - xx) * cosA + xx;
    mf->mf_raw.yx = (1.0f - cosA) * xy + axisZ * sinA;
    mf->mf_raw.zx = (1.0f - cosA) * xz - axisY * sinA;
    mf->mf_raw.wx = 0.0f;

    mf->mf_raw.xy = (1.0f - cosA) * xy - axisZ * sinA;
    mf->mf_raw.yy = (1.0f - yy) * cosA + yy;
    mf->mf_raw.zy = (1.0f - cosA) * yz + axisX * sinA;
    mf->mf_raw.wy = 0.0f;

    mf->mf_raw.xz = (1.0f - cosA) * xz + axisY * sinA;
    mf->mf_raw.yz = (1.0f - cosA) * yz - axisX * sinA;
    mf->mf_raw.zz = (1.0f - zz) * cosA + zz;
    mf->mf_raw.wz = 0.0f;

    mf->mf_raw.xw = mf->mf_raw.yw = mf->mf_raw.zw = 0.0f;
    mf->mf_raw.ww = 1.0f;
}

void func_800A8030(MtxF* mf, f32* arg1) {
    f32 n;
    f32 xNorm;
    f32 yNorm;
    f32 zNorm;
    f32 wxNorm;
    f32 wyNorm;
    f32 wzNorm;
    f32 xxNorm;
    f32 xyNorm;
    f32 xzNorm;
    f32 yyNorm;
    f32 yzNorm;
    f32 zzNorm;

    n = 2.0f / ((arg1[3] * arg1[3]) + ((arg1[2] * arg1[2]) + ((arg1[1] * arg1[1]) + (arg1[0] * arg1[0]))));
    xNorm = arg1[0] * n;
    yNorm = arg1[1] * n;
    zNorm = arg1[2] * n;

    wxNorm = arg1[3] * xNorm;
    wyNorm = arg1[3] * yNorm;
    wzNorm = arg1[3] * zNorm;
    xxNorm = arg1[0] * xNorm;
    xyNorm = arg1[0] * yNorm;
    xzNorm = arg1[0] * zNorm;
    yyNorm = arg1[1] * yNorm;
    yzNorm = arg1[1] * zNorm;
    zzNorm = arg1[2] * zNorm;

    mf->mf_raw.xx = (1.0f - (yyNorm + zzNorm));
    mf->mf_raw.yx = (xyNorm + wzNorm);
    mf->mf_raw.zx = (xzNorm - wyNorm);
    mf->mf_raw.wx = 0.0f;
    mf->mf_raw.xy = (xyNorm - wzNorm);
    mf->mf_raw.yy = (1.0f - (xxNorm + zzNorm));
    mf->mf_raw.zy = (yzNorm + wxNorm);
    mf->mf_raw.wy = 0.0f;
    mf->mf_raw.xz = (yzNorm + wyNorm);
    mf->mf_raw.yz = (yzNorm - wxNorm);
    mf->mf_raw.zz = (1.0f - (xxNorm + yyNorm));
    mf->mf_raw.wz = 0.0f;
    mf->mf_raw.xw = 0.0f;
    mf->mf_raw.yw = 0.0f;
    mf->mf_raw.ww = 1.0f;
    mf->mf_raw.zw = 0.0f;
}
