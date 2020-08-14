#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <array>

/* ============================================================================
	ASTC compressed file loading
============================================================================ 
static constexpr uint32_t ASTC_MAGIC_ID = 0x5CA1AB13;
#ifndef MAX
#define MAX(x,y) ((x)>(y)?(x):(y))
#endif

static unsigned int unpack_bytes(
	uint8_t a,
	uint8_t b,
	uint8_t c,
	uint8_t d
) {
	return ((unsigned int)(a)) +
		((unsigned int)(b) << 8) +
		((unsigned int)(c) << 16) +
		((unsigned int)(d) << 24);
}
struct astc_header
{
	uint8_t magic[4];
	uint8_t block_x;
	uint8_t block_y;
	uint8_t block_z;
	uint8_t dim_x[3];			// dims = dim[0] + (dim[1] << 8) + (dim[2] << 16)
	uint8_t dim_y[3];			// Sizes are given in texels;
	uint8_t dim_z[3];			// block count is inferred
};

struct astc_compressed_image
{
	unsigned int block_x;
	unsigned int block_y;
	unsigned int block_z;
	unsigned int dim_x;
	unsigned int dim_y;
	unsigned int dim_z;
	uint8_t* data;
	size_t data_len;
};
int load_cimage(
	const char* filename,
	astc_compressed_image& out_image
) {
	std::ifstream file(filename, std::ios::in | std::ios::binary);
	if (!file)
	{
		printf("ERROR: File open failed '%s'\n", filename);
		return 1;
	}

	astc_header hdr;
	file.read((char*)&hdr, sizeof(astc_header));
	if (!file)
	{
		printf("ERROR: File read failed '%s'\n", filename);
		return 1;
	}

	unsigned int magicval = unpack_bytes(hdr.magic[0], hdr.magic[1], hdr.magic[2], hdr.magic[3]);
	if (magicval != ASTC_MAGIC_ID)
	{
		printf("ERROR: File not recognized '%s'\n", filename);
		return 1;
	}

	// Ensure these are not zero to avoid div by zero
	unsigned int block_x = MAX(hdr.block_x, 1);
	unsigned int block_y = MAX(hdr.block_y, 1);
	unsigned int block_z = MAX(hdr.block_z, 1);

	unsigned int dim_x = unpack_bytes(hdr.dim_x[0], hdr.dim_x[1], hdr.dim_x[2], 0);
	unsigned int dim_y = unpack_bytes(hdr.dim_y[0], hdr.dim_y[1], hdr.dim_y[2], 0);
	unsigned int dim_z = unpack_bytes(hdr.dim_z[0], hdr.dim_z[1], hdr.dim_z[2], 0);

	if (dim_x == 0 || dim_z == 0 || dim_z == 0)
	{
		printf("ERROR: File corrupt '%s'\n", filename);
		return 1;
	}

	unsigned int xblocks = (dim_x + block_x - 1) / block_x;
	unsigned int yblocks = (dim_y + block_y - 1) / block_y;
	unsigned int zblocks = (dim_z + block_z - 1) / block_z;

	size_t data_size = xblocks * yblocks * zblocks * 16;
	uint8_t *buffer = new uint8_t[data_size];

	file.read((char*)buffer, data_size);
	if (!file)
	{
		printf("ERROR: File read failed '%s'\n", filename);
		return 1;
	}

	out_image.data = buffer;
	out_image.data_len = data_size;
	out_image.block_x = block_x;
	out_image.block_y = block_y;
	out_image.block_z = block_z;
	out_image.dim_x = dim_x;
	out_image.dim_y = dim_y;
	out_image.dim_z = dim_z;
	return 0;
}
*/

using u8 = std::uint8_t;   ///< 8-bit unsigned byte
using u16 = std::uint16_t; ///< 16-bit unsigned short
using u32 = std::uint32_t; ///< 32-bit unsigned word
using u64 = std::uint64_t; ///< 64-bit unsigned int

using s8 = std::int8_t;   ///< 8-bit signed byte
using s16 = std::int16_t; ///< 16-bit signed short
using s32 = std::int32_t; ///< 32-bit signed word
using s64 = std::int64_t; ///< 64-bit signed int

using f32 = float;  ///< 32-bit floating point
using f64 = double; ///< 64-bit floating point

using VAddr = u64;    ///< Represents a pointer in the userspace virtual address space.
using PAddr = u64;    ///< Represents a pointer in the ARM11 physical address space.
using GPUVAddr = u64; ///< Represents a pointer in the GPU virtual address space.
