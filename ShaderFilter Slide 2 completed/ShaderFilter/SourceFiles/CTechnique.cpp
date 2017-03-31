#include "pch.h"
#include "CTechnique.h"
#include <d3dcompiler.h>

using namespace DirectX;

// ���_�\��
static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

static const D3D11_INPUT_ELEMENT_DESC particlepackDesc[] = {
	{ "ID", 0, DXGI_FORMAT_R32_UINT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

//---------------------------------------------------------------
// static�����o�[�̏�����
//---------------------------------------------------------------
std::vector<ID3D11RasterizerState1*>	CTechnique::m_RSList;
std::vector<ID3D11DepthStencilState*>	CTechnique::m_DSList;
std::vector<ID3D11SamplerState*>		CTechnique::m_SSList;
std::vector<ID3D11BlendState1*>			CTechnique::m_BSList;
UINT									CTechnique::m_RefCount = 0;

//========================================================================================================
//	�y�R���X�g���N�^�z
//========================================================================================================
CTechnique::CTechnique(const std::shared_ptr<DeviceResources>& deviceResources) :
m_deviceResources(deviceResources),
m_VShader(nullptr),
m_PShader(nullptr),
m_GShader(nullptr),
m_CShader(nullptr),
m_InputLayout(nullptr),
m_RasterizerState(nullptr)
{
	// �ŏ���1��̂݃X�e�[�g�쐬
	if (m_RefCount == 0) {
		CreateDefaultStates();
	}
	m_RefCount++;

}

//========================================================================================================
//	�y�f�X�g���N�^�z
//========================================================================================================
CTechnique::~CTechnique()
{
	Reset();

	m_RefCount--;

	if (m_RefCount == 0) {
		// �X�e�[�g���	
		for (UINT i = 0; i < m_RSList.size(); i++) { if (m_RSList[i]) { m_RSList[i]->Release(); m_RSList[i] = nullptr; } }
		for (UINT i = 0; i < m_DSList.size(); i++) { if (m_DSList[i]) { m_DSList[i]->Release(); m_DSList[i] = nullptr; } }
		for (UINT i = 0; i < m_SSList.size(); i++) { if (m_SSList[i]) { m_SSList[i]->Release(); m_SSList[i] = nullptr; } }
		for (UINT i = 0; i < m_BSList.size(); i++) { if (m_BSList[i]) { m_BSList[i]->Release(); m_BSList[i] = nullptr; } }
	}
}

//========================================================================================================
//	�y���Z�b�g�z	�eCOM��Release()���Ăяo��
//========================================================================================================
void CTechnique::Reset()
{
	m_VShader.Reset();
	m_GShader.Reset();
	m_PShader.Reset();
	m_CShader.Reset();
	m_InputLayout.Reset();

	m_RasterizerState.Reset();
	m_DepthStencilState.Reset();
}

//========================================================================================================
//	������
//========================================================================================================
HRESULT CTechnique::Init(TECHNIQUE tech)
{
	HRESULT hr = S_OK;

	auto device = m_deviceResources->GetD3DDevice();


	//--------------------------------------------------------------------------------
	// �萔�o�b�t�@�쐬
	//--------------------------------------------------------------------------------

	CD3D11_BUFFER_DESC constantBufferDesc(sizeof(CBuffer), D3D11_BIND_CONSTANT_BUFFER);
	hr = device->CreateBuffer(&constantBufferDesc, nullptr, m_CBuffer.GetAddressOf());


	//--------------------------------------------------------------------------------
	// �V�F�[�_�[�I�u�W�F�N�g�쐬
	//--------------------------------------------------------------------------------

	switch (tech) {

	case TECH_2D_MAIN:
		hr = LoadVertexShader(L"ShaderFiles/2DShaders.hlsl", "mainVS", m_VShader.GetAddressOf(), vertexDesc, ARRAYSIZE(vertexDesc), m_InputLayout.GetAddressOf());
		hr = LoadPixelShader(L"ShaderFiles/2DShaders.hlsl", "mainPS", m_PShader.GetAddressOf());
		m_flgCB = FOR_PS;
		m_flgSRV = FOR_PS;
		break;

	case TECH_2D_BLUR:
		hr = LoadVertexShader(L"ShaderFiles/2DShaders.hlsl", "mainVS", m_VShader.GetAddressOf(), vertexDesc, ARRAYSIZE(vertexDesc), m_InputLayout.GetAddressOf());
		hr = LoadPixelShader(L"ShaderFiles/2DShaders.hlsl", "blurPS", m_PShader.GetAddressOf());
		m_flgCB = FOR_PS;
		m_flgSRV = FOR_PS;
		break;

	case TECH_2D_GAMMA:
		hr = LoadVertexShader(L"ShaderFiles/2DShaders.hlsl", "mainVS", m_VShader.GetAddressOf(), vertexDesc, ARRAYSIZE(vertexDesc), m_InputLayout.GetAddressOf());
		hr = LoadPixelShader(L"ShaderFiles/2DShaders.hlsl", "gammaPS", m_PShader.GetAddressOf());
		m_flgCB = FOR_PS;
		m_flgSRV = FOR_PS;
		break;

	case TECH_2D_UNSHARP:
		hr = LoadVertexShader(L"ShaderFiles/2DShaders.hlsl", "mainVS", m_VShader.GetAddressOf(), vertexDesc, ARRAYSIZE(vertexDesc), m_InputLayout.GetAddressOf());
		hr = LoadPixelShader(L"ShaderFiles/2DShaders.hlsl", "unsharpPS", m_PShader.GetAddressOf());
		m_flgCB = FOR_PS;
		m_flgSRV = FOR_PS;
		break;

	
	case TECH_2D_LIFEGAME:
		hr = LoadVertexShader(L"ShaderFiles/2DShaders.hlsl", "mainVS", m_VShader.GetAddressOf(), vertexDesc, ARRAYSIZE(vertexDesc), m_InputLayout.GetAddressOf());
		hr = LoadPixelShader(L"ShaderFiles/2DShaders.hlsl", "lifegamePS", m_PShader.GetAddressOf());
		m_flgCB = FOR_PS;
		m_flgSRV = FOR_PS;
		break;



	}

	return hr;
}

//========================================================================================================
//	�f�t�H���g�̃����_�����O�X�e�[�g�̍쐬
//========================================================================================================
void CTechnique::CreateDefaultStates()
{
	HRESULT hr = S_OK;

	auto device = m_deviceResources->GetD3DDevice();

	m_RSList.clear();
	m_DSList.clear();
	m_SSList.clear();
	m_BSList.clear();

	//--------------------------------------------------------------------------------
	// ���X�^���C�U�[�X�e�[�g
	//--------------------------------------------------------------------------------
	ID3D11RasterizerState1* pRasterizerState;

	// [0] RS_SOLID_CULLBACK
	{
		D3D11_RASTERIZER_DESC1 desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.FillMode = D3D11_FILL_SOLID;		// SOLID
		desc.CullMode = D3D11_CULL_BACK;		//BACK;
		desc.FrontCounterClockwise = TRUE;
		desc.MultisampleEnable = TRUE;
		desc.DepthClipEnable = TRUE;
		desc.AntialiasedLineEnable = TRUE;

		hr = device->CreateRasterizerState1(&desc, &pRasterizerState);
		if (SUCCEEDED(hr))		m_RSList.push_back(pRasterizerState);					// �ǉ�
	}

	// [1] RS_SOLID_CULLNONE
	{
		D3D11_RASTERIZER_DESC1 desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.FillMode = D3D11_FILL_SOLID;		// SOLID
		desc.CullMode = D3D11_CULL_NONE;		// NONE
		desc.FrontCounterClockwise = TRUE;
		desc.MultisampleEnable = TRUE;
		desc.DepthClipEnable = TRUE;
		desc.AntialiasedLineEnable = TRUE;

		hr = device->CreateRasterizerState1(&desc, &pRasterizerState);
		if (SUCCEEDED(hr))		m_RSList.push_back(pRasterizerState);					// �ǉ�
	}

	//--------------------------------------------------------------------------------
	// �f�v�X�X�e���V���X�e�[�g
	//--------------------------------------------------------------------------------
	ID3D11DepthStencilState* pDepthStencilState;

	// [0] DS_ENABLEDEPTHTESTWRITE
	{
		D3D11_DEPTH_STENCIL_DESC dsDesc;
		ZeroMemory(&dsDesc, sizeof(dsDesc));
		dsDesc.DepthEnable = TRUE;								// �[�x��r����
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;		// �[�x�������݂���
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
		dsDesc.StencilEnable = FALSE;

		hr = device->CreateDepthStencilState(&dsDesc, &pDepthStencilState);
		if (SUCCEEDED(hr))		m_DSList.push_back(pDepthStencilState);					// �ǉ�
	}

	// [1] DS_DISABLEDEPTHTESTWRITE
	{
		D3D11_DEPTH_STENCIL_DESC dsDesc;
		ZeroMemory(&dsDesc, sizeof(dsDesc));
		dsDesc.DepthEnable = FALSE;								// �[�x��r�Ȃ�
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;		// �[�x�������݂���
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
		dsDesc.StencilEnable = FALSE;

		hr = device->CreateDepthStencilState(&dsDesc, &pDepthStencilState);
		if (SUCCEEDED(hr))		m_DSList.push_back(pDepthStencilState);					// �ǉ�
	}

	//--------------------------------------------------------------------------------
	// �T���v���[�X�e�[�g
	//--------------------------------------------------------------------------------
	ID3D11SamplerState* pSamplerState;

	// [0] SS_POINT
	{
		D3D11_SAMPLER_DESC SamplerDesc;
		XMFLOAT4 color = { 0, 0, 0, 0 };
		SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

		SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		::CopyMemory(SamplerDesc.BorderColor, &color, sizeof(XMFLOAT4));
		SamplerDesc.MaxAnisotropy = 16;
		SamplerDesc.MinLOD = 0;
		SamplerDesc.MaxLOD = 0;// D3D11_FLOAT32_MAX;
		SamplerDesc.MipLODBias = 0;
		SamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;

		hr = device->CreateSamplerState(&SamplerDesc, &pSamplerState);
		if (SUCCEEDED(hr))		m_SSList.push_back(pSamplerState);					// �ǉ�
	}

	// [1] SS_LINEAR_CLAMP
	{
		D3D11_SAMPLER_DESC SamplerDesc;
		XMFLOAT4 color = { 0, 0, 0, 0 };
		SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

		SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		::CopyMemory(SamplerDesc.BorderColor, &color, sizeof(XMFLOAT4));
		SamplerDesc.MaxAnisotropy = 16;
		SamplerDesc.MinLOD = 0;
		SamplerDesc.MaxLOD = 0;// D3D11_FLOAT32_MAX;
		SamplerDesc.MipLODBias = 0;
		SamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;

		hr = device->CreateSamplerState(&SamplerDesc, &pSamplerState);
		if (SUCCEEDED(hr))		m_SSList.push_back(pSamplerState);					// �ǉ�
	}

	// [2] SS_ANISOTROPIC_CLAMP
	{
		D3D11_SAMPLER_DESC SamplerDesc;
		XMFLOAT4 color = { 1, 1, 1, 1 };
		SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

		SamplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		::CopyMemory(SamplerDesc.BorderColor, &color, sizeof(XMFLOAT4));
		SamplerDesc.MaxAnisotropy = 16;
		SamplerDesc.MinLOD = 0;
		SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;// 0;// D3D11_FLOAT32_MAX;
		SamplerDesc.MipLODBias = 0;
		SamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;

		hr = device->CreateSamplerState(&SamplerDesc, &pSamplerState);
		if (SUCCEEDED(hr))		m_SSList.push_back(pSamplerState);					// �ǉ�
	}

	//--------------------------------------------------------------------------------
	// �u�����h�X�e�[�g
	//--------------------------------------------------------------------------------
	ID3D11BlendState1*	pBlendState;

	// [0] BS_NONE
	{
		D3D11_BLEND_DESC1 desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.AlphaToCoverageEnable = FALSE;
		desc.IndependentBlendEnable = FALSE;
		desc.RenderTarget[0].BlendEnable = FALSE;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		hr = device->CreateBlendState1(&desc, &pBlendState);
		if (SUCCEEDED(hr))		m_BSList.push_back(pBlendState);					// �ǉ�
	}

	// [1] BS_ALPHA
	{
		D3D11_BLEND_DESC1 desc;
		ZeroMemory(&desc, sizeof(desc));

		desc.AlphaToCoverageEnable			= FALSE;// TRUE;
		desc.IndependentBlendEnable			= FALSE;
		desc.RenderTarget[0].BlendEnable	= TRUE;
		desc.RenderTarget[0].SrcBlend		= D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend		= D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp		= D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha	= D3D11_BLEND_ONE;// D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlendAlpha	= D3D11_BLEND_ONE;// D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOpAlpha	= D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		hr = device->CreateBlendState1(&desc, &pBlendState);
		if (SUCCEEDED(hr))		m_BSList.push_back(pBlendState);					// �ǉ�
	}

	// [2] BS_ADD
	{
		D3D11_BLEND_DESC1 desc;
		ZeroMemory(&desc, sizeof(desc));

		desc.AlphaToCoverageEnable			= FALSE;// TRUE;
		desc.IndependentBlendEnable			= FALSE;
		desc.RenderTarget[0].BlendEnable	= TRUE;
		desc.RenderTarget[0].SrcBlend		= D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlend		= D3D11_BLEND_ONE;
		desc.RenderTarget[0].BlendOp		= D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha	= D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha	= D3D11_BLEND_ONE;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		hr = device->CreateBlendState1(&desc, &pBlendState);
		if (SUCCEEDED(hr))		m_BSList.push_back(pBlendState);					// �ǉ�
	}

}

//========================================================================================================
//	�萔�o�b�t�@�̍X�V
//========================================================================================================
void CTechnique::UpdataConstantBuffer(CBuffer &data)
{
	m_deviceResources->GetD3DDeviceContext()->UpdateSubresource(m_CBuffer.Get(), 0, nullptr, &data, 0, 0);
}

//========================================================================================================
//	�V�F�[�_�[�t�@�C���̃R���p�C��
//========================================================================================================
HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob = nullptr;
	hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
	if (FAILED(hr)) {
		if (pErrorBlob) {
			OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
			pErrorBlob->Release();
		}
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}

//========================================================================================================
//	�t�@�C�����璸�_�V�F�[�_�[�I�u�W�F�N�g�̍쐬
//========================================================================================================
HRESULT CTechnique::LoadVertexShader(WCHAR *fileName, LPCSTR szEntryPoint, ID3D11VertexShader **ppShader,
	const D3D11_INPUT_ELEMENT_DESC *pInputElementDescs, UINT NumElements, ID3D11InputLayout **ppInputLayout)
{
	HRESULT hr = S_OK;

	auto device = m_deviceResources->GetD3DDevice();

	//------------------------------------------------------------------------------------
	// �t�@�C����ǂݍ��ݏI���� �� �V�F�[�_�[�R���p�C���Ɠ��̓��C�A�E�g���쐬
	//------------------------------------------------------------------------------------
	Microsoft::WRL::ComPtr<ID3DBlob> Blob;

	ID3DBlob* pVSBlob = nullptr;
	hr = CompileShaderFromFile(fileName, szEntryPoint, "vs_5_0", Blob.GetAddressOf());
	if (FAILED(hr))	return E_FAIL;

	// Create the vertex shader
	hr = device->CreateVertexShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), nullptr, ppShader);
	if (FAILED(hr)) {
		Blob.Reset();
		return E_FAIL;
	}

	if (pInputElementDescs) {
		// ���͒��_���C�A�E�g�̍쐬
		hr = device->CreateInputLayout(
			pInputElementDescs,
			NumElements,
			Blob->GetBufferPointer(),
			Blob->GetBufferSize(),
			&m_InputLayout);
		if (FAILED(hr)) {
			Blob.Reset();
			return E_FAIL;
		}
	}

	Blob.Reset();

	return hr;
}

