///////////////////////////////////////////////////////////////////////////////
// Convex SAT optimization test - (c) 2007-2008 Pierre Terdiman
// www.codercorner.com
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

void DrawLine(const btVector3& p0, const btVector3& p1, const btVector3& color, float line_width)
{
	glDisable(GL_LIGHTING);
	glLineWidth(line_width);
	glColor4f(color.x(), color.y(), color.z(), 1.0f);
	btVector3 tmp[] = {p0, p1};
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(btVector3), &tmp[0].x());
	glDrawArrays(GL_LINES, 0, 2);
	glDisableClientState(GL_VERTEX_ARRAY);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_LIGHTING);
}

void DrawTriangle(const btVector3& p0, const btVector3& p1, const btVector3& p2, const btVector3& color)
{
	glColor4f(color.x(), color.y(), color.z(), 1.0f);
	btVector3 tmp[] = {p0, p1, p2};
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(btVector3), &tmp[0].x());
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDisableClientState(GL_VERTEX_ARRAY);
}
