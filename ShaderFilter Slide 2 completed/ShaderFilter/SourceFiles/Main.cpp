
//-------------------------------------------------------------------------------------
//	�w�b�_�E�t�@�C��
//-------------------------------------------------------------------------------------
#include "pch.h"
#include "DeviceResources.h"
#include "MainResources.h"
#include "SceneRenderer.h"

//--------------------------------------------------------------------------------
//	�V�X�e���E�t���O
//--------------------------------------------------------------------------------
bool	flgFirstRendering = true;		// ���񃌃��_�����O
int		counterCaptureWindow = 0;		// �L���v�`���ΏۃE�B���h�E

//-------------------------------------------------------------------------------------
//	�A�_�v�^�֘A
//-------------------------------------------------------------------------------------
std::vector <Microsoft::WRL::ComPtr<IDXGIOutput1>>	g_pOutput;

//-------------------------------------------------------------------------------------
//	�E�B���h�E�֘A
//-------------------------------------------------------------------------------------
std::vector<HWND>						g_WinHandle;
std::vector<IDXGIOutputDuplication*>	g_pDeskDuplications;

//-------------------------------------------------------------------------------------
//	�O���[�o���ϐ�
//-------------------------------------------------------------------------------------
UINT		g_WindowWidth;
UINT		g_WindowHeight;

//---------------------------------------------------------------
//	�}�E�X�E�^�b�`�C�x���g�֘A
//---------------------------------------------------------------
HWND		g_hWndTouch = NULL;						// �^�b�`�E�C�x���g���N����Window�̃n���h��
HWND		g_hWndMouse = NULL;						// �}�E�X�E�C�x���g���N����Window�̃n���h��

TOUCHINPUT	g_TouchInfo[100];						// �^�b�`���
UINT		g_NumTouch = 0;

POINT		g_CursorPos;							// �}�E�X(�^�b�`)�J�[�\�����W(�N���C�A���g�̈�)
POINT		g_CursorPosOld;

bool		g_flgDown = false;

void		TouchEventProc(HTOUCHINPUT hTouchInput, UINT numInputs);
void		MouseEventProc(UINT uMsg, WPARAM wParam);

void		UIEvent(HWND hWnd, POINT p, bool flgDown, bool flgUp, bool flgMove);

//-------------------------------------------------------------------------------------
//	�O�����\�[�X
//-------------------------------------------------------------------------------------
std::shared_ptr<DeviceResources>	g_deviceResources;			// �f�o�C�X���\�[�X
std::shared_ptr<MainResources>		g_mainResources;			// �A�v���P�[�V�������\�[�X

std::unique_ptr<SceneRenderer>		g_sceneRenderer;			// �V�[�������_���[

//-------------------------------------------------------------------------------------
//	�֐��v���g�^�C�v
//-------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void	Cleanup();
HRESULT CheckAdapter();
//void	GetDuplications();
HRESULT SetupWindow(HINSTANCE hInstance);
HRESULT AppLoop();

void	Render();
void	Update();

void	SetClientSize(HWND hWnd, int width, int height);
void	SetupRenderTarget(int no);

void	GetScreenSize();

