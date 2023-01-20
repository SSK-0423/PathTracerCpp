#include "Renderer.hpp"
#include "IntersectionResult.hpp"
#include "Intersector.hpp"
#include "Mesh.hpp"
#include "BRDF.hpp"
#include "LightSource.hpp"

#include "../Utility/PathTracerMath.hpp"

#include <iostream>
#include "../Scene/Scene.hpp"

constexpr unsigned int MAX_BOUNCE = 3;

PathTracer::Renderer::Renderer()
{
	float val[4][4] = { {1,2,3,4},{5,6,7,8},{9,10,11,12},{13,14,15,16} };

	Matrix4x4 mat;
	mat.Show();

	mat = Matrix4x4::Identity();
	mat.Show();

	mat = Matrix4x4(Vector4(1, 0, 0, 0), Vector4(0, 1, 0, 0), Vector4(0, 0, 1, 0), Vector4(10, 20, 30, 1));
	mat.Show();

	Matrix4x4 mat2 = Matrix4x4(Vector4(1, 0, 0, 0), Vector4(0, 1, 0, 0), Vector4(0, 0, 1, 0), Vector4(10, -20, -30, 1));

	(mat * mat2).Show();

	Vector4 origin = Vector4(-5.f, 5.f, 0.f, 1.f);

	(mat * origin).Show();
}

PathTracer::Renderer::~Renderer()
{
}

void PathTracer::Renderer::Init(const unsigned int& width, const unsigned int& height, const unsigned int& sampleCount)
{
	m_width = width;
	m_height = height;
	m_sampleCount = sampleCount;
	m_renderTarget.Create(width, height);

	m_camera = Camera(Vector3(0, 0, -3.38), Vector3(0, 0, 1), Vector3(1, 0, 0), Vector3(0, 1, 0), 45.f, width, height);
}

void PathTracer::Renderer::Render(const Scene& scene)
{
	// �p�X�g���[�V���O
	for (size_t y = 0; y < m_height; y++) {
		for (size_t x = 0; x < m_width; x++) {
			Vector3 accumulatedRadiance = Vector3(0.f, 0.f, 0.f);
			Ray cameraRay = m_camera.GetCameraRay(x, y, m_width, m_height);

			IntersectionResult result = m_intersector.IntersectTriangles(cameraRay, scene);

			if (result.GetType() == INTERSECTION_TYPE::HIT) {
				m_renderTarget.Write(x, y, 1.f, 1.f, 1.f);
			}
			else {
				m_renderTarget.Write(x, y, 0.f, 1.f, 1.f);
			}

			//for (size_t s = 0; s < m_sampleCount; s++) {
			//	accumulatedRadiance += RayTrace(cameraRay, scene, 0);
			//}
			//accumulatedRadiance = accumulatedRadiance / m_sampleCount;
			//m_renderTarget.Write(x, y, accumulatedRadiance.r(), accumulatedRadiance.g(), accumulatedRadiance.b());
		}
	}

	// �p�X�g���[�V���O���ʂ��o��
	m_renderTarget.OutputImage("CornellBox.ppm");
}

