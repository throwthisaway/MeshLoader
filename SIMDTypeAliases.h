#pragma once
#ifdef PLATFORM_MAC_OS
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"
#endif
// TODO:: #define GLM_DEPTH_ZERO_TO_ONE
// TODO:: #define GLM_LEFT_HANDED
// TODO:: set GLM_CONFIG_CLIP_CONTROL to GLM_CLIP_CONTROL_LH_ZO for d3d GLM_CLIP_CONTROL_RH_ZO for metal???
#include <glm/glm.hpp>
#ifdef PLATFORM_MAC_OS
#pragma clang diagnostic pop
#endif
using vec2_t = glm::vec2;
using vec3_t = glm::vec3;
using vec4_t = glm::vec4;

using mat4x4_t = glm::mat4x4;