//====================================================================================================
// wWinMain	�G���g���[�|�C���g
//====================================================================================================
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{

	//------------------------------------------------------------------------------------
	//	�A�_�v�^(�r�f�I�J�[�h)�̃`�F�b�N
	//------------------------------------------------------------------------------------
	if (FAILED(CheckAdapter())) {
		MessageBox(NULL, L"�A�v���P�[�V�����������I�����܂��B", L"Error", MB_OK);
		return 1;
	}

	//------------------------------------------------------------------------------------
	//	�E�B���h�E�̃Z�b�g�A�b�v�@�� (Output�̐������E�B���h�E���쐬)
	//------------------------------------------------------------------------------------
	if (FAILED(SetupWindow(hInstance))) {
		MessageBox(NULL, L"�E�B���h�E�̃Z�b�g�A�b�v�Ɏ��s���܂����B�A�v���P�[�V�����������I�����܂��B", L"Error", MB_OK | MB_ICONEXCLAMATION);
		Cleanup();
		return 1;
	}

	//------------------------------------------------------------------------------------
	//	�L���v�`����ʂ̌���
	//------------------------------------------------------------------------------------
	GetScreenSize();

	//------------------------------------------------------------------------------------
	//	���\�[�X�̍쐬
	//------------------------------------------------------------------------------------

	// �f�o�C�X���\�[�X�̍쐬
	g_deviceResources = std::make_shared<DeviceResources>();

	// �L���v�`���p���\�[�X�̎擾
//	GetDuplications();

	// �E�B���h�E�o�^ & �E�B���h�E�T�C�Y�Ɉˑ����郊�\�[�X�̍쐬
	for (UINT i = 0; i < g_WinHandle.size(); i++)
		g_deviceResources->RegisterWindow(g_WinHandle[i]);		// �� �E�B���h�E�̐������X���b�v�`�F�[�����쐬

	// �A�v���P�[�V�������\�[�X
	g_mainResources = std::make_shared<MainResources>(g_deviceResources);

	// �V�[�������_���[
	g_sceneRenderer = std::unique_ptr<SceneRenderer>(new SceneRenderer(g_deviceResources, g_mainResources));

	//------------------------------------------------------------------------------------
	//	�A�v���P�[�V�������[�v
	//------------------------------------------------------------------------------------
	AppLoop();

	//------------------------------------------------------------------------------------
	//	���\�[�X���
	//------------------------------------------------------------------------------------
	Cleanup();


	return 0;
}


//====================================================================================================
//	���\�[�X�̉��
//====================================================================================================
void Cleanup()
{

}

//====================================================================================================
//	���b�Z�[�W�E�v���V�[�W��
//====================================================================================================
LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	switch (uMsg) {

		case
			WM_KEYDOWN:
				switch (wParam) {

				case 'Q':		g_mainResources->StartLifeGame();			break;
				case 'R':		g_mainResources->InitLifeGame();			break;
				case 'S':		g_mainResources->SetpLifeGame();			break;

				case '0':		g_mainResources->InitLifeGame(0);			break;
				case '1':		g_mainResources->InitLifeGame(1);			break;

		//		case VK_UP:		g_mainResources->RateFloat(1.01f);			break;
		//		case VK_DOWN:	g_mainResources->RateFloat(0.99f);			break;
		//		case VK_PRIOR:	g_mainResources->AddFloat(+0.1f);			break;
		//		case VK_NEXT:	g_mainResources->AddFloat(-0.1f);			break;


				}

		case WM_SYSKEYDOWN:
			if (VK_ESCAPE == wParam) {
				PostQuitMessage(0);			// �A�v���P�[�V�������[�v�I��
			}
			return 0;

		case WM_TOUCH:
			TouchEventProc((HTOUCHINPUT)lParam, (UINT)wParam);
			break;

			// �}�E�X
		case WM_MOUSEMOVE:					// �}�E�X���ړ������Ƃ�
		case WM_LBUTTONDOWN:				// ���{�^���������ꂽ�Ƃ�
		case WM_LBUTTONUP:					// ���{�^���������ꂽ�Ƃ�
			MouseEventProc(uMsg, wParam);
			break;

		case WM_RBUTTONDOWN:				// �E�{�^���������ꂽ�Ƃ�
		case WM_RBUTTONUP:					// �E�{�^���������ꂽ�Ƃ�
			MouseEventProc(uMsg, wParam);
			break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//====================================================================================================
//	�A�v���P�[�V�������[�v
//====================================================================================================
HRESULT AppLoop()
{
	HRESULT	hr = S_OK;

	//------------------------------------------------------------
	// ���b�Z�[�W���[�v
	//------------------------------------------------------------
	bool bGotMsg;
	MSG msg;
	msg.message = WM_NULL;
	PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);

	HWND hWnd = g_WinHandle[0];

	while (WM_QUIT != msg.message) {

		// ���b�Z�[�W�̃`�F�b�N 
		bGotMsg = (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) != 0);

		if (bGotMsg) {
			if (0 == TranslateAccelerator(hWnd, NULL, &msg)) {
				DispatchMessage(&msg);			// �v���V�[�W���ɑ��o
			}
		}

		else {
			Update();
			Render();
		}
	}

	return hr;
}

