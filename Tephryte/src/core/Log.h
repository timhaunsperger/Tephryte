//
// Created by timha on 7/10/2024.
//

#ifndef TEPHRYTE_LOG_H
#define TEPHRYTE_LOG_H

#include <iostream>
#include <sstream>
#include <glm/vec2.hpp>

#include "Math.h"

namespace Tephryte::Log{

    inline int logLevel = 0;
    inline std::string appName = "Application";
    inline std::ostream& out = std::cout;

    static constexpr char separator = ',';

    template<typename T>
    struct is_loggable : std::false_type {};

    template<glm::length_t L, typename T, glm::qualifier Q>
    struct is_loggable<glm::vec<L, T, Q>> : std::true_type {};

    template<typename T, glm::qualifier Q>
    struct is_loggable<glm::qua<T, Q>> : std::true_type {};

    template<glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
    struct is_loggable<glm::mat<C, R, T, Q>> : std::true_type {};

    template<typename T>
    concept Streamable = requires(T val, std::stringstream stream)
    {
        stream << val;
    };

    template<typename T>
    concept Iterable = !Streamable<T> && requires(T iterable)
    {
        std::begin(iterable) != std::end(iterable); // Has separate beginning and ending iterators
        ++std::declval<decltype(std::begin(iterable))&>();// Iterator can be incremented
        std::size(iterable); // Has defined size
    };

    // Format and append indexable types
    template<typename T>
    void combineIndexable(std::ostringstream& stream, const T& indexable, const int len)
    {
        stream << "(";
        stream << indexable[0];
        for (int i = 1; i < len; ++i) {
            stream << separator << indexable[i];
        }
        stream << ")";
    }

    // Append type name
    template<typename T>
    void combine(std::ostringstream& log_msg, const T&){
        log_msg << typeid(T).name();
    }

    // Append streamable token
    template<Streamable T>
    void combine(std::ostringstream& log_msg, const T& token){
        log_msg << token;
    }

    // Append glm vector
    template<glm::length_t L, typename T, glm::qualifier Q>
    void combine(std::ostringstream& log_msg, const glm::vec<L, T, Q> vec) {
        combineIndexable(log_msg, vec, L);
    }

    // Append glm quaternion
    template<glm::length_t L, typename T, glm::qualifier Q>
    void combine(std::ostringstream& log_msg, const glm::qua<T, Q> quat) {
        combineIndexable(log_msg, quat, 4);
    }

    // Format and append glm matrix
    template<glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
    void combine(std::ostringstream& log_msg, const glm::mat<C, R, T, Q>& mat) {
        log_msg << "[";
        for (int i = 0; i < R; ++i) {
            log_msg << "(";
            log_msg << mat[0][i];
            for (int j = 1; j < C; ++j) {
                log_msg << separator << mat[j][i];
            }
            log_msg << ")" << separator;
        }
        log_msg.seekp(-1, std::ostringstream::cur);
        log_msg << "]";
    }

    // Format and append an iterable type's contents
    template<Iterable T>
    void combine(std::ostringstream& stream, const T& iterable)
    {
        int len = std::size(iterable);
        stream << "(";
        combine(stream, iterable[0]);
        for (int i = 1; i < len; ++i) {
            stream << separator;
            combine(stream, iterable[i]);
        }
        stream << ")";
    }

    // Recursively call combine function to append all tokens to log stream
    template<typename T, typename... Ts>
    void combine(std::ostringstream& log_msg, const T& token, const Ts&... msg){
        combine(log_msg, token);
        log_msg << " ";
        combine(log_msg, msg...);
    }

    enum MsgLvl{
        EngineInfo,
        AppInfo,
        EngineWarn,
        AppWarn,
        EngineError,
        AppError
    };

    template<typename... Ts>
    void log(const MsgLvl lvl, const Ts&... msg){
        if (lvl / 2 < logLevel){
            return;
        }

        std::ostringstream log_msg;

        switch (lvl) {
            case EngineInfo:
                log_msg << "[TephryteEngine] [INFO] ";
                break;
            case AppInfo:
                log_msg << "[" + appName + "] [INFO] ";
                break;
            case EngineWarn:
                log_msg << "[TephryteEngine] [WARN] ";
                break;
            case AppWarn:
                log_msg << "[" + appName + "] [WARN] ";
                break;
            case EngineError:
                log_msg << "[TephryteEngine] [ERROR] ";
                break;
            case AppError:
                log_msg << "[" + appName + "] [ERROR] ";
        }

        combine(log_msg, msg...);

        out << log_msg.str() << "\n";
    }


}

#endif //TEPHRYTE_LOG_H
