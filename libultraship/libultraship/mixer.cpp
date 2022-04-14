#include "mixer.h"

#include <cstring>

#define ROUND_UP_64(v) (((v) + 63) & ~63)
#define ROUND_UP_32(v) (((v) + 31) & ~31)
#define ROUND_UP_16(v) (((v) + 15) & ~15)
#define ROUND_UP_8(v) (((v) + 7) & ~7)
#define ROUND_DOWN_16(v) ((v) & ~0xf)

#define DMEM_BUF_SIZE (0x1000 - 0x3C0 - 0x40)
#define BUF_U8(a) (rspa.buf.as_u8 + ((a) - 0x3C0))
#define BUF_S16(a) (rspa.buf.as_s16 + ((a) - 0x3C0) / sizeof(int16_t))

static struct {
    uint16_t in;
    uint16_t out;
    uint16_t nbytes;

    uint16_t vol[2];
    uint16_t rate[2];
    uint16_t vol_wet;
    uint16_t rate_wet;

    ADPCM_STATE *adpcm_loop_state;

    int16_t adpcm_table[8][2][8];

    uint16_t filter_count;
    int16_t filter[8];

    union {
        int16_t as_s16[DMEM_BUF_SIZE / sizeof(int16_t)];
        uint8_t as_u8[DMEM_BUF_SIZE];
    } buf;
} rspa;

