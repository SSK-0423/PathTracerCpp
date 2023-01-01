#include "Renderer.hpp"
#include "IntersectionResult.hpp"
#include "Intersector.hpp"

PathTracer::Renderer::Renderer()
{
}

PathTracer::Renderer::~Renderer()
{
}

void PathTracer::Renderer::Init(unsigned int width, unsigned int height)
{
	m_width = width;
	m_height = height;
	m_renderTarget.Create(width, height);

	m_camera = Camera(Vector3(0, 0, 0), Vector3(0, 0, 1), Vector3(1, 0, 0), Vector3(0, 1, 0), 45.f);
}

void PathTracer::Renderer::Render(const Scene& scene)
{
	// �p�X�g���[�V���O
	for (size_t y = 0; y < m_height; y++) {
		for (size_t x = 0; x < m_width; x++) {
			// �J�������C�𐶐�
			Ray cameraRay = m_camera.GetCameraRay(x, y, m_width, m_height);

			// �V�[���ƌ�������
			IntersectionResult result = m_intersector.Intersect(cameraRay, scene);

			// �V�F�[�f�B���O
			if (result.GetType() == INTERSECTION_TYPE::HIT)
				m_renderTarget.Write(x, y, 0, 1, 0);
			else
				m_renderTarget.Write(x, y, 0, 1, 1);
		}
	}

	// �p�X�g���[�V���O���ʂ��o��
	m_renderTarget.OutputImage("test.ppm");
}
