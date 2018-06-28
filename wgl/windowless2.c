
#include <stdio.h>
#include <stdlib.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>

typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
typedef Bool (*glXMakeContextCurrentARBProc)(Display*, GLXDrawable, GLXDrawable, GLXContext);
static glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
static glXMakeContextCurrentARBProc glXMakeContextCurrentARB = 0;



// dumps a PPM raw (P6) file on an already allocated memory array
void DumpPPM(FILE *fp, int width, int height)
{
    const int maxVal=255;
    register int y;
    int r = 0;
    int sum = 0;
    int b_width = 3*width;
    //printf("width = %d height = %d\n",width, height) ;
    fprintf(fp,	"P6 ");
    fprintf(fp,	"%d %d ", width, height);
    fprintf(fp,	"%d\n",	maxVal);
    unsigned char m_pixels[3*width];

	// glReadBuffer(GL_FRONT) ;

	for	( y = height-1;	y>=0; y-- )
	{
		// bzero(m_pixels, 3*width);
		glReadPixels(0,y,width,1,GL_RGB,GL_UNSIGNED_BYTE,
			(GLvoid *) m_pixels);
		fwrite(m_pixels, 3, width, fp);
	}
}
int m_frameCount = 0;
int save_PPM()
{
	int width = 300;
	int height = 300;

	char fname[128] ;
	sprintf(fname,"frame%d.ppm", m_frameCount) ;
	m_frameCount++ ;
	FILE *fp = fopen(fname,"wb") ;
	if( fp == NULL )
	{
		fprintf(stderr, "Cannot open file %s\n", fname) ;
		return -1 ;
	}
	DumpPPM(fp,width,height) ;
	fclose(fp) ;
	return 1 ;
}


int main(int argc, const char* argv[]){
        static int visual_attribs[] = {
                None
        };
        int context_attribs[] = {
                GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
                GLX_CONTEXT_MINOR_VERSION_ARB, 0,
                None
        };

        Display* dpy;
        int fbcount = 0;
        GLXFBConfig* fbc = NULL;
        GLXContext ctx;
        GLXPbuffer pbuf;

        /* open display */
        if ( ! (dpy = XOpenDisplay(0)) ){
                fprintf(stderr, "Failed to open display\n");
                exit(1);
        }

        /* get framebuffer configs, any is usable (might want to add proper attribs) */
        if ( !(fbc = glXChooseFBConfig(dpy, DefaultScreen(dpy), visual_attribs, &fbcount) ) ){
                fprintf(stderr, "Failed to get FBConfig\n");
                exit(1);
        }

        /* get the required extensions */
        glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB( (const GLubyte *) "glXCreateContextAttribsARB");
        glXMakeContextCurrentARB = (glXMakeContextCurrentARBProc)glXGetProcAddressARB( (const GLubyte *) "glXMakeContextCurrent");
        if ( !(glXCreateContextAttribsARB && glXMakeContextCurrentARB) ){
                fprintf(stderr, "missing support for GLX_ARB_create_context\n");
                XFree(fbc);
                exit(1);
        }

        /* create a context using glXCreateContextAttribsARB */
        if ( !( ctx = glXCreateContextAttribsARB(dpy, fbc[0], 0, True, context_attribs)) ){
                fprintf(stderr, "Failed to create opengl context\n");
                XFree(fbc);
                exit(1);
        }

        /* create temporary pbuffer */
        int pbuffer_attribs[] = {
	        GLX_PBUFFER_WIDTH, 800,
	        GLX_PBUFFER_HEIGHT, 600,
	        None
        };
        pbuf = glXCreatePbuffer(dpy, fbc[0], pbuffer_attribs);

        XFree(fbc);
        XSync(dpy, False);

        /* try to make it the current context */
        if ( !glXMakeContextCurrent(dpy, pbuf, pbuf, ctx) ){
                /* some drivers does not support context without default framebuffer, so fallback on
                 * using the default window.
                 */
                if ( !glXMakeContextCurrent(dpy, DefaultRootWindow(dpy), DefaultRootWindow(dpy), ctx) ){
                        fprintf(stderr, "failed to make current\n");
                        exit(1);
                }
        }

        /* try it out */
        printf("vendor: %s\n", (const char*)glGetString(GL_VENDOR));

        GLuint VertexArrayID;
        glGenVertexArrays(1, &VertexArrayID);
        glBindVertexArray(VertexArrayID);

        // An array of 3 vectors which represents 3 vertices
        static const GLfloat g_vertex_buffer_data[] = {
           -1.0f, -1.0f, 0.0f,
           1.0f, -1.0f, 0.0f,
           0.0f,  1.0f, 0.0f,
        };

        // This will identify our vertex buffer
        GLuint vertexbuffer;
        // Generate 1 buffer, put the resulting identifier in vertexbuffer
        glGenBuffers(1, &vertexbuffer);
        // The following commands will talk about our 'vertexbuffer' buffer
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        // Give our vertices to OpenGL.
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

        // 1st attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
           0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
           3,                  // size
           GL_FLOAT,           // type
           GL_FALSE,           // normalized?
           0,                  // stride
           (void*)0            // array buffer offset
        );
        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
        glDisableVertexAttribArray(0);


        save_PPM();
        return 0;
}
