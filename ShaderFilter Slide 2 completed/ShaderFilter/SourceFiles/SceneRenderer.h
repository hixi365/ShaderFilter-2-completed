#pragma once

#include "DeviceResources.h"
#include "MainResources.h"

#include "CMesh11.h"

//========================================================================================================
// �y�N���XSceneRenderer�z �V�[���`��N���X
//========================================================================================================
class SceneRenderer
{
public:
	SceneRenderer(const std::shared_ptr<DeviceResources>& deviceResources, const std::shared_ptr<MainResources>& mainRsources);		// �R���X�g���N�^
	~SceneRenderer();															// �f�X�g���N�^
		
	void	Update();
	void	Render();

private:

	void	DrawTexture(const std::shared_ptr<CTechnique>& technique, ID3D11ShaderResourceView *pSRV);

private:
	// ���L���\�[�X
	std::shared_ptr<DeviceResources>				m_deviceResources;
	std::shared_ptr<MainResources>					m_mainResources;

	// �e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_TestSRV;

	// ���b�V��
	std::unique_ptr<CMesh11>							m_MeshPlane;


	// �e�N�j�b�N
	std::shared_ptr<CTechnique>							m_TechMain;


	//---------------------------------------------------------------
	// �����_�����O ���[�v�Ŏg�p����ϐ��B
	//---------------------------------------------------------------
	bool	m_loadingComplete;
	float	m_degreesPerSecond;
	bool	m_tracking;
	bool	m_RTTexture = false;

};
