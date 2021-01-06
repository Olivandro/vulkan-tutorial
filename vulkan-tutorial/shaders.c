//
//  shaders.c
//  vulkan-tutorial
//
//  Created by Alejandro Ball on 24/12/2020.
//

#include "shaders.h"



//    11 :: Graphics pipeline :: SHADERS
//    After a long arguous process, we are finally at the graphics pipe line!

//    this whole section can be abstracted into functions and return a single variable
//    Functions needed: (1) parseShaders; (2) compileShaders; (3) createShaderProgram
    
//    7.1 : Shaders (Vertex Shader first)::
//    To Do: Abstract this fundtion
    

VkShaderModule createShaderProgram(VkDevice device, char* shaderPath, const char* input_file_name, const char* entry_point_name, int shaderType)
{
    
    shaderc_shader_kind enumShaderType;
    
    switch (shaderType) {
        case 0:
            enumShaderType = shaderc_glsl_vertex_shader;
            break;
        case 1:
            enumShaderType = shaderc_glsl_fragment_shader;
            break;
        case 2:
            enumShaderType = shaderc_glsl_compute_shader;
            break;
        case 3:
            enumShaderType = shaderc_glsl_geometry_shader;
            break;
        case 4:
            enumShaderType = shaderc_glsl_tess_control_shader;
            break;
        case 5:
            enumShaderType = shaderc_glsl_tess_evaluation_shader;
            break;
        default:
            enumShaderType = shaderc_glsl_default_vertex_shader;
            break;
    }
    
    
    
    shaderc_compiler_t compiler = shaderc_compiler_initialize();
    size_t source_text_size = strlen(shaderPath);
    
    shaderc_compilation_result_t compilerShaderResult = shaderc_compile_into_spv(compiler, shaderPath, source_text_size, enumShaderType, input_file_name, entry_point_name, NULL);
    
         // Do Error checking with compilation results.
    size_t numOfErrors = shaderc_result_get_num_errors(compilerShaderResult);
    if (numOfErrors != 0)
    {
        printf("%lu\n", numOfErrors);
        shaderc_compilation_status compileStatusResults = shaderc_result_get_compilation_status(compilerShaderResult);
        printf("%u\n", compileStatusResults);
        const char* compilerErrorMessages = shaderc_result_get_error_message(compilerShaderResult);
        printf("Compiler error message: %s\n", compilerErrorMessages);
    }
    
    
    size_t compiledByteSize = shaderc_result_get_length(compilerShaderResult);
    const char* compiledBytes = shaderc_result_get_bytes(compilerShaderResult);
    
//    Lets create the Vertinfo struct - this
    VkShaderModuleCreateInfo createShaderInfo =
    {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = compiledByteSize,
        .pCode = (const uint32_t*) compiledBytes
    };
//
//
    VkShaderModule ShaderModule;
    if (vkCreateShaderModule(device, &createShaderInfo, NULL, &ShaderModule) != VK_SUCCESS)
    {
        perror("failed to create shader module!\n");
    }
    
    
//    Now that we have our frag shader we free the compiled results, and free the compiler itself
    shaderc_result_release(compilerShaderResult);
    shaderc_compiler_release(compiler);
    
    return ShaderModule;

}

