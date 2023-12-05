struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
};
float4 main(VS_OUTPUT input) : SV_Target
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}
