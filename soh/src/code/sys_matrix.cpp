#include "global.h"

// clang-format off
Mtx gMtxClear = {
    65536,     0,     1,     0,
        0, 65536,     0,     1,
        0,     0,     0,     0,
        0,     0,     0,     0,
};

MtxF gMtxFClear = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
};
// clang-format on

MtxF* sMatrixStack;   // "Matrix_stack"
MtxF* sCurrentMatrix; // "Matrix_now"

void Matrix_Init(GameState* gameState) {
    sCurrentMatrix = static_cast<MtxF*>(GameState_Alloc(gameState, 20 * sizeof(MtxF), "../sys_matrix.c", 153));
    sMatrixStack = sCurrentMatrix;
}

void Matrix_Push(void) {
    Matrix_MtxFCopy(sCurrentMatrix + 1, sCurrentMatrix);
    sCurrentMatrix++;
}

void Matrix_Pop(void) {
    sCurrentMatrix--;
    ASSERT(sCurrentMatrix >= sMatrixStack, "Matrix_now >= Matrix_stack", "../sys_matrix.c", 176);
}

void Matrix_Get(MtxF* dest) {
    Matrix_MtxFCopy(dest, sCurrentMatrix);
}

void Matrix_Put(MtxF* src) {
    Matrix_MtxFCopy(sCurrentMatrix, src);
}

MtxF* Matrix_GetCurrent(void) {
    return sCurrentMatrix;
}

void Matrix_Mult(MtxF* mf, u8 mode) {
    MtxF* cmf = Matrix_GetCurrent();

    if (mode == MTXMODE_APPLY) {
        SkinMatrix_MtxFMtxFMult(cmf, mf, cmf);
    } else {
        Matrix_MtxFCopy(sCurrentMatrix, mf);
    }
}

void Matrix_Translate(f32 x, f32 y, f32 z, u8 mode) {
    MtxF* cmf = sCurrentMatrix;
    f32 tx;
    f32 ty;

    if (mode == MTXMODE_APPLY) {
        tx = cmf->mf_raw.xx;
        ty = cmf->mf_raw.xy;
        cmf->mf_raw.xw += tx * x + ty * y + cmf->mf_raw.xz * z;
        tx = cmf->mf_raw.yx;
        ty = cmf->mf_raw.yy;
        cmf->mf_raw.yw += tx * x + ty * y + cmf->mf_raw.yz * z;
        tx = cmf->mf_raw.zx;
        ty = cmf->mf_raw.zy;
        cmf->mf_raw.zw += tx * x + ty * y + cmf->mf_raw.zz * z;
        tx = cmf->mf_raw.wx;
        ty = cmf->mf_raw.wy;
        cmf->mf_raw.ww += tx * x + ty * y + cmf->mf_raw.wz * z;
    } else {
        SkinMatrix_SetTranslate(cmf, x, y, z);
    }
}

void Matrix_Scale(f32 x, f32 y, f32 z, u8 mode) {
    MtxF* cmf = sCurrentMatrix;

    if (mode == MTXMODE_APPLY) {
        cmf->mf_raw.xx *= x;
        cmf->mf_raw.yx *= x;
        cmf->mf_raw.zx *= x;
        cmf->mf_raw.xy *= y;
        cmf->mf_raw.yy *= y;
        cmf->mf_raw.zy *= y;
        cmf->mf_raw.xz *= z;
        cmf->mf_raw.yz *= z;
        cmf->mf_raw.zz *= z;
        cmf->mf_raw.wx *= x;
        cmf->mf_raw.wy *= y;
        cmf->mf_raw.wz *= z;
    } else {
        SkinMatrix_SetScale(cmf, x, y, z);
    }
}

void Matrix_RotateX(f32 x, u8 mode) {
    MtxF* cmf;
    f32 sin;
    f32 cos;
    f32 temp1;
    f32 temp2;

    if (mode == MTXMODE_APPLY) {
        if (x != 0) {
            cmf = sCurrentMatrix;

            sin = sinf(x);
            cos = cosf(x);

            temp1 = cmf->mf_raw.xy;
            temp2 = cmf->mf_raw.xz;
            cmf->mf_raw.xy = temp1 * cos + temp2 * sin;
            cmf->mf_raw.xz = temp2 * cos - temp1 * sin;

            temp1 = cmf->mf_raw.yy;
            temp2 = cmf->mf_raw.yz;
            cmf->mf_raw.yy = temp1 * cos + temp2 * sin;
            cmf->mf_raw.yz = temp2 * cos - temp1 * sin;

            temp1 = cmf->mf_raw.zy;
            temp2 = cmf->mf_raw.zz;
            cmf->mf_raw.zy = temp1 * cos + temp2 * sin;
            cmf->mf_raw.zz = temp2 * cos - temp1 * sin;

            temp1 = cmf->mf_raw.wy;
            temp2 = cmf->mf_raw.wz;
            cmf->mf_raw.wy = temp1 * cos + temp2 * sin;
            cmf->mf_raw.wz = temp2 * cos - temp1 * sin;
        }
    } else {
        cmf = sCurrentMatrix;

        if (x != 0) {
            sin = sinf(x);
            cos = cosf(x);
        } else {
            sin = 0.0f;
            cos = 1.0f;
        }

        cmf->mf_raw.yx = 0.0f;
        cmf->mf_raw.zx = 0.0f;
        cmf->mf_raw.wx = 0.0f;
        cmf->mf_raw.xy = 0.0f;
        cmf->mf_raw.wy = 0.0f;
        cmf->mf_raw.xz = 0.0f;
        cmf->mf_raw.wz = 0.0f;
        cmf->mf_raw.xw = 0.0f;
        cmf->mf_raw.yw = 0.0f;
        cmf->mf_raw.zw = 0.0f;
        cmf->mf_raw.xx = 1.0f;
        cmf->mf_raw.ww = 1.0f;
        cmf->mf_raw.yy = cos;
        cmf->mf_raw.zz = cos;
        cmf->mf_raw.zy = sin;
        cmf->mf_raw.yz = -sin;
    }
}

