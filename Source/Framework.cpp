#include <memory>
#include <sstream>

#include "Graphics/Graphics.h"
#include "Input/Input.h"
#include "Framework.h"
#include "Camera.h"
#include "EffectManager.h"
#include "SceneManager.h"

// ���������Ԋu�ݒ�
static const int syncInterval = 0;

// �ۑ���e
void DrawTaskGUI()
{
	ImGui::SetNextWindowPos(ImVec2(970, 10), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 150), ImGuiCond_Once);

	if (ImGui::Begin("UNIT.01", nullptr, ImGuiWindowFlags_None))
	{
		bool check = true;
		ImGui::Checkbox(u8"�Ȃɂ��\������", &check);
		ImGui::Checkbox(u8"�}�E�X����ŃJ��������", &check);
	}
	ImGui::End();
}

// �R���X�g���N�^
Framework::Framework(HWND hWnd)
	: hWnd(hWnd)
	, input(hWnd)
	, graphics(hWnd)
{

	EffectManager::Instance().Initialize();
	SceneManager::Instance().ChangeScene(new SceneTitle);

	// �V�[��������
	//scene	= new SceneTitle();
	//scene->Initialize();
}

// �f�X�g���N�^
Framework::~Framework()
{
	SceneManager::Instance().Clear();
	EffectManager::Instance().Finalize();
}

// �X�V����
void Framework::Update(float elapsedTime/*Elapsed seconds from last frame*/)
{
	// ���͍X�V����
	input.Update();

	// �J�����R���g���[���[�X�V����
	cameraController.Update(elapsedTime);

	// �V�[���X�V����
	//scene->Update(elapsedTime);
	SceneManager::Instance().Update(elapsedTime);
}

// �`�揈��
void Framework::Render(float elapsedTime/*Elapsed seconds from last frame*/)
{
	// �ʃX���b�h���Ƀf�o�C�X�R���e�L�X�g���g���Ă����ꍇ��
	// �����A�N�Z�X���Ȃ��悤�ɔr�����䂷��
	std::lock_guard<std::mutex> lock(graphics.GetMutex());

	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	Camera& camera = Camera::Instance();

	// IMGUI�t���[���J�n����
	graphics.GetImGuiRenderer()->NewFrame();

	// �V�[���`�揈��
	//scene->Render();
	SceneManager::Instance().Render();

	// �ۑ���e�`��
	//DrawTaskGUI();

	// IMGUI�f���E�C���h�E�`��iIMGUI�@�\�e�X�g�p�j
	//ImGui::ShowDemoWindow();

	// IMGUI�`��
	graphics.GetImGuiRenderer()->Render(dc);

	// �o�b�N�o�b�t�@�ɕ`�悵�������ʂɕ\������B
	graphics.GetSwapChain()->Present(syncInterval, 0);
}

// �t���[�����[�g�v�Z
void Framework::CalculateFrameStats()
{
	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame.  These stats 
	// are appended to the window caption bar.
	static int frames = 0;
	static float time_tlapsed = 0.0f;

	frames++;

	// Compute averages over one second period.
	if ((timer.TimeStamp() - time_tlapsed) >= 1.0f)
	{
		float fps = static_cast<float>(frames); // fps = frameCnt / 1
		float mspf = 1000.0f / fps;
		std::ostringstream outs;
		outs.precision(6);
		outs << "FPS : " << fps << " / " << "Frame Time : " << mspf << " (ms)";
		SetWindowTextA(hWnd, outs.str().c_str());

		// Reset for next average.
		frames = 0;
		time_tlapsed += 1.0f;
	}
}

// �A�v���P�[�V�������[�v
int Framework::Run()
{
	MSG msg = {};

	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			timer.Tick();
			CalculateFrameStats();

			float elapsedTime = syncInterval == 0
				? timer.TimeInterval()
				: syncInterval / 60.0f
				;
			Update(elapsedTime);
			Render(elapsedTime);
		}
	}
	return static_cast<int>(msg.wParam);
}

// ���b�Z�[�W�n���h��
LRESULT CALLBACK Framework::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (Graphics::Instance().GetImGuiRenderer()->HandleMessage(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc;
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) PostMessage(hWnd, WM_CLOSE, 0, 0);
		break;
	case WM_ENTERSIZEMOVE:
		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
		timer.Stop();
		break;
	case WM_EXITSIZEMOVE:
		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
		timer.Start();
		break;
	case WM_MOUSEWHEEL:
		Input::Instance().GetMouse().SetWheel(GET_WHEEL_DELTA_WPARAM(wParam));
		break;

	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}