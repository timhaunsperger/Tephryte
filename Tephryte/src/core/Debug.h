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
#define TPR_ENGINE_ERROR( ... ) ::Tephryte::Log::log(::Tephryte::Log::MsgLvl::EngineError, __VA_ARGS__, "\n",TPR_TRACE);
#define TPR_ENGINE_CRITICAL( ... ) ::Tephryte::Log::log(::Tephryte::Log::MsgLvl::EngineError, __VA_ARGS__, "\n", TPR_TRACE); TPR_EXIT

#define TPR_INFO( ... )  ::Tephryte::Log::log(::Tephryte::Log::MsgLvl::AppInfo, __VA_ARGS__);
#define TPR_WARN( ... )  ::Tephryte::Log::log(::Tephryte::Log::MsgLvl::AppWarn, __VA_ARGS__);
#define TPR_ERROR( ... ) ::Tephryte::Log::log(::Tephryte::Log::MsgLvl::AppError, __VA_ARGS__, " ", TPR_TRACE);

#define TPR_LOG_STREAM ::Tephryte::Log::out

#define TPR_TRACE "    At: ", __FILE__, ":", __LINE__, ", In function '", __FUNCTION__, "'"

#ifdef TPR_USE_ASSERTS
#define TPR_ENGINE_ASSERT( condition, ... ) if(!(condition)){ TPR_ENGINE_ERROR("Assertion Failed", __VA_OPT__(" - ") __VA_ARGS__, "\n") TPR_EXIT}
#define TPR_ASSERT( condition, ... ) if(!(condition)){ TPR_ENGINE_ERROR("Assertion Failed", __VA_OPT__(" - ") __VA_ARGS__, "\n") TPR_EXIT}
#else
#define TPR_ENGINE_ASSERT( ... )
#define TPR_ASSERT( ... )
#endif //TPR_USE_ASSERTS

#define CHECK_VK_RESULT( err ) if (err < 0) { TPR_ENGINE_ERROR("Vulkan function returned error code ", err, "\n") }\
if (err > 0) { TPR_ENGINE_WARN("Vulkan function returned status code ", err, "\n")}

#include "Log.h"

#endif //DEBUG_H
