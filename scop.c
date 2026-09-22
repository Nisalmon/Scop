#include "scop.h"
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>


int parse_obj(char *filename, Mesh *mesh) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Erreur: impossible de lire le fichier %s", filename);
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

    mesh->vertices = vertices;
    mesh->vertice_count = vertex_count;
    mesh->faces = faces;
    mesh->face_count = face_count;

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
            if (sscanf(line, "f %d %d %d %d", &v1, &v2, &v3, &v4) == 3) {
                faces[f_idx].v1 = v1;
                faces[f_idx].v2 = v2;
                faces[f_idx].v3 = v3;
                f_idx++;
            } else if (sscanf(line, "f %d %d %d %d", &v1, &v2, &v3, &v4) == 4) {
                faces[f_idx].v1 = v1;
                faces[f_idx].v2 = v2;
                faces[f_idx].v3 = v3;
                f_idx++;
                faces[f_idx].v1 = v1;
                faces[f_idx].v2 = v3;
                faces[f_idx].v3 = v4;
                f_idx++;
            }
        }
    }
}

GLuint create_shader_program() {
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    char *vertex_code = "#version 330 core\nlayout(location = 0) in vec3 vertexPosition_modelspace;\nvoid main(){\n\tgl_Position.xyz = vertexPosition_modelspace;\n\tgl_Position.w = 8.0;\n}";
    char *fragment_code = "#version 330 core\nout vec3 color;\nvoid main(){\n\tcolor = vec3(0.3,0.6,0.4);\n}";

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