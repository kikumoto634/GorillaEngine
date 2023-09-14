#pragma once
#include <DirectXMath.h>

#include "Vector2.h"
#include "Vector3.h"

	
Vector3 TransformNormal(const Vector3& v, const DirectX::XMMATRIX& m);