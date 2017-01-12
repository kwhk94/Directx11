#include "stdafx.h"
#include "Mesh.h"


void AABB::Merge(D3DXVECTOR3& d3dxvMinimum, D3DXVECTOR3& d3dxvMaximum)
{
	if (d3dxvMinimum.x < m_d3dxvMinimum.x) m_d3dxvMinimum.x = d3dxvMinimum.x;
	if (d3dxvMinimum.y < m_d3dxvMinimum.y) m_d3dxvMinimum.y = d3dxvMinimum.y;
	if (d3dxvMinimum.z < m_d3dxvMinimum.z) m_d3dxvMinimum.z = d3dxvMinimum.z;
	if (d3dxvMaximum.x > m_d3dxvMaximum.x) m_d3dxvMaximum.x = d3dxvMaximum.x;
	if (d3dxvMaximum.y > m_d3dxvMaximum.y) m_d3dxvMaximum.y = d3dxvMaximum.y;
	if (d3dxvMaximum.z > m_d3dxvMaximum.z) m_d3dxvMaximum.z = d3dxvMaximum.z;
}

void AABB::Merge(AABB *pAABB)
{
	Merge(pAABB->m_d3dxvMinimum, pAABB->m_d3dxvMaximum);
}

void AABB::Update(D3DXMATRIX *pmtxTransform)
{
	/*바운딩 박스의 최소점과 최대점은 회전을 하면 더 이상 최소점과 최대점이 되지 않는다. 그러므로 바운딩 박스의 최소점과 최대점에서 8개의 정점을 구하고 변환(회전)을 한 다음 최소점과 최대점을 다시 계산한다.*/
	D3DXVECTOR3 vVertices[8];
	vVertices[0] = D3DXVECTOR3(m_d3dxvMinimum.x, m_d3dxvMinimum.y, m_d3dxvMinimum.z);
	vVertices[1] = D3DXVECTOR3(m_d3dxvMinimum.x, m_d3dxvMinimum.y, m_d3dxvMaximum.z);
	vVertices[2] = D3DXVECTOR3(m_d3dxvMaximum.x, m_d3dxvMinimum.y, m_d3dxvMaximum.z);
	vVertices[3] = D3DXVECTOR3(m_d3dxvMaximum.x, m_d3dxvMinimum.y, m_d3dxvMinimum.z);
	vVertices[4] = D3DXVECTOR3(m_d3dxvMinimum.x, m_d3dxvMaximum.y, m_d3dxvMinimum.z);
	vVertices[5] = D3DXVECTOR3(m_d3dxvMinimum.x, m_d3dxvMaximum.y, m_d3dxvMaximum.z);
	vVertices[6] = D3DXVECTOR3(m_d3dxvMaximum.x, m_d3dxvMaximum.y, m_d3dxvMaximum.z);
	vVertices[7] = D3DXVECTOR3(m_d3dxvMaximum.x, m_d3dxvMaximum.y, m_d3dxvMinimum.z);
	m_d3dxvMinimum = D3DXVECTOR3(+FLT_MAX, +FLT_MAX, +FLT_MAX);
	m_d3dxvMaximum = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	//8개의 정점에서 x, y, z 좌표의 최소값과 최대값을 구한다.
	for (int i = 0; i < 8; i++)
	{
		//정점을 변환한다.
		D3DXVec3TransformCoord(&vVertices[i], &vVertices[i], pmtxTransform);
		if (vVertices[i].x < m_d3dxvMinimum.x) m_d3dxvMinimum.x = vVertices[i].x;
		if (vVertices[i].y < m_d3dxvMinimum.y) m_d3dxvMinimum.y = vVertices[i].y;
		if (vVertices[i].z < m_d3dxvMinimum.z) m_d3dxvMinimum.z = vVertices[i].z;
		if (vVertices[i].x > m_d3dxvMaximum.x) m_d3dxvMaximum.x = vVertices[i].x;
		if (vVertices[i].y > m_d3dxvMaximum.y) m_d3dxvMaximum.y = vVertices[i].y;
		if (vVertices[i].z > m_d3dxvMaximum.z) m_d3dxvMaximum.z = vVertices[i].z;
	}
}



CMesh::CMesh(ID3D11Device *pd3dDevice)
{
	m_nBuffers = 0;
	m_pd3dPositionBuffer = NULL;
	m_pd3dColorBuffer = NULL;
	m_ppd3dVertexBuffers = NULL;

	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_nSlot = 0;
	m_nStartVertex = 0;

	m_pd3dIndexBuffer = NULL;
	m_nIndices = 0;
	m_nStartIndex = 0;
	m_nBaseVertex = 0;
	m_nIndexOffset = 0;
	m_dxgiIndexFormat = DXGI_FORMAT_R32_UINT;

	m_pd3dRasterizerState = NULL;

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(+FLT_MAX, +FLT_MAX, +FLT_MAX);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	m_nReferences = 0;
}

