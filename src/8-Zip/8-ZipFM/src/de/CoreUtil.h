#pragma once
#include <de/Core.h>

std::string strip_tmp_zst_tar(std::string uri);

std::string strip_path(std::string uri);

std::string dbStrRemoveEnd(const std::string& src, const std::string& query);

int64_t de_powi(int32_t base, int32_t exponent);
