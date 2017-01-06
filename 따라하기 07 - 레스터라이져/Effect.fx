//카메라 변환 행렬과 투영 변환 행렬을 위한 쉐이더 변수를 선언한다(슬롯 0을 사용).
cbuffer cbViewProjectionMatrix : register(b0)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
};

//월드 변환 행렬을 위한 쉐이더 변수를 선언한다(슬롯 1을 사용). 
cbuffer cbWorldMatrix : register(b1)
{
	matrix gmtxWorld : packoffset(c0);
};

/*(주의) register(b0)에서 b는 레지스터가 상수 버퍼를 위해 사용되는 것을 의미한다. 0는 레지스터의 번호이며 응용 프로그램에서 상수 버퍼를 디바이스 컨텍스트에 연결할 때의 슬롯 번호와 일치하도록 해야 한다.
pd3dDeviceContext->VSSetConstantBuffers(VS_SLOT_WORLD_MATRIX, 1, &m_pd3dcbWorldMatrix);
*/
//정점-쉐이더의 출력을 위한 구조체이다.
struct VS_OUTPUT
{
	float4 position : SV_POSITION;
};

/*정점-쉐이더이다. 정점의 위치 벡터를 월드 변환, 카메라 변환, 투영 변환을 순서대로 수행한다. 이제 삼각형의 각 정점은 y-축으로의 회전을 나타내는 행렬에 따라 변환한다. 그러므로 삼각형은 회전하게 된다.*/
VS_OUTPUT VS(float4 position : POSITION)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.position = mul(position, gmtxWorld);
	output.position = mul(output.position, gmtxView);
	output.position = mul(output.position, gmtxProjection);
	return output;
}











float4 PS(float4 Pos : SV_POSITION) : SV_Target
{
	/*픽셀-쉐이더는 항상 색상을 나타내는 4개의 실수(float4)를 반환해야 한다. 입력 파라메터에 관계없이 노란색 색상을 반환한다. 그러므로 렌더링의 대상이 되는 모든 픽셀의 색상은 노란색이 된다(노란색 = 빨간색 + 녹색).*/
	return float4(1.0f, 1.0f, 0.0f, 1.0f);    // R=1.0f, G=1.0f, B=0.0f, A=1.0f, 
}


