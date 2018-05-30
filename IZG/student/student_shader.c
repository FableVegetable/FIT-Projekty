/*!
 * @file
 * @brief This file contains implemenation of phong vertex and fragment shader.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <assert.h>
#include <math.h>
#include <stdio.h>

#include <student/gpu.h>
#include <student/student_shader.h>
#include <student/uniforms.h>

/// \addtogroup shader_side Úkoly v shaderech
/// @{

void phong_vertexShader(GPUVertexShaderOutput *const      output,
                        GPUVertexShaderInput const *const input,
                        GPU const                         gpu) {
  assert(output);
  assert(input);
  assert(gpu);

  Uniforms const uniformsHandle = gpu_getUniformsHandle(gpu);                     //must be here for getting uniforms (?handle)
  Vec3 const *const pos = vs_interpretInputVertexAttributeAsVec3(gpu, input, 0);  //1st vertex attribute contains position in world-space
  Vec3 const *const norm = vs_interpretInputVertexAttributeAsVec3(gpu, input, 1); //2nd vertex attribute contains normal in world-space
  Mat4 mvp;  Vec4 pos4;   //matrix 4x4 mvp, vector 4x4 pos4
  
  multiply_Mat4_Mat4( &mvp,                                                                     //result of multiplication 2 matrices
    shader_interpretUniformAsMat4(uniformsHandle, getUniformLocation(gpu,"projectionMatrix")),  //first 4x4 projection matrix
    shader_interpretUniformAsMat4(uniformsHandle, getUniformLocation(gpu,"viewMatrix")));       //second 4x4 view matrix
  copy_Vec3Float_To_Vec4(&pos4, pos, 1.f);
  multiply_Mat4_Vec4(&output->gl_Position, &mvp, &pos4);

  init_Vec3(vs_interpretOutputVertexAttributeAsVec3(gpu, output, 0), pos->data[0], pos->data[1], pos->data[2]);     //set output vertex 1st attribute as vector containing position data 
  init_Vec3(vs_interpretOutputVertexAttributeAsVec3(gpu, output, 1), norm->data[0], norm->data[1], norm->data[2]);  //set output vertex 2nd attribute as vector containing normal data

  /// \todo Naimplementujte vertex shader, který transformuje vstupní vrcholy do
  /// clip-space.<br>
  /// <b>Vstupy:</b><br>
  /// Vstupní vrchol by měl v nultém atributu obsahovat pozici vrcholu ve
  /// world-space (vec3) a v prvním
  /// atributu obsahovat normálu vrcholu ve world-space (vec3).<br>
  /// <b>Výstupy:</b><br>
  /// Výstupní vrchol by měl v nultém atributu obsahovat pozici vrcholu (vec3)
  /// ve world-space a v prvním
  /// atributu obsahovat normálu vrcholu ve world-space (vec3).
  /// Výstupní vrchol obsahuje pozici a normálu vrcholu proto, že chceme počítat
  /// osvětlení ve world-space ve fragment shaderu.<br>
  /// <b>Uniformy:</b><br>
  /// Vertex shader by měl pro transformaci využít uniformní proměnné obsahující
  /// view a projekční matici.
  /// View matici čtěte z uniformní proměnné "viewMatrix" a projekční matici
  /// čtěte z uniformní proměnné "projectionMatrix".
  /// Zachovejte jména uniformních proměnných a pozice vstupních a výstupních
  /// atributů.
  /// Pokud tak neučiníte, akceptační testy selžou.<br>
  /// <br>
  /// Využijte vektorové a maticové funkce.
  /// Nepředávajte si data do shaderu pomocí globálních proměnných.
  /// Pro získání dat atributů použijte příslušné funkce vs_interpret*
  /// definované v souboru program.h.
  /// Pro získání dat uniformních proměnných použijte příslušné funkce
  /// shader_interpretUniform* definované v souboru program.h.
  /// Vrchol v clip-space by měl být zapsán do proměnné gl_Position ve výstupní
  /// struktuře.<br>
  /// <b>Seznam funkcí, které jistě použijete</b>:
  ///  - gpu_getUniformsHandle()
  ///  - getUniformLocation()
  ///  - shader_interpretUniformAsMat4()
  ///  - vs_interpretInputVertexAttributeAsVec3()
  ///  - vs_interpretOutputVertexAttributeAsVec3()
}


float clamp(float value){
  if(value < 0.f)
    return 0.f;
  if(value > 1.f)
    return 1.f;
  return value;
}

void phong_fragmentShader(GPUFragmentShaderOutput *const      output,
                          GPUFragmentShaderInput const *const input,
                          GPU const                           gpu) {
  /// \todo Naimplementujte fragment shader, který počítá phongův osvětlovací
  /// model s phongovým stínováním.<br>
  /// <b>Vstup:</b><br>
  /// Vstupní fragment by měl v nultém fragment atributu obsahovat
  /// interpolovanou pozici ve world-space a v prvním
  /// fragment atributu obsahovat interpolovanou normálu ve world-space.<br>
  /// <b>Výstup:</b><br>
  /// Barvu zapište do proměnné color ve výstupní struktuře.<br>
  /// <b>Uniformy:</b><br>
  /// Pozici kamery přečtěte z uniformní proměnné "cameraPosition" a pozici
  /// světla přečtěte z uniformní proměnné "lightPosition".
  /// Zachovejte jména uniformních proměnný.
  /// Pokud tak neučiníte, akceptační testy selžou.<br>
  /// <br>
  /// Dejte si pozor na velikost normálového vektoru, při lineární interpolaci v
  /// rasterizaci může dojít ke zkrácení.
  /// Zapište barvu do proměnné color ve výstupní struktuře.
  /// Shininess faktor nastavte na 40.f

  /// Difuzní barvu materiálu nastavte podle normály povrchu.
  /// V případě, že normála směřuje kolmo vzhůru je difuzní barva čistě bílá.
  /// V případě, že normála směřuje vodorovně nebo dolů je difuzní barva čiště zelená.
  /// Difuzní barvu spočtěte lineární interpolací zelené a bíle barvy pomocí interpolačního parameteru t.
  /// Interpolační parameter t spočtěte z y komponenty normály pomocí t = y*y (samozřejmě s ohledem na negativní čísla).

  /// Spekulární barvu materiálu nastavte na čistou bílou.
  /// Barvu světla nastavte na bílou.
  /// Nepoužívejte ambientní světlo.<br>
  /// <b>Seznam funkcí, které jistě využijete</b>:
  ///  - shader_interpretUniformAsVec3()
  ///  - fs_interpretInputAttributeAsVec3()


  //Fragment shader called for every pixel???
  Uniforms const uniformsHandle = gpu_getUniformsHandle(gpu);           //must be here for getting uniforms (?handle)
  Vec3 const *position = fs_interpretInputAttributeAsVec3(gpu,input,0); //1st fragment attribute contains position in world-space
  Vec3 const *normal   = fs_interpretInputAttributeAsVec3(gpu,input,1); //2nd fragment attribute contains normal in world-space
  Vec3 dLight = {.data[0] = 0.f, .data[1] = 1.f, .data[2] = 0.f};       //diffuse light (green light - will be interpolated)
  Vec3 sLight = {.data[0] = 1.f, .data[1] = 1.f, .data[2] = 1.f};       //specular light (white light)
  Vec3 L,V,R,norm,mixed,phong;  //in order: light position, camera position, reflection to count, normalized normal, mixed color, phong light

  sub_Vec3(&L, shader_interpretUniformAsVec3(uniformsHandle, getUniformLocation(gpu,"lightPosition" ) ), position); //get vector L by subtracting cameraposition(vector) from position(vector) 
  sub_Vec3(&V, shader_interpretUniformAsVec3(uniformsHandle, getUniformLocation(gpu,"cameraPosition") ), position);  //get vector V by subtracting lightposition(vector) from position(vector)
  normalize_Vec3(&L, &L);       //normalize vector L lightPosition
  normalize_Vec3(&V, &V);       //normalize vector V cameraPosition
  normalize_Vec3(&norm,normal); //normalize vector normal

  mix_Vec3(&mixed, &dLight, &sLight, (float)pow(norm.data[1], 2));    //mix white and green light (linear interpolation of colors) depending on normal's direction
  copy_Vec3(&dLight, norm.data[1] < 0 ? &dLight : &mixed);            //set dLight depending on normal's direction
  multiply_Vec3_Float(&dLight, &dLight, clamp(dot_Vec3(&norm, &L)));  //multiply dLight by clamped result of dot product of normalized normal and normalized L
  multiply_Vec3_Float(&L, &L, -1.0f);                                 //multiply vector L by -1
  reflect(&R, &L, &norm);                                             //count reflection to camera view depending on light direction and normal direction
  multiply_Vec3_Float(&sLight, &sLight, (float)pow(clamp(dot_Vec3(&V, &R)), 40.f)); //multiply specular light(white) by value dot product of camera view and reflextion, then power it on shininess factor 40.f
  add_Vec3(&phong, &sLight, &dLight);                                 //calculate result phong light 
  copy_Vec3Float_To_Vec4(&output->color, &phong, 1.f);                //set result color of pixel by phong light vector, (1.f as last argument because of color structure we work with)
}

/// @}
