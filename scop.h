#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>

#ifndef SCOP_H

typedef struct {
    int move;
    int help;
    int texture;
    char *texture_path;
} Flags;

typedef struct {
    float x;
    float y;
    float z;
} Vertex;

typedef struct {
    int v1;
    int v2;
    int v3;
    float color;
} Face;

typedef struct {
    char *MODEL_PATH;
    Vertex *vertices;
    int vertice_count;
    Face *faces;
    int face_count;
    float center_x;
    float center_y;
    float center_z;
    float min_x;
    float max_x;
    float min_y;
    float max_y;
    float min_z;
    float max_z;
} Mesh;

typedef struct {
    float x;
    float y;
    float z;
    float color;
    float u;
    float v;
} RenderVertex;

typedef struct {
    float m[16];
} Mat4;

int check_model_file(char *filename);
int ft_strlen(char *string);
int  parse_obj(char *filename, Mesh *mesh);
char *get_model_path();
void ft_strdup(char *src, char **dest);
void init_flags(Flags* flags);
void check_flags(Flags *flags, char **args);
void helper();
void get_vertex_values(FILE *file, Vertex *vertices);
void get_face_values(FILE *file, Face *faces);
void get_center_mesh(Mesh *mesh);
GLuint create_shader_program();
void reload_model(GLuint *VBO, Mesh *mesh, RenderVertex **render_vertices);
void move(float *pos_x, float *pos_y, float *pos_z, float *angle_x, float *angle_y, float *angle_z, GLFWwindow *window, float delta);
void bob(float *pos_y, float delta, int *dir, float *dist_from_origin);
void drop_callback(GLFWwindow *window, int count, const char** paths);
Mat4 mat4_identity(void);
Mat4 mat4_perspective(float fov,
                      float aspect,
                      float near,
                      float far);
Mat4 mat4_translate(float x, float y, float z);
Mat4 mat4_multiply(Mat4 a, Mat4 b);
Mat4 mat4_rotate_x(float angle);
Mat4 mat4_rotate_y(float angle);
Mat4 mat4_rotate_z(float angle);
RenderVertex *create_render_vertices(Mesh *mesh);
GLuint load_texture(const char *filename);

#endif