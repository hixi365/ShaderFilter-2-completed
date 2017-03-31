#pragma once

#include "pch.h"
#include "DeviceResources.h"
#include "CTechnique.h"

#define MeshInstDataMAX 100000

using namespace Microsoft::WRL;


//========================================================================================================
//	�\����
//========================================================================================================
	
//-------------------------------------------------------
// ���_�o�b�t�@�p�\����
//-------------------------------------------------------
struct MeshVertex
{
	DirectX::XMFLOAT3 pos;			// �ʒu
	DirectX::XMFLOAT3 normal;		// �@���x�N�g��
	DirectX::XMFLOAT4 color;		// �F
	DirectX::XMFLOAT2 tex;			// �e�N�X�`�����W
};

struct MeshInstData {
	DirectX::XMMATRIX mat;			// ���f���s��
};

struct IDPack {
	UINT			id;				// id
};

//========================================================================================================
//	�yCMesh11�N���X�z
//========================================================================================================
class CMesh11
{
public:
	CMesh11(const std::shared_ptr<DeviceResources>& deviceResources);
	~CMesh11();

	void Render(const std::shared_ptr<CTechnique>& technique, STATESET &states, CBuffer* cbData, ID3D11ShaderResourceView* pSRV[] = { nullptr }, UINT nSRVs = 0, ID3D11SamplerState* pSamplers[] = { nullptr }, UINT nSamplers = 0);
	void RenderInstanced(const std::shared_ptr<CTechnique>& technique, STATESET &states, CBuffer* cbData, ID3D11ShaderResourceView* pSRV[] = { nullptr }, UINT nSRVs = 0, ID3D11SamplerState* pSamplers[] = { nullptr }, UINT nSamplers = 0);

	void RenderInstancing(CBuffer&  mat, const std::shared_ptr<CTechnique>& technique, ComPtr<ID3D11SamplerState> SampleState, ComPtr<ID3D11BlendState> BlendState);

	ID3D11Buffer*				GetVertexBuffer() const { return m_VertexBuffer.Get(); }
	ID3D11UnorderedAccessView*	GetVertexUAV() const { return m_VertexUAV.Get(); }
	UINT						GetNumVertices() const { return m_NumVertices; }

	void	CreatePlane(float size);

	void	CreateIDPack(UINT numIDs);

	HRESULT SetMeshInstData(void* pData, UINT numMeshInstData);

private:

	std::shared_ptr<DeviceResources>					m_deviceResources;		// �f�o�C�X�E���\�[�X�ւ̃|�C���^(share)

	Microsoft::WRL::ComPtr<ID3D11Buffer>				m_ConstBuffer;			// �萔�o�b�t�@

	// ���b�V�����f���ɕK�v�ȃ��\�[�X
	Microsoft::WRL::ComPtr<ID3D11Buffer>				m_VertexBuffer;			// ���_�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D11Buffer>				m_IndexBuffer;			// �C���f�b�N�X
	Microsoft::WRL::ComPtr<ID3D11Buffer>				m_InstBuffer;			// �C���X�^���V���O�p�o�b�t�@
	UINT												m_NumMeshInstData;		// �C���X�^���V���O�f�[�^��

	Microsoft::WRL::ComPtr<ID3D11Buffer>				m_VertexBufCpy;			// ���_�o�b�t�@�̃R�s�[(STAGING) �� Particle�̂�
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>	m_VertexUAV;			// ���_�o�b�t�@�A�N�Z�XUAV

	DWORD												m_NumVertices;			// ���_��
	DWORD												m_NumIndices;			// �C���f�b�N�X��
	UINT												m_Stride = 0;			// 1���_������̃X�g���C�h(�o�C�g��)

	D3D11_PRIMITIVE_TOPOLOGY							m_Topology;				// PRIMITIVE_TOPOLOGY

	// �e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_SRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_WhiteSRV;


	// �Ǘ��p
	bool			m_Complete;

};