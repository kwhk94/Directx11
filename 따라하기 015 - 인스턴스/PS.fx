struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR0;
};
struct VS_INSTANCED_COLOR_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR0;
	//시스템 생성 변수로 정점 쉐이더에 전달되는 객체 인스턴스의 ID를 픽셀 쉐이더로 전달한다.
	float4 instanceID : INDEX;
};

float4 PS(VS_OUTPUT input) : SV_Target
{
	return input.color;
}
float4 PSInstancedDiffusedColor(VS_INSTANCED_COLOR_OUTPUT input) : SV_Target
{
	return input.color;
}