#include "../../Include/Common.h"

static const char * vs_source[] =
{
	"#version 410 core                                                 \n"
	"                                                                  \n"
	"layout(location = 0) in vec4 offset;                              \n"
	"                                                                  \n"
	"void main(void)                                                   \n"
	"{                                                                 \n"
	"    const vec4 vertices[] = vec4[](vec4( 0.25, -0.25, 0.5, 1.0),  \n"
	"                                   vec4(-0.25, -0.25, 0.5, 1.0),  \n"
	"                                   vec4( 0.25,  0.25, 0.5, 1.0)); \n"
	"                                                                  \n"
	"    gl_Position = vertices[gl_VertexID] + offset;				   \n"
	"}                                                                 \n"
};

static const char * fs_source[] =
{
	"#version 410 core                                                 \n"
	"                                                                  \n"
	"out vec4 color;                                                   \n"
	"                                                                  \n"
	"void main(void)                                                   \n"
	"{                                                                 \n"
	"    color = vec4(0.0, 0.8, 1.0, 1.0);                             \n"
	"}                                                                 \n"
};

GLuint          program;
GLuint          vao;
float			timer_count;

void My_Init()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	program = glCreateProgram();
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, fs_source, NULL);
	glCompileShader(fs);

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, vs_source, NULL);
	glCompileShader(vs);

	glAttachShader(program, vs);
	glAttachShader(program, fs);

	glLinkProgram(program);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
}

// GLUT callback. Called to draw the scene.
void My_Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(program);
	GLfloat attrib[] = {sinf(timer_count) * 0.5f, cosf(timer_count) * 0.6f, 0.0f, 0.0f};
	glVertexAttrib4fv(0, attrib);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glutSwapBuffers();
}

void My_Reshape(int width, int height)
{
	glViewport(0, 0, width, height);
}

void My_Timer(int val)
{
	timer_count += 0.05f;
	glutPostRedisplay();
	glutTimerFunc(16, My_Timer, val);
}

int main(int argc, char *argv[])
{
    // Change working directory to source code path
    chdir(__FILEPATH__);
	// Initialize GLUT and GLEW, then create a window.
	////////////////////
	glutInit(&argc, argv);
#ifdef _MSC_VER
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#else
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#endif
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(600, 600);
	glutCreateWindow(__FILENAME__); // You cannot use OpenGL functions before this line; The OpenGL context must be created first by glutCreateWindow()!
#ifdef _MSC_VER
	glewInit();
#endif
	dumpInfo();
	My_Init();
	////////////////////

	// Register GLUT callback functions.
	///////////////////////////////
	glutDisplayFunc(My_Display);
	glutReshapeFunc(My_Reshape);
	glutTimerFunc(16, My_Timer, 0); 
	///////////////////////////////

	// Enter main event loop.
	//////////////
	glutMainLoop();
	//////////////
	return 0;
}