CMesh::~CMesh()
{
	if (m_pd3dRasterizerState) m_pd3dRasterizerState->Release();
	if (m_pd3dPositionBuffer) m_pd3dPositionBuffer->Release();
	if (m_pd3dColorBuffer) m_pd3dColorBuffer->Release();
	if (m_pd3dIndexBuffer) m_pd3dIndexBuffer->Release();

	if (m_ppd3dVertexBuffers) delete[] m_ppd3dVertexBuffers;
	if (m_pnVertexStrides) delete[] m_pnVertexStrides;
	if (m_pnVertexOffsets) delete[] m_pnVertexOffsets;
}

void CMesh::AssembleToVertexBuffer(int nBuffers, ID3D11Buffer **ppd3dBuffers, UINT *pnBufferStrides, UINT *pnBufferOffsets)
{
	ID3D11Buffer **ppd3dNewVertexBuffers = new ID3D11Buffer*[m_nBuffers + nBuffers];
	UINT *pnNewVertexStrides = new UINT[m_nBuffers + nBuffers];
	UINT *pnNewVertexOffsets = new UINT[m_nBuffers + nBuffers];

	if (m_nBuffers > 0)
	{
		for (int i = 0; i < m_nBuffers; i++)
		{
			ppd3dNewVertexBuffers[i] = m_ppd3dVertexBuffers[i];
			pnNewVertexStrides[i] = m_pnVertexStrides[i];
			pnNewVertexOffsets[i] = m_pnVertexOffsets[i];
		}
		if (m_ppd3dVertexBuffers) delete[] m_ppd3dVertexBuffers;
		if (m_pnVertexStrides) delete[] m_pnVertexStrides;
		if (m_pnVertexOffsets) delete[] m_pnVertexOffsets;
	}

	for (int i = 0; i < nBuffers; i++)
	{
		ppd3dNewVertexBuffers[m_nBuffers + i] = ppd3dBuffers[i];
		pnNewVertexStrides[m_nBuffers + i] = pnBufferStrides[i];
		pnNewVertexOffsets[m_nBuffers + i] = pnBufferOffsets[i];
	}

	m_nBuffers += nBuffers;
	m_ppd3dVertexBuffers = ppd3dNewVertexBuffers;
	m_pnVertexStrides = pnNewVertexStrides;
	m_pnVertexOffsets = pnNewVertexOffsets;
}

void CMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	//메쉬의 정점은 여러 개의 정점 버퍼로 표현된다.
	pd3dDeviceContext->IASetVertexBuffers(m_nSlot, m_nBuffers, m_ppd3dVertexBuffers, m_pnVertexStrides, m_pnVertexOffsets);
	pd3dDeviceContext->IASetIndexBuffer(m_pd3dIndexBuffer, m_dxgiIndexFormat, m_nIndexOffset);
	pd3dDeviceContext->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dDeviceContext->RSSetState(m_pd3dRasterizerState);

	if (m_pd3dIndexBuffer)
		pd3dDeviceContext->DrawIndexed(m_nIndices, m_nStartIndex, m_nBaseVertex);
	else
		pd3dDeviceContext->Draw(m_nVertices, m_nStartVertex);
}
void CMesh::RenderInstanced(ID3D11DeviceContext *pd3dDeviceContext, int nInstances, int nStartInstance)
{
	//인스턴싱의 경우 입력 조립기에 메쉬의 정점 버퍼와 인스턴스 정점 버퍼가 연결된다.
	pd3dDeviceContext->IASetVertexBuffers(m_nSlot, m_nBuffers, m_ppd3dVertexBuffers, m_pnVertexStrides, m_pnVertexOffsets);
	pd3dDeviceContext->IASetIndexBuffer(m_pd3dIndexBuffer, m_dxgiIndexFormat, m_nIndexOffset);
	pd3dDeviceContext->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dDeviceContext->RSSetState(m_pd3dRasterizerState);

	//객체들의 인스턴스들을 렌더링한다. 
	if (m_pd3dIndexBuffer)
		pd3dDeviceContext->DrawIndexedInstanced(m_nIndices, nInstances, m_nStartIndex, m_nBaseVertex, nStartInstance);
	else
		pd3dDeviceContext->DrawInstanced(m_nVertices, nInstances, m_nStartVertex, nStartInstance);
}



