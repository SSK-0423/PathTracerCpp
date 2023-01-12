#include "Plane.hpp"
#include "Ray.hpp"
#include "IntersectionResult.hpp"
#include <float.h>

namespace PathTracer {
	const IntersectionResult& PathTracer::Plane::Intersect(const Ray& ray)
	{
		float DdotN = Dot(ray.GetDirection(), m_normal);
		// 分母0は交点なし
		if (DdotN == 0)
			return IntersectionResult(Vector3(0.f, 0.f, 0.f), FLT_MAX, -1, INTERSECTION_TYPE::NONE);

		float t = Dot(m_position - ray.GetOrigin(), m_normal) / DdotN;

		// 交点あり
		if (t > 0)
		{
			return IntersectionResult(
				ray.GetOrigin() + ray.GetDirection() * t,
				(ray.GetDirection() * t).Length(),
				m_objectID, INTERSECTION_TYPE::HIT);
		}
		// 交点なし
		else
		{
			return IntersectionResult(Vector3(0.f, 0.f, 0.f), FLT_MAX, -1, INTERSECTION_TYPE::NONE);
		}
	}
}
