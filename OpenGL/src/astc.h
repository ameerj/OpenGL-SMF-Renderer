// Copyright 2018 yuzu Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

#include <cstdint>
#include <vector>



/// Count the number of bits set in a number.
constexpr u32 Popcnt(u32 n) {
	u32 c = 0;
	for (; n; c++) {
		n &= n - 1;
	}
	return c;
}

enum class IntegerEncoding { JustBits, Qus32, Trit };

struct IntegerEncodedValue {
	constexpr IntegerEncodedValue() = default;

	constexpr IntegerEncodedValue(IntegerEncoding encoding_, u32 num_bits_)
		: encoding{ encoding_ }, num_bits{ num_bits_ } {}

	constexpr bool MatchesEncoding(const IntegerEncodedValue& other) const {
		return encoding == other.encoding && num_bits == other.num_bits;
	}

	// Returns the number of bits required to encode nVals values.
	u32 GetBitLength(u32 nVals) const {
		u32 totalBits = num_bits * nVals;
		if (encoding == IntegerEncoding::Trit) {
			totalBits += (nVals * 8 + 4) / 5;
		}
		else if (encoding == IntegerEncoding::Qus32) {
			totalBits += (nVals * 7 + 2) / 3;
		}
		printf("totalBits: %u   num_bits: %u\n", totalBits, num_bits);

		return totalBits;
	}

	IntegerEncoding encoding{};
	u32 num_bits = 0;
	u32 bit_value = 0;
	union {
		u32 qus32_value = 0;
		u32 trit_value;
	};
};

// Returns a new instance of this struct that corresponds to the
// can take no more than maxval values
static constexpr IntegerEncodedValue CreateEncoding(u32 maxVal) {
	while (maxVal > 0) {
		u32 check = maxVal + 1;

		// Is maxVal a power of two?
		if (!(check & (check - 1))) {
			return IntegerEncodedValue(IntegerEncoding::JustBits, Popcnt(maxVal));
		}

		// Is maxVal of the type 3*2^n - 1?
		if ((check % 3 == 0) && !((check / 3) & ((check / 3) - 1))) {
			return IntegerEncodedValue(IntegerEncoding::Trit, Popcnt(check / 3 - 1));
		}

		// Is maxVal of the type 5*2^n - 1?
		if ((check % 5 == 0) && !((check / 5) & ((check / 5) - 1))) {
			return IntegerEncodedValue(IntegerEncoding::Qus32, Popcnt(check / 5 - 1));
		}

		// Apparently it can't be represented with a bounded integer sequence...
		// just iterate.
		maxVal--;
	}
	return IntegerEncodedValue(IntegerEncoding::JustBits, 0);
}

static constexpr std::array<IntegerEncodedValue, 256> MakeEncodedValues() {
	std::array<IntegerEncodedValue, 256> encodings{};
	for (std::size_t i = 0; i < encodings.size(); ++i) {
		encodings[i] = CreateEncoding(static_cast<u32>(i));
	}
	return encodings;
}

static constexpr std::array<IntegerEncodedValue, 256> EncodingsValues = MakeEncodedValues();

namespace Tegra::Texture::ASTC {

std::vector<u8> Decompress(const uint8_t* data, uint32_t width, uint32_t height,
                                uint32_t depth, uint32_t block_width, uint32_t block_height);

} // namespace Tegra::Texture::ASTC
