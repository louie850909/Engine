#include "particle.hlsli"

Texture2D sprite_texture : register(t0);
SamplerState point_sampler : register(s0);
SamplerState linear_sampler : register(s1);
SamplerState anisotropic_sampler : register(s2);

float4 main(VS_OUT pin) : SV_TARGET
{
    if(!pin.instUse)
        discard;
    
    float4 color = sprite_texture.Sample(linear_sampler, pin.texcoord) * pin.color;
    if (color.r == 0 && color.g == 0 && color.b == 0)
        discard;
    return color;
}