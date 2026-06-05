#include <de/image/Image_PPM.h>

#if defined(DE_IMAGE_READER_PPM_ENABLED) || defined(DE_IMAGE_WRITER_PPM_ENABLED)
    // #include <cstdint>
    // #include <cstdlib>
    // #include <cstring>
#endif

#if 0
🎨 All official PAM (P7) TUPLTYPE values

These are the tuple types defined by the Netpbm specification.
1. BLACKANDWHITE
    Depth = 1
    Values: 0 or 1
    Equivalent to PBM (P1/P4)

2. GRAYSCALE
    Depth = 1
    0..MAXVAL
    Equivalent to PGM (P2/P5)

3. GRAYSCALE_ALPHA
    Depth = 2
    Channels: Gray, Alpha
    Equivalent to “LA” in PNG

4. RGB
    Depth = 3
    Channels: R, G, B
    Equivalent to PPM (P3/P6)

5. RGB_ALPHA
    Depth = 4
    Channels: R, G, B, A
    Equivalent to PNG RGBA

6. CMYK
    Depth = 4
    Channels: Cyan, Magenta, Yellow, Black
    Rare but fully supported by PAM

7. CMYK_ALPHA
    Depth = 5
    Channels: C, M, Y, K, A
    Very uncommon, but valid

8. YUV
    Depth = 3
    Channels: Y, U, V
    Used for video‑related workflows

9. YUV_ALPHA
    Depth = 4
    Channels: Y, U, V, A

10. MULTI
    Depth = arbitrary
    Means “generic N‑channel data”
    Used for scientific images, masks, float‑encoded data (scaled to MAXVAL), etc.

🧠 Why PAM supports so many formats

P7 (PAM) was designed as a universal container:
    Arbitrary number of channels
    Arbitrary semantics
    Arbitrary MAXVAL
    Arbitrary tuple type
    Arbitrary metadata lines
    Binary pixel data

PPM/PGM/PBM are just special cases of PAM.
📦 Summary table
    TupleType       Channels	Equivalent
    BLACKANDWHITE	1           PBM
    GRAYSCALE       1           PGM
    GRAYSCALE_ALPHA	2           LA
    RGB             3           PPM
    RGB_ALPHA       4           RGBA
    CMYK            4           CMYK
    CMYK_ALPHA      5           CMYKA
    YUV             3           YUV
    YUV_ALPHA       4           YUVA
    MULTI           N           Arbitrary data

🎯 1. Who needs MAXVAL?

MAXVAL is used by:
    P1/P4 PBM → implicitly 1‑bit (MAXVAL = 1)
    P2/P5 PGM → grayscale, arbitrary bit depth
    P3/P6 PPM → RGB, arbitrary bit depth
    P7 PAM → arbitrary channels, arbitrary bit depth

MAXVAL defines the numeric range of each channel:

    0 .. MAXVAL

Typical values:

    255 → 8‑bit per channel
    65535 → 16‑bit per channel
    1 → 1‑bit (PBM)
    1023 → 10‑bit HDR-ish grayscale
    4095 → 12‑bit
    anything → PAM allows arbitrary depth

MAXVAL is needed because the Netpbm family predates PNG/JPEG and was designed to support scientific images, not just 8‑bit consumer graphics.

🎯 2. Who actually uses MAXVAL?
Image loaders

They must scale values to internal representation:
    If MAXVAL = 255 → direct byte copy
    If MAXVAL = 65535 → read 2 bytes per channel
    If MAXVAL = 1023 → read 2 bytes but scale to 16‑bit or float

Scientific / medical imaging

PAM is used for:
    microscopy
    tomography
    HDR radiance maps
    segmentation masks
    multi‑channel data

These often use 10‑bit, 12‑bit, 14‑bit, 16‑bit.

Old Netpbm tools

    They rely on MAXVAL to know how to scale pixel values.

#endif

namespace de {
namespace image {

// ===========================================================================

#ifdef DE_IMAGE_READER_PPM_ENABLED

// ------------------------------------------------------------
// Helpers
// ------------------------------------------------------------
namespace {

struct PPM_Load
{
    static void
    skip_ws_and_comments(const uint8_t*& p, const uint8_t* end)
    {
        while (p < end)
        {
            if (*p == '#')
            {
                while (p < end && *p != '\n') p++;
            }
            else if (*p==' ' || *p=='\t' || *p=='\r' || *p=='\n')
            {
                p++;
            }
            else
            {
                return;
            }
        }
    }

