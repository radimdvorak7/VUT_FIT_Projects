/*!
 * @file
 * @brief This file contains implementation of gpu
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <student/gpu.hpp>
#include <cstring>

typedef struct{
  OutVertex point[3];
} Triangle;

void clear(GPUMemory&mem,ClearCommand cmd){
  auto current_buffer = mem.activatedFramebuffer;
  auto framebuffer = mem.framebuffers[current_buffer];
  if(cmd.clearColor == true && framebuffer.color.data != nullptr){
    if(framebuffer.color.format == Image::UINT8){
      auto pitch = framebuffer.color.pitch;
      auto bytesPerPixel = framebuffer.color.bytesPerPixel;
      auto pixel_uint = (uint8_t*)framebuffer.color.data;
      for(uint32_t x = 0; x < framebuffer.width; x++){
        for(uint32_t y = 0; y < framebuffer.height; y++){
          for(uint32_t i = 0; i < mem.framebuffers[current_buffer].color.channels;i++){
            pixel_uint[y*pitch + x*bytesPerPixel + i] = (uint8_t)(cmd.color[i] * 255.0f);
          }
        } 
      }
    }
  }
  if(cmd.clearDepth == true && framebuffer.depth.data != nullptr){
    if(framebuffer.depth.format == Image::FLOAT32){
      auto pitch = framebuffer.depth.pitch;
      auto bytesPerPixel = framebuffer.depth.bytesPerPixel;
      auto pixel_float = (float*)framebuffer.depth.data;
      for(uint32_t y = 0; y< framebuffer.height; y++){
        for(uint32_t x = 0; x < framebuffer.width; x++){
          pixel_float[y + x*(pitch/bytesPerPixel)] = cmd.depth;
        } 
      }
    }
  }
}

uint32_t computeVertexID(GPUMemory&mem,VertexArray const&vertex_arr,uint32_t shaderInvocation){
  if(vertex_arr.indexBufferID < 0)
    return shaderInvocation;
    
  auto indexBuffer = mem.buffers[vertex_arr.indexBufferID].data;
  auto index = (uint8_t*)indexBuffer + vertex_arr.indexOffset;

  switch(vertex_arr.indexType){
    case IndexType::UINT8:{
      return index[shaderInvocation];
    }
    case IndexType::UINT16:{
      uint16_t *ind = (uint16_t *)index;
      return ind[shaderInvocation];
    }
    case IndexType::UINT32:{
      uint32_t *ind = (uint32_t *)index;
      return ind[shaderInvocation];
    }
    default:{
      return shaderInvocation;
    }
  }
}

void getVertexAtributes(GPUMemory&mem,VertexArray const&vertex_arr,InVertex &inVertex){
  auto attrib = mem.vertexArrays[mem.activatedVertexArray].vertexAttrib;
  auto VertexID = inVertex.gl_VertexID;
  for(uint32_t attribut_i = 0; attribut_i < maxAttributes; attribut_i++){
    auto buffer = mem.buffers[attrib[attribut_i].bufferID];
    auto vertexBuffer=(uint8_t*)buffer.data;

    vertexBuffer+=attrib[attribut_i].offset+attrib[attribut_i].stride*VertexID ;
    memcpy(&inVertex.attributes[attribut_i],vertexBuffer,sizeof(float)*(uint32_t)attrib[attribut_i].type); 
  }
}

void vertexAsembly(GPUMemory&mem,InVertex &inVertex,uint32_t shaderInvocation){
    auto activeVertexArray = mem.vertexArrays[mem.activatedVertexArray];

    inVertex.gl_VertexID = computeVertexID(mem,activeVertexArray,shaderInvocation);
    getVertexAtributes(mem,activeVertexArray,inVertex);
}

void loadTriangle(GPUMemory&mem,Triangle &triangle, VertexArray &vertexArray,uint32_t tID){
  auto prg = mem.programs[mem.activatedProgram];
  for(uint32_t i = 0; i < 3; i++){
    InVertex inVertex;
    vertexAsembly(mem,inVertex,tID*3+i);
    ShaderInterface si;
    si.gl_DrawID = mem.gl_DrawID;
    prg.vertexShader(triangle.point[i],inVertex,si);
  }
}

void perspectiveDivision(Triangle tri){
  for(uint32_t i = 0; i < 3; i++){
    auto w_coord = tri.point[i].gl_Position[3];
    tri.point[i].gl_Position[0] /= w_coord;
    tri.point[i].gl_Position[1] /= w_coord;
    tri.point[i].gl_Position[2] /= w_coord;
    tri.point[i].gl_Position[3] = 1;
  }
}

void rasterize(GPUMemory&mem, Framebuffer &frame, Triangle const&triangle,Program const&prg){
  for(uint32_t x = 0;x < frame.width;x++){
    for(uint32_t y = 0;y < frame.height;y++){
      InFragment inFragment;
      inFragment.gl_FragCoord.x = x;
      inFragment.gl_FragCoord.y = y;
      OutFragment outFragment;
      ShaderInterface si;
      si.gl_DrawID = mem.gl_DrawID;
      prg.fragmentShader(outFragment,inFragment,si);
    } 
  }
}
//
void izg_enqueue_recursive(GPUMemory&mem,CommandBuffer const&cb){
  for(uint32_t i  = 0; i < cb.nofCommands;i++){
    auto type = cb.commands[i].type;
    auto data = cb.commands[i].data;
    switch (type)
    {
    case CommandType::CLEAR:{
      clear(mem,data.clearCommand);
      break;
    }
    case CommandType::BIND_FRAMEBUFFER:{
      mem.activatedFramebuffer=data.bindFramebufferCommand.id;
      break;
    }
    case CommandType::BIND_PROGRAM:{
      mem.activatedProgram=data.bindProgramCommand.id;
      break;
    }
    case CommandType::BIND_VERTEXARRAY:{
      mem.activatedVertexArray=data.bindVertexArrayCommand.id;
      break;
    }
    case CommandType::DRAW:{
      for(int t = 0; t < data.drawCommand.nofVertices/3u;++t){
        Triangle triangle;
        loadTriangle(mem,triangle,mem.vertexArrays[mem.activatedVertexArray],t);

        //TODO: Finish rasterization
        //perspectiveDivision(triangle);  
        //rasterize(mem,mem.framebuffers[mem.activatedFramebuffer],triangle,mem.programs[mem.activatedProgram]);
      }
      mem.gl_DrawID++;
      break;
    }
    case CommandType::SET_DRAW_ID:{
      mem.gl_DrawID = data.setDrawIdCommand.id;
      break;
    }
    case CommandType::SUB_COMMAND:{
      izg_enqueue_recursive(mem,*data.subCommand.commandBuffer);
      break;
    }
    default:
      break;
    }
  }
}

//! [izg_enqueue]
//Starts command buffer, sets drawID to 0 and ensures that subcommand buffer remember drawID of main command buffer
void izg_enqueue(GPUMemory&mem,CommandBuffer const&cb){
  mem.gl_DrawID = 0;
  izg_enqueue_recursive(mem,cb);
}
//! [izg_enqueue]

/**
 * @brief This function reads color from texture.
 *
 * @param texture texture
 * @param uv uv coordinates
 *
 * @return color 4 floats
 */