CTriangleMesh::CTriangleMesh(ID3D11Device *pd3dDevice) : CMesh(pd3dDevice)
{
	m_nVertices = 3;
	//m_nStride = sizeof(CDiffusedVertex);
	//m_nOffset = 0;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	/*정점(삼각형의 꼭지점)의 색상은 시계방향 순서대로 빨간색, 녹색, 파란색으로 지정한다. D3DXCOLOR 매크로는 RGBA(Red, Green, Blue, Alpha) 4개의 파라메터를 사용하여 색상을 표현하기 위하여 사용한다. 각 파라메터는 0.0~1.0 사이의 실수값을 가진다.*/
	CDiffusedVertex pVertices[3];
	pVertices[0] = CDiffusedVertex(D3DXVECTOR3(0.0f, 0.5f, 0.0f), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
	pVertices[1] = CDiffusedVertex(D3DXVECTOR3(0.5f, -0.5f, 0.0f), D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f));
	pVertices[2] = CDiffusedVertex(D3DXVECTOR3(-0.5f, -0.5f, 0.0f), D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f));

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//d3dBufferDesc.ByteWidth = m_nStride * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pVertices;
	//pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dVertexBuffer);

	CreateRasterizerState(pd3dDevice);

}


CTriangleMesh::~CTriangleMesh()
{
}


void CTriangleMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CMesh::Render(pd3dDeviceContext);
}

void CTriangleMesh::CreateRasterizerState(ID3D11Device *pd3dDevice)
{
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	//래스터라이저 단계에서 컬링(은면 제거)을 하지 않도록 래스터라이저 상태를 생성한다.
	d3dRasterizerDesc.CullMode = D3D11_CULL_BACK;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}

CCubeMesh::CCubeMesh(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth, D3DXCOLOR d3dxColor) : CMesh(pd3dDevice)
{
	m_nVertices = 8;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	//직육면체 메쉬는 2개의 정점 버퍼(위치 벡터 버퍼와 색상 버퍼)로 구성된다.
	//직육면체 메쉬의 정점 버퍼(위치 벡터 버퍼)를 생성한다.
	D3DXVECTOR3 pd3dxvPositions[8];
	pd3dxvPositions[0] = D3DXVECTOR3(-fx, +fy, -fz);
	pd3dxvPositions[1] = D3DXVECTOR3(+fx, +fy, -fz);
	pd3dxvPositions[2] = D3DXVECTOR3(+fx, +fy, +fz);
	pd3dxvPositions[3] = D3DXVECTOR3(-fx, +fy, +fz);
	pd3dxvPositions[4] = D3DXVECTOR3(-fx, -fy, -fz);
	pd3dxvPositions[5] = D3DXVECTOR3(+fx, -fy, -fz);
	pd3dxvPositions[6] = D3DXVECTOR3(+fx, -fy, +fz);
	pd3dxvPositions[7] = D3DXVECTOR3(-fx, -fy, +fz);

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pd3dxvPositions;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);

	//직육면체 메쉬의 정점 버퍼(색상 버퍼)를 생성한다.
	D3DXCOLOR pd3dxColors[8];
	for (int i = 0; i < 8; i++) pd3dxColors[i] = d3dxColor + RANDOM_COLOR;

	d3dBufferDesc.ByteWidth = sizeof(D3DXCOLOR) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxColors;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dColorBuffer);

	ID3D11Buffer *pd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dColorBuffer };
	UINT pnBufferStrides[2] = { sizeof(D3DXVECTOR3), sizeof(D3DXCOLOR) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer(2, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	m_nIndices = 18;
	UINT pIndices[18];
	pIndices[0] = 5; //5,6,4 - cw
	pIndices[1] = 6; //6,4,7 - ccw
	pIndices[2] = 4; //4,7,0 - cw
	pIndices[3] = 7; //7,0,3 - ccw
	pIndices[4] = 0; //0,3,1 - cw
	pIndices[5] = 3; //3,1,2 - ccw
	pIndices[6] = 1; //1,2,2 - cw 
	pIndices[7] = 2; //2,2,3 - ccw
	pIndices[8] = 2; //2,3,3 - cw  - Degenerated Index(2)
	pIndices[9] = 3; //3,3,7 - ccw - Degenerated Index(3)
	pIndices[10] = 3;//3,7,2 - cw  - Degenerated Index(3)
	pIndices[11] = 7;//7,2,6 - ccw
	pIndices[12] = 2;//2,6,1 - cw
	pIndices[13] = 6;//6,1,5 - ccw
	pIndices[14] = 1;//1,5,0 - cw
	pIndices[15] = 5;//5,0,4 - ccw
	pIndices[16] = 0;
	pIndices[17] = 4;

	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(UINT) * m_nIndices;
	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pIndices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer);

	CreateRasterizerState(pd3dDevice);

	//정점 버퍼 데이터를 생성한 다음 최소점과 최대점을 저장한다. 
	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-fx, -fy, -fz);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(+fx, +fy, +fz);

}