static int16_t resample_table[64][4] = {
    {static_cast<std::int16_t>(0x0c39), static_cast<std::int16_t>(0x66ad), static_cast<std::int16_t>(0x0d46), static_cast<std::int16_t>(0xffdf)}, {static_cast<std::int16_t>(0x0b39), static_cast<std::int16_t>(0x6696), static_cast<std::int16_t>(0x0e5f), static_cast<std::int16_t>(0xffd8)},
    {static_cast<std::int16_t>(0x0a44), static_cast<std::int16_t>(0x6669), static_cast<std::int16_t>(0x0f83), static_cast<std::int16_t>(0xffd0)}, {static_cast<std::int16_t>(0x095a), static_cast<std::int16_t>(0x6626), static_cast<std::int16_t>(0x10b4), static_cast<std::int16_t>(0xffc8)},
    {static_cast<std::int16_t>(0x087d), static_cast<std::int16_t>(0x65cd), static_cast<std::int16_t>(0x11f0), static_cast<std::int16_t>(0xffbf)}, {static_cast<std::int16_t>(0x07ab), static_cast<std::int16_t>(0x655e), static_cast<std::int16_t>(0x1338), static_cast<std::int16_t>(0xffb6)},
    {static_cast<std::int16_t>(0x06e4), static_cast<std::int16_t>(0x64d9), static_cast<std::int16_t>(0x148c), static_cast<std::int16_t>(0xffac)}, {static_cast<std::int16_t>(0x0628), static_cast<std::int16_t>(0x643f), static_cast<std::int16_t>(0x15eb), static_cast<std::int16_t>(0xffa1)},
    {static_cast<std::int16_t>(0x0577), static_cast<std::int16_t>(0x638f), static_cast<std::int16_t>(0x1756), static_cast<std::int16_t>(0xff96)}, {static_cast<std::int16_t>(0x04d1), static_cast<std::int16_t>(0x62cb), static_cast<std::int16_t>(0x18cb), static_cast<std::int16_t>(0xff8a)},
    {static_cast<std::int16_t>(0x0435), static_cast<std::int16_t>(0x61f3), static_cast<std::int16_t>(0x1a4c), static_cast<std::int16_t>(0xff7e)}, {static_cast<std::int16_t>(0x03a4), static_cast<std::int16_t>(0x6106), static_cast<std::int16_t>(0x1bd7), static_cast<std::int16_t>(0xff71)},
    {static_cast<std::int16_t>(0x031c), static_cast<std::int16_t>(0x6007), static_cast<std::int16_t>(0x1d6c), static_cast<std::int16_t>(0xff64)}, {static_cast<std::int16_t>(0x029f), static_cast<std::int16_t>(0x5ef5), static_cast<std::int16_t>(0x1f0b), static_cast<std::int16_t>(0xff56)},
    {static_cast<std::int16_t>(0x022a), static_cast<std::int16_t>(0x5dd0), static_cast<std::int16_t>(0x20b3), static_cast<std::int16_t>(0xff48)}, {static_cast<std::int16_t>(0x01be), static_cast<std::int16_t>(0x5c9a), static_cast<std::int16_t>(0x2264), static_cast<std::int16_t>(0xff3a)},
    {static_cast<std::int16_t>(0x015b), static_cast<std::int16_t>(0x5b53), static_cast<std::int16_t>(0x241e), static_cast<std::int16_t>(0xff2c)}, {static_cast<std::int16_t>(0x0101), static_cast<std::int16_t>(0x59fc), static_cast<std::int16_t>(0x25e0), static_cast<std::int16_t>(0xff1e)},
    {static_cast<std::int16_t>(0x00ae), static_cast<std::int16_t>(0x5896), static_cast<std::int16_t>(0x27a9), static_cast<std::int16_t>(0xff10)}, {static_cast<std::int16_t>(0x0063), static_cast<std::int16_t>(0x5720), static_cast<std::int16_t>(0x297a), static_cast<std::int16_t>(0xff02)},
    {static_cast<std::int16_t>(0x001f), static_cast<std::int16_t>(0x559d), static_cast<std::int16_t>(0x2b50), static_cast<std::int16_t>(0xfef4)}, {static_cast<std::int16_t>(0xffe2), static_cast<std::int16_t>(0x540d), static_cast<std::int16_t>(0x2d2c), static_cast<std::int16_t>(0xfee8)},
    {static_cast<std::int16_t>(0xffac), static_cast<std::int16_t>(0x5270), static_cast<std::int16_t>(0x2f0d), static_cast<std::int16_t>(0xfedb)}, {static_cast<std::int16_t>(0xff7c), static_cast<std::int16_t>(0x50c7), static_cast<std::int16_t>(0x30f3), static_cast<std::int16_t>(0xfed0)},
    {static_cast<std::int16_t>(0xff53), static_cast<std::int16_t>(0x4f14), static_cast<std::int16_t>(0x32dc), static_cast<std::int16_t>(0xfec6)}, {static_cast<std::int16_t>(0xff2e), static_cast<std::int16_t>(0x4d57), static_cast<std::int16_t>(0x34c8), static_cast<std::int16_t>(0xfebd)},
    {static_cast<std::int16_t>(0xff0f), static_cast<std::int16_t>(0x4b91), static_cast<std::int16_t>(0x36b6), static_cast<std::int16_t>(0xfeb6)}, {static_cast<std::int16_t>(0xfef5), static_cast<std::int16_t>(0x49c2), static_cast<std::int16_t>(0x38a5), static_cast<std::int16_t>(0xfeb0)},
    {static_cast<std::int16_t>(0xfedf), static_cast<std::int16_t>(0x47ed), static_cast<std::int16_t>(0x3a95), static_cast<std::int16_t>(0xfeac)}, {static_cast<std::int16_t>(0xfece), static_cast<std::int16_t>(0x4611), static_cast<std::int16_t>(0x3c85), static_cast<std::int16_t>(0xfeab)},
    {static_cast<std::int16_t>(0xfec0), static_cast<std::int16_t>(0x4430), static_cast<std::int16_t>(0x3e74), static_cast<std::int16_t>(0xfeac)}, {static_cast<std::int16_t>(0xfeb6), static_cast<std::int16_t>(0x424a), static_cast<std::int16_t>(0x4060), static_cast<std::int16_t>(0xfeaf)},
    {static_cast<std::int16_t>(0xfeaf), static_cast<std::int16_t>(0x4060), static_cast<std::int16_t>(0x424a), static_cast<std::int16_t>(0xfeb6)}, {static_cast<std::int16_t>(0xfeac), static_cast<std::int16_t>(0x3e74), static_cast<std::int16_t>(0x4430), static_cast<std::int16_t>(0xfec0)},
    {static_cast<std::int16_t>(0xfeab), static_cast<std::int16_t>(0x3c85), static_cast<std::int16_t>(0x4611), static_cast<std::int16_t>(0xfece)}, {static_cast<std::int16_t>(0xfeac), static_cast<std::int16_t>(0x3a95), static_cast<std::int16_t>(0x47ed), static_cast<std::int16_t>(0xfedf)},
    {static_cast<std::int16_t>(0xfeb0), static_cast<std::int16_t>(0x38a5), static_cast<std::int16_t>(0x49c2), static_cast<std::int16_t>(0xfef5)}, {static_cast<std::int16_t>(0xfeb6), static_cast<std::int16_t>(0x36b6), static_cast<std::int16_t>(0x4b91), static_cast<std::int16_t>(0xff0f)},
    {static_cast<std::int16_t>(0xfebd), static_cast<std::int16_t>(0x34c8), static_cast<std::int16_t>(0x4d57), static_cast<std::int16_t>(0xff2e)}, {static_cast<std::int16_t>(0xfec6), static_cast<std::int16_t>(0x32dc), static_cast<std::int16_t>(0x4f14), static_cast<std::int16_t>(0xff53)},
    {static_cast<std::int16_t>(0xfed0), static_cast<std::int16_t>(0x30f3), static_cast<std::int16_t>(0x50c7), static_cast<std::int16_t>(0xff7c)}, {static_cast<std::int16_t>(0xfedb), static_cast<std::int16_t>(0x2f0d), static_cast<std::int16_t>(0x5270), static_cast<std::int16_t>(0xffac)},
    {static_cast<std::int16_t>(0xfee8), static_cast<std::int16_t>(0x2d2c), static_cast<std::int16_t>(0x540d), static_cast<std::int16_t>(0xffe2)}, {static_cast<std::int16_t>(0xfef4), static_cast<std::int16_t>(0x2b50), static_cast<std::int16_t>(0x559d), static_cast<std::int16_t>(0x001f)},
    {static_cast<std::int16_t>(0xff02), static_cast<std::int16_t>(0x297a), static_cast<std::int16_t>(0x5720), static_cast<std::int16_t>(0x0063)}, {static_cast<std::int16_t>(0xff10), static_cast<std::int16_t>(0x27a9), static_cast<std::int16_t>(0x5896), static_cast<std::int16_t>(0x00ae)},
    {static_cast<std::int16_t>(0xff1e), static_cast<std::int16_t>(0x25e0), static_cast<std::int16_t>(0x59fc), static_cast<std::int16_t>(0x0101)}, {static_cast<std::int16_t>(0xff2c), static_cast<std::int16_t>(0x241e), static_cast<std::int16_t>(0x5b53), static_cast<std::int16_t>(0x015b)},
    {static_cast<std::int16_t>(0xff3a), static_cast<std::int16_t>(0x2264), static_cast<std::int16_t>(0x5c9a), static_cast<std::int16_t>(0x01be)}, {static_cast<std::int16_t>(0xff48), static_cast<std::int16_t>(0x20b3), static_cast<std::int16_t>(0x5dd0), static_cast<std::int16_t>(0x022a)},
    {static_cast<std::int16_t>(0xff56), static_cast<std::int16_t>(0x1f0b), static_cast<std::int16_t>(0x5ef5), static_cast<std::int16_t>(0x029f)}, {static_cast<std::int16_t>(0xff64), static_cast<std::int16_t>(0x1d6c), static_cast<std::int16_t>(0x6007), static_cast<std::int16_t>(0x031c)},
    {static_cast<std::int16_t>(0xff71), static_cast<std::int16_t>(0x1bd7), static_cast<std::int16_t>(0x6106), static_cast<std::int16_t>(0x03a4)}, {static_cast<std::int16_t>(0xff7e), static_cast<std::int16_t>(0x1a4c), static_cast<std::int16_t>(0x61f3), static_cast<std::int16_t>(0x0435)},
    {static_cast<std::int16_t>(0xff8a), static_cast<std::int16_t>(0x18cb), static_cast<std::int16_t>(0x62cb), static_cast<std::int16_t>(0x04d1)}, {static_cast<std::int16_t>(0xff96), static_cast<std::int16_t>(0x1756), static_cast<std::int16_t>(0x638f), static_cast<std::int16_t>(0x0577)},
    {static_cast<std::int16_t>(0xffa1), static_cast<std::int16_t>(0x15eb), static_cast<std::int16_t>(0x643f), static_cast<std::int16_t>(0x0628)}, {static_cast<std::int16_t>(0xffac), static_cast<std::int16_t>(0x148c), static_cast<std::int16_t>(0x64d9), static_cast<std::int16_t>(0x06e4)},
    {static_cast<std::int16_t>(0xffb6), static_cast<std::int16_t>(0x1338), static_cast<std::int16_t>(0x655e), static_cast<std::int16_t>(0x07ab)}, {static_cast<std::int16_t>(0xffbf), static_cast<std::int16_t>(0x11f0), static_cast<std::int16_t>(0x65cd), static_cast<std::int16_t>(0x087d)},
    {static_cast<std::int16_t>(0xffc8), static_cast<std::int16_t>(0x10b4), static_cast<std::int16_t>(0x6626), static_cast<std::int16_t>(0x095a)}, {static_cast<std::int16_t>(0xffd0), static_cast<std::int16_t>(0x0f83), static_cast<std::int16_t>(0x6669), static_cast<std::int16_t>(0x0a44)},
    {static_cast<std::int16_t>(0xffd8), static_cast<std::int16_t>(0x0e5f), static_cast<std::int16_t>(0x6696), static_cast<std::int16_t>(0x0b39)}, {static_cast<std::int16_t>(0xffdf), static_cast<std::int16_t>(0x0d46), static_cast<std::int16_t>(0x66ad), static_cast<std::int16_t>(0x0c39)}
};

