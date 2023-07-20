#include "Billboard.hlsli"

VS_OUT main(VS_IN input)
{
    VS_OUT output;
    
    output.position = mul(input.position, mul(world, mul(view, projection)));
    output.texcoord = input.texcoord;
    output.color = input.color;
    output.normal = input.normal;

    return output;
}