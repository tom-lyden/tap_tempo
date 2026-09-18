//
// Created by tomly on 17/09/2026.
//

#ifndef TAP_TEMPO_MATH_UTILS_H
#define TAP_TEMPO_MATH_UTILS_H

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(val, min, max) MIN(MAX(min, val), max)

#define PERCENTAGE(val, percent) ((val) * (percent) / 100)

#endif //TAP_TEMPO_MATH_UTILS_H
