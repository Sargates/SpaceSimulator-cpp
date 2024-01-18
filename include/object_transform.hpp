//!	From https://github.com/raylib-extras/examples-cpp/blob/main/transform_hierarchy/object_transform.h
/**********************************************************************************************
*
*   transforms.h * a sample object transfrom class
*
*
*   LICENSE: ZLIB
*
*   Copyright (c) 2022 Jeffery Myers
*
*   Permission is hereby granted, free of charge, to any person obtaining a copy
*   of this software and associated documentation files (the "Software"), to deal
*   in the Software without restriction, including without limitation the rights
*   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*   copies of the Software, and to permit persons to whom the Software is
*   furnished to do so, subject to the following conditions:
*
*   The above copyright notice and this permission notice shall be included in all
*   copies or substantial portions of the Software.
*
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*   SOFTWARE.
*
**********************************************************************************************/

//! Modified as needed for this project

#pragma once

#include "raylib.hpp"
#include "raymath.hpp"
#include "rlgl.hpp"

#include <vector>
#include <algorithm>

class ObjectTransform  {
private:
	Vector3 Position = { 0 };
	Quaternion Rotation = QuaternionIdentity();

	float yaw=0, pitch=0, roll=0; //* These values are not used inside this class, they are just here for storage if needed.

	bool Dirty = true;

	Matrix WorldMatrix = { 0 };
	Matrix GlWorldMatrix = { 0 };

	ObjectTransform* Parent = nullptr;
	std::vector<ObjectTransform*> Children;

public:

	inline float GetYaw() { return yaw;}
	inline float GetPitch() { return pitch;}
	inline float GetRoll() { return roll;}
	inline void SetYaw(float v) { yaw = v; }
	inline void SetPitch(float v) { pitch = v; }
	inline void SetRoll(float v) { roll = v; }

	ObjectTransform(Vector3 position, Quaternion rotation=QuaternionIdentity(), bool faceY = true) {
		Position = position;
		Rotation = rotation;
		if (faceY)
			Rotation = QuaternionFromAxisAngle(Vector3(0, 1, 0), 0);
	}

	inline ObjectTransform* GetParent() const { return Parent; }

	inline const std::vector<ObjectTransform*>& GetChildren() const { return Children; }

	inline ObjectTransform* AddChild(ObjectTransform* child) {
		if (!child) return nullptr;

		child->SetParent(this);

		return child;
	}

	inline ObjectTransform& AddChild(ObjectTransform& child) {
		child.SetParent(this);

		return child;
	}

	inline void SetParent(ObjectTransform* newParent) {
		if (Parent == newParent)
			return;

		if (newParent) {
			auto child = std::find(newParent->Children.begin(), newParent->Children.end(), this);
			if (child != newParent->Children.end())
				newParent->Children.erase(child);
		}

		Parent = newParent;
		if (Parent)
			Parent->Children.push_back(this);
	}

	inline void Detach() {
		if (!GetParent())
			return;

		// Matrix worldTransform = GetWorldMatrix();
		Position = Vector3Transform(Vector3Zero(), WorldMatrix);

		// Matrix translateMatrix = MatrixTranslate(Position.x, Position.y, Position.z);
		// Matrix orientationMatrix = MatrixMultiply(worldTransform, translateMatrix);

		Rotation = QuaternionFromMatrix(WorldMatrix);

		SetParent(nullptr);
	}

	inline void SetDirty() {
		Dirty = true;
		for (ObjectTransform* childTransform : Children) {
			if (childTransform != nullptr)
				childTransform->SetDirty();
		}
	}

	inline const Vector3& GetPosition() const { return Position; }

	inline Quaternion GetRotation() {
		return Rotation;
	}

	inline Vector3 GetEulerAngles() {
		return QuaternionToEuler(Rotation);
	}

	inline Vector3 GetForward() const {
		return Vector3Transform(Vector3{ 0, 0, 1 }, MatrixInvert(QuaternionToMatrix(Rotation)));
	}
	inline Vector3 GetBackwards() const {
		return Vector3Transform(Vector3{ 0, 0, -1 }, MatrixInvert(QuaternionToMatrix(Rotation)));
	}

	inline Vector3 GetUp() const {
		return Vector3Transform(Vector3{ 0, 1, 0 }, MatrixInvert(QuaternionToMatrix(Rotation)));
	}
	inline Vector3 GetDown() const {
		return Vector3Transform(Vector3{ 0, -1, 0 }, MatrixInvert(QuaternionToMatrix(Rotation)));
	}

	inline Vector3 GetLeft() const {
		return Vector3CrossProduct(GetUp(), GetForward());
	}

	inline Vector3 GetRight() const {
		return Vector3CrossProduct(GetForward(), GetUp());
	}

	inline Vector3 GetWorldPosition() {
		// Matrix worldTransform = GetWorldMatrix();
		return Vector3Transform(Vector3Zero(), WorldMatrix);
	}