void Matrix_RotateY(f32 y, u8 mode) {
    MtxF* cmf;
    f32 sin;
    f32 cos;
    f32 temp1;
    f32 temp2;

    if (mode == MTXMODE_APPLY) {
        if (y != 0) {
            cmf = sCurrentMatrix;

            sin = sinf(y);
            cos = cosf(y);

            temp1 = cmf->mf_raw.xx;
            temp2 = cmf->mf_raw.xz;
            cmf->mf_raw.xx = temp1 * cos - temp2 * sin;
            cmf->mf_raw.xz = temp1 * sin + temp2 * cos;

            temp1 = cmf->mf_raw.yx;
            temp2 = cmf->mf_raw.yz;
            cmf->mf_raw.yx = temp1 * cos - temp2 * sin;
            cmf->mf_raw.yz = temp1 * sin + temp2 * cos;

            temp1 = cmf->mf_raw.zx;
            temp2 = cmf->mf_raw.zz;
            cmf->mf_raw.zx = temp1 * cos - temp2 * sin;
            cmf->mf_raw.zz = temp1 * sin + temp2 * cos;

            temp1 = cmf->mf_raw.wx;
            temp2 = cmf->mf_raw.wz;
            cmf->mf_raw.wx = temp1 * cos - temp2 * sin;
            cmf->mf_raw.wz = temp1 * sin + temp2 * cos;
        }
    } else {
        cmf = sCurrentMatrix;

        if (y != 0) {
            sin = sinf(y);
            cos = cosf(y);
        } else {
            sin = 0.0f;
            cos = 1.0f;
        }

        cmf->mf_raw.yx = 0.0f;
        cmf->mf_raw.wx = 0.0f;
        cmf->mf_raw.xy = 0.0f;
        cmf->mf_raw.zy = 0.0f;
        cmf->mf_raw.wy = 0.0f;
        cmf->mf_raw.yz = 0.0f;
        cmf->mf_raw.wz = 0.0f;
        cmf->mf_raw.xw = 0.0f;
        cmf->mf_raw.yw = 0.0f;
        cmf->mf_raw.zw = 0.0f;
        cmf->mf_raw.yy = 1.0f;
        cmf->mf_raw.ww = 1.0f;
        cmf->mf_raw.xx = cos;
        cmf->mf_raw.zz = cos;
        cmf->mf_raw.zx = -sin;
        cmf->mf_raw.xz = sin;
    }
}

void Matrix_RotateZ(f32 z, u8 mode) {
    MtxF* cmf;
    f32 sin;
    f32 cos;
    f32 temp1;
    f32 temp2;

    if (mode == MTXMODE_APPLY) {
        if (z != 0) {
            cmf = sCurrentMatrix;

            sin = sinf(z);
            cos = cosf(z);

            temp1 = cmf->mf_raw.xx;
            temp2 = cmf->mf_raw.xy;
            cmf->mf_raw.xx = temp1 * cos + temp2 * sin;
            cmf->mf_raw.xy = temp2 * cos - temp1 * sin;

            temp1 = cmf->mf_raw.yx;
            temp2 = cmf->mf_raw.yy;
            cmf->mf_raw.yx = temp1 * cos + temp2 * sin;
            cmf->mf_raw.yy = temp2 * cos - temp1 * sin;

            temp1 = cmf->mf_raw.zx;
            temp2 = cmf->mf_raw.zy;
            cmf->mf_raw.zx = temp1 * cos + temp2 * sin;
            cmf->mf_raw.zy = temp2 * cos - temp1 * sin;

            temp1 = cmf->mf_raw.wx;
            temp2 = cmf->mf_raw.wy;
            cmf->mf_raw.wx = temp1 * cos + temp2 * sin;
            cmf->mf_raw.wy = temp2 * cos - temp1 * sin;
        }
    } else {
        cmf = sCurrentMatrix;

        if (z != 0) {
            sin = sinf(z);
            cos = cosf(z);
        } else {
            sin = 0.0f;
            cos = 1.0f;
        }

        cmf->mf_raw.zx = 0.0f;
        cmf->mf_raw.wx = 0.0f;
        cmf->mf_raw.zy = 0.0f;
        cmf->mf_raw.wy = 0.0f;
        cmf->mf_raw.xz = 0.0f;
        cmf->mf_raw.yz = 0.0f;
        cmf->mf_raw.wz = 0.0f;
        cmf->mf_raw.xw = 0.0f;
        cmf->mf_raw.yw = 0.0f;
        cmf->mf_raw.zw = 0.0f;
        cmf->mf_raw.zz = 1.0f;
        cmf->mf_raw.ww = 1.0f;
        cmf->mf_raw.xx = cos;
        cmf->mf_raw.yy = cos;
        cmf->mf_raw.yx = sin;
        cmf->mf_raw.xy = -sin;
    }
}

/**
 * Rotate using ZYX Tait-Bryan angles.
 * This means a (column) vector is first rotated around X, then around Y, then around Z, then (if `mode` is
 * `MTXMODE_APPLY`) gets transformed according to whatever the matrix was before adding the ZYX rotation.
 * Original Name: Matrix_RotateXYZ, changed to reflect rotation order.
 */
