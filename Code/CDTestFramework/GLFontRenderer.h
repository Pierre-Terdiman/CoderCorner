#ifndef GLFONTRENDERER_H
#define GLFONTRENDERER_H

class GLFontRenderer{
	
private:

	static bool m_isInit;
	static unsigned int m_textureObject;
	static int m_screenWidth;
	static int m_screenHeight;
	static float m_color[4];

public:
	
	static bool init();
	static void print(float x, float y, float fontSize, const char* pString, bool forceMonoSpace=false, int monoSpaceWidth=11, bool doOrthoProj=true);
	static void setScreenResolution(int screenWidth, int screenHeight);
	static void setColor(float r, float g, float b, float a);
	
};

#endif	// GLFONTRENDERER_H
