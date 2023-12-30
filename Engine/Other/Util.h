#pragma once
#include <string>
#include <filesystem>
#include <typeinfo>
/**
 * @file Util.h
 * @brief QOLが上がるような自作クラスが宣言されたファイル
 */


namespace Util {

    namespace Convert {
        std::string ToString(const std::wstring& wStr);
        std::string ToString(const char* cStr);
        std::wstring ToWString(const std::string& str);
        std::wstring ToWString(const char* cStr);

        // クラス名をstringで取得
        template<class T> inline std::string NameOf(bool removePrefix = false) {
            std::string str{ typeid(T).name() };

            if (removePrefix) {
                if (str.size() < 7)
                    return str;

                str[0] == 'c' ? str.erase(0, 6) : str.erase(0, 7);
                return str;
            }

            return str;
        }

        template<class T, size_t N> std::array<T, N> ToArray(const std::vector<T>& vec) {
            return std::array<T, vec.size()>{vec.begin(), vec.end()};
        }

        template<class T, size_t N> std::vector<T> ToVector(const std::array<T, N>& arr) {
            return std::vector<T>{arr.begin(), arr.end()};
        }
    }

    namespace Log {
        void PrintOutputWindow(const std::string& str);
        void PrintExternalText(const std::string& str, const std::filesystem::path& logTextPath = std::filesystem::current_path());
    }
}
