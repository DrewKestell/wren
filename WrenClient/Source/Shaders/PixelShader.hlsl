struct PixelInput
{
	float4 Pos : SV_POSITION;
	float4 Color : COLOR;
};

float4 main(PixelInput pin) : SV_TARGET
{
    return pin.Color;
}