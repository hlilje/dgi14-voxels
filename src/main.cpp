#include "../include/main.hpp"

using namespace std;

int init_resources()
{
    // Do an initial support check for textures
    int vertex_texture_units;
    glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &vertex_texture_units);
    if(!vertex_texture_units) {
        fprintf(stderr, "Your graphics cards does not support texture lookups in the vertex shader\n");
        return 0;
    }

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
    glGetShaderiv(vs, GL_COMPILE_STATUS, &compile_ok); // Get shader info

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
    "varying vec4 texcoord;         "
    "uniform sampler2D texture;     "
    "void main(void) {              "
    "    if(texcoord.w < 0.0)       "
    "        gl_FragColor = texture2D((fract(texcoord.x) + texcoord.w) / 16.0, texcoord.z);"
    "    else                       "
    "        gl_FragColor = texture2D((fract(texcoord.x + texcoord.z) + texcoord.w) / 16.0, texcoord.y);"
    "}";

    glShaderSource(fs, 1, &fs_source, NULL);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &compile_ok);

	// Get the shader log to print
	GLint logSize = 0;
    glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &logSize);
	GLchar* log = (GLchar*)malloc(logSize);
	glGetShaderInfoLog(fs, logSize, NULL, log);
	if(logSize > 1) printf("%s\n", log);
	free(log);

    if(!compile_ok)
    {
        fprintf(stderr, "Error in fragment shader\n");
        return 0;
    }

    program = glCreateProgram(); // Create empty program object
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &link_ok); // Get program info

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

    // Upload the texture with the datapoints
    glActiveTexture(GL_TEXTURE0); // Select active texture unit
    glGenTextures(1, &texture); // Generate texture names
    glBindTexture(GL_TEXTURE_2D, texture); // Bind name texture to texturing target
    // Specify 2D texture image
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textures.width, textures.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textures.pixel_data);
    glGenerateMipmap(GL_TEXTURE_2D); // Generate mipmaps for selected target

    return 1;
}

void keyPressed (unsigned char key, int x, int y)
{
	glm::vec3 sideDir = glm::normalize( glm::cross(cameraLook, glm::vec3(0, 1, 0)) );

	switch(key)
	{
		case 'w':
			cameraPos += cameraLook;
			glutPostRedisplay();
			break;
		case 's':
			cameraPos -= cameraLook;
			glutPostRedisplay();
			break;
		case 'a':
			cameraPos -= sideDir;
			glutPostRedisplay();
			break;
		case 'd':
			cameraPos += sideDir;
			glutPostRedisplay();
			break;
	}
}

void specialKeyPressed(int key, int x, int y)
{
	cameraLook = glm::normalize(cameraLook);
	glm::vec3 sideDir = glm::normalize( glm::cross(cameraLook, glm::vec3(0, 1, 0)) );
	glm::vec3 scaleVec(0.1, 0.0, 0.1);

	switch(key){
		case GLUT_KEY_UP:
			cameraLook.y += 0.1;
			glutPostRedisplay();
			break;
		case GLUT_KEY_DOWN:
			cameraLook.y -= 0.1;
			glutPostRedisplay();
			break;
		case GLUT_KEY_LEFT:
			cameraLook -= sideDir * scaleVec;
			glutPostRedisplay();
			break;
		case GLUT_KEY_RIGHT:
			cameraLook += sideDir * scaleVec;
			glutPostRedisplay();
			break;
	}
}

void updateMVP()
{
	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(70.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	glm::mat4 View       = glm::lookAt(
		cameraPos, // The position which the camera has in world space
		cameraPos + cameraLook, // and where it looks
		glm::vec3(0,1,0) // Head is up
	);
	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model      = glm::mat4(1.0f);  // Changes for each model!
	mvp        = Projection * View * Model;
}

void display()
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear current buffers

	glEnable(GL_DEPTH_TEST); // Do depth comparisons and update buffer
	glEnable(GL_POLYGON_OFFSET_FILL); // Add offset to fragments before depth comparison

	glUseProgram(program); // Set current renering state

    glUniform1i(uniform_texture, 0); // Specify location
    // Set texture interpolation mode
    // Use GL_NEAREST_MIPMAP_LINEAR to use mipmaps
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Enable generic vertex attribute array to access as defualt by vertex commands
	glEnableVertexAttribArray(attribute_coord);

	superchunk test;
	srand(time(NULL));

	// Create a PerlinNoise object with the reference permutation vector
	PerlinNoise pn;

	for(int x = 0; x < SCX; x++)
	{
		for(int y = 0; y < SCY; y++)
		{
			for(int z = 0; z < SCZ; z++)
			{
                // Generate noise
                double i = (double)x / ((double)SCX);
                double j = (double)y / ((double)SCY);
                double k = (double)z / ((double)SCZ);
                double n = pn.noise(10 * i, 10 * j, 10 * k);

                // Wood-like structure
                //n = 20 * pn.noise(i, j, k);
                //n = n - floor(n);

                int a = floor(x * n);
                int b = floor(y * n/4);
                int c = floor(z * n);
                test.set(a, b, c, 1);
			}
		}
	}

	updateMVP();
	// Specify the value of a uniform variable for the current program object
	glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));

	test.render();

	glDisableVertexAttribArray(attribute_coord);
	glutSwapBuffers(); // Buffer swap used layer for current window
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    // RGB mode, depth buffer, double buffer
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    glutCreateWindow(PROGRAM_NAME); // Create top-level window

    GLenum glew_status = glewInit();

	if (glew_status != GLEW_OK)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
		return 1;
	}

    init_resources();

	glutDisplayFunc(display); // Set display callback for current window
	glutKeyboardFunc(keyPressed); // Set keyboard callback for current window
	glutSpecialFunc(specialKeyPressed); // For func or dir keys
	glutMainLoop();

    glDeleteProgram(program); // Free resources
    return 0;
}
