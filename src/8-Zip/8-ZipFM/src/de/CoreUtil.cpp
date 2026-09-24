#include "CoreUtil.h"

std::string strip_tmp_zst_tar(std::string uri)
{
    DE_DEBUG("[Strip] uri = ",uri)

    if (dbStrEndsWith(uri,".tmp"))
    {
        uri = uri.substr(0,uri.size() - 4);
        DE_DEBUG("[Strip] .tmp ",uri)
    }
    if (dbStrEndsWith(uri,".zst"))
    {
        uri = uri.substr(0,uri.size() - 4);
        DE_DEBUG("[Strip] .zst ",uri)
    }
    if (dbStrEndsWith(uri,".tar"))
    {
        uri = uri.substr(0,uri.size() - 4);
        DE_DEBUG("[Strip] .tar ",uri)
    }
    return uri;
}

std::string dbStrRemoveEnd(const std::string& src, const std::string& query)
{
    if (dbStrEndsWith(src,query))
    {
        return src.substr(0,src.size() - query.size());
    }
    else
    {
        return src;
    }
}

std::string strip_path(std::string uri)
{
    DE_DEBUG("[Strip] uri = ",uri)

    size_t pos = uri.find_last_of('/');
    if (pos == std::string::npos)
    {
        return uri;
    }

    return uri.substr(pos + 1);
}
