#ifndef RENDERINGHELPERS_H
#define RENDERINGHELPERS_H

#include "Opcode.h"

	void	DrawLine(const Point& p0, const Point& p1, const Point& color, float line_width);
	void	DrawTriangle(const Point& p0, const Point& p1, const Point& p2, const Point& color);
	void	DrawSphere(const Sphere& sphere);
	void	DrawOBB(const OBB& obb);
	void	DrawCapsule(const Matrix4x4& world, const Point& p0, const Point& p1, float r);

#endif // RENDERINGHELPERS_H
