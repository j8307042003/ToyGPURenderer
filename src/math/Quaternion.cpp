#include "Quaternion.h"
#include "Matrix4.h"


Matrix4 Quaternion::GetMatrix() {
	return Matrix4(
		{1 - 2 * y * y - 2 * z * z,   2 * x * y + 2 * w * z,   2 * x * z - 2 * w * y, 0.0},
		{2 * x * y - 2 * w * z, 1 - 2 * x * x - 2 * z * z, 2 * y * z + 2 * w * x, 0.0},
		{2 * x * z + 2 * w * y, 2 * y * z - 2 * w * x, 1 - 2 * x * x - 2 * y * y, 0.0},
		{0.0, 0.0, 0.0, 1.0}
	);	
}
