#include "scop.h"
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <string.h>
#include "stb_image.h"


char *MODEL_PATH = NULL;

int check_model_file(char *filename) {
    if (ft_strlen(filename) <= ft_strlen(".obj")) {
        return 0;
    }
    int i, j;
    char *extension = ".obj";
    for (i = 0; i < ft_strlen(filename); i++) {
        j = 0;
        if (filename[i] == extension[j]) {
            while (extension[j]) {
                if (filename[i+j] != extension[j]) {
                    break;
                }
                j++;
            }
            if (i + j == ft_strlen(filename) && j == ft_strlen(extension)) {
                return 1;
            }
        }
    }
    return 0;
}

int ft_strlen(char *string) {
    int i = 0;
    while (string[i]) {
        i++;
    }
    return i;
}

int parse_obj(char *filename, Mesh *mesh) {
    if (!check_model_file(filename)) {
        printf("Erreur: le fichier %s doit etre un '.obj'\n", filename);
        return 0;
    }
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Erreur: impossible de lire le fichier %s\n", filename);
        return 0;
    }
    char line[256];
    int vertex_count = 0;
    int face_count = 0;

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == 'v' && line[1] == ' ') {
            vertex_count++;
        } else if (line[0] == 'f' && line[1] == ' ') {
            int v1, v2, v3, v4;
            if (sscanf(line, "f %d %d %d %d", &v1, &v2, &v3, &v4) == 3) {
                face_count++;
            } else if (sscanf(line, "f %d %d %d %d", &v1, &v2, &v3, &v4) == 4) {
                face_count += 2;
            }
        }
    }
    Vertex *vertices = malloc(vertex_count*sizeof(Vertex));
    Face *faces = malloc(face_count*sizeof(Face));
    if (!vertices || !faces) {
        printf("Erreur: impossible d'allouer de l'espace pour vertices ou faces.");
        vertices = NULL;
        faces = NULL;
        free(vertices);
        free(faces);
        fclose(file);
        return 0;
    }
    rewind(file);
    get_vertex_values(file, vertices);
    rewind(file);
    get_face_values(file, faces);

    MODEL_PATH = filename;
    mesh->MODEL_PATH = filename;
    mesh->vertices = vertices;
    mesh->vertice_count = vertex_count;
    mesh->faces = faces;
    mesh->face_count = face_count;

    get_center_mesh(mesh);

    vertices = NULL;
    faces = NULL;
    free(vertices);
    free(faces);
    fclose(file);
    return 1;
}

void get_vertex_values(FILE *file, Vertex *vertices) {
    char line[256];
    int v_idx = 0;

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == 'v' && line[1] == ' ') {
            sscanf(line, "v %f %f %f", &vertices[v_idx].x, &vertices[v_idx].y, &vertices[v_idx].z);
            v_idx++;
        }
    }
}

void get_face_values(FILE *file, Face *faces) {
    char line[256];
    int f_idx = 0;

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == 'f' && line[1] == ' ') {
            int v1, v2, v3, v4;
            float gray = 0.0f + ((float)rand() / (float)RAND_MAX) * 0.6f;
            if (sscanf(line, "f %d %d %d %d", &v1, &v2, &v3, &v4) == 3) {
                faces[f_idx].v1 = v1;
                faces[f_idx].v2 = v2;
                faces[f_idx].v3 = v3;
                faces[f_idx].color = gray;
                f_idx++;
            } else if (sscanf(line, "f %d %d %d %d", &v1, &v2, &v3, &v4) == 4) {
                faces[f_idx].v1 = v1;
                faces[f_idx].v2 = v2;
                faces[f_idx].v3 = v3;
                faces[f_idx].color = gray;
                f_idx++;
                gray = 0.0f + ((float)rand() / (float)RAND_MAX) * 0.6f;
                faces[f_idx].v1 = v1;
                faces[f_idx].v2 = v3;
                faces[f_idx].v3 = v4;
                faces[f_idx].color = gray;
                f_idx++;
            }
        }
    }
}