void Matrix_RotateZYX(s16 x, s16 y, s16 z, u8 mode) {
    MtxF* cmf = sCurrentMatrix;
    f32 temp1;
    f32 temp2;
    f32 sin;
    f32 cos;

    if (mode == MTXMODE_APPLY) {
        sin = Math_SinS(z);
        cos = Math_CosS(z);

        temp1 = cmf->mf_raw.xx;
        temp2 = cmf->mf_raw.xy;
        cmf->mf_raw.xx = temp1 * cos + temp2 * sin;
        cmf->mf_raw.xy = temp2 * cos - temp1 * sin;

        temp1 = cmf->mf_raw.yx;
        temp2 = cmf->mf_raw.yy;
        cmf->mf_raw.yx = temp1 * cos + temp2 * sin;
        cmf->mf_raw.yy = temp2 * cos - temp1 * sin;

        temp1 = cmf->mf_raw.zx;
        temp2 = cmf->mf_raw.zy;
        cmf->mf_raw.zx = temp1 * cos + temp2 * sin;
        cmf->mf_raw.zy = temp2 * cos - temp1 * sin;

        temp1 = cmf->mf_raw.wx;
        temp2 = cmf->mf_raw.wy;
        cmf->mf_raw.wx = temp1 * cos + temp2 * sin;
        cmf->mf_raw.wy = temp2 * cos - temp1 * sin;

        if (y != 0) {
            sin = Math_SinS(y);
            cos = Math_CosS(y);

            temp1 = cmf->mf_raw.xx;
            temp2 = cmf->mf_raw.xz;
            cmf->mf_raw.xx = temp1 * cos - temp2 * sin;
            cmf->mf_raw.xz = temp1 * sin + temp2 * cos;

            temp1 = cmf->mf_raw.yx;
            temp2 = cmf->mf_raw.yz;
            cmf->mf_raw.yx = temp1 * cos - temp2 * sin;
            cmf->mf_raw.yz = temp1 * sin + temp2 * cos;

            temp1 = cmf->mf_raw.zx;
            temp2 = cmf->mf_raw.zz;
            cmf->mf_raw.zx = temp1 * cos - temp2 * sin;
            cmf->mf_raw.zz = temp1 * sin + temp2 * cos;

            temp1 = cmf->mf_raw.wx;
            temp2 = cmf->mf_raw.wz;
            cmf->mf_raw.wx = temp1 * cos - temp2 * sin;
            cmf->mf_raw.wz = temp1 * sin + temp2 * cos;
        }

        if (x != 0) {
            sin = Math_SinS(x);
            cos = Math_CosS(x);

            temp1 = cmf->mf_raw.xy;
            temp2 = cmf->mf_raw.xz;
            cmf->mf_raw.xy = temp1 * cos + temp2 * sin;
            cmf->mf_raw.xz = temp2 * cos - temp1 * sin;

            temp1 = cmf->mf_raw.yy;
            temp2 = cmf->mf_raw.yz;
            cmf->mf_raw.yy = temp1 * cos + temp2 * sin;
            cmf->mf_raw.yz = temp2 * cos - temp1 * sin;

            temp1 = cmf->mf_raw.zy;
            temp2 = cmf->mf_raw.zz;
            cmf->mf_raw.zy = temp1 * cos + temp2 * sin;
            cmf->mf_raw.zz = temp2 * cos - temp1 * sin;

            temp1 = cmf->mf_raw.wy;
            temp2 = cmf->mf_raw.wz;
            cmf->mf_raw.wy = temp1 * cos + temp2 * sin;
            cmf->mf_raw.wz = temp2 * cos - temp1 * sin;
        }
    } else {
        SkinMatrix_SetRotateZYX(cmf, x, y, z);
    }
}

/**
 * Translate and rotate using ZYX Tait-Bryan angles.
 * This means a (column) vector is first rotated around X, then around Y, then around Z, then translated, then gets
 * transformed according to whatever the matrix was previously.
 */
