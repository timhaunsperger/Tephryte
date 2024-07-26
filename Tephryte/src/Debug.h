//
// Created by Timothy on 7/24/2024.
//

#ifndef DEBUG_H
#define DEBUG_H

#ifndef NDEBUG
#define TPR_DEBUG
#define TPR_USE_ASSERTS
#endif //NDEBUG

#define TPR_EXIT exit(-1);

#define TPR_ENGINE_INFO( ... )  ::Tephryte::Log::log(::Tephryte::Log::MsgLvl::EngineInfo, __VA_ARGS__);
#define TPR_ENGINE_WARN( ... )  ::Tephryte::Log::log(::Tephryte::Log::MsgLvl::EngineWarn, __VA_ARGS__);
#define TPR_ENGINE_ERROR( ... ) ::Tephryte::Log::log(::Tephryte::Log::MsgLvl::EngineError, __VA_ARGS__); TPR_EXIT

#define TPR_INFO( ... )  ::Tephryte::Log::log(::Tephryte::Log::MsgLvl::AppInfo, __VA_ARGS__);
#define TPR_WARN( ... )  ::Tephryte::Log::log(::Tephryte::Log::MsgLvl::AppWarn, __VA_ARGS__);
#define TPR_ERROR( ... ) ::Tephryte::Log::log(::Tephryte::Log::MsgLvl::AppError, __VA_ARGS__); TPR_EXIT

#define TPR_LOG_STREAM ::Tephryte::Log::out

#ifdef TPR_USE_ASSERTS
#define TPR_ENGINE_ASSERT( condition, ... ) if(!(condition)){ TPR_ENGINE_ERROR(__FILE__, ":", __LINE__, ": In function '", __FUNCTION__, "'" \
     __VA_OPT__(, ":", "\n", "Assertion Failed - " ,) __VA_ARGS__) }
#define TPR_ASSERT( condition, ... ) if(!(condition)){ TPR_ERROR(__FILE__, ":", __LINE__, ": In function '", __FUNCTION__, "'" \
     __VA_OPT__(, ":", "\n", "Assertion Failed - " ,) __VA_ARGS__) }
#else
#define TPR_ENGINE_ASSERT( ... )
#define TPR_ASSERT( ... )
#endif //TPR_USE_ASSERTS

#include "Log.h"

#endif //DEBUG_H