//====================================================================================================
//	�y�A�_�v�^(�O���t�B�b�N�J�[�h)�̏o�͊��̒����z
//====================================================================================================
HRESULT CheckAdapter()
{
	HRESULT hr = S_OK;

	UINT	NumAdapters = 0;		// Adapter(�O���{)�� 

	Microsoft::WRL::ComPtr<IDXGIFactory2> pDXGIFactory;

	// DXGI Factory�C���^�[�t�F�C�X�̍쐬
	hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&pDXGIFactory);
	if (FAILED(hr))	return hr;

	// Adapter(�O���{)���
	for (UINT i = 0;; i++) {

		IDXGIAdapter1 *pAdapter = NULL;

		hr = pDXGIFactory->EnumAdapters1(i, &pAdapter);
		if (DXGI_ERROR_NOT_FOUND == hr)	break;
		if (FAILED(hr))					break;

		NumAdapters++;

		DXGI_ADAPTER_DESC1 desc;
		pAdapter->GetDesc1(&desc);

		// �g�p�\��Adapter���񋓂��ꂽ�ꍇ�A�g�p�\��Output���
		for (UINT j = 0;; j++) {

			IDXGIOutput1* pOutput = NULL;
			hr = pAdapter->EnumOutputs(j, (IDXGIOutput**)&pOutput);

			if (DXGI_ERROR_NOT_FOUND == hr) break;
			if (FAILED(hr)) break;

			// ���p�\��Output�̋L�q���擾
			DXGI_OUTPUT_DESC desc;
			pOutput->GetDesc(&desc);
			if (!desc.AttachedToDesktop) { pOutput->Release(); continue; }

			// �L���ȏo�͂����o
			g_pOutput.push_back(pOutput);

		}
	}

	// �����Ŋ��̃`�F�b�N������
	if (g_pOutput.size()<1)						return E_FAIL;		// �\�����郂�j�^���Ȃ�

	return S_OK;
}