void get_center_mesh(Mesh *mesh) {
    float min_x = mesh->vertices[0].x;
    float max_x = mesh->vertices[0].x;
    float min_y = mesh->vertices[0].y;
    float max_y = mesh->vertices[0].y;
    float min_z = mesh->vertices[0].z;
    float max_z = mesh->vertices[0].z;

    for (int i = 0; i < mesh->vertice_count; i++) {
        if (mesh->vertices[i].x < min_x)
            min_x = mesh->vertices[i].x;
        if (mesh->vertices[i].x > max_x)
            max_x = mesh->vertices[i].x;

        if (mesh->vertices[i].y < min_y)
            min_y = mesh->vertices[i].y;
        if (mesh->vertices[i].y > max_y)
            max_y = mesh->vertices[i].y;

        if (mesh->vertices[i].z < min_z)
            min_z = mesh->vertices[i].z;
        if (mesh->vertices[i].z > max_z)
            max_z = mesh->vertices[i].z;
    }
    mesh->min_x = min_x;
    mesh->max_x = max_x;
    mesh->min_y = min_y;
    mesh->max_y = max_y;
    mesh->min_z = min_z;
    mesh->max_z = max_z;
    mesh->center_x = (min_x + max_x) / 2.0f;
    mesh->center_y = (min_y + max_y) / 2.0f;
    mesh->center_z = (min_z + max_z) / 2.0f;
}

GLuint create_shader_program() {
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    const char *vertex_code =
        "#version 330 core\n"

        "layout(location = 0) in vec3 vertexPosition_modelspace;\n"
        "layout(location = 1) in float vertexColor;\n"
        "layout(location = 2) in vec2 vertexUV;\n"

        "uniform mat4 projection;\n"
        "uniform mat4 model;\n"

        "out float color;\n"
        "out vec2 UV;\n"

        "void main()\n"
        "{\n"
        "    gl_Position = projection * model * vec4(vertexPosition_modelspace, 1.0);\n"
        "    color = vertexColor;\n"
        "    UV = vertexUV;\n"
        "}\n";
    const char *fragment_code =
        "#version 330 core\n"

        "in float color;\n"
        "in vec2 UV;\n"

        "uniform sampler2D textureSampler;\n"
        "uniform int useTexture;\n"

        "out vec4 finalColor;\n"

        "void main()\n"
        "{\n"
        "    if (useTexture == 1)\n"
        "        finalColor = texture(textureSampler, UV);\n"
        "    else\n"
        "        finalColor = vec4(color, color, color, 1.0);\n"
        "}\n";
    GLint Result = GL_FALSE;
    int InfoLogLength;

    char const *vertex_source_pointer = vertex_code;
    glShaderSource(VertexShaderID, 1, &vertex_source_pointer , NULL);
	glCompileShader(VertexShaderID);

    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);

    char const *fragement_source_code = fragment_code;
    glShaderSource(FragmentShaderID, 1, &fragement_source_code , NULL);
	glCompileShader(FragmentShaderID);

    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);

    GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

    glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);
	
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

    return ProgramID;
}

void move(float *pos_x, float *pos_y, float *pos_z, float *angle_x, float *angle_y, float *angle_z, GLFWwindow *window, float delta) {
    float speed = 2.0f;

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        *pos_x -= speed * delta;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        *pos_x += speed * delta;
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        *pos_y += speed * delta;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        *pos_y -= speed * delta;
    }

    if (glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS) {
        *pos_z += speed * delta;
    }
    if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS) {
        *pos_z -= speed * delta;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        *angle_x += speed * delta;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        *angle_x -= speed * delta;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        *angle_y += speed * delta;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        *angle_y -= speed * delta;
    }

    if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS) {
        *angle_z += speed * delta;
    }
    if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS) {
        *angle_z -= speed * delta;
    }
}

