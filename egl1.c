#include <QtCore/QCoreApplication>
#include <QtGui/QImage>
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <iostream>
#include <assert.h>

int main(int argc, char *argv[])
{
	EGLint ai32ContextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2,
												 EGL_NONE };
	char *dpyName = NULL;
	Display *x_dpy;
	x_dpy = XOpenDisplay(dpyName);
	if (!x_dpy) {
	   printf("Error: couldn't open display %s\n",
		  dpyName ? dpyName : getenv("DISPLAY"));
	   return -1;
	}

    // Step 1 - Get the default display.
    EGLDisplay eglDisplay = eglGetDisplay((EGLNativeDisplayType)0);
    std::cout << glGetError() << std::endl;

    // Step 2 - Initialize EGL.
    eglInitialize(eglDisplay, 0, 0);
    std::cout << glGetError() << std::endl;

    const char *s;
    s = eglQueryString(eglDisplay, EGL_VERSION);
    printf("EGL_VERSION = %s\n", s);
    s = eglQueryString(eglDisplay, EGL_VENDOR);
    printf("EGL_VENDOR = %s\n", s);
    s = eglQueryString(eglDisplay, EGL_EXTENSIONS);
    printf("EGL_EXTENSIONS = %s\n", s);
    s = eglQueryString(eglDisplay, EGL_CLIENT_APIS);
    printf("EGL_CLIENT_APIS = %s\n", s);


    // Step 3 - Make OpenGL ES the current API.
    // eglBindAPI(EGL_OPENGL_ES_API);
    std::cout << glGetError() << std::endl;

    // Step 4 - Specify the required configuration attributes.
    EGLint pi32ConfigAttribs[5];
    pi32ConfigAttribs[0] = EGL_SURFACE_TYPE;
    pi32ConfigAttribs[1] = EGL_WINDOW_BIT;
    pi32ConfigAttribs[2] = EGL_RENDERABLE_TYPE;
    pi32ConfigAttribs[3] = EGL_OPENGL_ES2_BIT;
    pi32ConfigAttribs[4] = EGL_NONE;
    static const EGLint attribs[] = {
          EGL_RED_SIZE, 1,
          EGL_GREEN_SIZE, 1,
          EGL_BLUE_SIZE, 1,
          EGL_DEPTH_SIZE, 1,
          EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
          EGL_NONE
       };
       static const EGLint ctx_attribs[] = {
          EGL_CONTEXT_CLIENT_VERSION, 3,
          EGL_NONE
       };

    // Step 5 - Find a config that matches all requirements.
    int iConfigs;
    EGLConfig eglConfig;
    eglChooseConfig(eglDisplay, attribs, &eglConfig, 1,
                                                    &iConfigs);
    if (iConfigs != 1) {
        printf("Error: eglChooseConfig(): config not found.\n");
        exit(-1);
    }
    assert(eglConfig);
    assert(iConfigs > 0);

    EGLint vid;
    if (!eglGetConfigAttrib(eglDisplay, eglConfig, EGL_NATIVE_VISUAL_ID, &vid)) {
          printf("Error: eglGetConfigAttrib() failed\n");
          exit(1);
       }

    eglBindAPI(EGL_OPENGL_ES_API);
    // eglBindAPI(EGL_OPENGL_API);
    // Step 7 - Create a context.
    EGLContext eglContext;
	eglContext = eglCreateContext(eglDisplay, eglConfig, NULL,
			ctx_attribs);
	std::cout << glGetError() << std::endl;
   /* test eglQueryContext() */
   {
	  EGLint val;
	  eglQueryContext(eglDisplay, eglContext, EGL_CONTEXT_CLIENT_VERSION, &val);
	  std::cout << glGetError() << " val: " << val << std::endl;
	  assert(val == 3);
   }

    // Step 6 - Create a surface to draw to.
    EGLSurface eglSurface;
    eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig,
                                  (EGLNativeWindowType)NULL, NULL);
    std::cout << glGetError() << std::endl;


    // Step 8 - Bind the context to the current thread
    eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);
    std::cout << glGetError() << std::endl;

    GLuint fboId = 0;
    GLuint renderBufferWidth = 512;
    GLuint renderBufferHeight = 512;


    // create a texture object
      GLuint textureId;
     glGenTextures(1, &textureId);
     std::cout << glGetError() << std::endl;
     glBindTexture(GL_TEXTURE_2D, textureId);
     std::cout << glGetError() << std::endl;
     glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
     std::cout << glGetError() << std::endl;
     glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
     std::cout << glGetError() << std::endl;
     //GL_LINEAR_MIPMAP_LINEAR
     glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
     std::cout << glGetError() << std::endl;
     glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
     std::cout << glGetError() << std::endl;
     glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_HINT, GL_TRUE); // automatic mipmap
     std::cout << glGetError() << std::endl;
     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, renderBufferWidth, renderBufferHeight, 0,
    		 	 GL_RGBA, GL_UNSIGNED_BYTE, 0);
     std::cout << glGetError() << std::endl;
     glBindTexture(GL_TEXTURE_2D, 0);
     std::cout << glGetError() << std::endl;
     // attach the texture to FBO color attachment point
     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D, textureId, 0);
     std::cout << glGetError() << std::endl;

     GLuint renderBuffer;
     glGenRenderbuffers(1, &renderBuffer);
     std::cout << glGetError() << std::endl;
     glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
     std::cout << glGetError() << std::endl;
     glRenderbufferStorage(GL_RENDERBUFFER, GL_UNSIGNED_BYTE, renderBufferWidth, renderBufferHeight);
     std::cout << glGetError() << std::endl;
     glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderBuffer);

      std::cout << glGetError() << std::endl;
      GLuint depthRenderbuffer;
      glGenRenderbuffers(1, &depthRenderbuffer);
      std::cout << glGetError() << std::endl;
      glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
      std::cout << glGetError() << std::endl;
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16,     renderBufferWidth, renderBufferHeight);
      std::cout << glGetError() << std::endl;
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // create a framebuffer object
    glGenFramebuffers(1, &fboId);
    std::cout << glGetError() << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, (GLuint) fboId);
    std::cout << glGetError() << std::endl;
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthRenderbuffer, 0);

      GLint format = 0, type = 0;
      glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_FORMAT, &format);
      std::cout << glGetError() << std::endl;
      glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_TYPE, &type);
      std::cout << glGetError() << std::endl;
      std::cout << "colour format: " << format << " colour type: " << type << std::endl;
      // check FBO status
      GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
      if(status != GL_FRAMEBUFFER_COMPLETE)
      {
    	  std::cout << "Problem with OpenGL framebuffer after specifying color render buffer: " << status << std::endl;
    	  switch (status)
    	  {
    	  case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
    		  std::cout << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT" << std::endl;
    		  break;
    	  case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
			  std::cout << "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS" << std::endl;
			  break;
    	  case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			  std::cout << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" << std::endl;
			  break;
    	  case GL_FRAMEBUFFER_UNSUPPORTED:
			  std::cout << "GL_FRAMEBUFFER_UNSUPPORTED" << std::endl;
			  break;
    	  default:
    		  std::cout << "No issue?" << std::endl;
    	  }
      }
      else
      {
    	  std::cout << "FBO creation succedded" << std::endl;
      }

  glClearColor(1.0,0.5,0.0,1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  std::cout << eglSwapBuffers(   eglDisplay, eglSurface);
  int size = 4 * renderBufferHeight * renderBufferWidth;
  std::cout << "print size" << std::endl;
  std::cout << "size " << size << std::endl;

  unsigned char *data = new unsigned char[size];

  glReadPixels(0,0,renderBufferWidth,renderBufferHeight,
		  GL_RGBA, GL_UNSIGNED_BYTE, data);

  	// Check output buffer to make sure you cleared it properly.
	// In 5-6-5 format, clearing to clearcolor=(1, 0, 1, 1)
	// you get 1111100000011111b = 0xF81F in hex
	if( (data[0] != 0x1F) || (data[1] != 0xF8))
	{
		std::cout << "Error rendering to offscreen buffern" << std::endl;
	}

  QImage image(data, renderBufferWidth,  renderBufferHeight,renderBufferWidth*2, QImage::Format_RGB16);

  image.save("result.png");


  std::cout << "done" << std::endl;

  // QCoreApplication a(argc, argv);

  // return a.exec();
  return 0;
}
