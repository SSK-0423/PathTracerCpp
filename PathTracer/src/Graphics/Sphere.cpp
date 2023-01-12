#include "Sphere.hpp"
#include "Ray.hpp"
#include "IntersectionResult.hpp"
#include "../Utility/PathTracerMath.hpp"
#include <float.h>

namespace PathTracer {
	const IntersectionResult& Sphere::Intersect(const Ray& ray)
	{
		// ���C�̃x�N�g�������� p = s + td;
		// ���̃x�N�g�������� |p|^2 = r^2;
		// |d|^2
		float A = Dot(ray.GetDirection(), ray.GetDirection());

		// (s - Pc)
		Vector3 raySphereSub = ray.GetOrigin() - m_origin;
		// 2{d�E(s - Pc)}
		float B = 2 * Dot(ray.GetDirection(), raySphereSub);
		// |s - Pc|^2 - r^2
		float C = Dot(raySphereSub, raySphereSub) - powf(m_radius, 2.f);

		float D = B * B - 4 * A * C;
		// ��_2��
		if (D > 0) {
			float t1 = (-B + sqrtf(D)) / 2.f * A;
			float t2 = (-B - sqrtf(D)) / 2.f * A;

			if (t1 >= 0 && t2 >= 0) {
				float t = Min(t1, t2);
				return IntersectionResult(
					ray.GetOrigin() + ray.GetDirection() * t,
					(ray.GetDirection() * t).Length(),
					m_objectID, INTERSECTION_TYPE::HIT);
			}
		}
		// ��_1��
		else if (D == 0) {
			float t = -B / 2.f * A;

			if (t >= 0) {
				return IntersectionResult(
					ray.GetOrigin() + ray.GetDirection() * t,
					(ray.GetDirection() * t).Length(),
					m_objectID, INTERSECTION_TYPE::HIT);
			}
		}

		return IntersectionResult(Vector3(0.f, 0.f, 0.f), FLT_MAX, -1, INTERSECTION_TYPE::NONE);
	}
}