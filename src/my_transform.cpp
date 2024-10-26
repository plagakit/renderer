#include "my_transform.h"

MyTransform::MyTransform()
{
	position = RVector3::Zero();
	rotation = RQuaternion::Identity();
	scale = RVector3::One();
}

const RVector3& MyTransform::GetPos() const
{
	return position;
}

const RQuaternion& MyTransform::GetRot() const
{
	return rotation;
}

const RVector3& MyTransform::GetScale() const
{
	return scale;
}

void MyTransform::SetPos(const RVector3& pos)
{
	dirtyMat = true;
	position.x = pos.x;
	position.y = pos.y;
	position.z = pos.z;
}

void MyTransform::SetRot(const RQuaternion& rot)
{
	dirtyMat = true;
	rotation.x = rot.x;
	rotation.y = rot.y;
	rotation.z = rot.z;
	rotation.w = rot.w;
}

void MyTransform::SetScale(const RVector3& s)
{
	dirtyMat = true;
	scale.x = s.x;
	scale.y = s.y;
	scale.z = s.z;
}

const RMatrix& MyTransform::GetMatrix()
{
	if (dirtyMat)
	{
		dirtyMat = false;

		RMatrix scaleMat = RMatrix::Scale(scale.x, scale.y, scale.z);
		rotation = rotation.Normalize();
		RMatrix rotMat = rotation.ToMatrix();
		RMatrix posMat = RMatrix::Translate(position.x, position.y, position.z);

		transform = scaleMat * rotMat * posMat;
	}
	
	return transform;
}
