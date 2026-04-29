#include "png.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

static uint32_t s_crc_tbl[256];
static int s_crc_ready = 0;

static void crc_init() {
    for (int n = 0 ;n<256; n++) {
        uint32_t c = (uint32_t)n;
        for (int k =0; k<8;k++) {
            c = (c &1u) ?(0xEDB88320u ^ (c >> 1)) : (c >> 1);
        }
        s_crc_tbl[n] = c;
    }
    s_crc_ready = 1;
}

static uint32_t crc_begin() {
    if (!s_crc_ready) {
        crc_init();
    }
    return 0xFFFFFFFFu;
}

static uint32_t crc_end(const uint32_t state) {
    return (state ^ 0xFFFFFFFFu);
}

static uint32_t crc_feed(uint32_t state, const void *data, size_t len) {
    const uint8_t *p = (const uint8_t *)data;
    for (size_t i = 0; i < len; i++)
        state = s_crc_tbl[(state ^ p[i]) & 0xFFu] ^ (state >> 8);
    return state;
}

static void put_u32be(uint8_t *p, uint32_t v) {
    p[0] = (uint8_t)(v >> 24);
    p[1] = (uint8_t)(v >> 16);
    p[2] = (uint8_t)(v >>  8);
    p[3] = (uint8_t)(v);
}

static void write_chunk(FILE *f, const char type[4],
                        const uint8_t *data, uint32_t len) {
    uint8_t tmp[4];

    put_u32be(tmp, len);
    fwrite(tmp, 1, 4, f);

    fwrite(type, 1, 4, f);

    if (len && data)
        fwrite(data, 1, len, f);

    uint32_t st = crc_begin();
    st = crc_feed(st, type, 4);
    if (len && data)
        st = crc_feed(st, data, len);
    put_u32be(tmp, crc_end(st));
    fwrite(tmp, 1, 4, f);
}

void write_png(FILE *f, const Color *buffer, int width, int height) {
    static const uint8_t sig[8] = {137, 80, 78, 71, 13, 10, 26, 10};
    fwrite(sig, 1, 8, f);

    uint8_t ihdr[13];
    put_u32be(ihdr + 0, (uint32_t)width);
    put_u32be(ihdr + 4, (uint32_t)height);
    ihdr[8]  = 8;
    ihdr[9]  = 2;
    ihdr[10] = 0;
    ihdr[11] = 0;
    ihdr[12] = 0;
    write_chunk(f, "IHDR", ihdr, 13);

    const size_t row_stride = 1u + (size_t)width * 3u;
    const size_t raw_size   = (size_t)height * row_stride;
    uint8_t *raw = (uint8_t *)malloc(raw_size);

    for (int y = 0; y < height; y++) {
        uint8_t *row = raw + y * row_stride;
        row[0] = 0;
        for (int x = 0; x < width; x++) {
            const Color *c = &buffer[y * width + x];
            double r = sqrt(c->x < 0.0 ? 0.0 : c->x);
            double g = sqrt(c->y < 0.0 ? 0.0 : c->y);
            double b = sqrt(c->z < 0.0 ? 0.0 : c->z);
            if (r > 0.9999) r = 0.9999;
            if (g > 0.9999) g = 0.9999;
            if (b > 0.9999) b = 0.9999;
            uint8_t *px = row + 1 + x * 3;
            px[0] = (uint8_t)(r * 256.0);
            px[1] = (uint8_t)(g * 256.0);
            px[2] = (uint8_t)(b * 256.0);
        }
    }
    uLongf compressed_size = compressBound((uLong)raw_size);
    uint8_t *idat = (uint8_t *)malloc(compressed_size);
    const int result = compress2(idat, &compressed_size,
                           raw, (uLong)raw_size,
                           Z_BEST_COMPRESSION);
    if (result != Z_OK) {
        free(idat);
        free(raw);
        return;
    }

    write_chunk(f, "IDAT", idat, (uint32_t)compressed_size);

    free(idat);
    free(raw);

    write_chunk(f, "IEND", NULL, 0);
}