Mat4 mat4_identity(void)
{
    Mat4 result = {0};

    result.m[0]  = 1.0f;
    result.m[5]  = 1.0f;
    result.m[10] = 1.0f;
    result.m[15] = 1.0f;

    return result;
}

Mat4 mat4_perspective(float fov,
                      float aspect,
                      float near,
                      float far)
{
    Mat4 result = {0};

    float tan_half_fov = tanf(fov / 2.0f);

    result.m[0] = 1.0f / (aspect * tan_half_fov);
    result.m[5] = 1.0f / tan_half_fov;

    result.m[10] = -(far + near) / (far - near);
    result.m[11] = -1.0f;

    result.m[14] = -(2.0f * far * near) / (far - near);

    return result;
}

Mat4 mat4_translate(float x, float y, float z)
{
    Mat4 result = mat4_identity();

    result.m[12] = x;
    result.m[13] = y;
    result.m[14] = z;

    return result;
}

Mat4 mat4_multiply(Mat4 a, Mat4 b)
{
    Mat4 result = {0};

    for (int col = 0; col < 4; col++) {
        for (int row = 0; row < 4; row++) {
            result.m[col * 4 + row] =
                a.m[0 * 4 + row] * b.m[col * 4 + 0] +
                a.m[1 * 4 + row] * b.m[col * 4 + 1] +
                a.m[2 * 4 + row] * b.m[col * 4 + 2] +
                a.m[3 * 4 + row] * b.m[col * 4 + 3];
        }
    }

    return result;
}

Mat4 mat4_rotate_y(float angle)
{
    Mat4 result = mat4_identity();

    float c = cosf(angle);
    float s = sinf(angle);

    result.m[0] = c;
    result.m[2] = -s;

    result.m[8] = s;
    result.m[10] = c;

    return result;
}

Mat4 mat4_rotate_x(float angle)
{
    Mat4 result = mat4_identity();

    float c = cosf(angle);
    float s = sinf(angle);

    result.m[5] = c;
    result.m[6] = s;

    result.m[9] = -s;
    result.m[10] = c;

    return result;
}

Mat4 mat4_rotate_z(float angle)
{
    Mat4 result = mat4_identity();

    float c = cosf(angle);
    float s = sinf(angle);

    result.m[0] = c;
    result.m[1] = s;

    result.m[4] = -s;
    result.m[5] = c;

    return result;
}

RenderVertex *create_render_vertices(Mesh *mesh)
{
    RenderVertex *vertices;

    vertices = malloc(mesh->face_count * 3 * sizeof(RenderVertex));
    if (!vertices)
        return NULL;

    float width = mesh->max_x - mesh->min_x;
    float height = mesh->max_y - mesh->min_y;

    if (width == 0.0f)
        width = 1.0f;

    if (height == 0.0f)
        height = 1.0f;

    for (int i = 0; i < mesh->face_count; i++)
    {
        Face *face = &mesh->faces[i];

        Vertex *v1 = &mesh->vertices[face->v1 - 1];
        Vertex *v2 = &mesh->vertices[face->v2 - 1];
        Vertex *v3 = &mesh->vertices[face->v3 - 1];

        vertices[i * 3 + 0].x = v1->x;
        vertices[i * 3 + 0].y = v1->y;
        vertices[i * 3 + 0].z = v1->z;
        vertices[i * 3 + 0].color = face->color;
        vertices[i * 3 + 0].u =
            (v1->x - mesh->min_x) / width;

        vertices[i * 3 + 0].v =
            1.0f - (v1->y - mesh->min_y) / height;

        vertices[i * 3 + 1].x = v2->x;
        vertices[i * 3 + 1].y = v2->y;
        vertices[i * 3 + 1].z = v2->z;
        vertices[i * 3 + 1].color = face->color;
        vertices[i * 3 + 1].u =
            (v2->x - mesh->min_x) / width;

        vertices[i * 3 + 1].v =
            1.0f - (v2->y - mesh->min_y) / height;

        vertices[i * 3 + 2].x = v3->x;
        vertices[i * 3 + 2].y = v3->y;
        vertices[i * 3 + 2].z = v3->z;
        vertices[i * 3 + 2].color = face->color;
        vertices[i * 3 + 2].u =
            (v3->x - mesh->min_x) / width;

        vertices[i * 3 + 2].v =
            1.0f - (v3->y - mesh->min_y) / height;
    }

    return vertices;
}