CCubeMesh::~CCubeMesh()
{
}

void CCubeMesh::CreateRasterizerState(ID3D11Device *pd3dDevice)
{
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	/*D3D11_CULL_NONE은 은면 제거를 하지 않음을 나타낸다. 즉, 모든 프리미티브를 그린다. D3D11_FILL_WIREFRAME은 프리미티브를 선분으로만 그린다.*/
	d3dRasterizerDesc.CullMode = D3D11_CULL_NONE;

	/*은면 제거를 하지 않고 그리면 위의 두 가지 경우(16개의 인덱스를 사용한 경우와 18개의 인덱스를 사용한 경우)의 출력의 결과가 같을 것이다. 하지만 은면 제거를 하도록 하면 18개의 인덱스를 사용한 경우에만 제대로 그려질 것이다.*/
	//d3dRasterizerDesc.CullMode = D3D11_CULL_BACK;

	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}

void CCubeMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CMesh::Render(pd3dDeviceContext);
}
CAirplaneMesh::CAirplaneMesh(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth, D3DXCOLOR d3dxColor) : CMesh(pd3dDevice)
{
	m_nVertices = 24 * 3;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	//비행기 메쉬는 2개의 정점 버퍼(위치 벡터 버퍼와 색상 버퍼)로 구성된다.
	D3DXVECTOR3 pd3dxvPositions[24 * 3];

	float x1 = fx * 0.2f, y1 = fy * 0.2f, x2 = fx * 0.1f, y3 = fy * 0.3f, y2 = ((y1 - (fy - y3)) / x1) * x2 + (fy - y3);
	int i = 0;
	//Upper Plane
	pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +(fy + y3), -fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -y1, -fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, 0.0f, -fz);

	pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +(fy + y3), -fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, 0.0f, -fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -y1, -fz);

	pd3dxvPositions[i++] = D3DXVECTOR3(+x2, +y2, -fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -y3, -fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -y1, -fz);

	pd3dxvPositions[i++] = D3DXVECTOR3(-x2, +y2, -fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -y1, -fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -y3, -fz);

	//Lower Plane
	pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +(fy + y3), +fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, 0.0f, +fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -y1, +fz);

	pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +(fy + y3), +fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -y1, +fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, 0.0f, +fz);

	pd3dxvPositions[i++] = D3DXVECTOR3(+x2, +y2, +fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -y1, +fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -y3, +fz);

	pd3dxvPositions[i++] = D3DXVECTOR3(-x2, +y2, +fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -y3, +fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -y1, +fz);

	//Right Plane
	pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +(fy + y3), -fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +(fy + y3), +fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(+x2, +y2, -fz);

	pd3dxvPositions[i++] = D3DXVECTOR3(+x2, +y2, -fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +(fy + y3), +fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(+x2, +y2, +fz);

	pd3dxvPositions[i++] = D3DXVECTOR3(+x2, +y2, -fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(+x2, +y2, +fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -y3, -fz);

	pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -y3, -fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(+x2, +y2, +fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -y3, +fz);

	//Back/Right Plane
	pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -y1, -fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -y3, -fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -y3, +fz);

	pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -y1, -fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -y3, +fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -y1, +fz);

	pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, 0.0f, -fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -y1, -fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -y1, +fz);

	pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, 0.0f, -fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -y1, +fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, 0.0f, +fz);

	//Left Plane
	pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +(fy + y3), +fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +(fy + y3), -fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(-x2, +y2, -fz);

	pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +(fy + y3), +fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(-x2, +y2, -fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(-x2, +y2, +fz);

	pd3dxvPositions[i++] = D3DXVECTOR3(-x2, +y2, +fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(-x2, +y2, -fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -y3, -fz);

	pd3dxvPositions[i++] = D3DXVECTOR3(-x2, +y2, +fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -y3, -fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -y3, +fz);

	//Back/Left Plane
	pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, 0.0f, -fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, 0.0f, +fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -y1, +fz);

	pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, 0.0f, -fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -y1, +fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -y1, -fz);

	pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -y1, -fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -y1, +fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -y3, +fz);

	pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -y1, -fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -y3, +fz);
	pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -y3, -fz);

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pd3dxvPositions;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);

	D3DXCOLOR pd3dxColors[24 * 3];
	for (int j = 0; j < m_nVertices; j++) pd3dxColors[j] = d3dxColor + RANDOM_COLOR;
	pd3dxColors[0] = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	pd3dxColors[3] = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	pd3dxColors[6] = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	pd3dxColors[9] = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	pd3dxColors[12] = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	pd3dxColors[15] = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	pd3dxColors[18] = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	pd3dxColors[21] = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);

	d3dBufferDesc.ByteWidth = sizeof(D3DXCOLOR) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxColors;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dColorBuffer);

	ID3D11Buffer *ppd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dColorBuffer };
	UINT pnBufferStrides[2] = { sizeof(D3DXVECTOR3), sizeof(D3DXCOLOR) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer(2, ppd3dBuffers, pnBufferStrides, pnBufferOffsets);

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-fx, -fy, -fz);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(+fx, +fy, +fz);
}

