#pragma once
#include "Object.h"
#include "Camera.h"
#include "Player.h"


struct VS_CB_WORLD_MATRIX
{
	D3DXMATRIX m_d3dxmtxWorld;
};



class CShader
{
private:
	int m_nReferences;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

public:
	CShader();
	virtual ~CShader();

	//�����ϵ� ����Ʈ ���Ͽ��� ���� ���̴��� �ȼ� ���̴��� �����ϴ� �Լ��̴�.
	void CreateVertexShaderFromCompiledFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, D3D11_INPUT_ELEMENT_DESC *pd3dInputLayout, UINT nElements);
	void CreatePixelShaderFromCompiledFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName);

	static void CreateShaderVariables(ID3D11Device *pd3dDevice);
	static void ReleaseShaderVariables();
	static void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, D3DXMATRIX *pd3dxmtxWorld);
	//���̴��� �����ϱ� ���� ȣ���ϴ� �����Լ��̴�.
	virtual void CreateShader(ID3D11Device *pd3dDevice);

	

	//���� ��ü���� �����ϰ� �ִϸ��̼� ó���� �ϰ� �������ϱ� ���� �Լ��̴�.
	virtual void BuildObjects(ID3D11Device *pd3dDevice);
	virtual void ReleaseObjects();
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void OnPrepareRender(ID3D11DeviceContext *pd3dDeviceContext);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera = NULL);


protected:
	ID3D11VertexShader *m_pd3dVertexShader;
	ID3D11InputLayout *m_pd3dVertexLayout;

	ID3D11PixelShader *m_pd3dPixelShader;

	//���̴� ��ü�� ���� ��ü���� ����Ʈ�� ������.
	CGameObject **m_ppObjects;
	int m_nObjects;

	//���� ��ȯ ����� ���� ��� ���۴� �ϳ��� �־ �ǹǷ� ���� ����� �����Ѵ�.
	static ID3D11Buffer *m_pd3dcbWorldMatrix;
};


//���� ��ü���� �������ϱ� ���� ���̴� Ŭ�����̴�.
class CSceneShader : public CShader
{
public:
	CSceneShader();
	virtual ~CSceneShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
	virtual void BuildObjects(ID3D11Device *pd3dDevice);
};

//�÷��̾ �������ϱ� ���� ���̴� Ŭ�����̴�.
class CPlayerShader : public CShader
{
public:
	CPlayerShader();
	virtual ~CPlayerShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
	virtual void BuildObjects(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera = NULL);

	CPlayer *GetPlayer(int nIndex = 0) { return((CPlayer *)m_ppObjects[nIndex]); }
};

class CInstancingShader : public CShader
{
public:
	CInstancingShader();
	~CInstancingShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);

	virtual void BuildObjects(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);

private:
	//���庯ȯ ��� �ν��Ͻ� ���� ������ �� ������ ũ��(����Ʈ)�� �������̴�.
	UINT m_nInstanceMatrixBufferStride;
	UINT m_nInstanceMatrixBufferOffset;
	//���� �ν��Ͻ� ���� ������ �� ������ ũ��(����Ʈ)�� �������̴�.
	UINT m_nInstanceColorBufferStride;
	UINT m_nInstanceColorBufferOffset;

	//������ü�� �ν��Ͻ� ���� ����(���庯ȯ ���)�̴�.
	ID3D11Buffer *m_pd3dCubeInstanceMatrixBuffer;
	//������ü�� �ν��Ͻ� ���� ����(����)�̴�.
	ID3D11Buffer *m_pd3dCubeInstanceColorBuffer;
	//���� �ν��Ͻ� ���� ����(���庯ȯ ���)�̴�.
	ID3D11Buffer *m_pd3dSphereInstanceMatrixBuffer;
	//���� �ν��Ͻ� ���� ����(����)�̴�.
	ID3D11Buffer *m_pd3dSphereInstanceColorBuffer;

	//�ν��Ͻ� ������ ���� ũ��� ���� �������̴�.
	UINT m_nInstanceBufferStride;
	UINT m_nInstanceBufferOffset;

	//ť�꿡 ���� �ν��Ͻ� �������� ���ۿ� ���� ���� �ν��Ͻ� ������ �����̴�.
	ID3D11Buffer *m_pd3dCubeInstanceBuffer;
	ID3D11Buffer *m_pd3dSphereInstanceBuffer;

public:
	//�ν��Ͻ� ���۸� �����Ѵ�.
	ID3D11Buffer *CreateInstanceBuffer(ID3D11Device *pd3dDevice, int nObjects, UINT nBufferStride, void *pBufferData);
};

