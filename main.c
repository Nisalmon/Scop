#include "scop.h"
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>



int main(int ac, char **av) {
    if (ac != 2) {
        printf("Erreur: veuillez lancer le programme en donnant uniquement le fichier .obj\n");
        return -1;
    }
    Mesh mesh;
    if (!parse_obj(av[1], &mesh)) {
        return -1;
    }
    
    GLFWwindow* window;
    if (!glfwInit()) {
        printf("Erreur: impossible de lancer la fenetre.\n");
        return -1;
    }
    window = glfwCreateWindow(800, 600, "Scop", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (glewInit() != GLEW_OK) {
        printf("Erreur: impossible d'initialiser GLEW\n");
        return -1;
    }
    GLuint *indices = malloc(mesh.face_count * 3 * sizeof(GLuint));
    for (int i = 0; i < mesh.face_count; i++) {
        indices[i * 3 + 0] = mesh.faces[i].v1 - 1;
        indices[i * 3 + 1] = mesh.faces[i].v2 - 1;
        indices[i * 3 + 2] = mesh.faces[i].v3 - 1;
    }
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertice_count * sizeof(Vertex), mesh.vertices, GL_STATIC_DRAW);
    GLuint EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        mesh.face_count * 3 * sizeof(GLuint),
        indices,
        GL_STATIC_DRAW
    );
    GLuint programID = create_shader_program();
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(programID);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(
            0, 3, GL_FLOAT,
            GL_FALSE, sizeof(Vertex), (void*)0
        );
        glDrawElements(
            GL_TRIANGLES,
            mesh.face_count * 3,
            GL_UNSIGNED_INT,
            (void *)0
        );
        glDisableVertexAttribArray(0);

        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return 0;
}