//====================================================================================================
//	�y�E�B���h�E�̃Z�b�g�A�b�v�z
//		�o�͐��ɍ��킹�ăE�B���h�E���쐬
//====================================================================================================
HRESULT SetupWindow(HINSTANCE hInstance)
{
	//---------------------------------------------------------------------------------
	// �E�B���h�E�N���X�̍쐬���o�^
	//---------------------------------------------------------------------------------
	WNDCLASS	WindowClass;									// �o�^�E�B���h�E�N���X
	WCHAR		szWindowClass[] = { L"Main" };					// �o�^�E�B���h�E�N���X��
	WCHAR		szWindowName[] = { L"Main" };					// �E�B���h�E��(Windowed)
	WCHAR		szExePath[MAX_PATH];
	GetModuleFileName(NULL, szExePath, MAX_PATH);
	HICON hIcon = ExtractIcon(hInstance, szExePath, 0);

	ZeroMemory(&WindowClass, sizeof(WNDCLASS));
	WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WindowClass.hIcon = hIcon;
	WindowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	WindowClass.style = CS_DBLCLKS;
	WindowClass.lpfnWndProc = MsgProc;
	WindowClass.hInstance = hInstance;
	WindowClass.lpszClassName = szWindowClass;

	// �o�^
	ATOM ClassAtom = RegisterClass(&WindowClass);
	if (ClassAtom == 0) {
		DWORD error = GetLastError();
		if (ERROR_CLASS_ALREADY_EXISTS != error) {
			MessageBox(NULL, L"�E�B���h�E�N���X�̓o�^�Ɏ��s���܂����B", L"Error", MB_OK | MB_ICONEXCLAMATION);
			return E_FAIL;
		}
	}

	//---------------------------------------------------------------------------------
	// �E�B���h�E�̍쐬
	//---------------------------------------------------------------------------------
	UINT numOutput = 1; //g_pOutput.size();

	for (UINT i = 0; i<numOutput; i++) {
		DWORD dwStyle = WS_OVERLAPPEDWINDOW;

		// �E�B���h�E�c���T�C�Y
		int W = WINDOW_WIDTH;
		int H = WINDOW_HEIGHT;

		// �E�B���h�E�\���ʒu
		DXGI_OUTPUT_DESC desc;
		int no = (i>numOutput - 1) ? numOutput - 1 : i;
		g_pOutput[no]->GetDesc(&desc);

		int X = desc.DesktopCoordinates.left;	// X���W 
		int Y = desc.DesktopCoordinates.top;	// Y���W

		HWND hWnd = CreateWindow(
			szWindowClass,					// �o�^�E�B���h�E�N���X��
			szWindowName,					// �E�B���h�E��
			dwStyle,						// �E�B���h�E�X�^�C��
			X, Y,							// �E�B���h�E�ʒu
			W, H,							// �E�B���h�E�c����
			NULL,							// �e�E�B���h�E�̃n���h��
			0,								// ���j���[�n���h��/�q�E�B���h�EID
			WindowClass.hInstance,			// App�C���X�^���X�̃n���h��
			NULL);							// �E�B���h�E�쐬�f�[�^

		g_WinHandle.push_back(hWnd);

		//---------------------------------------------------------------------------------
		// �^�b�`�W�F�X�`���[�̐ݒ�(�E�B���h�E��)
		//---------------------------------------------------------------------------------
		DWORD dwPanWant = GC_PAN_WITH_SINGLE_FINGER_HORIZONTALLY | GC_PAN_WITH_SINGLE_FINGER_VERTICALLY;
		DWORD dwPanBlock = GC_PAN_WITH_GUTTER | GC_PAN_WITH_INERTIA;

		GESTURECONFIG gc[] = { { GID_ZOOM, GC_ZOOM, 0 },
		{ GID_ROTATE, GC_ROTATE, 0 },
		{ GID_PAN, dwPanWant, dwPanBlock }
		};

		UINT uiGcs = 3;
		BOOL result = SetGestureConfig(hWnd, 0, uiGcs, gc, sizeof(GESTURECONFIG));

		if (!result) {
			DWORD err = GetLastError();
		}

		//---------------------------------------------------------------------------------
		// �^�b�`����(�E�B���h�E��)
		//---------------------------------------------------------------------------------
		RegisterTouchWindow(hWnd, TWF_WANTPALM);
	}

	//---------------------------------------------------------------------------------
	//	�E�B���h�E�̕\��
	//---------------------------------------------------------------------------------
	for (UINT i = 0; i < g_WinHandle.size(); i++) {
		ShowWindow(g_WinHandle[i], SW_SHOWDEFAULT);
		ValidateRect(g_WinHandle[i], 0);
	}

	return S_OK;
}

//======================================================================================
//	�}�E�X���b�Z�[�W�̏���
//======================================================================================
void MouseEventProc(UINT uMsg, WPARAM wParam)
{

	// �^�b�`�ɂ���Đ������}�E�X�C�x���g�𖳎�����
#define MOUSEEVENTF_FROMTOUCH 0xFF515700
	if ((GetMessageExtraInfo() & MOUSEEVENTF_FROMTOUCH) == MOUSEEVENTF_FROMTOUCH) {
		return;
	}

	bool flgMouseDOWN = (WM_LBUTTONDOWN == uMsg) ? true : false;						// �}�E�X���{�^���������ꂽ
	bool flgMouseUP = (WM_LBUTTONUP == uMsg) ? true : false;							// �}�E�X�̍��{�^���������ꂽ
	bool flgMouseMOVE = (WM_MOUSEMOVE == uMsg && MK_LBUTTON == wParam) ? true : false;	// �}�E�X���{�^���������ꂽ��Ԃňړ�

																						// �X�N���[�����W�ł̃J�[�\���ʒu���擾
	POINT	CursorPos;
	GetCursorPos(&CursorPos);

	// �C�x���g����
	UIEvent(g_hWndMouse, CursorPos, flgMouseDOWN, flgMouseUP, flgMouseMOVE);

}