void Matrix_TranslateRotateZYX(Vec3f* translation, Vec3s* rotation) {
    MtxF* cmf = sCurrentMatrix;
    f32 sin = Math_SinS(rotation->z);
    f32 cos = Math_CosS(rotation->z);
    f32 temp1;
    f32 temp2;

    temp1 = cmf->mf_raw.xx;
    temp2 = cmf->mf_raw.xy;
    cmf->mf_raw.xw += temp1 * translation->x + temp2 * translation->y + cmf->mf_raw.xz * translation->z;
    cmf->mf_raw.xx = temp1 * cos + temp2 * sin;
    cmf->mf_raw.xy = temp2 * cos - temp1 * sin;

    temp1 = cmf->mf_raw.yx;
    temp2 = cmf->mf_raw.yy;
    cmf->mf_raw.yw += temp1 * translation->x + temp2 * translation->y + cmf->mf_raw.yz * translation->z;
    cmf->mf_raw.yx = temp1 * cos + temp2 * sin;
    cmf->mf_raw.yy = temp2 * cos - temp1 * sin;

    temp1 = cmf->mf_raw.zx;
    temp2 = cmf->mf_raw.zy;
    cmf->mf_raw.zw += temp1 * translation->x + temp2 * translation->y + cmf->mf_raw.zz * translation->z;
    cmf->mf_raw.zx = temp1 * cos + temp2 * sin;
    cmf->mf_raw.zy = temp2 * cos - temp1 * sin;

    temp1 = cmf->mf_raw.wx;
    temp2 = cmf->mf_raw.wy;
    cmf->mf_raw.ww += temp1 * translation->x + temp2 * translation->y + cmf->mf_raw.wz * translation->z;
    cmf->mf_raw.wx = temp1 * cos + temp2 * sin;
    cmf->mf_raw.wy = temp2 * cos - temp1 * sin;

    if (rotation->y != 0) {
        sin = Math_SinS(rotation->y);
        cos = Math_CosS(rotation->y);

        temp1 = cmf->mf_raw.xx;
        temp2 = cmf->mf_raw.xz;
        cmf->mf_raw.xx = temp1 * cos - temp2 * sin;
        cmf->mf_raw.xz = temp1 * sin + temp2 * cos;

        temp1 = cmf->mf_raw.yx;
        temp2 = cmf->mf_raw.yz;
        cmf->mf_raw.yx = temp1 * cos - temp2 * sin;
        cmf->mf_raw.yz = temp1 * sin + temp2 * cos;

        temp1 = cmf->mf_raw.zx;
        temp2 = cmf->mf_raw.zz;
        cmf->mf_raw.zx = temp1 * cos - temp2 * sin;
        cmf->mf_raw.zz = temp1 * sin + temp2 * cos;

        temp1 = cmf->mf_raw.wx;
        temp2 = cmf->mf_raw.wz;
        cmf->mf_raw.wx = temp1 * cos - temp2 * sin;
        cmf->mf_raw.wz = temp1 * sin + temp2 * cos;
    }

    if (rotation->x != 0) {
        sin = Math_SinS(rotation->x);
        cos = Math_CosS(rotation->x);

        temp1 = cmf->mf_raw.xy;
        temp2 = cmf->mf_raw.xz;
        cmf->mf_raw.xy = temp1 * cos + temp2 * sin;
        cmf->mf_raw.xz = temp2 * cos - temp1 * sin;

        temp1 = cmf->mf_raw.yy;
        temp2 = cmf->mf_raw.yz;
        cmf->mf_raw.yy = temp1 * cos + temp2 * sin;
        cmf->mf_raw.yz = temp2 * cos - temp1 * sin;

        temp1 = cmf->mf_raw.zy;
        temp2 = cmf->mf_raw.zz;
        cmf->mf_raw.zy = temp1 * cos + temp2 * sin;
        cmf->mf_raw.zz = temp2 * cos - temp1 * sin;

        temp1 = cmf->mf_raw.wy;
        temp2 = cmf->mf_raw.wz;
        cmf->mf_raw.wy = temp1 * cos + temp2 * sin;
        cmf->mf_raw.wz = temp2 * cos - temp1 * sin;
    }
}

/**
 * Set the current matrix to translate and rotate using YXZ Tait-Bryan angles.
 * This means a (column) vector is first rotated around Z, then around X, then around Y, then translated.
 */
void Matrix_SetTranslateRotateYXZ(f32 translateX, f32 translateY, f32 translateZ, Vec3s* rot) {
    MtxF* cmf = sCurrentMatrix;
    f32 temp1 = Math_SinS(rot->y);
    f32 temp2 = Math_CosS(rot->y);
    f32 cos;
    f32 sin;

    cmf->mf_raw.xx = temp2;
    cmf->mf_raw.zx = -temp1;
    cmf->mf_raw.xw = translateX;
    cmf->mf_raw.yw = translateY;
    cmf->mf_raw.zw = translateZ;
    cmf->mf_raw.wx = 0.0f;
    cmf->mf_raw.wy = 0.0f;
    cmf->mf_raw.wz = 0.0f;
    cmf->mf_raw.ww = 1.0f;

    if (rot->x != 0) {
        sin = Math_SinS(rot->x);
        cos = Math_CosS(rot->x);

        cmf->mf_raw.zz = temp2 * cos;
        cmf->mf_raw.zy = temp2 * sin;
        cmf->mf_raw.xz = temp1 * cos;
        cmf->mf_raw.xy = temp1 * sin;
        cmf->mf_raw.yz = -sin;
        cmf->mf_raw.yy = cos;
    } else {
        cmf->mf_raw.zz = temp2;
        cmf->mf_raw.xz = temp1;
        cmf->mf_raw.yz = 0.0f;
        cmf->mf_raw.zy = 0.0f;
        cmf->mf_raw.xy = 0.0f;
        cmf->mf_raw.yy = 1.0f;
    }

    if (rot->z != 0) {
        sin = Math_SinS(rot->z);
        cos = Math_CosS(rot->z);

        temp1 = cmf->mf_raw.xx;
        temp2 = cmf->mf_raw.xy;
        cmf->mf_raw.xx = temp1 * cos + temp2 * sin;
        cmf->mf_raw.xy = temp2 * cos - temp1 * sin;

        temp1 = cmf->mf_raw.zx;
        temp2 = cmf->mf_raw.zy;
        cmf->mf_raw.zx = temp1 * cos + temp2 * sin;
        cmf->mf_raw.zy = temp2 * cos - temp1 * sin;

        temp2 = cmf->mf_raw.yy;
        cmf->mf_raw.yx = temp2 * sin;
        cmf->mf_raw.yy = temp2 * cos;
    } else {
        cmf->mf_raw.yx = 0.0f;
    }
}

Mtx* Matrix_MtxFToMtx(MtxF* src, Mtx* dest) {
    guMtxF2L(src, dest);
    return dest;
}

Mtx* Matrix_ToMtx(Mtx* dest, const char* file, s32 line) {
    return Matrix_MtxFToMtx(Matrix_CheckFloats(sCurrentMatrix, file, line), dest);
}

