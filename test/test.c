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

struct data{
  struct fond_buffer *buffer;
  int32_t text[500];
  size_t pos;
};

int load_stuff(char *file, struct data *data){
  struct fond_buffer *buffer = data->buffer;
  struct fond_font *font = buffer->font;
  
  font->file = file;
  font->size = 100.0;
  font->oversample = 2;
  font->characters =
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    " 0123456789.-;:?!/()*+^_\\";
  
  printf("Loading font... ");
  GLint max_size = 0;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_size);
  if(!fond_load_fit(font, max_size))
    return 0;
  printf("DONE (Atlas %ix%i)\n", font->width, font->height);

  buffer->font = font;
  buffer->width = 800;
  buffer->height = 600;
  printf("Loading buffer... ");
  if(!fond_load_buffer(buffer))
    return 0;
  printf("DONE\n");

  printf("Rendering buffer... ");
  if(!fond_render(buffer, "type", 0, 100, 0))
    return 0;
  printf("DONE\n");
  
  return 1;
}

int render_text(struct data *data){
  struct fond_extent extent = {0};
  fond_compute_extent_u(data->buffer->font, 0, 0, &extent);
  if(!fond_render_u(data->buffer, data->text, data->pos, 0, extent.t, 0)){
    printf("Failed to render: %s\n", fond_error_string(fond_error()));
    return 0;
  }
  return 1;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode){
  struct data *data = glfwGetWindowUserPointer(window);
  if(action == GLFW_RELEASE){
    switch(key){
    case GLFW_KEY_BACKSPACE:
      if(0 < data->pos){
        data->pos--;
        render_text(data);
      }
      break;
    case GLFW_KEY_ESCAPE:
      glfwSetWindowShouldClose(window, GL_TRUE);
      break;
    }
  }
}

void character_callback(GLFWwindow* window, unsigned int codepoint){
  struct data *data = glfwGetWindowUserPointer(window);
  if(data->pos < 500){
    data->text[data->pos] = (int32_t)codepoint;
    data->pos++;
    if(!render_text(data))
      data->pos--;
  }
}

void render(GLuint program, GLuint vao, struct fond_buffer *buffer){
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glBindTexture(GL_TEXTURE_2D, buffer->texture);
  glUseProgram(program);
  glBindVertexArray(vao);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

int main(int argc, char **argv){
  if(argc == 1){
    printf("Please specify a TTF file to load.\n");
    return 0;
  }

  printf("Initializing GL... ");
  
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  GLFWwindow* window = glfwCreateWindow(800, 600, "Fond Test", 0, 0);
  if(window == 0){
    printf("Failed to create GLFW window\n");
    goto main_cleanup;
  }
  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, key_callback);
  glfwSetCharCallback(window, character_callback);

  glewExperimental = GL_TRUE;
  if(glewInit() != GLEW_OK){
    printf("Failed to initialize GLEW\n");
    goto main_cleanup;
  }

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
     1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
     1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,  0.0f, 1.0f
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
    printf("FAILED\n");
    goto main_cleanup;
  }
  printf("DONE\n");

  // Construct data and tie it.
  struct data data = {0};
  struct fond_font font = {0};
  struct fond_buffer buffer = {0};
  data.buffer = &buffer;
  buffer.font = &font;
  
  glfwSetWindowUserPointer(window, &data);
  
  if(!load_stuff(argv[1], &data)){
    printf("Error: %s\n", fond_error_string(fond_error()));
    goto main_cleanup;
  }

  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);
  
  while(!glfwWindowShouldClose(window)){
    glfwPollEvents();

    render(program, vao, &buffer);
    
    glfwSwapBuffers(window);
  }
  
  fond_free(&font);
  fond_free_buffer(&buffer);

 main_cleanup:
  glfwTerminate();
  return 0;
}
