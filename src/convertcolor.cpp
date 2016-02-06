#include "skeleton_filter.hpp"

#if defined __SSSE3__  || (defined _MSC_VER && _MSC_VER >= 1500)
#  include "tmmintrin.h"
#  define HAVE_SSE
#endif

#include <string>
#include <sstream>

// Function for debug prints
template <typename T>
std::string __m128i_toString(const __m128i var)
{
    std::stringstream sstr;
    const T* values = (const T*) &var;
    if (sizeof(T) == 1)
    {
        for (unsigned int i = 0; i < sizeof(__m128i); i++)
        {
            sstr << (int) values[i] << " ";
        }
    }
    else
    {
        for (unsigned int i = 0; i < sizeof(__m128i) / sizeof(T); i++)
        {
            sstr << values[i] << " ";
        }
    }
    return sstr.str();
}

void ConvertColor_BGR2GRAY_BT709(const cv::Mat& src, cv::Mat& dst)
{
    CV_Assert(CV_8UC3 == src.type());
    cv::Size sz = src.size();
    dst.create(sz, CV_8UC1);

    const int bidx = 0;

    for (int y = 0; y < sz.height; y++)
    {
        const cv::Vec3b *psrc = src.ptr<cv::Vec3b>(y);
        uchar *pdst = dst.ptr<uchar>(y);

        for (int x = 0; x < sz.width; x++)
        {
            float color = 0.2126 * psrc[x][2-bidx] + 0.7152 * psrc[x][1] + 0.0722 * psrc[x][bidx];
            pdst[x] = (int)(color + 0.5);
        }
    }
}

void ConvertColor_BGR2GRAY_BT709_fpt(const cv::Mat& src, cv::Mat& dst)
{
    CV_Assert(CV_8UC3 == src.type());
    cv::Size sz = src.size();
    dst.create(sz, CV_8UC1);

	const ushort n = 8;

 	ushort a = 0.2126f * (1 << n) + 0.5f;
 	ushort b = 0.7152f * (1 << n) + 0.5f;
 	ushort c = 0.0722f * (1 << n) + 0.5f;
 	ushort zero_five = 1 << (n - 1);
  
	for (int y = 0; y < sz.height; y++)
	{
		const cv::Vec3b *psrc = src.ptr<cv::Vec3b>(y);
		uchar *pdst = dst.ptr<uchar>(y);  

        for (int x = 0; x < sz.width; x++)
        {
			pdst[x] = (a * psrc[x][2] + b * psrc[x][1] + c * psrc[x][0] + zero_five) >> n;
        }
	}
}

void ConvertColor_BGR2GRAY_BT709_simd(const cv::Mat& src, cv::Mat& dst)
{
    CV_Assert(CV_8UC3 == src.type());
    cv::Size sz = src.size();
    dst.create(sz, CV_8UC1);

#ifdef HAVE_SSE

	__m128i ssse3_red_indices_0   = _mm_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 14, 11,  8,  5,  2);
    __m128i ssse3_red_indices_1   = _mm_set_epi8(-1, -1, -1, -1, -1, -1, 13, 10,  7,  4,  1, -1, -1, -1, -1, -1);
    __m128i ssse3_red_indices_2   = _mm_set_epi8(15, 12,  9,  6,  3,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1);
    __m128i ssse3_green_indices_0 = _mm_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 13, 10,  7,  4,  1);
    __m128i ssse3_green_indices_1 = _mm_set_epi8(-1, -1, -1, -1, -1, 15, 12,  9,  6,  3,  0, -1, -1, -1, -1, -1);
    __m128i ssse3_green_indices_2 = _mm_set_epi8(14, 11,  8,  5,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1);
	__m128i ssse3_blue_indices_0  = _mm_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 15, 12,  9,  6,  3,  0);
    __m128i ssse3_blue_indices_1  = _mm_set_epi8(-1, -1, -1, -1, -1, 14, 11,  8,  5,  2, -1, -1, -1, -1, -1, -1);
    __m128i ssse3_blue_indices_2  = _mm_set_epi8(13, 10,  7,  4,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1);

    __m128i red_coeff   = _mm_set1_epi16(54);
    __m128i green_coeff = _mm_set1_epi16(183);
    __m128i blue_coeff  = _mm_set1_epi16(19); // 19 instead of 18 because the sum of 3 coeffs must be == 256

    __m128i bias = _mm_set1_epi16(128);
    __m128i zero = _mm_setzero_si128();
