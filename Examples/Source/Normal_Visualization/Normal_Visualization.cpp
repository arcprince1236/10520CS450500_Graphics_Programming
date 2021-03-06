#include "../../Include/Common.h"

#include <cstdlib>
#include <ctime>

using namespace glm;
using namespace std;

GLuint render_prog;
GLuint bunny_vao;
size_t index_count;
mat4 proj_matrix;

struct
{
    struct
    {
        GLint mvp_matrix;
    } render;
} uniforms;

const char *render_fs[] = 
{
    "#version 410 core                \n"
    "                                 \n"
	"out vec4 frag_color;             \n"
    "                                 \n"
    "void main(void)                  \n"
    "{                                \n"
	"    frag_color = vec4(1.0);      \n"
    "}                                \n"
};

const char *render_gs[] =
{
	"#version 410 core                                                                      \n"
	"                                                                                       \n"
	"layout(triangles, invocations = 1) in;                                                 \n"
	"layout(line_strip, max_vertices = 2) out;                                              \n"
	"                                                                                       \n"
	"uniform mat4 mvp_matrix;                                                               \n"
	"                                                                                       \n"
	"void main()                                                                            \n"
	"{                                                                                      \n"
	"    vec4 v10 = gl_in[1].gl_Position - gl_in[0].gl_Position;                            \n"
	"    vec4 v20 = gl_in[2].gl_Position - gl_in[0].gl_Position;                            \n"
	"    vec3 normal = normalize(cross(v10.xyz, v20.xyz));                                  \n"
	"    vec4 center = gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position;  \n"
	"    center /= 3.0;                                                                     \n"
	"    gl_Position = mvp_matrix * center;                                                 \n"
	"    EmitVertex();                                                                      \n"
	"    center.xyz += normal * 0.2;                                                        \n"
	"    gl_Position = mvp_matrix * center;                                                 \n"
	"    EmitVertex();                                                                      \n"
	"    EndPrimitive();                                                                    \n"
	"}                                                                                      \n"
};

const char *render_vs[] = 
{
    "#version 410 core                                 \n"
    "                                                  \n"
    "layout (location = 0) in vec3 position;           \n"
    "                                                  \n"
    "void main(void)                                   \n"
    "{                                                 \n"
	"    gl_Position = vec4(position, 1.0);            \n"
    "}                                                 \n"
};

void My_Init()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

	// ----- Begin Initialize Depth-Normal Only Program -----
	GLuint fs;
	GLuint vs;
	GLuint gs;
	render_prog = glCreateProgram();
	fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, render_fs, NULL);
	glCompileShader(fs);
	vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, render_vs, NULL);
	glCompileShader(vs);
	gs = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(gs, 1, render_gs, NULL);
	glCompileShader(gs);
	shaderLog(vs);
	shaderLog(gs);
	shaderLog(fs);
	glAttachShader(render_prog, vs);
	glAttachShader(render_prog, gs);
	glAttachShader(render_prog, fs);

	glLinkProgram(render_prog);
	glUseProgram(render_prog);
    uniforms.render.mvp_matrix = glGetUniformLocation(render_prog, "mvp_matrix");
	// ----- End Initialize Depth-Normal Only Program -----

	// ----- Begin Initialize Input Mesh -----
	vector<tinyobj::shape_t> shapes;
	vector<tinyobj::material_t> materials;
	string err;
	tinyobj::LoadObj(shapes, materials, err, "../../Media/Objects/bunny.obj");

	glGenVertexArrays(1, &bunny_vao);
	glBindVertexArray(bunny_vao);

	GLuint position_buffer;
	GLuint normal_buffer;
	GLuint index_buffer;

	glGenBuffers(1, &position_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
	glBufferData(GL_ARRAY_BUFFER, shapes[0].mesh.positions.size() * sizeof(float), shapes[0].mesh.positions.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &normal_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
	glBufferData(GL_ARRAY_BUFFER, shapes[0].mesh.normals.size() * sizeof(float), shapes[0].mesh.normals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, shapes[0].mesh.indices.size() * sizeof(unsigned int), shapes[0].mesh.indices.data(), GL_STATIC_DRAW);
	index_count = shapes[0].mesh.indices.size();
	// ----- End Initialize Input Mesh -----
}

void My_Display()
{
	static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	static const GLfloat ones[] = { 1.0f };
	float currentTime = glutGet(GLUT_ELAPSED_TIME) * 0.0005f;

	// ----- Begin Render Pass -----
	glClearBufferfv(GL_COLOR, 0, black);
	glClearBufferfv(GL_DEPTH, 0, ones);
	glUseProgram(render_prog);

	mat4 view_matrix = lookAt(vec3(25.0f * sinf(currentTime), 5.0f, 25.0f * cosf(currentTime)), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	mat4 mv_matrix = view_matrix * translate(mat4(), vec3(0.0f, -4.0f, 0.0f)) * scale(mat4(), vec3(5.0));
	mat4 mvp_matrix = proj_matrix * mv_matrix;
	glUniformMatrix4fv(uniforms.render.mvp_matrix, 1, GL_FALSE, &mvp_matrix[0][0]);

	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(bunny_vao);
	glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
	// ----- End Render Pass -----

	glutSwapBuffers();
}

void My_Reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	float viewportAspect = (float)width / (float)height;
	proj_matrix = perspective(deg2rad(60.0f), viewportAspect, 0.1f, 1000.0f);
}

void My_Timer(int val)
{
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