static inline int16_t clamp16(int32_t v) {
    if (v < -0x8000) {
        return -0x8000;
    } else if (v > 0x7fff) {
        return 0x7fff;
    }
    return (int16_t)v;
}

static inline int32_t clamp32(int64_t v) {
    if (v < -0x7fffffff - 1) {
        return -0x7fffffff - 1;
    } else if (v > 0x7fffffff) {
        return 0x7fffffff;
    }
    return (int32_t)v;
}

void aClearBufferImpl(uint16_t addr, int nbytes) {
    nbytes = ROUND_UP_16(nbytes);
    memset(BUF_U8(addr), 0, nbytes);
}

void aLoadBufferImpl(const void *source_addr, uint16_t dest_addr, uint16_t nbytes) {
    memcpy(BUF_U8(dest_addr), source_addr, ROUND_DOWN_16(nbytes));
}

void aSaveBufferImpl(uint16_t source_addr, int16_t *dest_addr, uint16_t nbytes) {
    memcpy(dest_addr, BUF_S16(source_addr), ROUND_DOWN_16(nbytes));
}

void aLoadADPCMImpl(int num_entries_times_16, const int16_t *book_source_addr) {
    memcpy(rspa.adpcm_table, book_source_addr, num_entries_times_16);
}

void aSetBufferImpl([[maybe_unused]] uint8_t flags, uint16_t in, uint16_t out, uint16_t nbytes) {
    rspa.in = in;
    rspa.out = out;
    rspa.nbytes = nbytes;
}

