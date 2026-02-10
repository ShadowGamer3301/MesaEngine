#pragma once
#include "Core.h"

namespace Mesa
{
    enum CameraMovement {
        CameraMovementForward,
        CameraMovementBackward,
        CameraMovementLeft,
        CameraMovementRight,
        CameraMovementUp,
        CameraMovementDown
    };

	class MSAPI Camera
	{
	public:
	};

    class MSAPI CameraDx11 : public Camera
	{
	public:
		CameraDx11();
		void SetProjectionValues(float fov, float aspectRatio, float nz, float fz);

		const DirectX::XMMATRIX& GetViewMatrix() const;
		const DirectX::XMMATRIX& GetProjectionMatrix() const;

		const DirectX::XMVECTOR& GetPositionVector() const;
		const DirectX::XMFLOAT3& GetPositionFloat3() const;
		const DirectX::XMVECTOR& GetRotationVector() const;
		const DirectX::XMFLOAT3& GetRotationFloat3() const;

		void SetPosition(const DirectX::XMVECTOR& pos);
		void SetPosition(float x, float y, float z);
		void AdjustPosition(const DirectX::XMVECTOR& pos);
		void AdjustPosition(float x, float y, float z);
		void SetRotation(const DirectX::XMVECTOR& rot);
		void SetRotation(float x, float y, float z);
		void AdjustRotation(const DirectX::XMVECTOR& rot);
		void AdjustRotation(float x, float y, float z);
		void SetLookAtPos(DirectX::XMFLOAT3 lookAtPos);
		const DirectX::XMVECTOR& GetForwardVector();
		const DirectX::XMVECTOR& GetRightVector();
		const DirectX::XMVECTOR& GetBackwardVector();
		const DirectX::XMVECTOR& GetLeftVector();

	private:
		void UpdateViewMatrix();
		DirectX::XMVECTOR m_PosVec;
		DirectX::XMVECTOR m_RotVec;
		DirectX::XMFLOAT3 m_Pos;
		DirectX::XMFLOAT3 m_Rot;
		DirectX::XMMATRIX m_View;
		DirectX::XMMATRIX m_Proj;

		const DirectX::XMVECTOR DEFAULT_FORWARD_VECTOR = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		const DirectX::XMVECTOR DEFAULT_UP_VECTOR = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		const DirectX::XMVECTOR DEFAULT_BACKWARD_VECTOR = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
		const DirectX::XMVECTOR DEFAULT_LEFT_VECTOR = DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
		const DirectX::XMVECTOR DEFAULT_RIGHT_VECTOR = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

		DirectX::XMVECTOR m_ForwardVec;
		DirectX::XMVECTOR m_LeftVec;
		DirectX::XMVECTOR m_RightVec;
		DirectX::XMVECTOR m_BackwardVec;
	};
}