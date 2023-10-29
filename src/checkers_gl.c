#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_SHADER_FILE_SIZE 300
#define MAX_INPUT_FILENAME_LENGHT 15

struct vec3 {
  float x;
  float y;
  float z;
};

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void _print_shader_info_log(GLuint shader_index);

const char *string_from_file(const char *filename);
void show_usage_end_exit(char *format, ...);
void process_input(int argc, char **argv);

void put_in_color_values(float **buf, bool is_white_square);
void put_in_coordinates_values(float **buf, struct vec3* p);
bool is_normalizedf(float f);
float normalizef(float *f);

int run(uint32_t *wp, uint32_t *bp, uint32_t *k);

const unsigned int WINDOW_WIDTH = 700;
const unsigned int WINDOW_HEIGHT = 700;

#define NUMBER_OF_SQUARES 64
const float white_square_color[3] = {1.0f, 1.0f, 1.0f};
const float black_square_color[3] = {0.0f, 0.0f, 0.0f};

const char *vertexShaderSource;
const char *fragmentShaderSource;

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

const char *string_from_file(const char *filename) {
  FILE *f = fopen(filename, "r");
  if (f == NULL) {
    fprintf(stderr, "Error while trying to open %s file\n", filename);
    exit(1);
  }
  char *buf = (char *)malloc(MAX_SHADER_FILE_SIZE);
  char c;
  size_t i = 0;
  while ((c = fgetc(f)) != EOF) {
    if (i >= MAX_SHADER_FILE_SIZE) {
      fprintf(stderr, "Buffer overflow while trying to load shader file\n");
      exit(1);
    }
    buf[i] = c;
    i++;
  }
  buf[i] = '\0';
  fclose(f);
  return buf;
}

void show_usage_end_exit(char *format, ...) {
  printf("Usage: grach inputfile (.grach file).");
  va_list va;
  va_start(va, format);
  vprintf(format, va);
  printf("\n");
  exit(0);
}

void _print_shader_info_log(GLuint shader_index) {
  int max_length = 2048;
  int actual_length = 0;
  char shader_log[2048];
  glGetShaderInfoLog(shader_index, max_length, &actual_length, shader_log);
  printf("shader info log for GL index %u:\n%s\n", shader_index, shader_log);
}

void render_image() {}

void glfw_error_callback(int code, const char *descriptor);
void glfw_error_callback(int code, const char *descriptor) {
  fprintf(stderr, "ERROR IN GLFW. %s\n", descriptor);
}

void put_in_color_values(float **buf, bool is_white_color) {
    if (is_white_color) {
      *((*buf)++) = white_square_color[0];
      *((*buf)++) = white_square_color[1];
      *((*buf)++) = white_square_color[2];
    }
    else {
      *((*buf)++) = black_square_color[0];
      *((*buf)++) = black_square_color[1];
      *((*buf)++) = black_square_color[2];
    }
}

void put_in_coordinates_values(float **buf, struct vec3* p) {
      *((*buf)++) = p->x;
      *((*buf)++) = p->y;
      *((*buf)++) = p->z;
}

bool is_normalizedf(float f) {
  return f > -1.0f && f < 1.0f;
}

float normalizef(float *f) {
  if (*f < -1.0f) {
    return 1.0f;
  } else if (*f > 1.0f) {
    return -1.0f;
  } else {
    return *f;
  }
}

