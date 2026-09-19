//
// Created by tomly on 17/09/2026.
//

#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(val, min, max) MIN(MAX(min, val), max)

#define PERCENTAGE(val, percent) ((val) * (percent) / 100)

#endif // MATH_UTILS_H
