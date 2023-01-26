#include "Intersector.hpp"
#include "IntersectionResult.hpp"
#include "Ray.hpp"
#include "Sphere.hpp"
#include "../Scene/Scene.hpp"

namespace PathTracer {
	Intersector::Intersector()
	{
	}
	Intersector::~Intersector()
	{
	}

	IntersectionResult Intersector::IntersectTriangles(const Ray& ray, const Scene& scene)
	{
		float minDistance = FLT_MAX;
		IntersectionResult intersectResult = IntersectionResult();

		for (auto mesh : scene.GetMeshes()) {
			Matrix4x4 modelMatrix = mesh->GetTransform().GetModelMatrix();

			const std::vector<Vector3>& meshVertices = mesh->GetVertices();
			std::vector<Vector3> transformedVertices(mesh->GetVertices().size());
			// ���W�ϊ�
			for (size_t i = 0; i < meshVertices.size(); i++) {
				Vector4 pos = Vector4(meshVertices[i], 1.f);
				transformedVertices[i] = (modelMatrix * pos).xyz();
			}

			const std::vector<unsigned int>& indices = mesh->GetIndices();
			const std::vector<Vector3>& normals = mesh->GetNormals();

			// ���b�V���̑S�|���S���ɑ΂��Č�������
			for (size_t i = 0; i < mesh->GetIndices().size(); i += 3) {
				// �O�p�`�|���S���̒��_�����o��
				Vector3 v0 = transformedVertices[indices[i]];
				Vector3 v1 = transformedVertices[indices[i + 1]];
				Vector3 v2 = transformedVertices[indices[i + 2]];

				float t = 0.f;
				// �o���Z���g���b�N���W(u,v)
				float u = 0.f;
				float v = 0.f;
				INTERSECTION_TYPE type = IntersectTriangle(ray, v0, v1, v2, t, u, v);

				if (type == INTERSECTION_TYPE::NONE) continue;

				float distance = (t * ray.GetDirection()).Length();
				if (distance < minDistance) {
					Vector3 pos = ray.GetOrigin() + t * ray.GetDirection();

					// �o���Z���g���b�N���W��p���Č����_�ɂ�����@�����Z�o����
					Vector3 n0 = normals[indices[i]];
					Vector3 n1 = normals[indices[i + 1]];
					Vector3 n2 = normals[indices[i + 2]];
					Vector3 normal = Normalize(n1 * u + n2 * v + n0 * (1.f - u - v));
					normal = Normalize(modelMatrix * Vector4(normal, 0.f)).xyz();

					minDistance = distance;
					intersectResult = IntersectionResult(pos, normal, distance, mesh->GetObjectID(), type);
				}
			}
		}
		return intersectResult;
	}

	INTERSECTION_TYPE Intersector::IntersectTriangle(const Ray& ray, const Vector3& v0, const Vector3& v1, const Vector3& v2, float& enlarge, float& barycentricU, float& barycentricV)
	{
		// [Moller97]�̎�@
		// �\���ǂ��炩��ł�������������ɂȂ��Ă���

		Vector3 dir = ray.GetDirection();
		Vector3 edge1 = v1 - v0;
		Vector3 edge2 = v2 - v0;
		Vector3 originDir = ray.GetOrigin() - v0;

		Vector3 alpha = Cross(dir, edge2);
		float det = Dot(alpha, edge1);
		// �[�����Z�΍�
		det >= 0 ? det = Max(det, EPSILON) : det = Min(det, -EPSILON);

		Vector3 beta = Cross(originDir, edge1);
		float invDet = 1.f / det;
		// t�����߂�
		float t = invDet * Dot(beta, edge2);
		if (t < 0.f) {
			return INTERSECTION_TYPE::NONE;
		}
		// U�����߂�
		float u = invDet * Dot(alpha, originDir);
		if (u < 0.f || u > 1.f) {
			return INTERSECTION_TYPE::NONE;
		}
		// V�����߂�
		float v = invDet * Dot(beta, dir);
		if (v < 0.f || u + v > 1.f) {
			return INTERSECTION_TYPE::NONE;
		}

		// ��_����
		enlarge = t;
		return INTERSECTION_TYPE::HIT;
	}
}
