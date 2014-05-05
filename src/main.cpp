#include "../include/main.hpp"

using namespace std;

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

void keyPressed (unsigned char key, int x, int y) {  
	
	glm::vec3 sideDir = glm::normalize( glm::cross(cameraPos, glm::vec3(0, 1, 0)) );
	glm::vec3 cameraPosNorm = glm::normalize(cameraPos);

	switch(key){
		case 'w':
			cameraLook -= cameraPosNorm;
			cameraPos -= cameraPosNorm;
			glutPostRedisplay();
			break;
		case 's':
			cameraLook += cameraPosNorm;
			cameraPos += cameraPosNorm;
			glutPostRedisplay();
			break;
		case 'a':
			cameraPos += sideDir;
			cameraLook += sideDir;
			glutPostRedisplay();
			break;
		case 'd':
			cameraPos -= sideDir;
			cameraLook -= sideDir;
			glutPostRedisplay();
			break;
	}
}  

void specialKeyPressed(int key, int x, int y) {
	
	glm::vec3 sideDir = glm::normalize( glm::cross(cameraPos, glm::vec3(0, 1, 0)) );
	glm::vec3 cameraPosNorm = glm::normalize(cameraPos);

	switch(key){
		case GLUT_KEY_UP:
			cameraLook.y += 1;
			glutPostRedisplay();
			break;
		case GLUT_KEY_DOWN:
			cameraLook.y -= 1;
			glutPostRedisplay();
			break;
		case GLUT_KEY_LEFT:
			cameraLook += sideDir;
			glutPostRedisplay();
			break;
		case GLUT_KEY_RIGHT:
			cameraLook -= sideDir;
			glutPostRedisplay();
			break;
	}
}

static void display(){
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_POLYGON_OFFSET_FILL);

	glUseProgram(program);
	glEnableVertexAttribArray(attribute_coord);

	superchunk test;
	srand(time(NULL));

	for(int x = 0; x < SCX; x++)
	{
		for(int y = 0; y < SCY; y++)
		{
			for(int z = 0; z < SCZ; z++)
			{
				if((rand() % 4) == 0)
				{
					test.set(x, y, z, 1);
				}
			}
		}
	}

	updateMVP();
	glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));

	test.render();

	glDisableVertexAttribArray(attribute_coord);
	glutSwapBuffers();
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    glutCreateWindow(PROGRAM_NAME);

    GLenum glew_status = glewInit();

	if (glew_status != GLEW_OK)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
		return 1;
	}

    init_resources();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyPressed);
	glutSpecialFunc(specialKeyPressed);
	glutMainLoop();

    glDeleteProgram(program); // Free resources
    return 0;
}
