#include "static_mesh.hlsli"
Texture2D color_map : register(t0);
Texture2D normal_map : register(t1);
SamplerState point_sampler : register(s0);
SamplerState linear_sampler : register(s1);
SamplerState anisotropic_sampler : register(s2);
float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = color_map.Sample(anisotropic_sampler, pin.Tex);
    float alpha = color.a;
    
    float3 N = normalize(pin.Normal.xyz);
    
    float3 T = float3(1.0001, 0, 0);
    float3 B = normalize(cross(N, T));
    T = normalize(cross(B, N));

    float4 normal = normal_map.Sample(anisotropic_sampler, pin.Tex);
    normal = (normal * 2.0) - 1.0;
    normal.w = 0;
    N = normalize((normal.x * T) + (normal.y * B) + (normal.z * N));

    
    float3 L = normalize(-light_direction.xyz);
    float3 diffuse = color.rgb * max(0.3, dot(N, L));
    
    float3 V = normalize(camera_position.xyz - pin.world_position.xyz);
    float3 specular = pow(max(0, dot(N, normalize(V + L))), 128);
    
    specular = float3(0, 0, 0);
    
    return float4(diffuse + specular, alpha) * pin.Color;
}