glm::vec4 read_texture(Texture const&texture,glm::vec2 uv){
  if(!texture.img.data)return glm::vec4(0.f);
  auto&img = texture.img;
  auto uv1 = glm::fract(glm::fract(uv)+1.f);
  auto uv2 = uv1*glm::vec2(texture.width-1,texture.height-1)+0.5f;
  auto pix = glm::uvec2(uv2);
  return texelFetch(texture,pix);
}

/**
 * @brief This function reads color from texture with clamping on the borders.
 *
 * @param texture texture
 * @param uv uv coordinates
 *
 * @return color 4 floats
 */
glm::vec4 read_textureClamp(Texture const&texture,glm::vec2 uv){
  if(!texture.img.data)return glm::vec4(0.f);
  auto&img = texture.img;
  auto uv1 = glm::clamp(uv,0.f,1.f);
  auto uv2 = uv1*glm::vec2(texture.width-1,texture.height-1)+0.5f;
  auto pix = glm::uvec2(uv2);
  return texelFetch(texture,pix);
}

/**
 * @brief This function fetches color from texture.
 *
 * @param texture texture
 * @param pix integer coorinates
 *
 * @return color 4 floats
 */
glm::vec4 texelFetch(Texture const&texture,glm::uvec2 pix){
  auto&img = texture.img;
  glm::vec4 color = glm::vec4(0.f,0.f,0.f,1.f);
  if(pix.x>=texture.width || pix.y >=texture.height)return color;
  if(img.format == Image::UINT8){
    auto colorPtr = (uint8_t*)getPixel(img,pix.x,pix.y);
    for(uint32_t c=0;c<img.channels;++c)
      color[c] = colorPtr[img.channelTypes[c]]/255.f;
  }
  if(texture.img.format == Image::FLOAT32){
    auto colorPtr = (float*)getPixel(img,pix.x,pix.y);
    for(uint32_t c=0;c<img.channels;++c)
      color[c] = colorPtr[img.channelTypes[c]];
  }
  return color;
}

