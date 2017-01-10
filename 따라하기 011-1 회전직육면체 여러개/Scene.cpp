#include "stdafx.h"
#include "Scene.h"


CScene::CScene()
{
	m_ppObjects = NULL;
	m_nObjects = 0;
}


CScene::~CScene()
{
}

void CScene::BuildObjects(ID3D11Device *pd3dDevice)
{
	CShader *pShader = new CShader();
	pShader->CreateShader(pd3dDevice);

	//가로x세로x높이가 12x12x12인 정육면체 메쉬를 생성한다.
	CCubeMesh *pMesh = new CCubeMesh(pd3dDevice, 15.0f, 15.0f, 15.0f);

	/*x-축, y-축, z-축 양의 방향의 객체 개수이다. 각 값을 1씩 늘려가면서 실행할 때 프레임 레이트가 어떻게 변하는 가를 살펴보기 바란다.*/
	int xObjects = 3, yObjects = 3, zObjects = 3;
	//<그림 2>와 같이 x-축, y-축, z-축으로 7개씩 총 7 x 7 x 7 = 343개의 정육면체를 배치한다. 
	m_nObjects = (xObjects * 2 + 1)*(yObjects * 2 + 1)*(zObjects * 2 + 1);
	m_ppObjects = new CGameObject*[m_nObjects];

	float fxPitch = 12.0f * 2.5f;
	float fyPitch = 12.0f * 2.5f;
	float fzPitch = 12.0f * 2.5f;
	CRotatingObject *pObject = NULL;
	for (int i = 0, z = +zObjects; z >= -zObjects; z--)
	{
		for (int y = -yObjects; y <= yObjects; y++)
		{
			for (int x = -xObjects; x <= xObjects; x++)
			{
				pObject = new CRotatingObject();
				pObject->SetMesh(pMesh);
				pObject->SetShader(pShader);
				//각 정육면체 객체의 위치를 설정한다.
				pObject->SetPosition(fxPitch*x, fyPitch*y, fzPitch*z);
				m_ppObjects[i++] = pObject;
			}
		}
	}
}

void CScene::ReleaseObjects()
{
	//게임 객체 리스트의 각 객체를 반환(Release)하고 리스트를 소멸시킨다.
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) m_ppObjects[j]->Release();
		delete[] m_ppObjects;
	}
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CScene::ProcessInput()
{
	return(false);
}

void CScene::AnimateObjects(float fTimeElapsed)
{
	for (int i = 0; i < m_nObjects; i++) m_ppObjects[i]->Animate(fTimeElapsed);
}

void CScene::Render(ID3D11DeviceContext*pd3dDeviceContext, CCamera *pCamera)
{
	for (int i = 0; i < m_nObjects; i++)
	{
		m_ppObjects[i]->Render(pd3dDeviceContext);
	}
}
