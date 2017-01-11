#include "stdafx.h"
#include "Shader.h"

//월드 변환 행렬을 위한 상수 버퍼는 쉐이더 객체의 정적(static) 데이터 멤버이다.
ID3D11Buffer *CShader::m_pd3dcbWorldMatrix = NULL;

BYTE *ReadCompiledEffectFile(WCHAR *pszFileName, int *pnReadBytes)
{
	FILE *pFile = NULL;
	::_wfopen_s(&pFile, pszFileName, L"rb");
	::fseek(pFile, 0, SEEK_END);
	int nFileSize = ::ftell(pFile);
	BYTE *pByteCode = new BYTE[nFileSize];
	::rewind(pFile);
	*pnReadBytes = ::fread(pByteCode, sizeof(BYTE), nFileSize, pFile);
	::fclose(pFile);
	return(pByteCode);
}
//컴파일된 쉐이더 코드에서 정점 쉐이더를 생성한다.
void CShader::CreateVertexShaderFromCompiledFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, D3D11_INPUT_ELEMENT_DESC *pd3dInputLayout, UINT nElements)
{
	int nReadBytes = 0;
	BYTE *pByteCode = ReadCompiledEffectFile(pszFileName, &nReadBytes);
	HRESULT hResult = pd3dDevice->CreateVertexShader(pByteCode, nReadBytes, NULL, &m_pd3dVertexShader);
	pd3dDevice->CreateInputLayout(pd3dInputLayout, nElements, pByteCode, nReadBytes, &m_pd3dVertexLayout);

	if (pByteCode) delete[] pByteCode;
}

//컴파일된 쉐이더 코드에서 픽셀 쉐이더를 생성한다.
void CShader::CreatePixelShaderFromCompiledFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName)
{
	int nReadBytes = 0;
	BYTE *pByteCode = ReadCompiledEffectFile(pszFileName, &nReadBytes);
	HRESULT hResult = pd3dDevice->CreatePixelShader(pByteCode, nReadBytes, NULL, &m_pd3dPixelShader);

	if (pByteCode) delete[] pByteCode;
}

void CShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputLayout);
	CreateVertexShaderFromCompiledFile(pd3dDevice, L"VS.fxo", d3dInputLayout, nElements);
	CreatePixelShaderFromCompiledFile(pd3dDevice, L"PS.fxo");
	//컴파일된 쉐이더 코드의 이름이 VS.fxo와 PS.fxo이다.

	CreateShaderVariables(pd3dDevice);
}


void CShader::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	//월드 변환 행렬을 위한 상수 버퍼를 생성한다.
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VS_CB_WORLD_MATRIX);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&bd, NULL, &m_pd3dcbWorldMatrix);
}

void CShader::ReleaseShaderVariables()
{
	//월드 변환 행렬을 위한 상수 버퍼를 반환한다.
	if (m_pd3dcbWorldMatrix) m_pd3dcbWorldMatrix->Release();
}

void CShader::UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, D3DXMATRIX *pd3dxmtxWorld)
{
	//월드 변환 행렬을 상수 버퍼에 복사한다.
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbWorldMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_CB_WORLD_MATRIX *pcbWorldMatrix = (VS_CB_WORLD_MATRIX *)d3dMappedResource.pData;
	D3DXMatrixTranspose(&pcbWorldMatrix->m_d3dxmtxWorld, pd3dxmtxWorld);
	pd3dDeviceContext->Unmap(m_pd3dcbWorldMatrix, 0);

	//상수 버퍼를 디바이스의 슬롯(VS_SLOT_WORLD_MATRIX)에 연결한다.
	pd3dDeviceContext->VSSetConstantBuffers(VS_SLOT_WORLD_MATRIX, 1, &m_pd3dcbWorldMatrix);
}

CShader::CShader()
{
	m_ppObjects = NULL;
	m_nObjects = 0;

	m_pd3dVertexShader = NULL;
	m_pd3dVertexLayout = NULL;
	m_pd3dPixelShader = NULL;
}

CShader::~CShader()
{
	if (m_pd3dVertexShader) m_pd3dVertexShader->Release();
	if (m_pd3dVertexLayout) m_pd3dVertexLayout->Release();
	if (m_pd3dPixelShader) m_pd3dPixelShader->Release();
}


void CShader::BuildObjects(ID3D11Device *pd3dDevice)
{
}

void CShader::ReleaseObjects()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j]) delete m_ppObjects[j];
		delete[] m_ppObjects;
	}
}

void CShader::AnimateObjects(float fTimeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(fTimeElapsed);
	}
}

void CShader::OnPrepareRender(ID3D11DeviceContext *pd3dDeviceContext)
{
	pd3dDeviceContext->IASetInputLayout(m_pd3dVertexLayout);
	pd3dDeviceContext->VSSetShader(m_pd3dVertexShader, NULL, 0);
	pd3dDeviceContext->PSSetShader(m_pd3dPixelShader, NULL, 0);
}

void CShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	OnPrepareRender(pd3dDeviceContext);
	//쉐이더 객체의 모드 게임 객체들을 렌더링한다.
	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j])
		{
			m_ppObjects[j]->Render(pd3dDeviceContext);
		}
	}
}

CSceneShader::CSceneShader()
{
}

CSceneShader::~CSceneShader()
{
}

