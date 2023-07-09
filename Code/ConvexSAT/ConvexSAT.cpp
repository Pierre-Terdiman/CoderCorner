///////////////////////////////////////////////////////////////////////////////
// Convex SAT optimization test - (c) 2007-2008 Pierre Terdiman
// www.codercorner.com
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

static bool gTestReferenceCode		= false;
static bool gTestOptimReferenceCode	= false;
static bool gDrawUnitLengthNormal	= false;
static bool gRenderWireframe		= false;

static const float gDisp = 0.01f;
static const float gCamSpeed = 0.1f;
static btVector3 Eye(3.0616338f, 1.1985892f, 2.5769043f);
static btVector3 Dir(-0.66853905,-0.14004262,-0.73037237);
static btVector3 N;
static int mx = 0;
static int my = 0;

static MyConvex gConvex0;
static MyConvex gConvex1;
static float gAngle = 0.0f;
static float gAngle2 = 0.0f;

	__forceinline void	StartProfile(int& val)
	{
#ifdef WIN32
		__asm{
			cpuid
			rdtsc
			mov		ebx, val
			mov		[ebx], eax
		}
#endif
	}

	__forceinline void	EndProfile(int& val)
	{
#ifdef WIN32
		__asm{
			cpuid
			rdtsc
			mov		ebx, val
			sub		eax, [ebx]
			mov		[ebx], eax
		}
#endif
	}


static void PrintHelp()
{
	printf("Use arrow keys and the mouse to move the camera\n");
	printf("Press 2,4,6,8 (keypad) to move the convex along X or Y\n");
	printf("Press 1,3,7,9 (keypad) to rotate the convex\n");

	printf("Press R to enable the reference code (result = blue wireframe object)\n");
	printf("Press O to enable the optimized code (result = orange wireframe object)\n");
}

static void KeyboardCallback(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:	exit(0); break;

	case 'O':
	case 'o':
		gTestOptimReferenceCode = !gTestOptimReferenceCode;
		break;

	case 'R':
	case 'r':
		gTestReferenceCode = !gTestReferenceCode;
		break;

	case 'U':
	case 'u':
		gDrawUnitLengthNormal = !gDrawUnitLengthNormal;
		break;

	case 'W':
	case 'w':
		gRenderWireframe = !gRenderWireframe;
		break;

	case '4':
		gConvex0.mTransform.setOrigin(gConvex0.mTransform.getOrigin() + btVector3(-gDisp,0,0));
		break;
	case '6':
		gConvex0.mTransform.setOrigin(gConvex0.mTransform.getOrigin() + btVector3(gDisp,0,0));
		break;
	case '8':
		gConvex0.mTransform.setOrigin(gConvex0.mTransform.getOrigin() + btVector3(0,gDisp,0));
		break;
	case '2':
		gConvex0.mTransform.setOrigin(gConvex0.mTransform.getOrigin() + btVector3(0,-gDisp,0));
		break;

	case '7':
		gAngle += 0.1f;
		gConvex0.mTransform.setRotation(btQuaternion(0.0f, gAngle, gAngle2));
		break;
	case '9':
		gAngle -= 0.1f;
		gConvex0.mTransform.setRotation(btQuaternion(0.0f, gAngle, gAngle2));
		break;
	case '1':
		gAngle2 += 0.1f;
		gConvex0.mTransform.setRotation(btQuaternion(0.0f, gAngle, gAngle2));
		break;
	case '3':
		gAngle2 -= 0.1f;
		gConvex0.mTransform.setRotation(btQuaternion(0.0f, gAngle, gAngle2));
		break;

	case 101:	Eye += Dir * gCamSpeed; break;
	case 103:	Eye -= Dir * gCamSpeed; break;
	case 100:	Eye -= N * gCamSpeed; break;
	case 102:	Eye += N * gCamSpeed; break;
	}
}

static void ArrowKeyCallback(int key, int x, int y)
{
	KeyboardCallback(key,x,y);
}
	
static void MouseCallback(int button, int state, int x, int y)
{
	mx = x;
	my = y;
}

static const float NxPiF32		= 3.141592653589793f;

float degToRad(float a)
	{
	return (float)0.01745329251994329547 * a;
	}

class NxQuat
	{
	public:
	NxQuat(){}

	NxQuat(const float angle, const btVector3 & axis)
	{
	x = axis.x();
	y = axis.y();
	z = axis.z();

	const float i_length =  1.0f / sqrtf( x*x + y*y + z*z );
	x = x * i_length;
	y = y * i_length;
	z = z * i_length;

	float Half = degToRad(angle * 0.5f);

	w = cosf(Half);
	const float sin_theta_over_two = sinf(Half );
	x = x * sin_theta_over_two;
	y = y * sin_theta_over_two;
	z = z * sin_theta_over_two;
	}

void NxQuat::multiply(const NxQuat& left, const btVector3& right)
	{
	float a,b,c,d;

	a = - left.x*right.x() - left.y*right.y() - left.z *right.z();
	b =   left.w*right.x() + left.y*right.z() - right.y()*left.z;
	c =   left.w*right.y() + left.z*right.x() - right.z()*left.x;
	d =   left.w*right.z() + left.x*right.y() - right.x()*left.y;

	w = a;
	x = b;
	y = c;
	z = d;
	}

void NxQuat::rotate(btVector3 & v) const
	{
	NxQuat myInverse;
	myInverse.x = -x;
	myInverse.y = -y;
	myInverse.z = -z;
	myInverse.w =  w;

	NxQuat left;
	left.multiply(*this,v);
	float vx = left.w*myInverse.x + myInverse.w*left.x + left.y*myInverse.z - myInverse.y*left.z;
	float vy = left.w*myInverse.y + myInverse.w*left.y + left.z*myInverse.x - myInverse.z*left.x;
	float vz = left.w*myInverse.z + myInverse.w*left.z + left.x*myInverse.y - myInverse.x*left.y;
	v.setValue(vx, vy, vz);
	}

    float x,y,z,w;
	};


