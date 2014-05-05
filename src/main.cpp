#include "../include/main.hpp"

using namespace std;

// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
glm::mat4 Projection = glm::perspective(70.0f, 4.0f / 3.0f, 0.1f, 100.0f);
// Camera matrix
glm::mat4 View       = glm::lookAt(
    glm::vec3(25, 25, 25), // The position which the camera has in world space
    glm::vec3(0,0,0), // and looks at the origin
    glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
);
// Model matrix : an identity matrix (model will be at the origin)
glm::mat4 Model      = glm::mat4(1.0f);  // Changes for each model !
// Our ModelViewProjection : multiplication of our 3 matrices
glm::mat4 mvp        = Projection * View * Model; // Remember, matrix multiplication is the other way around

int init_resources()
{
    GLint compile_ok = GL_FALSE, link_ok = GL_FALSE;

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    const char *vs_source =
#ifdef GL_ES_VERSION_2_0
    "#version 100\n"  // OpenGL ES 2.0
#else
    "#version 120\n"  // OpenGL 2.1
#endif
    "attribute vec4 coord;                       "
    "varying vec4 texcoord;                      "
    "uniform mat4 mvp;                           "
    "void main(void) {                           "
    "    texcoord = coord;                       "
    "    gl_Position = mvp * vec4(coord.xyz, 1); "
    "}";

    glShaderSource(vs, 1, &vs_source, NULL);
    glCompileShader(vs);
    glGetShaderiv(vs, GL_COMPILE_STATUS, &compile_ok);

    if(!compile_ok)
    {
        fprintf(stderr, "Error in vertex shader\n");
        return 0;
    }

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    const char *fs_source =
#ifdef GL_ES_VERSION_2_0
    "#version 100\n"  // OpenGL ES 2.0
#else
    "#version 120\n"  // OpenGL 2.1
#endif
    "varying vec4 texcoord;  "
    "void main(void) {        "
    "    gl_FragColor = vec4(texcoord.x / 16.0, texcoord.y / 16.0, texcoord.z / 16.0, 1.0);"
    "}";

    glShaderSource(fs, 1, &fs_source, NULL);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &compile_ok);

    if(!compile_ok)
    {
        fprintf(stderr, "Error in fragment shader\n");
        return 0;
    }

    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &link_ok);

    if(!link_ok)
    {
        fprintf(stderr, "glLinkProgram:");
        return 0;
    }

    attribute_coord = glGetAttribLocation(program, "coord");
	uniform_mvp = glGetUniformLocation(program, "mvp");

    if(attribute_coord == -1 || uniform_mvp == -1)
    {
        fprintf(stderr, "Could not bind 'some' attribute\n");
        return 0;
    }

    return 1;
}

static void display(){
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_POLYGON_OFFSET_FILL);

	glUseProgram(program);
	glEnableVertexAttribArray(attribute_coord);

	chunk test;
	srand(time(NULL));
	for(int x = 0; x < CX; x++){
		for(int y = 0; y < CY; y++){
			for(int z = 0; z < CZ; z++){
				if((rand() % 4) == 0){ 
					test.set(x, y, z, 1);
				}
			}
		}
	}

	glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
	
	test.render();

	glDisableVertexAttribArray(attribute_coord);
	glutSwapBuffers();
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Voxel Renderer");

    GLenum glew_status = glewInit();
	
	if (glew_status != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
		return 1;
	}

    init_resources();

	glutDisplayFunc(display);
	glutMainLoop();

    glDeleteProgram(program); // Free resources
    return 0;
}
