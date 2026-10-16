#include "scop.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "string.h"



int main(int ac, char **av) {
    if (ac < 2) {
        helper();
        return -1;
    }
    Flags flags;
    init_flags(&flags);
    check_flags(&flags, av);
    if (flags.help) {
        helper();
        return 1;
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
    window = glfwCreateWindow(1280, 720, "SCOP", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (glewInit() != GLEW_OK) {
        printf("Erreur: impossible d'initialiser GLEW\n");
        return -1;
    }
    RenderVertex *render_vertices = create_render_vertices(&mesh);
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.face_count * 3 * sizeof(RenderVertex), render_vertices, GL_STATIC_DRAW);
    GLuint programID = create_shader_program();
    GLint projectionID = glGetUniformLocation(programID, "projection");
    GLint modelID = glGetUniformLocation(programID, "model");
    GLint textureID = glGetUniformLocation(programID, "textureSampler");
    GLint useTextureID = glGetUniformLocation(programID, "useTexture");
    int use_texture = flags.texture;
    GLuint texture = 0;
    if (use_texture == 1 && flags.texture_path[0] != '\0')
    {
        texture = load_texture(flags.texture_path);
        if (texture == 0)
            return -1;
    }
    float time = (float)glfwGetTime();
    float delta;
    glEnable(GL_DEPTH_TEST);
    float fov = 45.0f * 3.14159265359f / 180.0f;

    Mat4 projection = mat4_perspective(
        fov,
        800.0f / 600.0f,
        0.1f,
        100.0f
    );
    float pos_x = 0.0f;
    float pos_y = 0.0f;
    float pos_z = -15.0f;
    float angle_x = 0.0f;
    float angle_y = 0.0f;
    float angle_z = 0.0f;
    Mat4 to_origin = mat4_translate(
        -mesh.center_x,
        -mesh.center_y,
        -mesh.center_z
    );
    Mat4 from_origin = mat4_translate(
        mesh.center_x,
        mesh.center_y,
        mesh.center_z
    );
    glfwSetDropCallback(window, drop_callback);
    Mat4 model = mat4_translate(pos_x, pos_y, pos_z);
    int dir = 1;
    float dist_from_origin = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        if (strcmp(mesh.MODEL_PATH, get_model_path()) != 0) {
            reload_model(&VBO, &mesh, &render_vertices);
        }
        glfwPollEvents();
        float current = (float)glfwGetTime();
        delta = current - time;
        time = current;
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
        if (!flags.move) {
            angle_y -= 2.0f * delta;
            bob(&pos_y, delta, &dir, &dist_from_origin);
        } else {
            move(&pos_x, &pos_y, &pos_z, &angle_x, &angle_y, &angle_z, window, delta);
        }
        Mat4 translation = mat4_translate(
            pos_x,
            pos_y,
            pos_z
        );
        Mat4 rotation_x = mat4_rotate_x(angle_x);
        Mat4 rotation_y = mat4_rotate_y(angle_y);
        Mat4 rotation_z = mat4_rotate_z(angle_z);

        Mat4 rotation =
            mat4_multiply(rotation_y, rotation_x);

        rotation =
            mat4_multiply(rotation, rotation_z);
        model = mat4_multiply(
            translation,
            from_origin
        );
        model = mat4_multiply(
            model,
            rotation
        );
        model = mat4_multiply(
            model,
            to_origin
        );
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(programID);
        glUniformMatrix4fv(
            projectionID,
            1,
            GL_FALSE,
            projection.m
        );
        glUniformMatrix4fv(
            modelID,
            1,
            GL_FALSE,
            model.m
        );
        glUniform1i(
            useTextureID,
            use_texture
        );
        if (use_texture == 1 && texture != 0)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);

            glUniform1i(
                textureID,
                0
            );
        }
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(
            0, 3, GL_FLOAT,
            GL_FALSE, sizeof(RenderVertex), (void*)offsetof(RenderVertex, x)
        );
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(
            1,
            1,
            GL_FLOAT,
            GL_FALSE,
            sizeof(RenderVertex),
            (void *)offsetof(RenderVertex, color)
        );
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(
            2,
            2,
            GL_FLOAT,
            GL_FALSE,
            sizeof(RenderVertex),
            (void *)offsetof(RenderVertex, u)
        );
        glDrawArrays(
            GL_TRIANGLES,
            0,
            mesh.face_count * 3
        );
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
