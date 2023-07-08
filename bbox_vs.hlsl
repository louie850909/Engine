#include "bbox.hlsli"
VS_OUT main( float4 pos : POSITION )
{
    VS_OUT vout;
    vout.Pos = mul(pos, mul(world, view_projection));
    return vout;
}