int run(uint32_t *wp, uint32_t *bp, uint32_t *k) {
  assert(glfwInit());
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  glfwSetErrorCallback(glfw_error_callback);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  GLFWwindow *window =
      glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Checkers", NULL, NULL);
  if (window == NULL) {
    glfwTerminate();
    fprintf(stderr, "Error while creating window\n");
    return -1;
  }
  glfwSetWindowAspectRatio(window, 1, 1);
  glfwSetWindowSizeLimits(window, WINDOW_WIDTH, WINDOW_HEIGHT, GL_DONT_CARE, GL_DONT_CARE);
  printf("Pizda\n");
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    fprintf(stderr, "Error while loading GL functions\n");
    return -1;
  }

  vertexShaderSource = string_from_file("src/shaders/main_vert.glsl");
  fragmentShaderSource = string_from_file("src/shaders/main_frag.glsl");

  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    fprintf(stderr, "Error while compiling vertex shader\n");
    _print_shader_info_log(vertexShader);
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
  }

  unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    fprintf(stderr, "Error while compiling fragment shader\n");
    _print_shader_info_log(fragmentShader);
  }

  unsigned int shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  // check for linking errors
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    printf("Linking errors\n");
  }
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  #define BYTES_PER_SQUEARE (6 * 4 * sizeof(float))
  #define SIZEOF_SQUARES_BUFFER (NUMBER_OF_SQUARES * BYTES_PER_SQUEARE)

  float *vertices = malloc(SIZEOF_SQUARES_BUFFER);
  
  if (vertices == NULL) {
    fprintf(stderr, "ERROR while trying to allocate board image data\n");
    exit(1);
  }

  struct vec3 l_top = {.x = -1.0f, .y = 1.0f, .z = 0.0f};
  struct vec3 l_bottom = {.x = -1.0f, .y = 0.750f, .z = 0.0f};
  struct vec3 r_top = {.x = -0.750f, .y = 1.0f, .z = 0.0f};
  struct vec3 r_bottom = {.x = -0.750f, .y = 0.750f, .z = 0.0f};

  float *c_vertices = vertices;
  
  bool white = true;
  size_t i = 0;

  while (i < NUMBER_OF_SQUARES) {
    if (i % 8 == 0 && i > 0) {
      l_top.y -= 0.250f;
      l_bottom.y -= 0.250f;
      r_top.y -= 0.250f;
      r_bottom.y -= 0.250f;

      l_top.x = -1.0f;
      r_top.x = -0.750f;
      l_bottom.x = -1.0f;
      r_bottom.x = -0.750f;
      white = !white;
    }

    put_in_coordinates_values(&c_vertices, &l_top);
    put_in_color_values(&c_vertices, white);
    put_in_coordinates_values(&c_vertices, &l_bottom);
    put_in_color_values(&c_vertices, white);
    put_in_coordinates_values(&c_vertices, &r_top);
    put_in_color_values(&c_vertices, white);
    put_in_coordinates_values(&c_vertices, &r_bottom);
    put_in_color_values(&c_vertices, white);

    l_top.x += 0.250f;
    l_bottom.x += 0.250f;
    r_top.x += 0.250f;
    r_bottom.x += 0.250f;

    white = !white;
    i++;
  }
  
  for (size_t i = 0; i < SIZEOF_SQUARES_BUFFER / 4; i++) {
    printf("vert[%ld] = %f\n", i, vertices[i]);
  }

  printf("SIZEOF_SQUARES_BUFFER = %ld, BYTES_PER_SQUEARE = %ld\n", SIZEOF_SQUARES_BUFFER, BYTES_PER_SQUEARE);

  #define SIZEOF_INDICES_BUFFER (NUMBER_OF_SQUARES * 24 * sizeof(unsigned short int))
  unsigned short int *indices = malloc(SIZEOF_INDICES_BUFFER);
  memset(indices, 0, NUMBER_OF_SQUARES * 24 * sizeof(unsigned short int));

  struct vec3 index = {.x = 0, .y = 1, .z = 2};
  i = 0;
  uint8_t line_n = 4;
  while (i < NUMBER_OF_SQUARES * 24 - 12) {
    if (i % (48 * line_n - 12) == 0 && i != 0) {
      index.x++;
      index.y++;
      index.z++;
      i += 3;
      line_n += 4;
    }
    indices[i++] = index.x++;
    indices[i++] = index.y++;
    indices[i++] = index.z++;
    if (i % 6 == 0 && i > 0) {
      i += 6;
    }
  }

  for (size_t i = 0; i < NUMBER_OF_SQUARES * 6; i++) {
    printf("indices[%ld] = %d\n", i, indices[i]);
  }
  
  printf(" NUMBER_OF_SQUARES * 6 * sizeof(unsigned short int)= %ld \n", NUMBER_OF_SQUARES * 6 * sizeof(unsigned short int));
  
  unsigned int VBO, VAO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, SIZEOF_SQUARES_BUFFER, vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, SIZEOF_INDICES_BUFFER, indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0.3f, 0.3f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(
        VAO); 
    //glDrawArrays(GL_TRIANGLES, 0, NUMBER_OF_SQUARES * 6);
    glDrawElements(GL_TRIANGLES, NUMBER_OF_SQUARES * 24, GL_UNSIGNED_SHORT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteProgram(shaderProgram);

  free(vertices);
  free(indices);

  glfwTerminate();
  return 0;
}