//========================================================================================================
//	�t�@�C������W�I���g���V�F�[�_�[�I�u�W�F�N�g�̍쐬
//========================================================================================================
HRESULT CTechnique::LoadGeometryShader(WCHAR *fileName, LPCSTR szEntryPoint, ID3D11GeometryShader **ppShader)
{
	HRESULT hr = S_OK;

	auto device = m_deviceResources->GetD3DDevice();

	Microsoft::WRL::ComPtr<ID3DBlob> Blob;

	ID3DBlob* pVSBlob = nullptr;
	hr = CompileShaderFromFile(fileName, szEntryPoint, "gs_5_0", Blob.GetAddressOf());
	if (FAILED(hr))	return E_FAIL;

	// Create the geometry shader
	hr = device->CreateGeometryShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), nullptr, ppShader);
	if (FAILED(hr)) {
		Blob.Reset();
		return E_FAIL;
	}

	Blob.Reset();

	return hr;
}

//========================================================================================================
//	�t�@�C������s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̍쐬
//========================================================================================================
HRESULT CTechnique::LoadPixelShader(WCHAR *fileName, LPCSTR szEntryPoint, ID3D11PixelShader **ppShader)
{
	HRESULT hr = S_OK;

	auto device = m_deviceResources->GetD3DDevice();

	Microsoft::WRL::ComPtr<ID3DBlob> Blob;

	ID3DBlob* pVSBlob = nullptr;
	hr = CompileShaderFromFile(fileName, szEntryPoint, "ps_5_0", Blob.GetAddressOf());
	if (FAILED(hr))	return E_FAIL;

	// Create the pixel shader
	hr = device->CreatePixelShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), nullptr, ppShader);
	if (FAILED(hr)) {
		Blob.Reset();
		return E_FAIL;
	}

	Blob.Reset();

	return hr;
}