Mtx* Matrix_NewMtx(GraphicsContext* gfxCtx, const char* file, s32 line) {
    return Matrix_ToMtx(static_cast<Mtx*>(Graph_Alloc(gfxCtx, sizeof(Mtx))), file, line);
}

Mtx* Matrix_MtxFToNewMtx(MtxF* src, GraphicsContext* gfxCtx) {
    return Matrix_MtxFToMtx(src, static_cast<Mtx*>(Graph_Alloc(gfxCtx, sizeof(Mtx))));
}

void Matrix_MultVec3f(Vec3f* src, Vec3f* dest) {
    MtxF* cmf = sCurrentMatrix;

    dest->x = cmf->mf_raw.xw + (cmf->mf_raw.xx * src->x + cmf->mf_raw.xy * src->y + cmf->mf_raw.xz * src->z);
    dest->y = cmf->mf_raw.yw + (cmf->mf_raw.yx * src->x + cmf->mf_raw.yy * src->y + cmf->mf_raw.yz * src->z);
    dest->z = cmf->mf_raw.zw + (cmf->mf_raw.zx * src->x + cmf->mf_raw.zy * src->y + cmf->mf_raw.zz * src->z);
}

void Matrix_MtxFCopy(MtxF* dest, MtxF* src) {
    dest->mf_raw.xx = src->mf_raw.xx;
    dest->mf_raw.yx = src->mf_raw.yx;
    dest->mf_raw.zx = src->mf_raw.zx;
    dest->mf_raw.wx = src->mf_raw.wx;
    dest->mf_raw.xy = src->mf_raw.xy;
    dest->mf_raw.yy = src->mf_raw.yy;
    dest->mf_raw.zy = src->mf_raw.zy;
    dest->mf_raw.wy = src->mf_raw.wy;
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
    dest->mf_raw.xz = src->mf_raw.xz;
    dest->mf_raw.yz = src->mf_raw.yz;
    dest->mf_raw.zz = src->mf_raw.zz;
    dest->mf_raw.wz = src->mf_raw.wz;
    dest->mf_raw.xw = src->mf_raw.xw;
    dest->mf_raw.yw = src->mf_raw.yw;
    dest->mf_raw.zw = src->mf_raw.zw;
    dest->mf_raw.ww = src->mf_raw.ww;
}

void Matrix_MtxToMtxF(Mtx* src, MtxF* dest) {
    guMtxL2F(dest, src);
}

void Matrix_MultVec3fExt(Vec3f* src, Vec3f* dest, MtxF* mf) {
    dest->x = mf->mf_raw.xw + (mf->mf_raw.xx * src->x + mf->mf_raw.xy * src->y + mf->mf_raw.xz * src->z);
    dest->y = mf->mf_raw.yw + (mf->mf_raw.yx * src->x + mf->mf_raw.yy * src->y + mf->mf_raw.yz * src->z);
    dest->z = mf->mf_raw.zw + (mf->mf_raw.zx * src->x + mf->mf_raw.zy * src->y + mf->mf_raw.zz * src->z);
}

void Matrix_Transpose(MtxF* mf) {
    f32 temp;

    temp = mf->mf_raw.yx;
    mf->mf_raw.yx = mf->mf_raw.xy;
    mf->mf_raw.xy = temp;

    temp = mf->mf_raw.zx;
    mf->mf_raw.zx = mf->mf_raw.xz;
    mf->mf_raw.xz = temp;

    temp = mf->mf_raw.zy;
    mf->mf_raw.zy = mf->mf_raw.yz;
    mf->mf_raw.yz = temp;
}

/**
 * Changes the 3x3 part of the current matrix to `mf` * S, where S is the scale in the current matrix.
 *
 * In details, S is a diagonal where each coefficient is the norm of the column in the 3x3 current matrix.
 * The 3x3 part can then be written as R * S where R has its columns normalized.
 * Since R is typically a rotation matrix, and the 3x3 part is changed from R * S to `mf` * S, thisv operation can be
 * seen as replacing the R rotation with `mf`, hence the function name.
 */
void Matrix_ReplaceRotation(MtxF* mf) {
    MtxF* cmf = sCurrentMatrix;
    f32 acc;
    f32 temp;
    f32 curColNorm;

    // compute the Euclidean norm of the first column of the current matrix
    acc = cmf->mf_raw.xx;
    acc *= acc;
    temp = cmf->mf_raw.yx;
    acc += SQ(temp);
    temp = cmf->mf_raw.zx;
    acc += SQ(temp);
    curColNorm = sqrtf(acc);

    cmf->mf_raw.xx = mf->mf_raw.xx * curColNorm;
    cmf->mf_raw.yx = mf->mf_raw.yx * curColNorm;
    cmf->mf_raw.zx = mf->mf_raw.zx * curColNorm;

    // second column
    acc = cmf->mf_raw.xy;
    acc *= acc;
    temp = cmf->mf_raw.yy;
    acc += SQ(temp);
    temp = cmf->mf_raw.zy;
    acc += SQ(temp);
    curColNorm = sqrtf(acc);

    cmf->mf_raw.xy = mf->mf_raw.xy * curColNorm;
    cmf->mf_raw.yy = mf->mf_raw.yy * curColNorm;
    cmf->mf_raw.zy = mf->mf_raw.zy * curColNorm;

    // third column
    acc = cmf->mf_raw.xz;
    acc *= acc;
    temp = cmf->mf_raw.yz;
    acc += SQ(temp);
    temp = cmf->mf_raw.zz;
    acc += SQ(temp);
    curColNorm = sqrtf(acc);

    cmf->mf_raw.xz = mf->mf_raw.xz * curColNorm;
    cmf->mf_raw.yz = mf->mf_raw.yz * curColNorm;
    cmf->mf_raw.zz = mf->mf_raw.zz * curColNorm;
}

