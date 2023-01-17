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
				Vector3 pos = ray.GetOrigin() + ray.GetDirection() * t;
				Vector3 normal = Normalize(pos - m_origin);
				float distance = (ray.GetDirection() * t).Length();
				return IntersectionResult(pos, normal, distance, m_objectID, INTERSECTION_TYPE::HIT);
			}
		}
		// ��_1��
		else if (D == 0) {
			float t = -B / 2.f * A;

			if (t >= 0) {
				Vector3 pos = ray.GetOrigin() + ray.GetDirection() * t;
				Vector3 normal = Normalize(pos - m_origin);
				float distance = (ray.GetDirection() * t).Length();
				return IntersectionResult(pos, normal, distance, m_objectID, INTERSECTION_TYPE::HIT);
			}
		}

		return IntersectionResult(Vector3(0.f, 0.f, 0.f), Vector3(0.f, 0.f, 0.f), FLT_MAX, -1, INTERSECTION_TYPE::NONE);
	}
}