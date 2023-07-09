#ifndef COLLISIONTEST_H
#define COLLISIONTEST_H

	class CollisionTest
	{
		public:

						CollisionTest()		{}
		virtual			~CollisionTest()	{}

		virtual	void	Init()				= 0;
		virtual	void	Release()			= 0;
		virtual	void	PerformTest()		= 0;
		virtual	void	Select()											{}
		virtual	void	Deselect()											{}
		virtual	void	KeyboardCallback(unsigned char key, int x, int y)	{}
		virtual	void	MouseCallback(int button, int state, int x, int y)	{}
		virtual	void	MotionCallback(int x, int y)						{}
	};

	class OpcodeSettings
	{
		public:
						OpcodeSettings();

				void	AddToTweakBar(TwBar* tbar);
				void	SetupCollider(Collider& collider)	const;

				bool	mPrimitiveTests;
				bool	mFirstContact;
				bool	mUseCache;
	};

#endif	// COLLISIONTEST_H
