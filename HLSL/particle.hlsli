struct VS_IN
{
    float4 position     : POSITION;
    float4 normal       : NORMAL;
    float4 color        : COLOR;
    float2 texcoord     : TEXCOORD;
    float4 instPos      : INSTPOS;
    float4 instScl      : INSTSCL;
    float4 instColor    : INSTCOLOR;
    float4 instMove     : INSTMOVE;
    float  instLife     : INSTLIFE;
    int    instUse      : INSTUSE;
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 normal   : NORMAL;
    float4 color    : COLOR;
    float2 texcoord : TEXCOORD;
    bool   instUse  : INSTUSE;
};

cbuffer SCENE_CONSTANT_BUFFER : register(b1)
{
    row_major float4x4 view;
    row_major float4x4 projection;
    float4 light_direction;
};