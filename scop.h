#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifndef SCOP_H

typedef struct {
    float x;
    float y;
    float z;
} Vertex;

typedef struct {
    int v1;
    int v2;
    int v3;
} Face;

typedef struct {
    Vertex *vertices;
    int vertice_count;
    Face *faces;
    int face_count;
} Mesh;

int  parse_obj(char *filename, Mesh *mesh);
void get_vertex_values(FILE *file, Vertex *vertices);
void get_face_values(FILE *file, Face *faces);
GLuint create_shader_program();


#endif