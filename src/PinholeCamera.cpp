#include "PinholeCamera.h"

PinholeCamera::PinholeCamera(Vector3d p_position, Vector3d p_target, Vector3d p_up, float p_fov, int p_width, int p_height): Camera(p_width, p_height, p_position) {
	const float PI = 3.14159265358979323846f;
    Vector3d posToTarget(p_position);
    posToTarget = posToTarget - p_target;
	onb.initFromWV(posToTarget, p_up);
    au = tan(((p_fov * PI) / 180.0f) / 2.0f);
    av = (p_height * au) / p_width;
}

Ray PinholeCamera::castRay(float x, float y){
    float du = -au + ((2.0f * au * x) / (width - 1.0f));
    float dv = -av + ((2.0f * av * y) / (height - 1.0f));
	Vector3d vec(du, dv, -1);
	Vector3d transfvec = onb.transform(vec);
	transfvec.normalize();
	Ray ray(position, transfvec);
    return ray;
}