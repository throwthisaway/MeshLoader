#pragma once
#ifdef PLATFORM_MAC_OS
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"
#endif
#include <glm/glm.hpp>
#ifdef PLATFORM_MAC_OS
#pragma clang diagnostic pop
#endif
using vec2_t = glm::vec2;
using vec3_t = glm::vec3;
using vec4_t = glm::vec4;

using mat4x4_t = glm::mat4x4;
