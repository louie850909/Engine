#include "instancingBillboard.hlsli"

Texture2D sprite_texture : register(t0);
SamplerState point_sampler : register(s0);
SamplerState linear_sampler : register(s1);
SamplerState anisotropic_sampler : register(s2);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = sprite_texture.Sample(linear_sampler, pin.texcoord) * pin.color;
    if(color.a < 0.1f) 
        discard;
    return color;
}