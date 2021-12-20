#ifndef _GLALL_H_
#define	_GLALL_H_

#define RELEASE(x)		{if((x)){(x)->Release();(x)=nullptr;}}

#include "GLType.h"

#include "GLWindow.h"
#include "GLInput.h"

#include "GLSound.h"
#include "GLWave.h"

#include "GLDraw.h"
#include "GLTexture.h"
#include "GLFont.h"
#include "GLMesh.h"

#endif // !_GLALL_H_