void CSceneShader::CreateShader(ID3D11Device *pd3dDevice)
{
	CShader::CreateShader(pd3dDevice);
}
void CSceneShader::BuildObjects(ID3D11Device *pd3dDevice)
{
	int xObjects = 6, yObjects = 6, zObjects = 6, i = 0;
	/*정육면체 객체를 x-축, y-축, z-축으로 각각 13개씩 총 2,197개(13x13x13) 생성하고 구 객체를 16개 생성한다.*/  
		m_nObjects = (xObjects * 2 + 1)*(yObjects * 2 + 1)*(zObjects * 2 + 1) + (8 * 2);
	m_ppObjects = new CGameObject*[m_nObjects];

	//정육면체 메쉬를 생성한다.
	CCubeMesh *pCubeMesh = new CCubeMesh(pd3dDevice, 12.0f, 12.0f, 12.0f, D3DCOLOR_XRGB(0, 0, 128));

	float fxPitch = 12.0f * 1.7f;
	float fyPitch = 12.0f * 1.7f;
	float fzPitch = 12.0f * 1.7f;
	CRotatingObject *pRotatingObject = NULL;
	for (int x = -xObjects; x <= xObjects; x++)
	{
		for (int y = -yObjects; y <= yObjects; y++)
		{
			for (int z = -zObjects; z <= zObjects; z++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(pCubeMesh);
				pRotatingObject->SetPosition(fxPitch*x, fyPitch*y, fzPitch*z);
				pRotatingObject->SetRotationAxis(D3DXVECTOR3(0.0f, 1.0f, 0.0f));
				pRotatingObject->SetRotationSpeed(10.0f*(i % 10));
				m_ppObjects[i++] = pRotatingObject;
			}
		}
	}

	//구 메쉬를 생성한다.
	CSphereMesh *pSphereMesh = new CSphereMesh(pd3dDevice, 12.0f, 20, 20, D3DCOLOR_XRGB(64, 0, 0));

	/*구 객체는 공전하는 객체이다. 8개는 월드좌표계의 xy-평면에서 원점을 중심으로 45도 간격으로 위치한다. z-축으로 회전한다. 8개는 월드좌표계의 yz-평면에서 원점을 중심으로 45도 간격으로 위치한다. x-축으로 회전한다.*/
	CRevolvingObject *pRevolvingObject = NULL;
	float fRadius = (xObjects * fxPitch * 2.0f) * 1.5f;
	for (int j = 0; j < 8; j++)
	{
		pRevolvingObject = new CRevolvingObject();
		pRevolvingObject->SetMesh(pSphereMesh);
		pRevolvingObject->SetPosition(fRadius*cosf(D3DXToRadian(45.0f)*j), fRadius*sinf(D3DXToRadian(45.0f)*j), 0.0f);
		pRevolvingObject->SetRevolutionAxis(D3DXVECTOR3(0.0f, 0.0f, 1.0f));
		pRevolvingObject->SetRevolutionSpeed(10.0f);
		m_ppObjects[i++] = pRevolvingObject;
	}
	for (int j = 0; j < 8; j++)
	{
		pRevolvingObject = new CRevolvingObject();
		pRevolvingObject->SetMesh(pSphereMesh);
		pRevolvingObject->SetPosition(0.0f, fRadius*cosf(D3DXToRadian(45.0f)*j), fRadius*sinf(D3DXToRadian(45.0f)*j));
		pRevolvingObject->SetRevolutionAxis(D3DXVECTOR3(1.0f, 0.0f, 0.0f));
		pRevolvingObject->SetRevolutionSpeed(20.0f);
		m_ppObjects[i++] = pRevolvingObject;
	}
}
CPlayerShader::CPlayerShader()
{
}

CPlayerShader::~CPlayerShader()
{
}

void CPlayerShader::CreateShader(ID3D11Device *pd3dDevice)
{
	CShader::CreateShader(pd3dDevice);
}

void CPlayerShader::BuildObjects(ID3D11Device *pd3dDevice)
{
	m_nObjects = 1;
	m_ppObjects = new CGameObject*[m_nObjects];

	ID3D11DeviceContext *pd3dDeviceContext = NULL;
	pd3dDevice->GetImmediateContext(&pd3dDeviceContext);

	CMesh *pAirplaneMesh = new CAirplaneMesh(pd3dDevice, 20.0f, 20.0f, 4.0f, D3DCOLOR_XRGB(0, 255, 0));
	CAirplanePlayer *pAirplanePlayer = new CAirplanePlayer(pd3dDevice);
	pAirplanePlayer->SetMesh(pAirplaneMesh);
	pAirplanePlayer->CreateShaderVariables(pd3dDevice);
	pAirplanePlayer->ChangeCamera(pd3dDevice, SPACESHIP_CAMERA, 0.0f);

	CCamera *pCamera = pAirplanePlayer->GetCamera();
	pCamera->SetViewport(pd3dDeviceContext, 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
	pCamera->GenerateProjectionMatrix(1.01f, 50000.0f, ASPECT_RATIO, 60.0f);
	pCamera->GenerateViewMatrix();

	pAirplanePlayer->SetPosition(D3DXVECTOR3(0.0f, 10.0f, -50.0f));
	m_ppObjects[0] = pAirplanePlayer;

	if (pd3dDeviceContext) pd3dDeviceContext->Release();
}

void CPlayerShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	//3인칭 카메라일 때 플레이어를 렌더링한다.
	DWORD nCameraMode = (pCamera) ? pCamera->GetMode() : 0x00;
	if (nCameraMode == THIRD_PERSON_CAMERA)
	{
		CShader::Render(pd3dDeviceContext, pCamera);
	}
}


