#include "Intersector.hpp"
#include "IntersectionResult.hpp"
#include "Ray.hpp"
#include "Sphere.hpp"
#include "../Scene/Scene.hpp"
#include "../Utility/PathTracerMath.hpp"

PathTracer::Intersector::Intersector()
{
}
PathTracer::Intersector::~Intersector()
{
}

PathTracer::IntersectionResult PathTracer::Intersector::Intersect(const Ray& ray, const Scene& scene)
{
	return IntersectSphere(ray, scene);
}

PathTracer::IntersectionResult PathTracer::Intersector::IntersectSphere(const Ray& ray, const Scene& scene)
{
	float minDistance = FLT_MAX;

	// ���C�̃x�N�g�������� p = s + td;
	// ���̃x�N�g�������� |p|^2 = r^2;
	// |d|^2
	float A = Dot(ray.GetDirection(), ray.GetDirection());

	// ���̎擾
	for (auto sphere : scene.GetSpheres()) {
		Vector3 raySphereSub = ray.GetOrigin() - sphere->GetOrigin();
		// 2{d�E(s - Pc)}
		float B = 2 * Dot(ray.GetDirection(), raySphereSub);
		// |s - Pc|^2 - r^2
		float C = Dot(raySphereSub, raySphereSub) - powf(sphere->GetRadius(), 2.f);

		float D = B * B - 4 * A * C;
		// ��_2��
		if (D > 0) {
			float t1 = (-B + sqrtf(D)) / 2.f * A;
			float t2 = (-B - sqrtf(D)) / 2.f * A;

			// 
			if (t1 >= 0 && t2 >= 0) {
				float t = Min(t1, t2);
				return IntersectionResult(
					ray.GetOrigin() + ray.GetDirection() * t,
					(ray.GetDirection() * t).Length(),
					sphere->GetObjectID(), INTERSECTION_TYPE::HIT);
			}
		}
		// ��_1��
		else if (D == 0) {
			float t = -B / 2.f * A;

			if (t >= 0) {
				return IntersectionResult(
					ray.GetOrigin() + ray.GetDirection() * t,
					(ray.GetDirection() * t).Length(),
					sphere->GetObjectID(), INTERSECTION_TYPE::HIT);
			}
		}
	}

	return IntersectionResult(Vector3(0.f, 0.f, 0.f), FLT_MAX, -1, INTERSECTION_TYPE::NONE);
}