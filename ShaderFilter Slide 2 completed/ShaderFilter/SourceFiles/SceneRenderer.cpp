#include "pch.h"
#include "SceneRenderer.h"

#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"

using namespace DirectX;
using namespace Windows::Foundation;

//====================================================================================================
//	�y�R���X�g���N�^�z
//	�t�@�C�����璸�_�ƃs�N�Z�� �V�F�[�_�[��ǂݍ��݁A�L���[�u�̃W�I���g�����C���X�^���X�����܂��B
//====================================================================================================
SceneRenderer::SceneRenderer(const std::shared_ptr<DeviceResources>& deviceResources, const std::shared_ptr<MainResources>& mainRsources) :
m_loadingComplete(false),
m_degreesPerSecond(45),
m_tracking(false),
m_deviceResources(deviceResources),
m_mainResources(mainRsources)
{

	m_loadingComplete = true;

	auto context = m_deviceResources->GetD3DDeviceContext();
	auto device = m_deviceResources->GetD3DDevice();

	HRESULT hr = S_OK;

	//------------------------------------------------------------------------------------
	// ���f���̓ǂݍ���
	//------------------------------------------------------------------------------------
	m_MeshPlane = std::unique_ptr<CMesh11>(new CMesh11(m_deviceResources));
	m_MeshPlane->CreatePlane(1.0f);

	//------------------------------------------------------------------------------------
	// �e�N�j�b�N
	//------------------------------------------------------------------------------------
	m_TechMain = std::make_shared<CTechnique>(m_deviceResources);
	m_TechMain->Init(TECH_2D_MAIN);

	//------------------------------------------------------------------------------------
	// �e�N�X�`��
	//------------------------------------------------------------------------------------

}

//====================================================================================================
//	�y�f�X�g���N�^�z
//====================================================================================================
SceneRenderer::~SceneRenderer()
{

}

//====================================================================================================
//====================================================================================================
void SceneRenderer::Update()
{


}

//====================================================================================================
//====================================================================================================
void SceneRenderer::Render()
{
	if (!m_loadingComplete)		{ return; }

	auto context = m_deviceResources->GetD3DDeviceContext();

	DrawTexture(m_TechMain, m_mainResources->GetOutSRV());

}

//====================================================================================================
//	�X�N���[���S�ʂɃe�N�X�`����`��
//====================================================================================================
void SceneRenderer::DrawTexture(const std::shared_ptr<CTechnique>& technique, ID3D11ShaderResourceView *pSRV)
{

	CBuffer CData;

	STATESET states = CTechnique::GetStates(RS_SOLID_CULLBACK, DS_DISABLEDEPTHTESTWRITE, BS_NONE);
	auto pSampler = CTechnique::GetSampler(SS_ANISOTROPIC_CLAMP);

	m_MeshPlane->Render(technique, states, &CData, &pSRV, 1, &pSampler, 1);

}