//========================================================================================================
//	�t�@�C������R���s���[�g�V�F�[�_�[�I�u�W�F�N�g�̍쐬
//========================================================================================================
HRESULT CTechnique::LoadComputeShader(WCHAR *fileName, LPCSTR szEntryPoint, ID3D11ComputeShader **ppShader)
{
	HRESULT hr = S_OK;

	auto device = m_deviceResources->GetD3DDevice();

	Microsoft::WRL::ComPtr<ID3DBlob> Blob;

	ID3DBlob* pVSBlob = nullptr;
	hr = CompileShaderFromFile(fileName, szEntryPoint, "cs_5_0", Blob.GetAddressOf());
	if (FAILED(hr))	return E_FAIL;

	// Create the pixel shader
	hr = device->CreateComputeShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), nullptr, ppShader);
	if (FAILED(hr)) {
		Blob.Reset();
		return E_FAIL;
	}

	Blob.Reset();

	return hr;
}

//========================================================================================================
//	�����_�[�X�e�[�g�̃Z�b�g��Ԃ�
//========================================================================================================
STATESET CTechnique::GetStates(UINT rsID, UINT dsID, UINT bsID)
{
	ID3D11RasterizerState1  *pRS = (rsID < m_RSList.size()) ? m_RSList[rsID] : nullptr;
	ID3D11DepthStencilState *pDS = (dsID < m_DSList.size()) ? m_DSList[dsID] : nullptr;
	ID3D11BlendState1       *pBS = (bsID < m_BSList.size()) ? m_BSList[bsID] : nullptr;

	STATESET set(pRS, pDS, pBS);

	return set;
}
