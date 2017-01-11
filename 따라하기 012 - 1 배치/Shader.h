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

	//컴파일된 이펙트 파일에서 정점 쉐이더와 픽셀 쉐이더를 생성하는 함수이다.
	void CreateVertexShaderFromCompiledFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, D3D11_INPUT_ELEMENT_DESC *pd3dInputLayout, UINT nElements);
	void CreatePixelShaderFromCompiledFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName);

	static void CreateShaderVariables(ID3D11Device *pd3dDevice);
	static void ReleaseShaderVariables();
	static void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, D3DXMATRIX *pd3dxmtxWorld);
	//쉐이더를 생성하기 위해 호출하는 가상함수이다.
	virtual void CreateShader(ID3D11Device *pd3dDevice);

	

	//게임 객체들을 생성하고 애니메이션 처리를 하고 렌더링하기 위한 함수이다.
	virtual void BuildObjects(ID3D11Device *pd3dDevice);
	virtual void ReleaseObjects();
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void OnPrepareRender(ID3D11DeviceContext *pd3dDeviceContext);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera = NULL);


protected:
	ID3D11VertexShader *m_pd3dVertexShader;
	ID3D11InputLayout *m_pd3dVertexLayout;

	ID3D11PixelShader *m_pd3dPixelShader;

	//쉐이더 객체가 게임 객체들의 리스트를 가진다.
	CGameObject **m_ppObjects;
	int m_nObjects;

	//월드 변환 행렬을 위한 상수 버퍼는 하나만 있어도 되므로 정적 멤버로 선언한다.
	static ID3D11Buffer *m_pd3dcbWorldMatrix;
};


//게임 객체들을 렌더링하기 위한 쉐이더 클래스이다.
class CSceneShader : public CShader
{
public:
	CSceneShader();
	virtual ~CSceneShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
	virtual void BuildObjects(ID3D11Device *pd3dDevice);
};

//플레이어를 렌더링하기 위한 쉐이더 클래스이다.
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
