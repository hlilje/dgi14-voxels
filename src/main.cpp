#include "../include/main.hpp"

using namespace std;

// Reads a string from file
std::string read_file(const char *filePath)
{
    std::string content;
    std::ifstream fileStream(filePath, std::ios::in);

    if(!fileStream.is_open())
    {
        std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
        return "";
    }

    std::string line = "";
    while(!fileStream.eof())
    {
        std::getline(fileStream, line);
        content.append(line + "\n");
    }

    fileStream.close();
    return content;
}

int init_resources()
{
    // Do an initial support check for textures
    int vertex_texture_units;
    glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &vertex_texture_units);
    if(!vertex_texture_units)
    {
        fprintf(stderr, "Your graphics cards does not support texture lookups in the vertex shader\n");
        return 0;
    }

    GLint compile_ok = GL_FALSE, link_ok = GL_FALSE;

    // Create vertex shader
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);

#ifdef _MSC_VER
    string vs_string = read_file("../shader/shader.v.glsl");
#else
    string vs_string = read_file("shader/shader.v.glsl");
#endif
    const GLchar* vstr = vs_string.c_str();

    glShaderSource(vs, 1, &vstr, NULL);
    glCompileShader(vs);
    glGetShaderiv(vs, GL_COMPILE_STATUS, &compile_ok); // Get shader info

    // Get the vertex shader log to print
    GLint log_size = 0;
    glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &log_size);
    GLchar* log = (GLchar*)malloc(log_size);
    glGetShaderInfoLog(vs, log_size, NULL, log);
    if(log_size > 1) printf("%s\n", log);
    free(log);

    if(!compile_ok)
    {
        fprintf(stderr, "Error in vertex shader\n");
        return 0;
    }

    // Create fragment shader
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

#ifdef _MSC_VER
    string fs_string = read_file("../shader/shader.f.glsl");
#else
    string fs_string = read_file("shader/shader.f.glsl");
#endif
    const GLchar* fstr = fs_string.c_str();

    glShaderSource(fs, 1, &fstr, NULL);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &compile_ok);

    // Print the fragment shader log
    glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &log_size);
    log = (GLchar*)malloc(log_size);
    glGetShaderInfoLog(fs, log_size, NULL, log);
    if(log_size > 1) printf("%s\n", log);
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

    glUseProgram(program); // Set current renering state

    glUniform1i(uniform_texture, 0); // Specify location
    // Set texture interpolation mode
    // Use GL_NEAREST_MIPMAP_LINEAR to use mipmaps
    // GL_NEAREST gives the pixel closest to the coordinates
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Give red borders for debug
    float border_colour[] = { 1.0f, 0.0f, 0.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_colour);

    // Enable generic vertex attribute array to access as default by vertex commands
    glEnableVertexAttribArray(attribute_coord);

    return 1;
}

void keyPressed (unsigned char key, int x, int y)
{
    glm::vec3 side_dir = glm::normalize(glm::cross(camera_look, glm::vec3(0, 1, 0)));

    switch(key)
    {
        case 'w':
            camera_pos += camera_look;
            break;

        case 's':
            camera_pos -= camera_look;
            break;

        case 'a':
            camera_pos -= side_dir;
            break;

        case 'd':
            camera_pos += side_dir;
            break;

        case 32: //Space bar
            camera_pos.y += 1.0;
            break;

        case 27: //Escape key
            glDeleteProgram(program);
            exit (0);
            break;
    }

    glutPostRedisplay();
}

void specialKeyPressed(int key, int x, int y)
{
    camera_look = glm::normalize(camera_look);
    glm::vec3 side_dir = glm::normalize(glm::cross(camera_look, glm::vec3(0, 1, 0)));
    glm::vec3 scale_vec(0.1, 0.0, 0.1);

    switch(key)
    {
        case GLUT_KEY_UP:
            camera_look.y += 0.1;
            break;

        case GLUT_KEY_DOWN:
            camera_look.y -= 0.1;
            break;

        case GLUT_KEY_LEFT:
            camera_look -= side_dir * scale_vec;
            break;

        case GLUT_KEY_RIGHT:
            camera_look += side_dir * scale_vec;
            break;
    }

    glutPostRedisplay();
}

void motion(int x, int y)
{
    glm::vec3 side_dir = glm::normalize(glm::cross(camera_look, glm::vec3(0, 1, 0)));
    camera_look = glm::normalize(camera_look);

    static bool wrap = false;
    float mouse_sensitivity = 1.0 / 200.0; // How many units the camera moves per pixel of mouse movement

    if(!wrap)
    {
        int ww = glutGet(GLUT_WINDOW_WIDTH);
        int wh = glutGet(GLUT_WINDOW_HEIGHT);

        int dx = x - ww / 2;
        int dy = y - wh / 2;

        // Do something with dx and dy here
        camera_look += side_dir * glm::vec3(float(dx) * mouse_sensitivity, 0, float(dx) * mouse_sensitivity);
        camera_look.y -= float(dy) * mouse_sensitivity;
        glutPostRedisplay();

        // Move mouse pointer back to the center of the window
        wrap = true;
        glutWarpPointer(ww / 2, wh / 2);
    }
    else
    {
        wrap = false;
    }
}

void update_mvp()
{
    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    glm::mat4 projection = glm::perspective(70.0f, 4.0f / 3.0f, 0.1f, 500.0f);
    // Camera matrix
    glm::mat4 view = glm::lookAt(
        camera_pos, // The position which the camera has in world space
        camera_pos + camera_look, // and where it looks
        glm::vec3(0,1,0) // Head is up
    );
    mvp = projection * view * model;
}

void display()
{
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear current buffers

    glEnable(GL_DEPTH_TEST); // Do depth comparisons and update buffer
    glEnable(GL_POLYGON_OFFSET_FILL); // Add offset to fragments before depth comparison
    glEnable(GL_TEXTURE_2D); // Needed for fixed pipeline
    glEnable(GL_LIGHTING); // Needed for fixed pipeline

    world.render(); // Render the superchunk

    //glDisableVertexAttribArray(attribute_coord);
    glutSwapBuffers(); // Buffer swap used layer for current window
}

void generate_terrain()
{
    srand(time(NULL));
    int seed = rand();
    double noise;

    for(int x = 0; x < (CX * SCX); x++)
    {
        for(int z = 0; z < (CZ * SCZ); z++)
        {
            // The noise function expects coordinates from 0.1 to 1.0
            noise = noise2d_perlin(double(x) / double(CX * SCX), double(z) / double(CZ * SCZ), seed, 10, 0.60);

            for(int y = 0; y < (CY * SCY); y++)
            {
                if(y < CY * SCY * (0.15 + noise / 10))
                {
                    world.set(x, y, z, 3);
                }
            }
        }
    }

    world.render();

    update_mvp();
    // Specify the value of a uniform variable for the current program object
    glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
}



int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    // RGB mode, depth buffer, double buffer
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    glutCreateWindow(PROGRAM_NAME); // Create top-level window

    GLenum glew_status = glewInit();

    if(glew_status != GLEW_OK)
    {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
        return 1;
    }

    init_resources();
    generate_terrain();

    glutDisplayFunc(display); // Set display callback for current window
    glutKeyboardFunc(keyPressed); // Set keyboard callback for current window
    glutSpecialFunc(specialKeyPressed); // For func or dir keys
    glutMotionFunc(motion);
    glutPassiveMotionFunc(motion);
    glutSetCursor(GLUT_CURSOR_NONE);

    glutMainLoop();

    glDeleteProgram(program); // Free resources
    return 0;
}
