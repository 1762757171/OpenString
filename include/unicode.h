#pragma once
#include "definitions.h"
#include "types.h"

NS_EASY_BEGIN

namespace unicode
{
	/**
	 * @param c start of a utf8 sequence
	 * @return length of utf8 sequence, return 0 if this is not start of a utf8 sequence
	 */
	constexpr i32 parse_utf8_length(const char c) noexcept
	{
		if (c == 0)
			return 1;
		constexpr char mask = 0b10000000i8;
		i32 size = 0;
		char v = c;
		while (v & mask)
		{
			++size;
			v <<= 1;
		}
		// 1 byte when start with 0
		// 0 byte when start with 10
		// 2 bytes when start with 110
		// n bytes when start with 1..(n)..0
		return size > 1 ? size : 1 - size;
	}

	/**
	 * @param length length of a utf8 sequence
	 * @return mask of utf8 code unit
	 */
	constexpr char get_utf8_mask(const i32 length) noexcept
	{
		constexpr char masks[] =
		{
			0b00111111i8,
			0b01111111i8,
			0b00011111i8,
			0b00001111i8,
			0b00000111i8,
		};
		return masks[length];
	}

	/**
	 * @param utf8 input utf-8 code unit sequence
	 * @param length length of utf-8 code unit sequence
	 * @return codepoint of input utf8 code unit sequence
	 */
	constexpr char32_t utf8_to_utf32(char const* const utf8, const i32 length) noexcept
	{
		if (!utf8) 
			return 0;
		const char c0 = utf8[0];
		const char lead_mask = get_utf8_mask(length);
		char32_t utf32 = c0 & lead_mask;
		constexpr char following_mask = get_utf8_mask(0);
		for (i32 i = 1; i < length; ++i)
		{
			constexpr i32 bits_following = 6;
			const char ci = utf8[i];
			utf32 <<= bits_following;
			utf32 |= ci & following_mask;
		}
		return utf32;
	}

	/**
	 * @param utf32 input utf-32 code unit
	 * @return decoded utf-8 code unit sequence from utf-32 code unit
	 */
	constexpr std::array<char, 4> utf32_to_utf8(const char32_t utf32) noexcept
	{
		if (utf32 < 0x80) return
			{ static_cast<char>(utf32),
				0, 0, 0 };
		if (utf32 < 0x800) return
			{ static_cast<char>((utf32 >> 6) | 0xc0), 
				static_cast<char>((utf32 & 0x3f) | 0x80),
				0, 0 };
		if (utf32 < 0x10000) return
			{ static_cast<char>((utf32 >> 12) | 0xe0),
				static_cast<char>(((utf32 >> 6) & 0x3f) | 0x80),
				static_cast<char>((utf32 & 0x3f) | 0x80),
				0 };
		return
			{ static_cast<char>((utf32 >> 18) | 0xf0),
				static_cast<char>(((utf32 >> 12) & 0x3f) | 0x80),
				static_cast<char>(((utf32 >> 6) & 0x3f) | 0x80),
				static_cast<char>((utf32 & 0x3f) | 0x80) };
	}
}

NS_EASY_END