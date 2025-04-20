#pragma once

float map(float val, float cl, float cr, float tl, float tr) {
	return tl + (val - cl) / (cr - cl) * (tr - tl);
}