//====================================================================================================
//	�^�b�`�ɂ�铮��
//====================================================================================================
void TouchEventProc(HTOUCHINPUT hTouchInput, UINT numInputs)
{
	if (!hTouchInput)	return;
	if (numInputs <= 0)	return;

	// �^�b�`���̎擾
	BOOL ret = GetTouchInputInfo(hTouchInput, numInputs, g_TouchInfo, sizeof(TOUCHINPUT));
	g_NumTouch = numInputs;

	// �^�b�`�C�x���g
	int no = numInputs - 1;		// ���}���`�^�b�`�̂������[0]�������Ă��Ȃ�
	bool flgTouchUp = (g_TouchInfo[no].dwFlags & TOUCHEVENTF_UP) ? true : false;
	bool flgTouchDown = (g_TouchInfo[no].dwFlags & TOUCHEVENTF_DOWN) ? true : false;
	bool flgTouchMove = (g_TouchInfo[no].dwFlags & TOUCHEVENTF_MOVE) ? true : false;

	// �^�b�`�ʒu (������ʍ��W���X�N���[�����W)
	POINT	CursorPos;
	CursorPos.x = g_TouchInfo[no].x / 100;
	CursorPos.y = g_TouchInfo[no].y / 100;

	// �^�b�`���̓n���h�������
	CloseTouchInputHandle(hTouchInput);

	// �C�x���g����
	UIEvent(g_hWndTouch, CursorPos, flgTouchDown, flgTouchUp, flgTouchMove);

}

//======================================================================================
//	�}�E�X�E�^�b�`�̏���
//======================================================================================
void UIEvent(HWND hWnd, POINT pCrnt, bool flgDown, bool flgUp, bool flgMove)
{

	//---------------------------------------------------------------------------
	//	�ړ��̕ω���
	//---------------------------------------------------------------------------	
	static POINT pPrev = pCrnt;
	LONG DifX = pCrnt.x - pPrev.x;
	LONG DifY = pCrnt.y - pPrev.y;
	pPrev = pCrnt;

	g_flgDown = flgMove;

	//---------------------------------------------------------------------------
	// �N���C�A���g�̈�ł̍��W�ɕϊ� (pCrnt �� mp)
	//---------------------------------------------------------------------------
	POINT mp = pCrnt;
	LONG x = mp.x;
	LONG y = mp.y;

	g_CursorPosOld = g_CursorPos;
	g_CursorPos = mp;


	//---------------------------------------------------------------------------
	// �O���
	//---------------------------------------------------------------------------
	static bool flgPrevD = false;		// �O���Down�t���O�̏��
	static bool flgPrevU = false;		// �O���Up�t���O�̏��

	flgPrevD = flgDown;
	flgPrevU = flgUp;

}

//====================================================================================================
//	�y�X�V�z
//====================================================================================================
void Update()
{

	//-------------------------------------------------------------------------
	// �V�[�� �I�u�W�F�N�g���X�V
	//-------------------------------------------------------------------------
	g_mainResources->Update();
	g_sceneRenderer->Update();

}

//====================================================================================================
//	�y�����_�����O�z	
//====================================================================================================
void Render()
{
	// ����X�V�O�Ƀ����_�����O�͍s��Ȃ�
	if (flgFirstRendering) { flgFirstRendering = false; return; }

	auto context = g_deviceResources->GetD3DDeviceContext();

	context->ClearState();		// �S�ẴX�e�[�g��������

	//---------------------------------------------------------------------------------
	// �E�B���h�E���Ƃ̃����_�����O
	//---------------------------------------------------------------------------------
	for (UINT i = 0; i < g_WinHandle.size(); i++) {

		// ���݂̃����_�[�^�[�Q�b�g��ʒm(DisplayNo���X�V)
		g_deviceResources->SetRenderTarget(i);

		// �r���[�|�[�g�����Z�b�g���đS��ʂ��^�[�Q�b�g�ɂ��A�N���A
		SetupRenderTarget(i);

		// �V�[���`��
		g_sceneRenderer->Render();

		// �r���[�|�[�g�����Z�b�g���đS��ʂ�
		auto viewport = g_deviceResources->GetViewport(i);
		context->RSSetViewports(1, &viewport);
		auto pRTV = g_deviceResources->GetRenderTargetView(i);
		auto pDSV = g_deviceResources->GetDepthStencilView(i);
		context->OMSetRenderTargets(1, &pRTV, pDSV);
		context->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	}

	//---------------------------------------------------------------------------------
	// Present�@(�o�b�N�o�b�t�@���t�����g�ɕ\��)
	//---------------------------------------------------------------------------------
	int numWnd = g_WinHandle.size();
	for (UINT i = 0; i < g_WinHandle.size(); i++) {
		auto swapChain = g_deviceResources->GetSwapChain(i);
		swapChain->Present(0, 0);
	}

	return;

}

