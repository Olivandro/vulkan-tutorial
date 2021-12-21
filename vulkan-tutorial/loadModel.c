//
//  loadModel.c
//  vulkan-tutorial
//
//  Created by Alejandro Ball on 21/12/2021.
//

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj_loader_c.h"

#include "loadModel.h"

static char* mmap_file(size_t* len, const char* filename) {
#ifdef _WIN64
  HANDLE file =
      CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                  FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

  if (file == INVALID_HANDLE_VALUE) { /* E.g. Model may not have materials. */
    return NULL;
  }

  HANDLE fileMapping = CreateFileMapping(file, NULL, PAGE_READONLY, 0, 0, NULL);
  assert(fileMapping != INVALID_HANDLE_VALUE);

  LPVOID fileMapView = MapViewOfFile(fileMapping, FILE_MAP_READ, 0, 0, 0);
  char* fileMapViewChar = (char*)fileMapView;
  assert(fileMapView != NULL);

  DWORD file_size = GetFileSize(file, NULL);
  (*len) = (size_t)file_size;

  return fileMapViewChar;
#else

  FILE* f;
  long file_size;
  struct stat sb;
  char* p;
  int fd;

  (*len) = 0;

  f = fopen(filename, "r");
  if (!f) {
    perror("open");
    return NULL;
  }
  fseek(f, 0, SEEK_END);
  file_size = ftell(f);
  fclose(f);

  fd = open(filename, O_RDONLY);
  if (fd == -1) {
    perror("open");
    return NULL;
  }

  if (fstat(fd, &sb) == -1) {
    perror("fstat");
    return NULL;
  }

  if (!S_ISREG(sb.st_mode)) {
    fprintf(stderr, "%s is not a file\n", "lineitem.tbl");
    return NULL;
  }

  p = (char*)mmap(0, (size_t)file_size, PROT_READ, MAP_SHARED, fd, 0);

  if (p == MAP_FAILED) {
    perror("mmap");
    return NULL;
  }

  if (close(fd) == -1) {
    perror("close");
    return NULL;
  }

  (*len) = (size_t)file_size;

  return p;

#endif
}

/* path will be modified */
static char* get_dirname(char* path) {
  char* last_delim = NULL;

  if (path == NULL) {
    return path;
  }

#if defined(_WIN32)
  /* TODO: Unix style path */
  last_delim = strrchr(path, '\\');
#else
  last_delim = strrchr(path, '/');
#endif

  if (last_delim == NULL) {
    /* no delimiter in the string. */
    return path;
  }

  /* remove '/' */
  last_delim[0] = '\0';

  return path;
}

static void get_file_data(void* ctx, const char* filename, const int is_mtl, const char* obj_filename, char** data, size_t* len)
{
  // NOTE: If you allocate the buffer with malloc(),
  // You can define your own memory management struct and pass it through `ctx`
  // to store the pointer and free memories at clean up stage(when you quit an
  // app)
  // This example uses mmap(), so no free() required.
  (void)ctx;

  if (!filename) {
    fprintf(stderr, "null filename\n");
    (*data) = NULL;
    (*len) = 0;
    return;
  }

  const char* ext = strrchr(filename, '.');

  size_t data_len = 0;

  if (strcmp(ext, ".gz") == 0) {
    return; /* todo */

  } else {
    char* basedirname = NULL;

    char tmp[1024];
    tmp[0] = '\0';

    /* For .mtl, extract base directory path from .obj filename and append .mtl
     * filename */
    if (is_mtl && obj_filename) {
      /* my_strdup is from tinyobjloader-c.h implementation(since strdup is not
       * a C standard library function */
      basedirname = my_strdup(obj_filename, strlen(obj_filename));
      basedirname = get_dirname(basedirname);
      printf("basedirname = %s\n", basedirname);
    }

    if (basedirname) {
      strncpy(tmp, basedirname, strlen(basedirname) + 1);

#if defined(_WIN32)
      strncat(tmp, "/", 1023 - strlen(tmp));
#else
      strncat(tmp, "/", 1023 - strlen(tmp));
#endif
      strncat(tmp, filename, 1023 - strlen(tmp));
    } else {
      strncpy(tmp, filename, strlen(filename) + 1);
    }

    printf("tmp = %s\n", tmp);

    if (basedirname) {
      free(basedirname);
    }

    *data = mmap_file(&data_len, tmp);
  }

  (*len) = data_len;
}

/**
 TO DO:
    1. Still have to create a rtn struct DrawingData that is of only unique vertices
 */
void loadModel(struct DrawingData** vertices, uint64_t* num_vertices, uint32_t** indices, uint64_t* num_indices, const char* MODEL_PATH)
{
    tinyobj_attrib_t attrib;
    tinyobj_shape_t* shapes = NULL;
    size_t num_shapes;
    tinyobj_material_t* materials = NULL;
    size_t num_materials;
    
    int ret = tinyobj_parse_obj(&attrib, &shapes, &num_shapes, &materials, &num_materials, MODEL_PATH, get_file_data, NULL, TINYOBJ_FLAG_TRIANGULATE);
    
    if (ret != TINYOBJ_SUCCESS) {
        printf("model failed to load\n");
    }
    printf("# of shapes    = %d\n", (int)num_shapes);
    printf("# of materials = %d\n", (int)num_materials);
    
    uint32_t t_indices[attrib.num_faces];
    struct DrawingData t_vertices[attrib.num_faces];
    for (size_t i = 0; i < attrib.num_faces; i++) {

//        t_indices[i] = (uint32_t) attrib.faces[i].v_idx;
        t_indices[i] = (uint32_t) i;
    
        t_vertices[i].pos[0] = attrib.vertices[3 * attrib.faces[i].v_idx + 0];
        t_vertices[i].pos[1] = attrib.vertices[3 * attrib.faces[i].v_idx + 1];
        t_vertices[i].pos[2] = attrib.vertices[3 * attrib.faces[i].v_idx + 2];

        t_vertices[i].color[0] = 1.0f;
        t_vertices[i].color[1] = 1.0f;
        t_vertices[i].color[2] = 1.0f;

        if (attrib.faces[i].vt_idx >= 0) {
            t_vertices[i].texCoord[0] = attrib.texcoords[2 * attrib.faces[i].vt_idx + 0];
            t_vertices[i].texCoord[1] = 1.0f - attrib.texcoords[2 * attrib.faces[i].vt_idx + 1];
        }
    }

    *vertices = (struct DrawingData*) calloc(attrib.num_faces, sizeof(struct DrawingData)); // malloc(sizeof(struct DrawingData) * attrib.num_faces);
    if (!vertices) printf("memory not allocated!\n");
    memcpy(*vertices, &t_vertices, sizeof(t_vertices));
    *num_vertices = (uint64_t) attrib.num_faces;
    
    *indices = (uint32_t*) calloc(attrib.num_faces, sizeof(uint32_t)); // malloc(sizeof(uint32_t) * attrib.num_faces);
    if (!indices) printf("memory not allocated!\n");
    memcpy(*indices, &t_indices, sizeof(t_indices));
    *num_indices = (uint64_t) attrib.num_faces;
    
    tinyobj_attrib_free(&attrib);
    tinyobj_shapes_free(shapes, num_shapes);
    tinyobj_materials_free(materials, num_materials);
}
