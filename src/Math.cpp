#include "math.h"

float map(float val, float cl, float cr, float tl, float tr) {
	return tl + (val - cl) / (cr - cl) * (tr - tl);
}