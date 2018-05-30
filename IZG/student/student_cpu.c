/*!
 * @file
 * @brief This file contains implementation of cpu side for phong shading.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 *
 */

#include <assert.h>
#include <math.h>

#include <student/buffer.h>
#include <student/bunny.h>
#include <student/camera.h>
#include <student/gpu.h>
#include <student/linearAlgebra.h>
#include <student/mouseCamera.h>
#include <student/student_cpu.h>
#include <student/student_pipeline.h>
#include <student/student_shader.h>
#include <student/swapBuffers.h>
#include <student/uniforms.h>
#include <student/vertexPuller.h>

#include <student/globals.h>
#define FIRST 0
#define SECOND 1
#define VERTICES 0
#define INDICES 1


/**
 * @brief This structure contains all global variables for this method.
 */
struct PhongVariables {
 ///This variable contains GPU handle.
  GPU gpu;
  ///This variable contains light poistion in world-space.
  Vec3 lightPosition;
  //This variable contains puller id
  VertexPullerID puller;
  //This variable contains program id
  ProgramID program;
  //This variables represents vertices and indices buffers
  BufferID buffs[2];
} phong;  ///<instance of all global variables for triangle example.

/// \addtogroup cpu_side Úkoly v cpu části
/// @{

void phong_onInit(int32_t width, int32_t height) {
  
  phong.gpu = cpu_createGPU(); //create gpu
  cpu_setViewportSize(phong.gpu,(size_t)width,(size_t)height); //set viewport size
  cpu_initMatrices(width,height); //init matrices
  init_Vec3(&phong.lightPosition,1000.f,1000.f,1000.f); //init lightPosition 
 
  //set local variables and count sizeofs
  size_t fsize = sizeof(float);
  size_t usize = sizeof(unsigned);
  size_t bunnyversize = sizeof(bunnyVertices);
  size_t bunnyindsize = sizeof(bunnyIndices);

  //reserve uniform variables
  cpu_reserveUniform(phong.gpu, "projectionMatrix", UNIFORM_MAT4);
  cpu_reserveUniform(phong.gpu, "viewMatrix"      , UNIFORM_MAT4);
  cpu_reserveUniform(phong.gpu, "cameraPosition"  , UNIFORM_VEC3);
  cpu_reserveUniform(phong.gpu, "lightPosition"   , UNIFORM_VEC3);
  
  //create program object    
  phong.program = cpu_createProgram(phong.gpu);
  cpu_attachVertexShader(phong.gpu, phong.program, phong_vertexShader);
  cpu_attachFragmentShader(phong.gpu, phong.program, phong_fragmentShader);
  
  //set interpolation attributues
  cpu_setAttributeInterpolation(phong.gpu, phong.program, FIRST, ATTRIB_VEC3,SMOOTH);
  cpu_setAttributeInterpolation(phong.gpu, phong.program, SECOND, ATTRIB_VEC3,SMOOTH);
  
  //create and fill buffers with data
  cpu_createBuffers(phong.gpu, 2, phong.buffs);
  cpu_bufferData(phong.gpu, phong.buffs[VERTICES], bunnyversize, bunnyVertices);
  cpu_bufferData(phong.gpu, phong.buffs[INDICES], bunnyindsize, bunnyIndices);
        
  //create vertex puller and set position and normal attributes,
  cpu_createVertexPullers(phong.gpu, 1, &(phong.puller)); 
  cpu_setVertexPullerHead(phong.gpu, phong.puller, FIRST, phong.buffs[VERTICES], fsize*0,fsize*6);  //set position attribute
  cpu_setVertexPullerHead(phong.gpu, phong.puller, SECOND, phong.buffs[VERTICES], fsize*3,fsize*6); //set normal attribute
                                   
  //enable vertexPuller heads
  cpu_enableVertexPullerHead(phong.gpu, phong.puller, FIRST);  
  cpu_enableVertexPullerHead(phong.gpu, phong.puller, SECOND);
     
  //set indexing
  cpu_setIndexing(phong.gpu,phong.puller,phong.buffs[INDICES],usize);
}

/// @}

void phong_onExit() { cpu_destroyGPU(phong.gpu); }

/// \addtogroup cpu_side
/// @{

void phong_onDraw(SDL_Surface* surface) {
  assert(surface);
  cpu_clearDepth(phong.gpu, +INFINITY);           //clear depth buffer
  Vec4 color; init_Vec4(&color, .1f, .1f, .1f, 1.f);
  cpu_clearColor(phong.gpu, &color);              //clear color buffer
  cpu_useProgram(phong.gpu, phong.program);       //shader program activated
  cpu_bindVertexPuller(phong.gpu, phong.puller);  //vertex puller activated

  //data loaded into uniform variables
  cpu_uniformMatrix4fv(phong.gpu,  getUniformLocation(phong.gpu,"viewMatrix"), (float *)&viewMatrix);
  cpu_uniformMatrix4fv(phong.gpu,  getUniformLocation(phong.gpu,"projectionMatrix"), (float *)&projectionMatrix);
  cpu_uniform3f(phong.gpu,  getUniformLocation(phong.gpu,"cameraPosition"), cameraPosition.data[0], cameraPosition.data[1], cameraPosition.data[2]);
  cpu_uniform3f(phong.gpu,  getUniformLocation(phong.gpu,"lightPosition"), phong.lightPosition.data[0], phong.lightPosition.data[1],phong.lightPosition.data[2]);

  size_t const nofVertices = sizeof(bunnyIndices)/sizeof(VertexIndex);  //get number of vertices to draw
  cpu_drawTriangles(phong.gpu, nofVertices);                            //draw bunny as triangles 
  cpu_swapBuffers(surface, phong.gpu);                                  //copy image from gpu to SDL surface
}

/// @}