const Vector3 PathTracer::Renderer::RayTrace(const Ray& ray, const Scene& scene, unsigned int bounce)
{
	if (bounce > MAX_BOUNCE) {
		return Vector3(0.f, 0.f, 0.f);
	}

	// �V�[���ƃ��C�̌�������
	IntersectionResult result = m_intersector.Intersect(ray, scene);

	// �V�F�[�f�B���O
	if (result.GetType() == INTERSECTION_TYPE::HIT) {
		// �}�e���A��
		Material material = scene.GetMesh(result.GetObjectID())->GetMaterial();


		// ����������1�_���T���v�����O
		Vector3 surfaceNormal = result.GetNormal();
		Vector3 incidentDirection = SamplePointOnHemisphere(surfaceNormal);
		Vector3 viewDir = Normalize(-1.f * ray.GetDirection());	// �o�˕�����Ray�̋t�x�N�g��
		Vector3 halfVector = Normalize(surfaceNormal + incidentDirection);

		// �V�F�[�f�B���O
		Vector3 diffuseBRDF = DiffuseBRDF::NormalizeLambert(material.GetBaseColor()) * (1.f - material.GetMetallic());
		Vector3 emittion = material.GetEmittedColor();

		// �����Ȃ��^�v�Z�I��
		if (emittion.r() > 0.f || emittion.g() > 0.f || emittion.b() > 0.f) {
			return material.GetEmittedColor();
		}

		// ����������1�_���T���v�����O����
		// �S���̊p��4�΂Ŕ����ɂ��Ă̂ݍl����̂Ŋm�����x�֐�pdf�� 1 / 2��
		// ������BRDF�ɏ]���`�ɂ���Ε��U��ጸ�ł���
		float pdf = 1.f / (2.f * PI);

		// ���V�A�����[���b�g
		std::random_device device;
		std::mt19937_64 mt(device());
		std::uniform_real_distribution<> randamGenerator(0.f, 1.f);
		float random = randamGenerator(mt);


		// ���ˊm���̍ő�l��0.5�Ƃ���
		float albedo = 1.f - material.GetMetallic();
		// �g�U����
		if (random < albedo) {
			// �V�������C�𐶐�
			Ray newRay = Ray(result.GetPosition() + surfaceNormal * EPSILON, incidentDirection);
			// Li(x,��)
			Vector3 incidentLight = diffuseBRDF * RayTrace(newRay, scene, ++bounce);
			// Lo = Le(x,��') + Li(x,��) * BRDF * cos�� / pdf
			return emittion + incidentLight * Saturate(Dot(surfaceNormal, incidentDirection)) / pdf * (1.f / albedo);
		}
		// ���ʔ���
		else if (random >= albedo && random < albedo + material.GetMetallic()) {
			// �V�������C�𐶐�
			// TODO: ���ʔ���BRDF�Ɋ�Â����d�_�I�T���v�����O
			// ���͂Ƃ肠�������S���ʔ��˂݂̂��l����
			Vector3 reflectDir = Normalize(2.f * Dot(viewDir, surfaceNormal) * surfaceNormal - viewDir);
			Ray newRay = Ray(result.GetPosition() + surfaceNormal * EPSILON, reflectDir);
			// Li(x,��)
			Vector3 incidentLight = RayTrace(newRay, scene, ++bounce);
			// Lo = Le(x,��') + Li(x,��) * BRDF * cos�� / pdf
			return emittion + incidentLight * Saturate(Dot(surfaceNormal, incidentDirection)) / pdf * (1.f / material.GetMetallic());
		}
		else {
			return emittion;
		}
	}

	return Vector3(0.f, 0.f, 0.f);
}

const Vector3 PathTracer::Renderer::SamplePointOnHemisphere(const Vector3& surfaceNormal)
{
	// ��l�����𐶐�
	std::random_device device;
	std::mt19937_64 mt(device());
	std::uniform_real_distribution<> randamGenerator(0.f, 1.f);
	float u1 = randamGenerator(mt);
	float u2 = randamGenerator(mt);

	// Z���𒸓_�����Ƃ��Ĕ�����̂P�_���T���v��
	float phi = 2.f * PI * u1;
	float x = sqrtf(u2) * cosf(phi);
	float y = sqrtf(u2) * sinf(phi);
	float z = sqrtf(1.f - u2);

	// ���̕\�ʏ�̖@���ɍ��킹�Ċ��ϊ�
	// 1. ���̕\�ʏ�̖@�����܂߂������v�Z����
	Vector3 w, u, v;
	w = surfaceNormal;
	// �@��������Y��(0,1,0)�ƕ��s�ȏꍇ�͊��x�N�g���̎Z�o��Y�����g�p�ł��Ȃ����߁A
	// X�����g�p���Ċ��x�N�g�����Z�o����
	// �@����x�l������Ȃ�0�ɋ߂��ꍇ��Y���ƕ��s�ł���Ƃ݂Ȃ�
	if (fabs(w.x) < EPSILON) {
		u = Normalize(Cross(Vector3(1.f, 0.f, 0.f), w));
	}
	else {
		u = Normalize(Cross(Vector3(0.f, 1.f, 0.f), w));
	}
	v = Cross(w, u);

	return Normalize(u * x + v * y + w * z);
}

const bool PathTracer::Renderer::RussianRoulette(const Material& material)
{
	// ��l�����𐶐�
	std::random_device device;
	std::mt19937_64 mt(device());
	std::uniform_real_distribution<> randamGenerator(0.f, 1.f);
	float random = randamGenerator(mt);
	// ���ˊm���̍ő�l��0.5�Ƃ���
	float albedo = Min(1.f - material.GetMetallic(), 0.5f);
	if (random < albedo) {
		return true;
	}
	else {
		return false;
	}
}