	inline Vector3 GetWorldTarget() {
		// Matrix worldTransform = GetWorldMatrix();
		Vector3 pos = Vector3Transform(Vector3Zero(), WorldMatrix);

		// Matrix translateMatrix = MatrixTranslate(Position.x, Position.y, Position.z);
		// Matrix orientationMatrix = MatrixMultiply(worldTransform, translateMatrix);

		return Vector3Add(pos, Vector3Transform(Vector3{ 0 , 1 , 0 }, WorldMatrix));
	}

	inline void SetPosition(float x, float y, float z) {
		Position.x = x;
		Position.y = y;
		Position.z = z;
		SetDirty();
	}

	inline void SetPosition(const Vector3& pos) {
		Position = pos;
		SetDirty();
	}

	// inline void SetRotation(const Vector3& eulerAngles) {
	// 	Vector3 angles = Vector3Scale(eulerAngles, DEG2RAD);
	// 	Rotation = QuaternionFromEuler(angles.x, angles.y, angles.z);
	// 	SetDirty();
	// }

	inline void SetRotation(const Quaternion& rot) {
		Rotation = rot;
		SetDirty();
	}

	inline bool IsDirty() {
		return Dirty;
	}

	inline void LookAt(const Vector3& target, const Vector3& up) {
		SetDirty();
		Matrix mat = MatrixLookAt(Position, target, up);
		Rotation = QuaternionFromMatrix(mat);
	}

	inline Matrix GetLocalMatrix() {
		Matrix orient = QuaternionToMatrix(Rotation);
		Matrix translation = MatrixTranslate(Position.x, Position.y, Position.z);

		return MatrixMultiply(MatrixInvert(orient), translation);
	}

	inline void UpdateWorldMatrix() {
		Matrix parentMatrix = MatrixIdentity();

		if (Parent)
			parentMatrix = Parent->GetWorldMatrix();

		WorldMatrix = MatrixMultiply(GetLocalMatrix(), parentMatrix);
		GlWorldMatrix = MatrixTranspose(WorldMatrix);

		Dirty = false;
	}
	
	inline const Matrix& GetWorldMatrix() {
		if (!IsDirty())
			return WorldMatrix;

		UpdateWorldMatrix();
		return WorldMatrix;
	}

	inline const Matrix& GetGLWorldMatrix() {
		if (!IsDirty())
			return GlWorldMatrix;

		UpdateWorldMatrix();
		return GlWorldMatrix;
	}

	inline Vector3 ToLocalPos(const Vector3& inPos) {
		return Vector3Transform(inPos, MatrixInvert(GetWorldMatrix()));
	}

	inline void AddPosition(const Vector3& inPos) {
		SetDirty();
		Position += inPos;
	}
	inline void AddRotation(const Quaternion& newRot) {
		SetDirty();
		Rotation = QuaternionMultiply(Rotation, newRot);
	}

	inline void MoveForward(float distance) {
		SetDirty();
		Position += GetForward() * distance;
	}
	inline void MoveBackward(float distance) { MoveForward(-distance); }

	inline void MoveUp(float distance) {
		SetDirty();
		Position += GetUp() * distance;
	}
	inline void MoveDown(float distance) { MoveUp(-distance); }

	inline void MoveRight(float distance) {
		SetDirty();
		Position += GetRight() * distance;
	}
	inline void MoveLeft(float distance) { MoveRight(-distance); }

	inline void RotatePitch(float angle) {
		SetDirty();
		auto rot = QuaternionFromEuler(angle * DEG2RAD, 0, 0);
		Rotation = QuaternionMultiply(Rotation, rot);
	}

	inline void RotateYaw(float angle) {
		SetDirty();
		auto rot = QuaternionFromEuler(0, -angle * DEG2RAD, 0);
		Rotation = QuaternionMultiply(Rotation, rot);
	}

	inline void RotateRoll(float angle) {
		SetDirty();
		auto rot = QuaternionFromEuler(0, 0, -angle * DEG2RAD);
		Rotation = QuaternionMultiply(Rotation, rot);
	}

	inline void RotateAbsX(float angle) {
		SetDirty();
		auto rot = QuaternionFromEuler(angle * DEG2RAD, 0, 0);
		Rotation = QuaternionMultiply(rot, Rotation);
	}

	inline void RotateAbsY(float angle) {
		SetDirty();
		auto rot = QuaternionFromEuler(0, -angle * DEG2RAD, 0);
		Rotation = QuaternionMultiply(rot, Rotation);
	}

	inline void RotateAbsZ(float angle) {
		SetDirty();
		auto rot = QuaternionFromEuler(0, 0, -angle * DEG2RAD);
		Rotation = QuaternionMultiply(rot, Rotation);
	}

	inline void PushMatrix() {
		const Matrix& glMatrix = GetGLWorldMatrix();
		rlPushMatrix();
		rlMultMatrixf((float*)(&glMatrix.m0));
	}

	inline void PopMatrix() {
		rlPopMatrix();
	}
};