#endif

	const ushort n = 8;

 	ushort r_c = 0.2126f * (1 << n) + 0.5f;
 	ushort g_c = 0.7152f * (1 << n) + 0.5f;
 	ushort b_c = 0.0722f * (1 << n) + 0.5f;

    for (int y = 0; y < sz.height; y++)
    {
        const uchar *psrc = src.ptr<uchar>(y);
        uchar *pdst = dst.ptr<uchar>(y);

        int x = 0;

#ifdef HAVE_SSE
        // Here is 16 times unrolled loop for vector processing
        for (; x <= sz.width - 16; x += 16)
        {

            __m128i chunk0 = _mm_loadu_si128((const __m128i*)(psrc + x * 3 + 16 * 0));
            __m128i chunk1 = _mm_loadu_si128((const __m128i*)(psrc + x * 3 + 16 * 1));
            __m128i chunk2 = _mm_loadu_si128((const __m128i*)(psrc + x * 3 + 16 * 2));

            __m128i red = _mm_or_si128(_mm_or_si128(_mm_shuffle_epi8(chunk0, ssse3_red_indices_0),
                                                    _mm_shuffle_epi8(chunk1, ssse3_red_indices_1)),
                                                    _mm_shuffle_epi8(chunk2, ssse3_red_indices_2));

			__m128i green = _mm_or_si128(_mm_or_si128(_mm_shuffle_epi8(chunk0, ssse3_green_indices_0),
                                                    _mm_shuffle_epi8(chunk1, ssse3_green_indices_1)),
                                                    _mm_shuffle_epi8(chunk2, ssse3_green_indices_2));

			__m128i blue = _mm_or_si128(_mm_or_si128(_mm_shuffle_epi8(chunk0, ssse3_blue_indices_0),
                                                    _mm_shuffle_epi8(chunk1, ssse3_blue_indices_1)),
                                                    _mm_shuffle_epi8(chunk2, ssse3_blue_indices_2));

			__m128i red_16bit_lo = _mm_unpacklo_epi8(red, zero);
			__m128i green_16bit_lo = _mm_unpacklo_epi8(green, zero);
			__m128i blue_16bit_lo = _mm_unpacklo_epi8(blue, zero);
			__m128i red_16bit_hi= _mm_unpackhi_epi8(red, zero);
			__m128i green_16bit_hi = _mm_unpackhi_epi8(green, zero);
			__m128i blue_16bit_hi = _mm_unpackhi_epi8(blue, zero);

			__m128i color_lo = _mm_add_epi16(_mm_mullo_epi16(red_coeff, red_16bit_lo), _mm_mullo_epi16(green_coeff, green_16bit_lo));
			color_lo = _mm_add_epi16(color_lo, _mm_mullo_epi16(blue_coeff, blue_16bit_lo));
			color_lo = _mm_add_epi16(color_lo, bias);

			color_lo = _mm_srli_epi16(color_lo, 8);

			__m128i color_hi = _mm_add_epi16(_mm_mullo_epi16(red_coeff, red_16bit_hi), _mm_mullo_epi16(green_coeff, green_16bit_hi));
			color_hi = _mm_add_epi16(color_hi, _mm_mullo_epi16(blue_coeff, blue_16bit_hi));
			color_hi = _mm_add_epi16(color_hi, bias);

			color_hi = _mm_srli_epi16(color_hi, 8);

			__m128i gray_packed = _mm_packus_epi16(color_lo, color_hi);

            _mm_storeu_si128((__m128i*)(pdst + x), gray_packed);
        }
#endif
		// Process leftover pixels
		for (int rest = x; rest < sz.width; rest++)
		{
			pdst[x] = (r_c * psrc[x + 2] + g_c * psrc[x + 1] + b_c * psrc[x] + 1 << (n - 1)) >> n;
		}
	}
 }