/**
 * Gets the rotation the specified matrix represents, using Tait-Bryan YXZ angles.
 * The flag value doesn't matter for a rotation matrix. Not 0 does extra calculation.
 */
void Matrix_MtxFToYXZRotS(MtxF* mf, Vec3s* rotDest, s32 flag) {
    f32 temp;
    f32 temp2;
    f32 temp3;
    f32 temp4;

    temp = mf->mf_raw.xz;
    temp *= temp;
    temp += SQ(mf->mf_raw.zz);
    rotDest->x = Math_FAtan2F(-mf->mf_raw.yz, sqrtf(temp)) * (0x8000 / std::numbers::pi_v<float>);

    if ((rotDest->x == 0x4000) || (rotDest->x == -0x4000)) {
        rotDest->z = 0;

        rotDest->y = Math_FAtan2F(-mf->mf_raw.zx, mf->mf_raw.xx) * (0x8000 / std::numbers::pi_v<float>);
    } else {
        rotDest->y = Math_FAtan2F(mf->mf_raw.xz, mf->mf_raw.zz) * (0x8000 / std::numbers::pi_v<float>);

        if (!flag) {
            rotDest->z = Math_FAtan2F(mf->mf_raw.yx, mf->mf_raw.yy) * (0x8000 / std::numbers::pi_v<float>);
        } else {
            temp = mf->mf_raw.xx;
            temp2 = mf->mf_raw.zx;
            temp3 = mf->mf_raw.zy;

            temp *= temp;
            temp += SQ(temp2);
            temp2 = mf->mf_raw.yx;
            temp += SQ(temp2);
            /* temp = xx^2+zx^2+yx^2 == 1 for a rotation matrix */
            temp = sqrtf(temp);
            temp = temp2 / temp;

            temp2 = mf->mf_raw.xy;
            temp2 *= temp2;
            temp2 += SQ(temp3);
            temp3 = mf->mf_raw.yy;
            temp2 += SQ(temp3);
            /* temp2 = xy^2+zy^2+yy^2 == 1 for a rotation matrix */
            temp2 = sqrtf(temp2);
            temp2 = temp3 / temp2;

            /* for a rotation matrix, temp == yx and temp2 == yy
             * which is the same as in the !flag branch */
            rotDest->z = Math_FAtan2F(temp, temp2) * (0x8000 / std::numbers::pi_v<float>);
        }
    }
}

/**
 * Gets the rotation the specified matrix represents, using Tait-Bryan ZYX angles.
 * The flag value doesn't matter for a rotation matrix. Not 0 does extra calculation.
 */
void Matrix_MtxFToZYXRotS(MtxF* mf, Vec3s* rotDest, s32 flag) {
    f32 temp;
    f32 temp2;
    f32 temp3;
    f32 temp4;

    temp = mf->mf_raw.xx;
    temp *= temp;
    temp += SQ(mf->mf_raw.yx);
    rotDest->y = Math_FAtan2F(-mf->mf_raw.zx, sqrtf(temp)) * (0x8000 / std::numbers::pi_v<float>);

    if ((rotDest->y == 0x4000) || (rotDest->y == -0x4000)) {
        rotDest->x = 0;
        rotDest->z = Math_FAtan2F(-mf->mf_raw.xy, mf->mf_raw.yy) * (0x8000 / std::numbers::pi_v<float>);
    } else {
        rotDest->z = Math_FAtan2F(mf->mf_raw.yx, mf->mf_raw.xx) * (0x8000 / std::numbers::pi_v<float>);

        if (!flag) {
            rotDest->x = Math_FAtan2F(mf->mf_raw.zy, mf->mf_raw.zz) * (0x8000 / std::numbers::pi_v<float>);
        } else {
            // see Matrix_MtxFToYXZRotS
            temp = mf->mf_raw.xy;
            temp2 = mf->mf_raw.yy;
            temp3 = mf->mf_raw.yz;

            temp *= temp;
            temp += SQ(temp2);
            temp2 = mf->mf_raw.zy;
            temp += SQ(temp2);
            temp = sqrtf(temp);
            temp = temp2 / temp;

            temp2 = mf->mf_raw.xz;
            temp2 *= temp2;
            temp2 += SQ(temp3);
            temp3 = mf->mf_raw.zz;
            temp2 += SQ(temp3);
            temp2 = sqrtf(temp2);
            temp2 = temp3 / temp2;

            rotDest->x = Math_FAtan2F(temp, temp2) * (0x8000 / std::numbers::pi_v<float>);
        }
    }
}

/*
 * Rotate the matrix by `angle` radians around a unit vector `axis`.
 * NB: `axis` is assumed to be a unit vector.
 */
