#include <clara.hpp>
#include <iostream>
#include <string>

enum class verbosity_level { low, normal, debug };

int main(int argc, char const * const * argv)
{
    using namespace clara;

    auto source    = std::string{};
    auto dest      = std::string{};
    auto threshold = 0.0;
    int verbosity = int(verbosity_level::low);
    int depth     = 0;
    bool ignore    = false;
    bool showhelp  = false;

    auto parser =
    Opt(threshold, "threshold")["-t"]("Threshold for analysis inclusion") |
    Opt(ignore)["-i"]["--ignore"]("Ignore conflicts") |
    Opt(verbosity, "low|normal|debug")["-v"]["--verbosity"]("The verbosity level") |
    Opt([&depth](int const d) 
        {
            if (d < 0 || d > 10)
            {
                return ParserResult::runtimeError("Depth must be between 1 and 10");
            }
            else
            {
                depth = d;
                return ParserResult::ok(ParseResultType::Matched);
            }
        }, "depth")
        ["-d"]
        ("Depth of analysis (1 to 10)") |
    Arg(source, "source")("The path of the source") |
    Arg(dest, "destination")("The path to the result") |
    Help(showhelp);

    try
    {
        auto result = parser.parse(Args(argc, argv));
        if (!result)
        {
        std::cerr << "Error in command line: " << result.errorMessage() << std::endl;
        return 1;
        }
        else if (showhelp)
        {
        parser.writeToStream(std::cout);
        }
        else
        {
            std::cout << "source:    " << source    << '\n';
            std::cout << "dest:      " << dest      << '\n';
            std::cout << "threshold: " << threshold << '\n';
            std::cout << "ignore:    " << ignore    << '\n';
            std::cout << "verbosity: " << int(verbosity) << '\n';
            std::cout << "dept:      " << depth     << '\n';
            std::cout << std::flush;
        }
    }
    catch (std::exception const & e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
