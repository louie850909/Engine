#include "instancingBillboard.hlsli"
#include "MatrixOperation.hlsli"

VS_OUT main(VS_IN input)
{
    matrix world, scl, trans;
    // ビルドボード処理
    world = inverse(view);
    world._41 = 0.0f;
    world._42 = 0.0f;
    world._43 = 0.0f;
    
    trans = float4x4(1.0f, 0.0f, 0.0f, 0.0f,
                     0.0f, 1.0f, 0.0f, 0.0f,
                     0.0f, 0.0f, 1.0f, 0.0f,
                     input.instPos.x, input.instPos.y, input.instPos.z, 1.0f);
    
    scl = float4x4(input.instScl.x, 0.0f, 0.0f, 0.0f,
                   0.0f, input.instScl.y, 0.0f, 0.0f,
                   0.0f, 0.0f, input.instScl.z, 0.0f,
                   0.0f, 0.0f, 0.0f, 1.0f);
    
    world = mul(world, scl);
    world = mul(world, trans);
    
    
    VS_OUT output;
    
    output.position = mul(input.position, mul(world, mul(view, projection)));
    output.texcoord = input.texcoord;
    output.color = input.color;
    output.normal = input.normal;

    return output;
}