void aInterleaveImpl(uint16_t dest, uint16_t left, uint16_t right, uint16_t c) {
    int count = ROUND_UP_8(c) / sizeof(int16_t) / 4;
    int16_t *l = BUF_S16(left);
    int16_t *r = BUF_S16(right);
    int16_t *d = BUF_S16(dest);
    while (count > 0) {
        int16_t l0 = *l++;
        int16_t l1 = *l++;
        int16_t l2 = *l++;
        int16_t l3 = *l++;
        int16_t r0 = *r++;
        int16_t r1 = *r++;
        int16_t r2 = *r++;
        int16_t r3 = *r++;
        *d++ = l0;
        *d++ = r0;
        *d++ = l1;
        *d++ = r1;
        *d++ = l2;
        *d++ = r2;
        *d++ = l3;
        *d++ = r3;
        --count;
    }
}

void aDMEMMoveImpl(uint16_t in_addr, uint16_t out_addr, int nbytes) {
    nbytes = ROUND_UP_16(nbytes);
    memmove(BUF_U8(out_addr), BUF_U8(in_addr), nbytes);
}

void aSetLoopImpl(ADPCM_STATE* adpcm_loop_state) {
    rspa.adpcm_loop_state = adpcm_loop_state;
}

void aADPCMdecImpl(uint8_t flags, ADPCM_STATE state) {
    uint8_t *in = BUF_U8(rspa.in);
    int16_t *out = BUF_S16(rspa.out);
    int nbytes = ROUND_UP_32(rspa.nbytes);
    if (flags & A_INIT) {
        memset(out, 0, 16 * sizeof(int16_t));
    } else if (flags & A_LOOP) {
        memcpy(out, rspa.adpcm_loop_state, 16 * sizeof(int16_t));
    } else {
        memcpy(out, state, 16 * sizeof(int16_t));
    }
    out += 16;

    while (nbytes > 0) {
        int shift = *in >> 4; // should be in 0..12 or 0..14
        int table_index = *in++ & 0xf; // should be in 0..7
        int16_t (*tbl)[8] = rspa.adpcm_table[table_index];
        int i;

        for (i = 0; i < 2; i++) {
            int16_t ins[8];
            int16_t prev1 = out[-1];
            int16_t prev2 = out[-2];
            int j, k;
			if (flags & 4) {
				for (j = 0; j < 2; j++) {
					ins[j * 4] = (((*in >> 6) << 30) >> 30) << shift;
					ins[j * 4 + 1] = ((((*in >> 4) & 0x3) << 30) >> 30) << shift;
					ins[j * 4 + 2] = ((((*in >> 2) & 0x3) << 30) >> 30) << shift;
					ins[j * 4 + 3] = (((*in++ & 0x3) << 30) >> 30) << shift;
				}
			} else {
				for (j = 0; j < 4; j++) {
					ins[j * 2] = (((*in >> 4) << 28) >> 28) << shift;
					ins[j * 2 + 1] = (((*in++ & 0xf) << 28) >> 28) << shift;
				}
			}
            for (j = 0; j < 8; j++) {
                int32_t acc = tbl[0][j] * prev2 + tbl[1][j] * prev1 + (ins[j] << 11);
                for (k = 0; k < j; k++) {
                    acc += tbl[1][((j - k) - 1)] * ins[k];
                }
                acc >>= 11;
                *out++ = clamp16(acc);
            }
        }
        nbytes -= 16 * sizeof(int16_t);
    }
    memcpy(state, out - 16, 16 * sizeof(int16_t));
}

