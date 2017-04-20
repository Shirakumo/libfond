#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "../src/fond.h"

const GLchar *vertex_shader = "#version 330 core\n"
  "layout (location = 0) in vec3 position;\n"
  "layout (location = 1) in vec2 in_texcoord;\n"
  "out vec2 texcoord;\n"
  "\n"
  "void main(){\n"
  "  gl_Position = vec4(position, 1.0);\n"
  "  texcoord = in_texcoord;\n"
  "}";

const GLchar *fragment_shader = "#version 330 core\n"
  "uniform sampler2D tex_image;\n"
  "in vec2 texcoord;\n"
  "out vec4 color;\n"
  "\n"
  "void main(){\n"
  "  color = texture(tex_image, texcoord);\n"
  "}\n";

int load_stuff(char *file, struct fond_font *font, struct fond_buffer *buffer){
  font->file = file;
  font->size = 12.0;
  font->characters =
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789.-;:?!/()😺";
  
  printf("Loading font...\n");
  if(!fond_load_fit(font, 2048))
    return 0;

  buffer->font = font;
  buffer->width = 256;
  buffer->height = 256;
  printf("Loading buffer...\n");
  if(!fond_load_buffer(buffer))
    return 0;

  printf("Rendering buffer...\n");
  if(!fond_render(buffer, "Hello Everynyan! 😺"))
    return 0;
  
  return 1;
}

void render(GLuint program, GLuint vao, struct fond_buffer *buffer){
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glBindTexture(GL_TEXTURE_2D, buffer->texture);
  glUseProgram(program);
  glBindVertexArray(vao);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

int main(int argc, char **argv){
  if(argc == 1){
    printf("Please specify a TTF file to load.\n");
    return 0;
  }

  printf("Initializing GL...\n");
  
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLFWwindow* window = glfwCreateWindow(800, 600, "Fond Test", 0, 0);
  if(window == 0){
    printf("Failed to create GLFW window\n");
    goto main_cleanup;
  }
  glfwMakeContextCurrent(window);

  glewExperimental = GL_TRUE;
  if(glewInit() != GLEW_OK){
    printf("Failed to initialize GLEW\n");
    goto main_cleanup;
  }

  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);

  GLuint vert, frag, program, vbo, ebo, vao;
  vert = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vert, 1, &vertex_shader, 0);
  glCompileShader(vert);

  frag = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(frag, 1, &fragment_shader, 0);
  glCompileShader(frag);

  program = glCreateProgram();
  glAttachShader(program, vert);
  glAttachShader(program, frag);
  glLinkProgram(program);

  GLfloat vertices[] = {
     0.5f,  0.5f, 0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, 0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, 0.0f,  0.0f, 1.0f
  };
  GLuint indices[] = {
    0, 1, 3,
    1, 2, 3
  };

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (GLvoid *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (GLvoid *)(3*sizeof(float)));
  glEnableVertexAttribArray(1);

  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  
  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  if(glGetError() != GL_NO_ERROR){
    printf("Failed to initialize GL.\n");
    goto main_cleanup;
  }

  struct fond_font font = {0};
  struct fond_buffer buffer = {0};
  if(!load_stuff(argv[1], &font, &buffer)){
    printf("Failed to load: %s\n", fond_error_string(fond_error()));
    goto main_cleanup;
  }

  printf("Done.\n");
  while(!glfwWindowShouldClose(window)){
    glfwPollEvents();

    render(program, vao, &buffer);
    
    glfwSwapBuffers(window);
  }

 main_cleanup:
  glfwTerminate();
  return 0;
}
