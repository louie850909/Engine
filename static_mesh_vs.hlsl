#include "static_mesh.hlsli"

VS_OUT main( float4 pos : POSITION, float4 normal : NORMAL, float4 color : COLOR, float2 tex : TEXCOORD )
{
    VS_OUT vout;
    vout.Pos = mul(pos, mul(world, mul(view, projection)));
    vout.world_position = mul(pos, world);
    
    normal.w = 0;
    vout.Normal = normalize(mul(normal, world));
    
    vout.Color = material_color;
    vout.Tex = tex;
    return vout;
}