void aResampleImpl(uint8_t flags, uint16_t pitch, RESAMPLE_STATE state) {
    int16_t tmp[16];
    int16_t *in_initial = BUF_S16(rspa.in);
    int16_t *in = in_initial;
    int16_t *out = BUF_S16(rspa.out);
    int nbytes = ROUND_UP_16(rspa.nbytes);
    uint32_t pitch_accumulator;
    int i;
    int16_t *tbl;
    int32_t sample;

    if (flags & A_INIT) {
        memset(tmp, 0, 5 * sizeof(int16_t));
    } else {
        memcpy(tmp, state, 16 * sizeof(int16_t));
    }
    if (flags & 2) {
        memcpy(in - 8, tmp + 8, 8 * sizeof(int16_t));
        in -= tmp[5] / sizeof(int16_t);
    }
    in -= 4;
    pitch_accumulator = (uint16_t)tmp[4];
    memcpy(in, tmp, 4 * sizeof(int16_t));

    do {
        for (i = 0; i < 8; i++) {
            tbl = resample_table[pitch_accumulator * 64 >> 16];
            sample = ((in[0] * tbl[0] + 0x4000) >> 15) +
                     ((in[1] * tbl[1] + 0x4000) >> 15) +
                     ((in[2] * tbl[2] + 0x4000) >> 15) +
                     ((in[3] * tbl[3] + 0x4000) >> 15);
            *out++ = clamp16(sample);

            pitch_accumulator += (pitch << 1);
            in += pitch_accumulator >> 16;
            pitch_accumulator %= 0x10000;
        }
        nbytes -= 8 * sizeof(int16_t);
    } while (nbytes > 0);

    state[4] = (int16_t)pitch_accumulator;
    memcpy(state, in, 4 * sizeof(int16_t));
    i = (in - in_initial + 4) & 7;
    in -= i;
    if (i != 0) {
        i = -8 - i;
    }
    state[5] = static_cast<short>(i);
    memcpy(state + 8, in, 8 * sizeof(int16_t));
}

