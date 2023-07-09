#ifndef CAMERA_H
#define CAMERA_H

	const Point&	GetCameraPos();
	void			RotateCamera(int dx, int dy);

	void			MoveCameraForward();
	void			MoveCameraBackward();
	void			MoveCameraRight();
	void			MoveCameraLeft();

	void			SetupCameraMatrix();
	Point			ComputeWorldRay(int xs, int ys);

#endif	// CAMERA_H