//======================================================================================
//	�w��̏o��(�f�B�X�v���C)�Ƀ����_�[�^�[�Q�b�g��ݒ�
//======================================================================================
void SetupRenderTarget(int no)
{
	auto context = g_deviceResources->GetD3DDeviceContext();


	// �r���[�|�[�g�����Z�b�g���đS��ʂ��^�[�Q�b�g��
	auto viewport = g_deviceResources->GetViewport(no);				// �r���[�|�[�g�擾(�S���)
	auto pRTV = g_deviceResources->GetRenderTargetView(no);			// �����_�[�^�[�Q�b�g�E�r���[(RTV)�擾
	auto pDSV = g_deviceResources->GetDepthStencilView(no);			// �f�v�X�X�e���V���E�r���[(DSV)�擾

																	//	float clear[4] = { 1, 1, 1, 1 };
																	//	float clear[4] = { 0, 0, 1, 1 };
	float clear[4] = { 0, 0, 0, 1 };

	context->RSSetViewports(1, &viewport);							// �r���[�|�[�g��ݒ�
	context->OMSetRenderTargets(1, &pRTV, pDSV);					// RTV�EDSV��ݒ�
	context->ClearRenderTargetView(pRTV, clear);					// RTV���w��̐F�ŃN���A
	context->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);	// DSV���N���A

}

//======================================================================================
//	�N���C�A���g�̈���w��̃T�C�Y�ɂ���悤�ɃE�B���h�E�T�C�Y��ύX
//	 (�ʏ�̓^�C�g���o�[�Ȃǂ��܂߂Ďw��T�C�Y�ɂȂ��Ă��邽��)
//======================================================================================
inline void SetClientSize(HWND hWnd, int width, int height)
{
	RECT wRect, cRect;
	int W_WIDTH, W_HEIGET;					// �E�B���h�E�̕��ƍ���������ϐ�
	int C_WIDTH, C_HEIGET;					// �N���C�A���g�̕��ƍ���������ϐ�
	int S_WIDTH, S_HEIGET;					// �E�B���h�E�̊O�g�̕��ƍ���������ϐ�
	int M_WIDTH, M_HEIGET;					// �w�肵���E�B���h�E�̃T�C�Y������ϐ�

	GetWindowRect(hWnd, &wRect);			// �E�B���h�E�S�̂̍�����W�ƍ������W�����߂�
	W_WIDTH = wRect.right - wRect.left;		// �E�B���h�E�̕������߂�
	W_HEIGET = wRect.bottom - wRect.top;	// �E�B���h�E�̍��������߂�

	GetClientRect(hWnd, &cRect);			// �N���C�A���g�̍�����W�ƍ������W�����߂�
	C_WIDTH = cRect.right - cRect.left;		// �N���C�A���g�̕������߂�
	C_HEIGET = cRect.bottom - cRect.top;	// �N���C�A���g�̍��������߂�

	S_WIDTH = W_WIDTH - C_WIDTH;
	S_HEIGET = W_HEIGET - C_HEIGET;

	M_WIDTH = width + S_WIDTH;
	M_HEIGET = height + S_HEIGET;

	// �v�Z�������ƍ������E�B���h�E�ɐݒ�
	SetWindowPos(hWnd, HWND_TOP, 0, 0, M_WIDTH, M_HEIGET, SWP_NOMOVE);

}

//========================================================================================================
//	�f�B�X�v���C�T�C�Y�̎擾
//========================================================================================================
void	GetScreenSize()
{

	HWND desktop;
	RECT rc;
	static int width, height;

	//�X�N���[���̏��𓾂�
	desktop = GetDesktopWindow();
	GetWindowRect(desktop, &rc);
	width = rc.right;
	height = rc.bottom;

	g_WindowWidth = width;
	g_WindowHeight = height;

}