void bob(float *pos_y, float delta, int *dir, float *dist_from_origin) {
    if (*dist_from_origin >= 0.8) {
        *dir = -1;
    } else if (*dist_from_origin <= -0.8) {
        *dir = 1;
    }
    *pos_y += *dir * delta;
    *dist_from_origin += *dir * delta;
}

void init_flags(Flags* flags) {
    flags->move = 0;
    flags->help = 0;
}

void check_flags(Flags *flags, char **args) {
    int i = 0;
    flags->texture_path = "\0";
    while (args[i]) {
        if (strcmp(args[i], "--move") == 0)
            flags->move = 1;
        else if (strcmp(args[i], "--help") == 0) {
            flags->help = 1;
        }
        else if (strcmp(args[i], "--texture") == 0) {
            flags->texture = 1;
            if (args[i + 1])
                flags->texture_path = args[i + 1];
            else
                flags->texture = 0;
        }
        i++;
    }
}

void helper() {
    printf("Welcome to scop.\n");
    printf("To run this program, you must execute:\n");
    printf("\t./scop ...\n\n");
    printf("You need to replace '...' with the model and flags.\n");
    printf("After the program name, you must put your model you want to render.\n");
    printf("For the flags, here is the list:\n");
    printf("\t--help: Will print the helper.\n");
    printf("\t--move: Will allow you to move the model freely.\n");

}

GLuint load_texture(const char *filename)
{
    int width;
    int height;
    int channels;

    unsigned char *data = stbi_load(
        filename,
        &width,
        &height,
        &channels,
        0
    );

    if (!data)
    {
        printf("Erreur: impossible de charger la texture %s\n", filename);
        return 0;
    }

    GLenum format;

    if (channels == 4)
        format = GL_RGBA;
    else if (channels == 3)
        format = GL_RGB;
    else
    {
        printf("Erreur: format de texture non supporte\n");
        stbi_image_free(data);
        return 0;
    }

    GLuint texture;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_S,
        GL_REPEAT
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_T,
        GL_REPEAT
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_LINEAR_MIPMAP_LINEAR
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MAG_FILTER,
        GL_LINEAR
    );

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        format,
        width,
        height,
        0,
        format,
        GL_UNSIGNED_BYTE,
        data
    );

    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    return texture;
}

void drop_callback(GLFWwindow *window, int count, const char** paths) {
    int i;
    if (count != 1)
        return;
    MODEL_PATH = NULL;
    ft_strdup((char*)paths[0], &MODEL_PATH);
}

char *get_model_path() {
    return MODEL_PATH;
}

void reload_model(GLuint *VBO, Mesh *mesh, RenderVertex **render_vertices) {
    mesh->MODEL_PATH = get_model_path();
    if (!parse_obj(mesh->MODEL_PATH, mesh)) {
        return;
    }
    *render_vertices = create_render_vertices(mesh);
    glGenBuffers(1, VBO);
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh->face_count * 3 * sizeof(RenderVertex), *render_vertices, GL_STATIC_DRAW);
}

void ft_strdup(char *src, char **dest) {
    int len_src = ft_strlen(src);
    *dest = malloc(sizeof(char) * len_src + 1);
    if (*dest == NULL)
        return;
    int i;
    for (i = 0; i < len_src; i++) {
        (*dest)[i] = src[i];
    }
    (*dest)[i] = '\0';
}
