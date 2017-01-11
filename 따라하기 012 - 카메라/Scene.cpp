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

	//가로x세로x높이가 12x12x12인 정육면체 메쉬를 생성한다(기본 색상은 파란색).
	CCubeMesh *pCubeMesh = new CCubeMesh(pd3dDevice, 12.0f, 12.0f, 12.0f, D3DCOLOR_XRGB(0, 0, 128));

	int xObjects = 5, yObjects = 5, zObjects = 5, i = 0;
	/*<그림 21>과 같이 x-축, y-축, z-축으로 11개씩 총 11 x 11 x 11 = 1,331개의 정육면체를 배치한다. x-축, y-축, z-축에서 원점의 왼쪽으로 5개, 오른쪽으로 5개, 축 위에 1개 배치한다.*/
	m_nObjects = (xObjects * 2 + 1)*(yObjects * 2 + 1)*(zObjects * 2 + 1);
	m_ppObjects = new CGameObject*[m_nObjects];

	float fxPitch = 12.0f * 2.5f;
	float fyPitch = 12.0f * 2.5f;
	float fzPitch = 12.0f * 2.5f;
	//정육면체 객체는 자전한다.
	CRotatingObject *pRotatingObject = NULL;
	for (int x = -xObjects; x <= xObjects; x++)
	{
		for (int y = -yObjects; y <= yObjects; y++)
		{
			for (int z = -zObjects; z <= zObjects; z++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(pCubeMesh);
				pRotatingObject->SetShader(pShader);
				//각 정육면체 객체의 위치를 설정한다.
				pRotatingObject->SetPosition(fxPitch*x, fyPitch*y, fzPitch*z);
				//각 정육면체 객체의 회전축을 y-축으로 설정한다.
				pRotatingObject->SetRotationAxis(D3DXVECTOR3(0.0f, 1.0f, 0.0f));
				pRotatingObject->SetRotationSpeed(30.0f + 10.0f*(i % 10));
				m_ppObjects[i++] = pRotatingObject;
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
