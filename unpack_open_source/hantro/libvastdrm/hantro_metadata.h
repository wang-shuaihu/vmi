/*
 * Copyright(C) 2018 Verisilicon
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
 * IN NO EVENT SHALL PRECISION INSIGHT AND/OR ITS SUPPLIERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef HANTRO_METADATA_H
#define HANTRO_METADATA_H

#include <stdio.h>
#include <stdint.h>
#include <stdio.h>
#include <pthread.h>
#include "drm/drm.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define HANTRO_MAGIC(ch0, ch1, ch2, ch3) \
    ((unsigned long)(unsigned char) (ch0) | ((unsigned long)(unsigned char) (ch1) << 8) | \
    ((unsigned long)(unsigned char) (ch2) << 16) | ((unsigned long)(unsigned char) (ch3) << 24 ))

#define HANTRO_IMAGE_VIV_META_DATA_MAGIC HANTRO_MAGIC('V','I','V','M')
#define HANTRO_IMAGE_VIV_META_INFO_MAGIC HANTRO_MAGIC('d','e','c','4')

/**
 * name of image format for exchange.
 */
typedef enum viv_image_format
{
    IMAGE_BYTE = 0, // pure data, not image
    IMAGE_YUV420, // 3 plane YUV420 8 bits
    IMAGE_NV12, // 2 plane NV12 8 bits
    IMAGE_UYVY, // packed YUV422 8 bits
    IMAGE_YUY2, // packed YUV422 8 bits
    IMAGE_Y8, // single plane 8 bits
    IMAGE_UV8, // single UV interleave plan 8 bits
    IMAGE_MS_P010, // 2 plane Simiplanar 10 bits, MSB valid
    IMAGE_P010, // 3 plane YUV420 10 bits, MSB valid
    IMAGE_Y210, // packed YUV422 10 bits, LSB valid
    IMAGE_Y10, // single plane 10 bits
    IMAGE_UV10, // single UV interleave plan 10 bits
    IMAGE_XRGB8888, // packed XRGB 8 bits
    IMAGE_ARGB8888,
    IMAGE_A2R10G10B10,
    IMAGE_X2R10G10B10,
    IMAGE_BAYER10, // packed
    IMAGE_BAYER12,
    IMAGE_BAYER14,
    IMAGE_BAYER16,
    IMAGE_FORMAT_MAX
} _VIV_IMG_FORMAT;

/**
 * dec400 tile format, defines how one tile is scaned.
 */
typedef enum viv_tile_format
{
    TILE_NONE = 0,         // raster scane linear
    TILE_Y_4x4,            // luma 4x4
    TILE_Y_8x8,            // luma 8x8
    TILE_4x4_INTERLEAVE,   // U_4x4 tile followed with V4x4,
    TILE_8x8_X_MAJOR,      // RGB super tile X majory
    TILE_8x8_Y_MAJOR,      // RGB super tile y major
    TILE_FMT_MAX
} _VIV_TILE_FORMAT;

/**
 * dec400 tile mode, defines the tile size for each compression block
 */
typedef enum viv_dec_tile_mode
{
    cmvDEC_TILE_MODE_8x8_X_MAJOR  = 0,
    cmvDEC_TILE_MODE_8x8_Y_MAJOR  = 1,
    cmvDEC_TILE_MODE_16x4         = 2,
    cmvDEC_TILE_MODE_8x4          = 3,
    cmvDEC_TILE_MODE_4x8          = 4,
    cmvDEC_TILE_MODE_4x4          = 5,
    cmvDEC_TILE_MODE_16x4_RASTER  = 6,
    cmvDEC_TILE_MODE_64x4         = 7,
    cmvDEC_TILE_MODE_32x4         = 8,
    cmvDEC_TILE_MODE_256x1_RASTER = 9,
    cmvDEC_TILE_MODE_128x1_RASTER = 10,
    cmvDEC_TILE_MODE_64x4_RASTER  = 11,
    cmvDEC_TILE_MODE_256x2_RASTER = 12,
    cmvDEC_TILE_MODE_128x2_RASTER = 13,
    cmvDEC_TILE_MODE_128x4_RASTER = 14,
    cmvDEC_TILE_MODE_64x1_RASTER  = 15,
    cmvDEC_TILE_MODE_16x8_RASTER  = 16,
    cmvDEC_TILE_MODE_8x16_RASTER  = 17,
    cmvDEC_TILE_MODE_512x1_RASTER = 18,
    cmvDEC_TILE_MODE_32x4_RASTER  = 19,
    cmvDEC_TILE_MODE_64x2_RASTER  = 20,
    cmvDEC_TILE_MODE_32x2_RASTER  = 21,
    cmvDEC_TILE_MODE_32x1_RASTER  = 22,
    cmvDEC_TILE_MODE_16x1_RASTER  = 23,
    cmvDEC_TILE_MODE_128x4  = 24,
    cmvDEC_TILE_MODE_256x4  = 25,
    cmvDEC_TILE_MODE_512x4  = 26,
    cmvDEC_TILE_MODE_16x16  = 27,
    cmvDEC_TILE_MODE_32x16  = 28,
} cmeDEC_TILE_MODE;

/**
 * The surface meta data saved in meta data buffer
 */
struct viv_vidmem_metadata
{
    uint32_t magic;        // __FOURCC('v', 'i', 'v', 'm')
    uint32_t dmabuf_size;  // DMABUF buffer size in byte (Maximum 4GB)
    uint32_t time_stamp;   // time stamp for the DMABUF buffer

    uint32_t image_format; // ImageFormat, determined plane number.
    uint32_t compressed;   // if DMABUF buffer is compressed by DEC400
    struct {
        uint32_t offset; // plane buffer address offset from DMABUF address
        uint32_t stride; // pitch in bytes
        uint32_t width;  // width in pixels
        uint32_t height; // height in pixels

        uint32_t tile_format; // uncompressed tile format
        uint32_t compress_format; // tile mode for DEC400

        /** tile status buffer offset within this plane buffer. when it is 0，
         *  indicates using seperate tile status buffer
         */
        uint32_t ts_offset;
        /** fd of seperate tile status buffer of the plane buffer */
        int32_t ts_fd;
        /** valid fd of the ts buffer in consumer side. */
        int32_t ts_fd2;
        /** the vpu virtual address for this ts data buffer */
        int32_t  ts_vaddr;

        /** gpu fastclear enabled for the plane buffer */
        uint32_t fc_enabled;
        /** gpu fastclear color value (lower 32 bits) for the plane buffer */
        uint32_t fc_value_lower;
        /** gpu fastclear color value (upper 32 bits) for the plane buffer */
        uint32_t fc_value_upper;
    } plane[3];
} ;

/**
 * the meta data location information saved in kernel
 */
struct dec_buf_info {
    int32_t    magic_num;    /* should be ’dec4’ */
    int32_t    meta_fd;      /* fd generated by producer process */
    int32_t    meta_fd2;     /* fd2 is used by consumer process */
    uint32_t   meta_offset;  /* offset in the buffer derived by fd or fd2 contains meta data */
    uint32_t   slice_index;  /* gpu slice node index defines different memory region */
};

struct hantro_enc_dma_buf
{
    uint8_t* enc_out_buf;
    uint32_t enc_out_buf_size;
};
#if defined(__cplusplus)
}
#endif

#endif /* HANTRO_METADATA_H */