static void MotionCallback(int x, int y)
{
	int dx = mx - x;
	int dy = my - y;
	
	Dir = Dir.normalize();
	N = Dir.cross(btVector3(0,1,0));

	NxQuat qx(NxPiF32 * dx * 20/ 180.0f, btVector3(0,1,0));
	qx.rotate(Dir);
	NxQuat qy(NxPiF32 * dy * 20/ 180.0f, N);
	qy.rotate(Dir);

	mx = x;
	my = y;
}

static void RenderCallback()
{
	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
	// Setup camera
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, ((float)glutGet(GLUT_WINDOW_WIDTH))/((float)glutGet(GLUT_WINDOW_HEIGHT)), 1.0f, 10000.0f);
	gluLookAt(Eye.x(), Eye.y(), Eye.z(), Eye.x() + Dir.x(), Eye.y() + Dir.y(), Eye.z() + Dir.z(), 0.0f, 1.0f, 0.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_LIGHTING);

	if(gRenderWireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Reference
	bool ReferenceStatus = false;
	ConvexResults ReferenceResults;
	int DefaultTime;
	StartProfile(DefaultTime);
	if(gTestReferenceCode)
		ReferenceStatus = ReferenceAlgo(ReferenceResults, gConvex0, gConvex1);
	EndProfile(DefaultTime);

	ConvexResults OptimizedResults;
	int OptimizedTime;
	StartProfile(OptimizedTime);
	if(gTestOptimReferenceCode)
		ReferenceStatus = OptimizedReferenceAlgo(OptimizedResults, gConvex0, gConvex1);
	EndProfile(OptimizedTime);

	// Visualize all
	btVector3 color(0,0,0);
	gConvex0.Render(false, color);	// Moving
	gConvex1.Render(false, color);	// Fixed

	if(gTestReferenceCode)
	{
		ReferenceResults.Visualize(gConvex0, btVector3(0.0f, 0.5f, 1.0f), btVector3(0.0f, 0.0f, 0.0f), gDrawUnitLengthNormal);
	}
	if(gTestOptimReferenceCode)
	{
		OptimizedResults.Visualize(gConvex0, btVector3(1.0f, 0.5f, 0.0f), btVector3(0.0f, 0.0f, 0.0f), gDrawUnitLengthNormal);
	}

	int Ratio = 0;
	if(gTestReferenceCode && gTestOptimReferenceCode)
	{
		Ratio = OptimizedTime ? DefaultTime/OptimizedTime : 0;
	}

	printf("(%d) | %d | %d | %.6f | %.6f                  \r", Ratio, DefaultTime/1024, OptimizedTime/1024, ReferenceResults.mDepth, OptimizedResults.mDepth);

	glutSwapBuffers();
}

static void ReshapeCallback(int width, int height)
{
	glViewport(0, 0, width, height);
}

static void IdleCallback()
{
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	// Initialize Glut
	glutInit(&argc, argv);
	glutInitWindowSize(512, 512);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	int mainHandle = glutCreateWindow("ConvexSAT");
	glutSetWindow(mainHandle);
	glutDisplayFunc(RenderCallback);
	glutReshapeFunc(ReshapeCallback);
	glutIdleFunc(IdleCallback);
	glutKeyboardFunc(KeyboardCallback);
	glutSpecialFunc(ArrowKeyCallback);
	glutMouseFunc(MouseCallback);
	glutMotionFunc(MotionCallback);
	MotionCallback(0,0);

	// Setup default render states
	glClearColor(0.3f, 0.4f, 0.5f, 1.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_CULL_FACE);

	// Setup lighting
	glEnable(GL_LIGHTING);
	float AmbientColor[] = { 0.0f, 0.1f, 0.2f, 0.0f };		glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientColor);
	float DiffuseColor[] = { 1.0f, 1.0f, 1.0f, 0.0f };		glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseColor);
	float SpecularColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };		glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularColor);
	float Position[] = { -10.0f, 1000.0f, -4.0f, 1.0f };	glLightfv(GL_LIGHT0, GL_POSITION, Position);
	glEnable(GL_LIGHT0);

	PrintHelp();

	//
	bool Status = gConvex0.LoadFromFile("c:\\convex0.bin");
	if(!Status)
	{
		Status = gConvex0.LoadFromFile("convex0.bin");
		if(!Status)
		{
			printf("Failed to load object!\n");
			exit(0);
		}
	}
	Status = gConvex1.LoadFromFile("c:\\convex1.bin");
	if(!Status)
	{
		Status = gConvex1.LoadFromFile("convex1.bin");
		if(!Status)
		{
			printf("Failed to load object!\n");
			exit(0);
		}
	}

//	gConvex0.mTransform.setOrigin(btVector3(1.0f, 1.0f, 0.0f));
	gConvex0.mTransform.setOrigin(btVector3(0.20000069f, 0.95000005f, 0.0f));
	gConvex0.mTransform.setOrigin(btVector3(0.20000069f, 0.3000005f, 0.0f));
//	gConvex0.mTransform.setRotation(btQuaternion(0.1f, 0.25f, 0.5f));

//	gConvex1.mTransform.setRotation(btQuaternion(0.1f, 0.25f, 0.5f));

	printf("\n\n");

	// Run
	glutMainLoop();

	return 0;
}

