#pragma once

#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "REX/REX.h"

#define W32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include "ClibUtil/utils.hpp"
#undef ERROR

#include <expected>
#include <unordered_set>
#include <fstream>
#include <spdlog/sinks/basic_file_sink.h>
#include <json/json.h>

#include "Plugin.h"

#define DLLEXPORT __declspec(dllexport)

#ifndef NDEBUG
#define LOG_DEBUG(msg, ...) REX::DEBUG(msg, ##__VA_ARGS__)
#else
#define LOG_DEBUG(msg, ...)
#endif

using namespace std::literals;
namespace util
{
    [[nodiscard]] constexpr int ascii_tolower(int ch) noexcept
    {
        if (ch >= 'A' && ch <= 'Z')
            ch += 'a' - 'A';
        return ch;
    }

    struct iless
    {
        using is_transparent = int;

        template <std::ranges::contiguous_range S1, std::ranges::contiguous_range S2>
            requires(
        std::is_same_v<std::ranges::range_value_t<S1>, char>&&
            std::is_same_v<std::ranges::range_value_t<S2>, char>)
            constexpr bool operator()(S1&& a_str1, S2&& a_str2) const
        {
            std::size_t count = std::ranges::size(a_str2);
            const std::size_t len1 = std::ranges::size(a_str1);
            const bool shorter = len1 < count;
            if (shorter)
                count = len1;

            if (count) {
                const char* p1 = std::ranges::data(a_str1);
                const char* p2 = std::ranges::data(a_str2);

                do {
                    const int ch1 = ascii_tolower(*p1++);
                    const int ch2 = ascii_tolower(*p2++);
                    if (ch1 != ch2)
                        return ch1 < ch2;
                } while (--count);
            }

            return shorter;
        }
    };

    template <class T>
    using istring_map = std::map<std::string, T, iless>;
}

namespace stl {
    template <typename R, typename... Args>
    inline std::uintptr_t function_ptr(R(*fn)(Args...))
    {
        return reinterpret_cast<std::uintptr_t>(fn);
    }

    template <typename Class, typename R, typename... Args>
    inline std::uintptr_t function_ptr(R(Class::* fn)(Args...))
    {
        return reinterpret_cast<std::uintptr_t>((void*&)fn);
    }
}

// Used as a compile guard in certain templated function (see INISettings.h, if present)
template <class T>
inline constexpr bool always_false = false;

#define SECTION_SEPARATOR REX::INFO("=========================================================="sv)
#define _loggerDebug REX::DEBUG
#define _loggerInfo REX::INFO
#define _loggerError REX::ERROR
#define _debugEDID clib_util::editorID::get_editorID

#ifdef SKYRIM_AE
#	define OFFSET(se, ae) ae
#	define OFFSET_3(se, ae, vr) ae
#else
#	define OFFSET(se, ae) se
#	define OFFSET_3(se, ae, vr) se
#endif