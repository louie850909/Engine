#include "geometric_primitive.hlsli"

VS_OUT main(float4 position : POSITION, float4 normal : NORMAL, float4 color : COLOR)
{
    VS_OUT vout;
    vout.Pos = mul(position, mul(world, view_projection));
    
    normal.w = 0;
    float4 N = normalize(mul(normal, world));
    float4 L = normalize(-light_direction);
    
    vout.Color.rgb = color.rgb * max(0, dot(N, L));
    vout.Color.a = color.a;
    
    //vout.Color = color;
    
    return vout;
}