void aEnvSetup1Impl(uint8_t initial_vol_wet, uint16_t rate_wet, uint16_t rate_left, uint16_t rate_right) {
    rspa.vol_wet = (uint16_t)(initial_vol_wet << 8);
    rspa.rate_wet = rate_wet;
    rspa.rate[0] = rate_left;
    rspa.rate[1] = rate_right;
}

void aEnvSetup2Impl(uint16_t initial_vol_left, uint16_t initial_vol_right) {
    rspa.vol[0] = initial_vol_left;
    rspa.vol[1] = initial_vol_right;
}

void aEnvMixerImpl(uint16_t in_addr, uint16_t n_samples, bool swap_reverb,
				   bool neg_3, bool neg_2,
                   bool neg_left, bool neg_right,
                   int32_t wet_dry_addr, u32)
{
    int16_t *in = BUF_S16(in_addr);
    int16_t *dry[2] = {BUF_S16(((wet_dry_addr >> 24) & 0xFF) << 4), BUF_S16(((wet_dry_addr >> 16) & 0xFF) << 4)};
    int16_t *wet[2] = {BUF_S16(((wet_dry_addr >> 8) & 0xFF) << 4), BUF_S16(((wet_dry_addr) & 0xFF) << 4)};
    int16_t negs[4] = {neg_left ? -1 : 0, neg_right ? -1 : 0, neg_3 ? -4 : 0, neg_2 ? -2 : 0};
    int swapped[2] = {swap_reverb ? 1 : 0, swap_reverb ? 0 : 1};
    int n = ROUND_UP_16(n_samples);

    uint16_t vols[2] = {rspa.vol[0], rspa.vol[1]};
    uint16_t rates[2] = {rspa.rate[0], rspa.rate[1]};
    uint16_t vol_wet = rspa.vol_wet;
    uint16_t rate_wet = rspa.rate_wet;

    do {
        for (int i = 0; i < 8; i++) {
            int16_t samples[2] = {*in, *in}; in++;
            for (int j = 0; j < 2; j++) {
                samples[j] = (samples[j] * vols[j] >> 16) ^ negs[j];
            }
        	for (int j = 0; j < 2; j++) {
                *dry[j] = clamp16(*dry[j] + samples[j]); dry[j]++;
                *wet[j] = clamp16(*wet[j] + ((samples[swapped[j]] * vol_wet >> 16) ^ negs[2 + j])); wet[j]++;
            }
        }
        vols[0] += rates[0];
        vols[1] += rates[1];
        vol_wet += rate_wet;

        n -= 8;
    } while (n > 0);
}

void aMixImpl(uint16_t count, int16_t gain, uint16_t in_addr, uint16_t out_addr) {
    int nbytes = ROUND_UP_32(ROUND_DOWN_16(count << 4));
    int16_t *in = BUF_S16(in_addr);
    int16_t *out = BUF_S16(out_addr);
    int i;
    int32_t sample;

    if (gain == -0x8000) {
        while (nbytes > 0) {
            for (i = 0; i < 16; i++) {
                sample = *out - *in++;
                *out++ = clamp16(sample);
            }
            nbytes -= 16 * sizeof(int16_t);
        }
    }

    while (nbytes > 0) {
        for (i = 0; i < 16; i++) {
            sample = ((*out * 0x7fff + *in++ * gain) + 0x4000) >> 15;
            *out++ = clamp16(sample);
        }

        nbytes -= 16 * sizeof(int16_t);
    }
}