CAirplaneMesh::~CAirplaneMesh()
{
}

CSphereMesh::CSphereMesh(ID3D11Device *pd3dDevice, float fRadius, int nSlices, int nStacks, D3DXCOLOR d3dxColor) : CMesh(pd3dDevice)
{
	m_nVertices = (nSlices * nStacks) * 3 * 2;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	//구 메쉬는 2개의 정점 버퍼(위치 벡터 버퍼와 색상 버퍼)로 구성된다.
	//구 메쉬의 정점 버퍼(위치 벡터 버퍼)를 생성한다.
	D3DXVECTOR3 *pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	float theta_i, theta_ii, phi_j, phi_jj, fRadius_j, fRadius_jj, y_j, y_jj;
	for (int j = 0, k = 0; j < nStacks; j++)
	{
		phi_j = float(D3DX_PI / nStacks) * j;
		phi_jj = float(D3DX_PI / nStacks) * (j + 1);
		fRadius_j = fRadius * sinf(phi_j);
		fRadius_jj = fRadius * sinf(phi_jj);
		y_j = fRadius*cosf(phi_j);
		y_jj = fRadius*cosf(phi_jj);
		for (int i = 0; i < nSlices; i++)
		{
			theta_i = float(2 * D3DX_PI / nSlices) * i;
			theta_ii = float(2 * D3DX_PI / nSlices) * (i + 1);
			pd3dxvPositions[k++] = D3DXVECTOR3(fRadius_j*cosf(theta_i), y_j, fRadius_j*sinf(theta_i));
			pd3dxvPositions[k++] = D3DXVECTOR3(fRadius_jj*cosf(theta_i), y_jj, fRadius_jj*sinf(theta_i));
			pd3dxvPositions[k++] = D3DXVECTOR3(fRadius_j*cosf(theta_ii), y_j, fRadius_j*sinf(theta_ii));
			pd3dxvPositions[k++] = D3DXVECTOR3(fRadius_jj*cosf(theta_i), y_jj, fRadius_jj*sinf(theta_i));
			pd3dxvPositions[k++] = D3DXVECTOR3(fRadius_jj*cosf(theta_ii), y_jj, fRadius_jj*sinf(theta_ii));
			pd3dxvPositions[k++] = D3DXVECTOR3(fRadius_j*cosf(theta_ii), y_j, fRadius_j*sinf(theta_ii));
		}
	}

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pd3dxvPositions;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);

	delete[] pd3dxvPositions;

	//구 메쉬의 정점 버퍼(색상 버퍼)를 생성한다.
	D3DXCOLOR *pd3dxColors = new D3DXCOLOR[m_nVertices];
	for (int i = 0; i < m_nVertices; i++) pd3dxColors[i] = d3dxColor + RANDOM_COLOR;

	d3dBufferDesc.ByteWidth = sizeof(D3DXCOLOR) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxColors;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dColorBuffer);

	delete[] pd3dxColors;

	ID3D11Buffer *pd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dColorBuffer };
	UINT pnBufferStrides[2] = { sizeof(D3DXVECTOR3), sizeof(D3DXCOLOR) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer(2, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-fRadius, -fRadius, -fRadius);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(+fRadius, +fRadius, +fRadius);
}


CSphereMesh::~CSphereMesh()
{
}

void CSphereMesh::CreateRasterizerState(ID3D11Device *pd3dDevice)
{
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	d3dRasterizerDesc.CullMode = D3D11_CULL_BACK;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	d3dRasterizerDesc.DepthClipEnable = true;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}