    static bool
    read_int(const uint8_t*& p, const uint8_t* end, int& out)
    {
        skip_ws_and_comments(p, end);
        if (p >= end) return false;
        if (*p < '0') { DE_ERROR("<0") return false; }
        if (*p > '9') { DE_ERROR(">9") return false; }

        int v = 0;
        while (p < end && *p >= '0' && *p <= '9')
        {
            v = (v * 10) + (*p - '0');
            p++;
        }
        out = v;
        return true;
    }

    // ------------------------------------------------------------
    // Parse P3 (ASCII PPM)
    // ------------------------------------------------------------
    static bool
    parse_p3(Image& img, const uint8_t* data, size_t size)
    {
        const uint8_t* p = data;
        const uint8_t* end = data + size;

        p += 2; // skip "P3"

        int width = 0;
        int height = 0;
        int maxval = 0;

        if (!read_int(p, end, width)) return false;
        if (!read_int(p, end, height)) return false;
        if (!read_int(p, end, maxval)) return false;

        img.setPixelFormat(PixelFormat::R8G8B8);
        img.resize(width,height);

        uint8_t* __restrict__ pDst = img.data();

        const size_t nBytes = (size_t)width * (uint32_t)height * 3u;

        for (size_t i = 0; i < nBytes; ++i)
        {
            int v;
            if (!read_int(p, end, v)) return false;
            pDst[i] = (uint8_t)v;
        }
        return true;
    }

    // ------------------------------------------------------------
    // Parse P6 (Binary PPM)
    // ------------------------------------------------------------
    static bool
    parse_p6(Image& img, const uint8_t* data, size_t size)
    {
        const uint8_t* p = data;
        const uint8_t* end = data + size;

        p += 2; // skip "P6"

        int width = 0;
        int height = 0;
        int maxval = 0;

        if (!read_int(p, end, width)) return false;
        if (!read_int(p, end, height)) return false;
        if (!read_int(p, end, maxval)) return false;

        // One whitespace after maxval
        if (p >= end)
            return false;
        if (*p==' ' || *p=='\n' || *p=='\r' || *p=='\t')
            p++;
        else
            return false;

        const size_t nBytes = (size_t)width * (uint32_t)height * 3u;
        if ((size_t)(end - p) < nBytes) return false;

        img.setPixelFormat(PixelFormat::R8G8B8);
        img.resize(width,height);

        const uint8_t* __restrict__ pSrc = p;
              uint8_t* __restrict__ pDst = img.data();

        DE_ASSUME_NO_OVERLAP(pSrc,pDst,nBytes);

        std::memcpy(pDst, pSrc, nBytes);
        return true;
    }

