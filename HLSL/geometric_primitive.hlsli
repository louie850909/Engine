struct VS_OUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
};

cbuffer OBJECT_CONSTANT_BUFFER : register(b0)
{
    row_major float4x4 world;
    float4 material_color;
};

cbuffer SCENE_CONSTANT_BUFFER : register(b1)
{
    row_major float4x4 view;
    row_major float4x4 projection;
    float4 light_direction;
    float4 camera_position;
};