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
//    Parse shader module
    char* src = parseShader(shaderPath);
    
//    Determine what type of shader is being used
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
    
    
//    initialise our shader compiler
    shaderc_compiler_t compiler = shaderc_compiler_initialize();
    size_t source_text_size = strlen(src);
    
//    Compile shader
    shaderc_compilation_result_t compilerShaderResult = shaderc_compile_into_spv(compiler, src, source_text_size, enumShaderType, input_file_name, entry_point_name, NULL);
    
         // Do Error checking with compilation results.
    size_t numOfErrors = shaderc_result_get_num_errors(compilerShaderResult);
    if (numOfErrors != 0)
    {
        printf("%lu\n", numOfErrors);
        shaderc_compilation_status compileStatusResults = shaderc_result_get_compilation_status(compilerShaderResult);
        printf("%u\n", compileStatusResults);
        const char* compilerErrorMessages = shaderc_result_get_error_message(compilerShaderResult);
        printf("Compiler error message: %s\n", compilerErrorMessages);
        assert();
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
        assert();
    }
    
    
//    Now that we have our frag shader we free the compiled results, and free the compiler itself
    shaderc_result_release(compilerShaderResult);
    shaderc_compiler_release(compiler);
    
    free(src);
    return ShaderModule;

}


char* parseShader(const char* filepath)
{

    // Open our target file for parsing. This is the shader source
    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        printf("Could not open\n");
        assert();
    }
    
    // Establish a buffer array for the file stream
    char buffer[1000];

    // unsigned char ptr array that has the same size array as the buffer and
    // struct variables. This is probably not needed... Checking in on
    // that.
    char* src = malloc(sizeof(char) * 1000);

    // Determiners... These values determine when parsing if source code pertains to
    // a vertex or a fragment shader.
    char version[] = "#version";

    while(fgets(buffer, 1000, fp) != NULL)
    {

        if (strstr(buffer, version))
        {
            strcpy(src, buffer);
        }
        else
        {
            strcat(src, buffer);
        }
    }
    // Lets close that file as we don't need it.
    fclose(fp);

    // Return our parsed shader sources.
    return src;
}


VkPipelineShaderStageCreateInfo* CreateShaderStages(VkShaderModule vertShaderModule, VkShaderModule fragShaderModule)
{
    VkPipelineShaderStageCreateInfo vertShaderStageInfo =
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = vertShaderModule,
        .pName = "main"
    };


    VkPipelineShaderStageCreateInfo fragShaderStageInfo =
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = fragShaderModule,
        .pName = "main"
    };

//    Final shader stage info for graphics pipeline
    VkPipelineShaderStageCreateInfo* shaderStages = malloc(sizeof(VkPipelineShaderStageCreateInfo) * 2);
    shaderStages[0] = vertShaderStageInfo;
    shaderStages[1] = fragShaderStageInfo;
    return shaderStages;
}