    // ------------------------------------------------------------
    // Parse P7 (PAM)
    // ------------------------------------------------------------
    static bool
    parse_p7(Image& img, const uint8_t* data, size_t size)
    {
        const uint8_t* p = data;
        const uint8_t* end = data + size;

        p += 2; // skip "P7"

        bool gotW=false, gotH=false, gotD=false, gotM=false;

        int width = 0;
        int height = 0;
        int maxval = 0;
        int depth = 0;

        while (p < end)
        {
            skip_ws_and_comments(p, end);

            if (p >= end)
                return false;

            if (memcmp(p, "ENDHDR", 6) == 0)
            {
                p += 6;
                break;
            }
            if (memcmp(p, "WIDTH", 5) == 0)
            {
                p += 5;
                if (!read_int(p, end, width)) return false;
                gotW = true;
            }
            else if (memcmp(p, "HEIGHT", 6) == 0)
            {
                p += 6;
                if (!read_int(p, end, height)) return false;
                gotH = true;
            }
            else if (memcmp(p, "DEPTH", 5) == 0)
            {
                p += 5;
                if (!read_int(p, end, depth)) return false;
                gotD = true;
            }
            else if (memcmp(p, "MAXVAL", 6) == 0)
            {
                p += 6;
                if (!read_int(p, end, maxval)) return false;
                gotM = true;
            }
            else if (memcmp(p, "TUPLTYPE", 8) == 0)
            {
                // skip line
                while (p < end && *p != '\n') p++;
            }
            else
            {
                // unknown field → skip line
                while (p < end && *p != '\n') p++;
            }
        }

        if (!(gotW && gotH && gotD && gotM))
        {
            DE_ERROR("malformed P7")
            return false;
        }

        if (depth == 3)
        {
            img.setPixelFormat(PixelFormat::R8G8B8);
        }
        else if (depth == 4)
        {
            img.setPixelFormat(PixelFormat::R8G8B8A8);
        }
        else
        {
            DE_ERROR("Unsupported depth ",depth)
            return false;
        }

        img.resize(width,height);

        const size_t nBytes = (size_t)width * (uint32_t)height * (uint32_t)depth;
        if ((size_t)(end - p) < nBytes)
        {
            DE_ERROR("Not enough to read")
            return false;
        }

        const uint8_t* __restrict__ pSrc = p;
              uint8_t* __restrict__ pDst = img.data();

        DE_ASSUME_NO_OVERLAP(pSrc,pDst,nBytes);

        std::memcpy(pDst, pSrc, nBytes);

        return true;
    }
};

} // end namespace.

bool
ImageReaderPPM::load( Image & img, const uint8_t* data, size_t size, const std::string& uri )
{
    if (size < 2) return false;

    if (data[0]=='P' && data[1]=='3')
        return PPM_Load::parse_p3(img, data, size);

    if (data[0]=='P' && data[1]=='6')
        return PPM_Load::parse_p6(img, data, size);

    if (data[0]=='P' && data[1]=='7') // PAM (RGBA, RGBA_ALPHA, GRAYSCALE_ALPHA, etc.)
        return PPM_Load::parse_p7(img, data, size);

    return false;
}

#endif // DE_IMAGE_READER_PPM_ENABLED

// ===========================================================================

#ifdef DE_IMAGE_WRITER_PPM_ENABLED

namespace {

struct PPM_Save
{
    // ASCII pixel data
    static bool
    write_ppm_p3(const Image& img, const std::string& uri)
    {
        File file(uri,eFileMode::Write);
        if (!file.is_open())
        {
            DE_ERROR("File not open for write, ",uri)
            return false;
        }

        // Header
        std::ostringstream o; o <<
        "P3\n"
        "" << img.w() << " " << img.h() << "\n"
        "255\n";

        // Pixels
        const uint32_t bytesPerPixel = img.bytesPerPixel();

        const uint8_t* __restrict__ pixels = img.data();

        for (uint32_t y = 0; y < img.h(); ++y)
        {
            for (uint32_t x = 0; x < img.w(); ++x)
            {
                uint8_t r = pixels[0];
                uint8_t g = pixels[1];
                uint8_t b = pixels[2];
                pixels += bytesPerPixel;
                if (x > 0) o << " ";
                o << int(r) << " " << int(g) << " " << int(b);
            }
            o << "\n";
        }

        const std::string s = o.str();
        const char* __restrict__ src = s.c_str();
        file.write(src,s.size());
        return true;
    }
/*
    static bool write_ppm_p6(const Image& img, const std::string& uri)
    {
        FILE* f = std::fopen(path, "wb");
        if (!f) return false;

        // Header
        std::fprintf(f, "P6\n%d %d\n%d\n", width, height, maxval);

        // Binary pixel data
        size_t total = (size_t)width * height * 3;
        std::fwrite(rgb, 1, total, f);

        std::fclose(f);
        return true;
    }
*/
    static bool
    write_pam(const Image& img, const std::string& uri)
    {
        File file(uri,eFileMode::Write);
        if (!file.is_open())
        {
            DE_ERROR("File not open for write, ",uri)
            return false;
        }

        std::ostringstream o; o <<
        "P7\n"
        "WIDTH " << img.w() << "\n"
        "HEIGHT " << img.h() << "\n"
        "DEPTH " << img.bytesPerPixel() << "\n"
        "MAXVAL 255\n";
        if (img.pixelFormat() == PixelFormat::R8G8B8)
        {
            o << "TUPLTYPE RGB\n";
        }
        else if (img.pixelFormat() == PixelFormat::R8G8B8A8)
        {
            o << "TUPLTYPE RGB_ALPHA\n";
        }
        o << "ENDHDR\n";

        std::string s = o.str();
        file.write(s.c_str(),s.size());

        const uint8_t* __restrict__ src = img.data();
        file.write(src,img.size());

        return true;
    }
};

} // end namespace.

bool
ImageWriterPPM::save( Image const & img, std::string const & uri, uint32_t /* quality */ )
{
    if ((img.pixelFormat() != PixelFormat::R8G8B8)
        && (img.pixelFormat() != PixelFormat::R8G8B8A8))
    {
        DE_ERROR("Unsupported image format for PPM/PAM writer. ", img.str())
        return false;
    }

    auto suffix = FileSystem::fileSuffix(uri);

    if (suffix == "pam")
    {
        return PPM_Save::write_pam(img,uri);
    }
    else
    {
        return PPM_Save::write_ppm_p3(img,uri);
    }
}

#endif // DE_IMAGE_WRITER_PPM_ENABLED

// ===========================================================================

} // end namespace image.
} // end namespace de.
