#pragma once
#include <DarkImage.h>

// =======================================================================
struct OdinAssetUtil
// =======================================================================
{
    // I won't use constexpr because of narrowing.
    // Meaning i can't use uint_t but only char since API wants char
    // If i use constexpr i can't write byte values > 127,
    // thanks to effing constexpr narrowing. (Totally unwarranted for me)

    // Declare size and array (Dont use constexpr because narrowing)
    static std::string
    bin2hpp( std::string dataName )
    {
        if (dataName.empty())
        {
            DE_WARN("Got empty data name")
            return {};
        }

        std::ostringstream o;
        o << "extern const size_t " << dataName << "Size;\n";
        o << "extern const uint8_t " << dataName << "[];\n";
        return o.str();
    }

    // Define size and array in .cpp (Dont use constexpr because narrowing)
    static std::string
    bin2cpp( const std::vector<uint8_t>& bytes, std::string dataName )
    {
        if (bytes.empty())
        {
            DE_WARN("Got empty data")
            return {};
        }

        if (dataName.empty())
        {
            DE_WARN("Got empty data name")
            return {};
        }

        std::ostringstream o;
        o << "const size_t " << dataName << "Size = " << bytes.size() << ";\n";
        o << "const uint8_t " << dataName << "[] = \n"  //  << "[" << bytes.size() << "]
        "{\n\t";

        uint64_t k = 0;
        for ( size_t i = 0; i < bytes.size(); ++i )
        {
            o << "0x" << de::StringUtil::hex( bytes[ i ] );
            if ( bytes.size() > 1 && (i + 1 < bytes.size()) )
            {
                o << ", ";
            }

            // Better formatting
            ++k;
            if ( k >= 16 )
            {
                k -= 16;
                o << "\n\t";
            }
        }
        o << "};\n";

        return o.str();
    }

    static void convertFiles(
        const std::string& outputFileNameH,
        const std::string& inputDirectory, bool bRecursive = false)
    {

        std::string outputFileNameDir = de::FileSystem::fileDir(outputFileNameH);
        std::string outputFileNameCPP = outputFileNameDir + "/" + de::FileSystem::fileBase(outputFileNameH) + ".cpp";

        DE_DEBUG("outputFileNameH = ",outputFileNameH)
        DE_DEBUG("outputFileNameCPP = ",outputFileNameCPP)

        std::vector<std::string> inputFileNames = de::FileSystem::entries(
            inputDirectory, bRecursive, true, false);

        struct Bin
        {
            std::string origUri;
            std::string binName;
            uint64_t binSize;
        };

        std::vector<Bin> bins;

        // <CREATE.cpp>
        std::ostringstream o;
        o << "// (c) 2025 by Benjamin Hampe <benjaminhampe@gmx.de>\n";
        o << "// This file contains the binary data (images/fonts/etc..) for Odin2.\n";
        o << "#include <cstdint>\n";
        o << "\n";
        o << "namespace BinaryData {\n";
        o << "\n";

        std::vector<uint8_t> blob;
        for (const auto& inputFileName : inputFileNames)
        {
            bins.emplace_back();
            auto & bin = bins.back();
            bin.origUri = inputFileName;
            bin.binName = de::FileSystem::fileName( inputFileName );

            // If starts with number [0-9] -> Prefix with '_'
            if (bin.binName[0] >= '0' && bin.binName[0] <= '9')
                bin.binName = std::string("_") + bin.binName;

            // Replace:
            bin.binName = de::StringUtil::replace(bin.binName, " ", "_");
            bin.binName = de::StringUtil::replace(bin.binName, ".", "_");
            bin.binName = de::StringUtil::replace(bin.binName, ",", "");
            bin.binName = de::StringUtil::replace(bin.binName, "&", "");
            bin.binName = de::StringUtil::replace(bin.binName, "-", "");
            bin.binName = de::StringUtil::replace(bin.binName, "(", "");
            bin.binName = de::StringUtil::replace(bin.binName, ")", "");
            bin.binName = de::StringUtil::replace(bin.binName, "[", "");
            bin.binName = de::StringUtil::replace(bin.binName, "]", "");
            // bin.binName = de::StringUtil::replace(bin.binName, "____", "_");
            // bin.binName = de::StringUtil::replace(bin.binName, "___", "_");
            // bin.binName = de::StringUtil::replace(bin.binName, "__", "_");
            de::FileSystem::loadBin(inputFileName, blob);
            bin.binSize = blob.size();

            auto cppText = bin2cpp(blob, bin.binName);
            o << cppText << "\n";
        }

        o << "} // end namespace BinaryData.\n";
        o << "\n";
#if 0
        // <OPTIONAL> DebugLog content table
        o << "\n";
        o << "/*";
        o << "BinNames" << bins.size() << "\n";
        o << "{\n";
        for (const auto& bin : bins)
        {
            o << "BinaryData::" << bin.binName << ",\n";
        }
        o << "}\n";

        o << "BinSizes" << bins.size() << "\n";
        o << "{\n";
        for (const auto& bin : bins)
        {
            o << "BinaryData::" << bin.binName << "_size = " << bin.binSize << ",\n";
        }
        o << "}\n";
        o << "*/\n";
#endif
        // <.cpp>
        de::FileSystem::saveStr(outputFileNameCPP, o.str());

        // <CREATE.h>
        std::ostringstream h;
        h << "// (c) 2025 by Benjamin Hampe <benjaminhampe@gmx.de>\n";
        h << "// This file contains the binary data (images/fonts/etc..) for Odin2.\n";
        h << "#pragma once\n";
        h << "#include <cstdint>\n";
        h << "\n";
        h << "namespace BinaryData {\n";
        h << "\n";

        for (const auto& bin : bins)
        {
            auto hppText = bin2hpp(bin.binName);
            h << hppText << "\n";
        }

        h << "} // end namespace BinaryData.\n";

        // <.h>
        de::FileSystem::saveStr(outputFileNameH, h.str());
    }

};