void Matrix_RotateAxis(f32 angle, Vec3f* axis, u8 mode) {
    MtxF* cmf;
    f32 sin;
    f32 cos;
    f32 rCos;
    f32 temp1;
    f32 temp2;
    f32 temp3;
    f32 temp4;

    if (mode == MTXMODE_APPLY) {
        if (angle != 0) {
            cmf = sCurrentMatrix;

            sin = sinf(angle);
            cos = cosf(angle);

            temp1 = cmf->mf_raw.xx;
            temp2 = cmf->mf_raw.xy;
            temp3 = cmf->mf_raw.xz;
            temp4 = (axis->x * temp1 + axis->y * temp2 + axis->z * temp3) * (1.0f - cos);
            cmf->mf_raw.xx = temp1 * cos + axis->x * temp4 + sin * (temp2 * axis->z - temp3 * axis->y);
            cmf->mf_raw.xy = temp2 * cos + axis->y * temp4 + sin * (temp3 * axis->x - temp1 * axis->z);
            cmf->mf_raw.xz = temp3 * cos + axis->z * temp4 + sin * (temp1 * axis->y - temp2 * axis->x);

            temp1 = cmf->mf_raw.yx;
            temp2 = cmf->mf_raw.yy;
            temp3 = cmf->mf_raw.yz;
            temp4 = (axis->x * temp1 + axis->y * temp2 + axis->z * temp3) * (1.0f - cos);
            cmf->mf_raw.yx = temp1 * cos + axis->x * temp4 + sin * (temp2 * axis->z - temp3 * axis->y);
            cmf->mf_raw.yy = temp2 * cos + axis->y * temp4 + sin * (temp3 * axis->x - temp1 * axis->z);
            cmf->mf_raw.yz = temp3 * cos + axis->z * temp4 + sin * (temp1 * axis->y - temp2 * axis->x);

            temp1 = cmf->mf_raw.zx;
            temp2 = cmf->mf_raw.zy;
            temp3 = cmf->mf_raw.zz;
            temp4 = (axis->x * temp1 + axis->y * temp2 + axis->z * temp3) * (1.0f - cos);
            cmf->mf_raw.zx = temp1 * cos + axis->x * temp4 + sin * (temp2 * axis->z - temp3 * axis->y);
            cmf->mf_raw.zy = temp2 * cos + axis->y * temp4 + sin * (temp3 * axis->x - temp1 * axis->z);
            cmf->mf_raw.zz = temp3 * cos + axis->z * temp4 + sin * (temp1 * axis->y - temp2 * axis->x);
        }
    } else {
        cmf = sCurrentMatrix;

        if (angle != 0) {
            sin = sinf(angle);
            cos = cosf(angle);
            rCos = 1.0f - cos;

            cmf->mf_raw.xx = axis->x * axis->x * rCos + cos;
            cmf->mf_raw.yy = axis->y * axis->y * rCos + cos;
            cmf->mf_raw.zz = axis->z * axis->z * rCos + cos;

            if (0) {}

            temp2 = axis->x * rCos * axis->y;
            temp3 = axis->z * sin;
            cmf->mf_raw.yx = temp2 + temp3;
            cmf->mf_raw.xy = temp2 - temp3;

            temp2 = axis->x * rCos * axis->z;
            temp3 = axis->y * sin;
            cmf->mf_raw.zx = temp2 - temp3;
            cmf->mf_raw.xz = temp2 + temp3;

            temp2 = axis->y * rCos * axis->z;
            temp3 = axis->x * sin;
            cmf->mf_raw.zy = temp2 + temp3;
            cmf->mf_raw.yz = temp2 - temp3;

            cmf->mf_raw.wx = cmf->mf_raw.wy = cmf->mf_raw.wz = cmf->mf_raw.xw = cmf->mf_raw.yw = cmf->mf_raw.zw = 0.0f;
            cmf->mf_raw.ww = 1.0f;
        } else {
            cmf->mf_raw.yx = 0.0f;
            cmf->mf_raw.zx = 0.0f;
            cmf->mf_raw.wx = 0.0f;
            cmf->mf_raw.xy = 0.0f;
            cmf->mf_raw.zy = 0.0f;
            cmf->mf_raw.wy = 0.0f;
            cmf->mf_raw.xz = 0.0f;
            cmf->mf_raw.yz = 0.0f;
            cmf->mf_raw.wz = 0.0f;
            cmf->mf_raw.xw = 0.0f;
            cmf->mf_raw.yw = 0.0f;
            cmf->mf_raw.zw = 0.0f;
            cmf->mf_raw.xx = 1.0f;
            cmf->mf_raw.yy = 1.0f;
            cmf->mf_raw.zz = 1.0f;
            cmf->mf_raw.ww = 1.0f;
        }
    }
}

MtxF* Matrix_CheckFloats(MtxF* mf, const char* file, s32 line) {
    s32 i, j;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (!(-32768.0f <= mf->mf[i][j]) || !(mf->mf[i][j] < 32768.0f)) {
                osSyncPrintf("%s %d: [%s] =\n"
                             "/ %12.6f %12.6f %12.6f %12.6f \\\n"
                             "| %12.6f %12.6f %12.6f %12.6f |\n"
                             "| %12.6f %12.6f %12.6f %12.6f |\n"
                             "\\ %12.6f %12.6f %12.6f %12.6f /\n",
                             file, line, "mf", mf->mf_raw.xx, mf->mf_raw.xy, mf->mf_raw.xz, mf->mf_raw.xw, mf->mf_raw.yx, mf->mf_raw.yy, mf->mf_raw.yz, mf->mf_raw.yw, mf->mf_raw.zx,
                             mf->mf_raw.zy, mf->mf_raw.zz, mf->mf_raw.zw, mf->mf_raw.wx, mf->mf_raw.wy, mf->mf_raw.wz, mf->mf_raw.ww);
                //Fault_AddHungupAndCrash(file, line);
            }
        }
    }

    return mf;
}

void Matrix_SetTranslateUniformScaleMtxF(MtxF* mf, f32 scale, f32 translateX, f32 translateY, f32 translateZ) {
    mf->mf_raw.yx = 0.0f;
    mf->mf_raw.zx = 0.0f;
    mf->mf_raw.wx = 0.0f;
    mf->mf_raw.xy = 0.0f;
    mf->mf_raw.zy = 0.0f;
    mf->mf_raw.wy = 0.0f;
    mf->mf_raw.xz = 0.0f;
    mf->mf_raw.yz = 0.0f;
    mf->mf_raw.wz = 0.0f;
    mf->mf_raw.xx = scale;
    mf->mf_raw.yy = scale;
    mf->mf_raw.zz = scale;
    mf->mf_raw.xw = translateX;
    mf->mf_raw.yw = translateY;
    mf->mf_raw.zw = translateZ;
    mf->mf_raw.ww = 1.0f;
}