void aS8DecImpl(uint8_t flags, ADPCM_STATE state) {
    uint8_t *in = BUF_U8(rspa.in);
    int16_t *out = BUF_S16(rspa.out);
    int nbytes = ROUND_UP_32(rspa.nbytes);
    if (flags & A_INIT) {
        memset(out, 0, 16 * sizeof(int16_t));
    } else if (flags & A_LOOP) {
        memcpy(out, rspa.adpcm_loop_state, 16 * sizeof(int16_t));
    } else {
        memcpy(out, state, 16 * sizeof(int16_t));
    }
    out += 16;

    while (nbytes > 0) {
        *out++ = (int16_t)(*in++ << 8);
        *out++ = (int16_t)(*in++ << 8);
        *out++ = (int16_t)(*in++ << 8);
        *out++ = (int16_t)(*in++ << 8);
        *out++ = (int16_t)(*in++ << 8);
        *out++ = (int16_t)(*in++ << 8);
        *out++ = (int16_t)(*in++ << 8);
        *out++ = (int16_t)(*in++ << 8);
        *out++ = (int16_t)(*in++ << 8);
        *out++ = (int16_t)(*in++ << 8);
        *out++ = (int16_t)(*in++ << 8);
        *out++ = (int16_t)(*in++ << 8);
        *out++ = (int16_t)(*in++ << 8);
        *out++ = (int16_t)(*in++ << 8);
        *out++ = (int16_t)(*in++ << 8);
        *out++ = (int16_t)(*in++ << 8);

        nbytes -= 16 * sizeof(int16_t);
    }

    memcpy(state, out - 16, 16 * sizeof(int16_t));
}

void aAddMixerImpl(uint16_t in_addr, uint16_t out_addr, uint16_t count) {
    int16_t *in = BUF_S16(in_addr);
    int16_t *out = BUF_S16(out_addr);
    int nbytes = ROUND_UP_64(ROUND_DOWN_16(count));

    do {
        *out = clamp16(*out + *in++); out++;
        *out = clamp16(*out + *in++); out++;
        *out = clamp16(*out + *in++); out++;
        *out = clamp16(*out + *in++); out++;
        *out = clamp16(*out + *in++); out++;
        *out = clamp16(*out + *in++); out++;
        *out = clamp16(*out + *in++); out++;
        *out = clamp16(*out + *in++); out++;
        *out = clamp16(*out + *in++); out++;
        *out = clamp16(*out + *in++); out++;
        *out = clamp16(*out + *in++); out++;
        *out = clamp16(*out + *in++); out++;
        *out = clamp16(*out + *in++); out++;
        *out = clamp16(*out + *in++); out++;
        *out = clamp16(*out + *in++); out++;
        *out = clamp16(*out + *in++); out++;

        nbytes -= 16 * sizeof(int16_t);
    } while (nbytes > 0);
}

void aDuplicateImpl(uint16_t count, uint16_t in_addr, uint16_t out_addr) {
    uint8_t* in = BUF_U8(in_addr);
    uint8_t *out = BUF_U8(out_addr);

    uint8_t tmp[128];
    memcpy(tmp, in, 128);
    do {
        memcpy(out, tmp, 128);
        out += 128;
    } while (count-- > 0);
}

void aResampleZohImpl(uint16_t pitch, uint16_t start_fract) {
    int16_t *in = BUF_S16(rspa.in);
    int16_t *out = BUF_S16(rspa.out);
    int nbytes = ROUND_UP_8(rspa.nbytes);
    uint32_t pos = start_fract;
    uint32_t pitch_add = pitch << 2;

    do {
        *out++ = in[pos >> 17]; pos += pitch_add;
        *out++ = in[pos >> 17]; pos += pitch_add;
        *out++ = in[pos >> 17]; pos += pitch_add;
        *out++ = in[pos >> 17]; pos += pitch_add;

        nbytes -= 4 * sizeof(int16_t);
    } while (nbytes > 0);
}

