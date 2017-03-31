#pragma once

//======================================================================================
//	�w�b�_�E�t�@�C��
//======================================================================================
#include "Main.h"
#include "DeviceResources.h"

#include "CTechnique.h"
#include "CMesh11.h"

//======================================================================================
//	�}�N��
//======================================================================================
using namespace D2D1;
using namespace DirectX;
	
//======================================================================================
//	�N���XMainResources
//======================================================================================
class MainResources
{

private:

	//--------------------------------------------------------------------------------
	//	�f�o�C�X���\�[�X
	//--------------------------------------------------------------------------------
	std::shared_ptr<DeviceResources>	m_deviceResources;

	//--------------------------------------------------------------------
	//	�e�N�X�`��
	//--------------------------------------------------------------------
	Microsoft::WRL::ComPtr<ID3D11Texture2D>				m_OutTex;	// �o��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_OutSRV;	// �o��
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		m_OutRTV;	// �o��

	Microsoft::WRL::ComPtr<ID3D11Texture2D>				m_LifeGameOldTex;	// LifeGame
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_LifeGameOldSRV;	// LifeGame
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		m_LifeGameOldRTV;	// LifeGame

	Microsoft::WRL::ComPtr<ID3D11Texture2D>				m_LifeGameNewTex;	// LifeGame
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_LifeGameNewSRV;	// LifeGame
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		m_LifeGameNewRTV;	// LifeGame

	//--------------------------------------------------------------------
	//	�e�N�j�b�N
	//--------------------------------------------------------------------
	std::shared_ptr<CTechnique>							m_TechLifeGame;	// LifeGame

	//--------------------------------------------------------------------
	//	���b�V��
	//--------------------------------------------------------------------
	std::unique_ptr<CMesh11>		m_MeshPlane;

	//--------------------------------------------------------------------
	//	�ʏ�ϐ�
	//--------------------------------------------------------------------
	float	m_floatAX = 1.0f;
	int		m_patternLifeGame = 0;
	bool	m_flgLifeGame = true;

private:

	void CreateTechResources();
	void CreateMeshResources();
	void CreateLifeGameResources();
	void UpdateLifeGame();

public:
								MainResources(const std::shared_ptr<DeviceResources>& deviceResources);
								~MainResources();
	
	
	ID3D11ShaderResourceView*	GetOutSRV() { return m_OutSRV.Get(); }

	void						Update();
	void						AddFloat(float x) { m_floatAX += x; }
	void						SetFloat(float x) { m_floatAX = x; }
	void						RateFloat(float x) { m_floatAX *= x; }

	void						SaveTexture();

	void						InitLifeGame(int pattern = -1);
	void						SetpLifeGame(){ m_flgLifeGame = false; UpdateLifeGame(); }
	void						StartLifeGame(){m_flgLifeGame = true; }

};