#pragma once

#include <raylib-cpp.hpp>

class MyTransform {

public:
	MyTransform();

	const RVector3& GetPos() const;
	const RQuaternion& GetRot() const;
	const RVector3& GetScale() const;

	void SetPos(const RVector3& pos);
	void SetRot(const RQuaternion& rot);
	void SetScale(const RVector3& scale);

	const RMatrix& GetMatrix();


private:
	RVector3 position;
	RQuaternion rotation;
	RVector3 scale;

	RMatrix transform;
	bool dirtyMat;

};