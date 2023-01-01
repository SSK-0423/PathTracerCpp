#include <iostream>
#include "PathTracer.hpp"
#include "Scene/Scene.hpp"
#include "Scene/TestScene.hpp"

int main() {
	PathTracer::PathTracer pathTracer;
	PathTracer::TestScene testScene;
	testScene.Init();

	if (pathTracer.Init(512, 512) == PathTracer::RESULT::FAILED) {
		std::cout << "�p�X�g���[�T�[�̏������Ɏ��s" << std::endl;
	}
	pathTracer.Render(testScene);
	pathTracer.Final();

	return 0;
}