void Matrix_SetTranslateUniformScaleMtx(Mtx* mtx, f32 scale, f32 translateX, f32 translateY, f32 translateZ) {
    MtxF mf;

    Matrix_SetTranslateUniformScaleMtxF(&mf, scale, translateX, translateY, translateZ);
    guMtxF2L(&mf, mtx);
}

void Matrix_SetTranslateUniformScaleMtx2(Mtx* mtx, f32 scale, f32 translateX, f32 translateY, f32 translateZ) {
    u16* intPart = (u16*)&mtx->m[0][0];
    u16* fracPart = (u16*)&mtx->m[2][0];
    u32 fixedPoint;

    fixedPoint = (s32)(scale * 0x10000);
    fracPart[0] = fixedPoint & 0xFFFF;
    intPart[0] = (fixedPoint >> 16) & 0xFFFF;

    fixedPoint = (s32)(scale * 0x10000);
    intPart[5] = (fixedPoint >> 16) & 0xFFFF;
    fracPart[5] = fixedPoint & 0xFFFF;

    fixedPoint = (s32)(scale * 0x10000);
    intPart[10] = (fixedPoint >> 16) & 0xFFFF;
    fracPart[10] = fixedPoint & 0xFFFF;

    fixedPoint = (s32)(translateX * 0x10000);
    intPart[12] = (fixedPoint >> 16) & 0xFFFF;
    fracPart[12] = fixedPoint & 0xFFFF;

    fixedPoint = (s32)(translateY * 0x10000);
    intPart[13] = (fixedPoint >> 16) & 0xFFFF;
    fracPart[13] = fixedPoint & 0xFFFF;

    fixedPoint = (s32)(translateZ * 0x10000);
    intPart[14] = (fixedPoint >> 16) & 0xFFFF;
    fracPart[14] = fixedPoint & 0xFFFF;

    intPart[1] = 0;
    intPart[2] = 0;
    intPart[3] = 0;
    intPart[4] = 0;
    intPart[6] = 0;
    intPart[7] = 0;
    intPart[8] = 0;
    intPart[9] = 0;
    intPart[11] = 0;
    intPart[15] = 1;

    fracPart[1] = 0;
    fracPart[2] = 0;
    fracPart[3] = 0;
    fracPart[4] = 0;
    fracPart[6] = 0;
    fracPart[7] = 0;
    fracPart[8] = 0;
    fracPart[9] = 0;
    fracPart[11] = 0;
    fracPart[15] = 0;
}

void Matrix_SetTranslateScaleMtx1(Mtx* mtx, f32 scaleX, f32 scaleY, f32 scaleZ, f32 translateX, f32 translateY,
                                  f32 translateZ) {
    u16* intPart = (u16*)&mtx->m[0][0];
    u16* fracPart = (u16*)&mtx->m[2][0];
    u32 fixedPoint;

    fixedPoint = (s32)(scaleX * 0x10000);
    intPart[0] = (fixedPoint >> 16) & 0xFFFF;
    fracPart[0] = fixedPoint & 0xFFFF;

    fixedPoint = (s32)(scaleY * 0x10000);
    intPart[5] = (fixedPoint >> 16) & 0xFFFF;
    fracPart[5] = fixedPoint & 0xFFFF;

    fixedPoint = (s32)(scaleZ * 0x10000);
    intPart[10] = (fixedPoint >> 16) & 0xFFFF;
    fracPart[10] = fixedPoint & 0xFFFF;

    fixedPoint = (s32)(translateX * 0x10000);
    intPart[12] = (fixedPoint >> 16) & 0xFFFF;
    fracPart[12] = fixedPoint & 0xFFFF;

    fixedPoint = (s32)(translateY * 0x10000);
    intPart[13] = (fixedPoint >> 16) & 0xFFFF;
    fracPart[13] = fixedPoint & 0xFFFF;

    fixedPoint = (s32)(translateZ * 0x10000);
    intPart[14] = (fixedPoint >> 16) & 0xFFFF;
    fracPart[14] = fixedPoint & 0xFFFF;

    intPart[1] = 0;
    intPart[2] = 0;
    intPart[3] = 0;
    intPart[4] = 0;
    intPart[6] = 0;
    intPart[7] = 0;
    intPart[8] = 0;
    intPart[9] = 0;
    intPart[11] = 0;
    intPart[15] = 1;

    fracPart[1] = 0;
    fracPart[2] = 0;
    fracPart[3] = 0;
    fracPart[4] = 0;
    fracPart[6] = 0;
    fracPart[7] = 0;
    fracPart[8] = 0;
    fracPart[9] = 0;
    fracPart[11] = 0;
    fracPart[15] = 0;
}

void Matrix_SetTranslateScaleMtx2(Mtx* mtx, f32 scaleX, f32 scaleY, f32 scaleZ, f32 translateX, f32 translateY,
                                  f32 translateZ) {
    MtxF mtxf = { {
            { scaleX, 0, 0, 0 },
            { 0, scaleY, 0, 0 },
            { 0, 0, scaleZ, 0 },
            { translateX, translateY, translateZ, 1 }
        } };
    guMtxF2L(&mtxf, mtx);
}
