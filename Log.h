//
// Created by timha on 7/10/2024.
//

#ifndef TEPHRYTE_LOG_H
#define TEPHRYTE_LOG_H


#include <iostream>
#include <sstream>

namespace Tephryte::Log{

    int logLevel = 0;
    std::string AppName = "Application";

    enum MsgType{
        EngineInfo,
        AppInfo,
        EngineWarn,
        AppWarn,
        EngineError,
        AppError
    };

    template<typename T>
    void combine(std::ostringstream& log_msg, T token){
        log_msg << token << "\n";
    }

    template<typename T, typename... Msg>
    void combine(std::ostringstream& log_msg, T token, Msg... msg){
        log_msg << token << " ";
        combine(log_msg, msg...);
    }

    template<typename... Msg>
    void log(MsgType type, Msg... msg){
        if (type / 2 < logLevel){
            return;
        }

        std::ostringstream log_msg;

        switch (type) {
            case 0:
                log_msg << "[INFO] TephryteEngine - ";
                break;
            case 1:
                log_msg << "[INFO] " + AppName + " - ";
                break;
            case 2:
                log_msg << "[WARN] TephryteEngine - ";
                break;
            case 3:
                log_msg << "[WARN] " + AppName + " - ";
                break;
            case 4:
                log_msg << "[ERROR] TephryteEngine - ";
                break;
            case 5:
                log_msg << "[ERROR] " + AppName + " - ";
        }

        combine(log_msg, msg...);

        std::cout << log_msg.str();
    }


}

#define TPR_ENGINE_INFO( ... )  ::Tephryte::Log::log(::Tephryte::Log::MsgType::EngineInfo, __VA_ARGS__);
#define TPR_ENGINE_WARN( ... )  ::Tephryte::Log::log(::Tephryte::Log::MsgType::EngineWarn, __VA_ARGS__);
#define TPR_ENGINE_ERROR( ... ) ::Tephryte::Log::log(::Tephryte::Log::MsgType::EngineError, __VA_ARGS__);

#define TPR_INFO( ... )  ::Tephryte::Log::log(::Tephryte::Log::MsgType::AppInfo, __VA_ARGS__);
#define TPR_WARN( ... )  ::Tephryte::Log::log(::Tephryte::Log::MsgType::AppWarn, __VA_ARGS__);
#define TPR_ERROR( ... ) ::Tephryte::Log::log(::Tephryte::Log::MsgType::AppError, __VA_ARGS__);

#endif //TEPHRYTE_LOG_H