void aInterlImpl(uint16_t in_addr, uint16_t out_addr, uint16_t n_samples) {
    int16_t *in = BUF_S16(in_addr);
    int16_t *out = BUF_S16(out_addr);
    int n = ROUND_UP_8(n_samples);

    do {
        *out++ = *in++; in++;
        *out++ = *in++; in++;
        *out++ = *in++; in++;
        *out++ = *in++; in++;
        *out++ = *in++; in++;
        *out++ = *in++; in++;
        *out++ = *in++; in++;
        *out++ = *in++; in++;

        n -= 8;
    } while (n > 0);
}

void aFilterImpl(uint8_t flags, uint16_t count_or_buf, int16_t *state_or_filter) {
    if (flags > A_INIT) {
        rspa.filter_count = ROUND_UP_16(count_or_buf);
        memcpy(rspa.filter, state_or_filter, sizeof(rspa.filter));
    } else {
        int16_t tmp[16], tmp2[8];
        int count = rspa.filter_count;
        int16_t *buf = BUF_S16(count_or_buf);

        if (flags == A_INIT) {
            memset(tmp, 0, 8 * sizeof(int16_t));
            memset(tmp2, 0, 8 * sizeof(int16_t));
        } else {
            memcpy(tmp, state_or_filter, 8 * sizeof(int16_t));
            memcpy(tmp2, state_or_filter + 8, 8 * sizeof(int16_t));
        }
        
        for (int i = 0; i < 8; i++) {
            rspa.filter[i] = (tmp2[i] + rspa.filter[i]) / 2;
        }

        do {
            memcpy(tmp + 8, buf, 8 * sizeof(int16_t));
            for (int i = 0; i < 8; i++) {
                int64_t sample = 0x4000; // round term
                for (int j = 0; j < 8; j++) {
                    sample += tmp[i + j] * rspa.filter[7 - j];
                }
                buf[i] = clamp16((int32_t)(sample >> 15));
            }
            memcpy(tmp, tmp + 8, 8 * sizeof(int16_t));

            buf += 8;
            count -= 8 * sizeof(int16_t);
        } while (count > 0);

        memcpy(state_or_filter, tmp, 8 * sizeof(int16_t));
        memcpy(state_or_filter + 8, rspa.filter, 8 * sizeof(int16_t));
    }
}

void aHiLoGainImpl(uint8_t g, uint16_t count, uint16_t addr) {
    int16_t *samples = BUF_S16(addr);
    int nbytes = ROUND_UP_32(count);

    do {
        *samples = clamp16((*samples * g) >> 4); samples++;
        *samples = clamp16((*samples * g) >> 4); samples++;
        *samples = clamp16((*samples * g) >> 4); samples++;
        *samples = clamp16((*samples * g) >> 4); samples++;
        *samples = clamp16((*samples * g) >> 4); samples++;
        *samples = clamp16((*samples * g) >> 4); samples++;
        *samples = clamp16((*samples * g) >> 4); samples++;
        *samples = clamp16((*samples * g) >> 4); samples++;

        nbytes -= 8;
    } while (nbytes > 0);
}

void aUnkCmd3Impl(uint16_t, uint16_t, uint16_t) {
}

void aUnkCmd19Impl(uint8_t f, uint16_t count, uint16_t out_addr, uint16_t in_addr) {
    int nbytes = ROUND_UP_64(count);
    int16_t *in = BUF_S16(in_addr + f);
    int16_t *out = BUF_S16(out_addr);
    int16_t tbl[32];

    memcpy(tbl, in, 32 * sizeof(int16_t));
    do {
        for (int i = 0; i < 32; i++) {
            out[i] = clamp16(out[i] * tbl[i]);
        }
        out += 32;
        nbytes -= 32 * sizeof(int16_t);
    } while (nbytes > 0);
}
