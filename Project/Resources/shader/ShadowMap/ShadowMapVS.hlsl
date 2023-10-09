#include "ShadowMap.hlsli"

SPSIn main(SVSIn vsIn)
{
    SPSIn psIn;
    psIn.pos = mul(mWorld, vsIn.pos);
    psIn.pos = mul(mView, psIn.pos);
    psIn.pos = mul(mProj, psIn.pos);
    return psIn;
}