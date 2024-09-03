#ifndef _COMMON_UTILS_H_
#define _COMMON_UTILS_H_

#include <cstddef>

void mat4f_load_ortho1(float left, float right, float bottom, float top, float near, float far, float* mat4f);
void mat4f_load_rotation_z1(float rotation, float* mat4f);
void mat4f_load_scale1(float scaleX, float scaleY, float scaleZ, float* mat4f);
float aspect_ratio_correction1(bool fillScreen, size_t backingWidth, size_t backingHeight, size_t width, size_t height);

#endif //_COMMON_UTILS_H_
