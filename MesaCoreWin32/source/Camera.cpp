#include <Mesa/Camera.h>

namespace Mesa
{
	CameraDx11::CameraDx11()
	{
		m_Pos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_PosVec = DirectX::XMLoadFloat3(&m_Pos);
		m_Rot = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_RotVec = DirectX::XMLoadFloat3(&m_Rot);
		UpdateViewMatrix();
	}

	void CameraDx11::SetProjectionValues(float fov, float aspectRatio, float nz, float fz)
	{
		float fovRadians = (fov / 360.0f) * DirectX::XM_2PI;
		m_Proj = DirectX::XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nz, fz);
	}

	void CameraDx11::HandleMovement(CameraMovement direction, float deltaTime)
	{
		switch (direction)
		{
		case Mesa::CameraMovementForward:
			AdjustPosition(DirectX::XMVectorScale(m_ForwardVec, 1.0f * deltaTime));
			break;
		case Mesa::CameraMovementBackward:
			AdjustPosition(DirectX::XMVectorScale(m_BackwardVec, 1.0f * deltaTime));
			break;
		case Mesa::CameraMovementLeft:
			AdjustPosition(DirectX::XMVectorScale(m_LeftVec, 1.0f * deltaTime));
			break;
		case Mesa::CameraMovementRight:
			AdjustPosition(DirectX::XMVectorScale(m_RightVec, 1.0f * deltaTime));
			break;
		case Mesa::CameraMovementUp:
			break;
		case Mesa::CameraMovementDown:
			break;
		default:
			break;
		}
	}

	const DirectX::XMMATRIX& CameraDx11::GetViewMatrix() const
	{
		return m_View;
	}

	const DirectX::XMMATRIX& CameraDx11::GetProjectionMatrix() const
	{
		return m_Proj;
	}

	const DirectX::XMVECTOR& CameraDx11::GetPositionVector() const
	{
		return m_PosVec;
	}

	const DirectX::XMFLOAT3& CameraDx11::GetPositionFloat3() const
	{
		return m_Pos;
	}

	const DirectX::XMVECTOR& CameraDx11::GetRotationVector() const
	{
		return m_RotVec;
	}

	const DirectX::XMFLOAT3& CameraDx11::GetRotationFloat3() const
	{
		return m_Rot;
	}

	void CameraDx11::SetPosition(const DirectX::XMVECTOR& pos)
	{
		DirectX::XMStoreFloat3(&this->m_Pos, pos);
		m_PosVec = pos;
		UpdateViewMatrix();
	}

	void CameraDx11::SetPosition(float x, float y, float z)
	{
		m_Pos = DirectX::XMFLOAT3(x, y, z);
		m_PosVec = DirectX::XMLoadFloat3(&m_Pos);
		UpdateViewMatrix();
	}

	void CameraDx11::AdjustPosition(const DirectX::XMVECTOR& pos)
	{
		m_PosVec = DirectX::XMVectorAdd(m_PosVec, pos);
		DirectX::XMStoreFloat3(&this->m_Pos, m_PosVec);
		UpdateViewMatrix();
	}

	void CameraDx11::AdjustPosition(float x, float y, float z)
	{
		m_Pos.x += x;
		m_Pos.y += y;
		m_Pos.z += z;
		m_PosVec = DirectX::XMLoadFloat3(&m_Pos);
		UpdateViewMatrix();
	}

	void CameraDx11::SetRotation(const DirectX::XMVECTOR& rot)
	{
		m_RotVec = rot;
		DirectX::XMStoreFloat3(&this->m_Rot, m_RotVec);
		UpdateViewMatrix();
	}

	void CameraDx11::SetRotation(float x, float y, float z)
	{
		m_Rot = DirectX::XMFLOAT3(x, y, z);
		m_RotVec = DirectX::XMLoadFloat3(&m_Rot);
		UpdateViewMatrix();
	}

	void CameraDx11::AdjustRotation(const DirectX::XMVECTOR& rot)
	{
		m_RotVec = DirectX::XMVectorAdd(m_RotVec, rot);
		DirectX::XMStoreFloat3(&this->m_Rot, m_RotVec);
		UpdateViewMatrix();
	}

	void CameraDx11::AdjustRotation(float x, float y, float z)
	{
		m_Rot.x += x;
		m_Rot.y += y;
		m_Rot.z += z;
		m_RotVec = DirectX::XMLoadFloat3(&m_Rot);
		UpdateViewMatrix();
	}

	void CameraDx11::SetLookAtPos(DirectX::XMFLOAT3 lookAtPos)
	{
		if (lookAtPos.x == m_Pos.x && lookAtPos.y == m_Pos.y && lookAtPos.z == m_Pos.z)
			return;

		lookAtPos.x = m_Pos.x - lookAtPos.x;
		lookAtPos.y = m_Pos.y - lookAtPos.y;
		lookAtPos.z = m_Pos.z - lookAtPos.z;

		float pitch = 0.0f;
		if (lookAtPos.y != 0.0f)
		{
			const float distance = sqrt(lookAtPos.x * lookAtPos.x + lookAtPos.z * lookAtPos.z);
			pitch = atan(lookAtPos.y / distance);
		}

		float yaw = 0.0f;
		if (lookAtPos.x != 0.0f)
		{
			yaw = atan(lookAtPos.x / lookAtPos.z);
		}
		if (lookAtPos.z > 0)
			yaw += DirectX::XM_PI;

		SetRotation(pitch, yaw, 0.0f);
	}

	const DirectX::XMVECTOR& CameraDx11::GetForwardVector()
	{
		return m_ForwardVec;
	}

	const DirectX::XMVECTOR& CameraDx11::GetRightVector()
	{
		return m_RightVec;
	}

	const DirectX::XMVECTOR& CameraDx11::GetBackwardVector()
	{
		return m_BackwardVec;
	}

	const DirectX::XMVECTOR& CameraDx11::GetLeftVector()
	{
		return m_LeftVec;
	}

	void CameraDx11::UpdateViewMatrix()
	{

		DirectX::XMMATRIX camRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(this->m_Rot.x, this->m_Rot.y, this->m_Rot.z);
		DirectX::XMVECTOR camTarget = DirectX::XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, camRotationMatrix);
		camTarget = DirectX::XMVectorAdd(camTarget, m_PosVec);
		DirectX::XMVECTOR upDir = DirectX::XMVector3TransformCoord(DEFAULT_UP_VECTOR, camRotationMatrix);
		m_View = DirectX::XMMatrixLookAtLH(m_PosVec, camTarget, upDir);

		DirectX::XMMATRIX vecRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(0.0f, m_Rot.y, 0.0f);
		m_ForwardVec = DirectX::XMVector3TransformCoord(DEFAULT_FORWARD_VECTOR, vecRotationMatrix);
		m_BackwardVec = DirectX::XMVector3TransformCoord(DEFAULT_BACKWARD_VECTOR, vecRotationMatrix);
		m_LeftVec = DirectX::XMVector3TransformCoord(DEFAULT_LEFT_VECTOR, vecRotationMatrix);
		m_RightVec = DirectX::XMVector3TransformCoord(DEFAULT_RIGHT_VECTOR, vecRotationMatrix);
	}
}

