/**
 * Copyright (C) 2015-2018 Think Silicon S.A. (https://think-silicon.com/)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public v3
 * License as published by the Free Software Foundation;
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 */

/**
 *  @file       glslangShaderCompiler.h
 *  @author     Think Silicon
 *  @date       25/07/2018
 *  @version    1.0
 *
 *  @brief      Shaders compilation and linking module. It implements ShaderCompiler interface, using glslang 
 *
 */

#ifndef __GLSLANGSHADERCOMPILER_H__
#define __GLSLANGSHADERCOMPILER_H__

#include "resources/shaderCompiler.h"
#include "shaderConverter.h"
#include "glslangCompiler.h"
#include "glslangLinker.h"
#include "resources/shaderReflection.h"
#include "resources/shaderProgram.h"

class GlslangShaderCompiler : public ShaderCompiler {
private:

    typedef enum {
        SHADER_COMPILER_VERTEX   = 0,
        SHADER_COMPILER_FRAGMENT = 1,
        SHADER_COMPILER_TYPE_MAX
    } shader_compiler_type_t;

    static bool             mInitialized;
    static TBuiltInResource mTBuiltInResource;

    GlslangCompiler*        mShaderCompiler[SHADER_COMPILER_TYPE_MAX];
    GlslangLinker*          mProgramLinker;
    ShaderConverter*        mShaderConverter;
    ShaderReflection*       mShaderReflection;

    std::string             mVertSource;
    std::string             mFragSource;
    std::string             mVertSource400;
    std::string             mFragSource400;
    std::vector<uint32_t>   mVertSpv;
    std::vector<uint32_t>   mFragSpv;

    bool mDumpVulkanShaderReflection;
    bool mDumpInputShaderReflection;
    bool mDumpProcessedShaderSource;
    bool mSaveBinaryToFiles;
    bool mSaveSourceToFiles;
    bool mSaveSpvTextToFile;

/// Init Functions
    void InitCompiler(void);
    void InitCompilerResources(void);
    void InitReflection(void);

/// Terminate Functions
    void TerminateCompiler(void);

/// Release Functions
    void Release(void);

    const char* ConvertShader(ShaderProgram& program, shader_type_t shaderType, bool isYInverted);
    void CompileShader400(ShaderProgram& program, shader_type_t shaderType);
    void DumpSlangProgramReflection(const glslang::TProgram* prog) const;

    void PrintReadableSPV(ShaderProgram* program);
    void SaveBinaryToFiles(ShaderProgram* program);
    void SaveShaderSourceToFile(ShaderProgram* program, bool processed, const char* source, shader_type_t shaderType) const;

    void CompileAttributes(const glslang::TProgram* prog);
    void CompileUniforms(const glslang::TProgram* prog);
    void SetUniformBlocksOffset(const glslang::TProgram* prog);
    void SetUniformBlocksSizes(const glslang::TProgram* prog);
    void BuildUniformReflection(void);

    bool GetAttributeHasLocation(const glslang::TProgram* prog, int index) const;
    int  GetAttributeLocation(const glslang::TProgram* prog, int index) const;

    bool GetUniformHasLocation(const glslang::TProgram* prog, int index) const;
    int  GetUniformLocation(const glslang::TProgram* prog, int index) const;

    bool GetUniformHasBinding(const glslang::TProgram* prog, int index) const;
    int  GetUniformBinding(const glslang::TProgram* prog, int index) const;

    bool GetUniformHasSet(const glslang::TProgram* prog, int index) const;
    int  GetUniformSet(const glslang::TProgram* prog, int index) const;

    /// All active uniform variables as reported by glslang
    std::vector<uniform_t>              mUniforms;

    /// uniforms that are an aggregation of basic types
    aggregateMap_t                      mAggregates;

    /// All uniform blocks that will be constructed in the final source.
    /// This map associates all uniform variable names, as reported by glslang,
    /// with the corresponding uniform block that will encapsulate them in the final shader source.
    /// These variables can be any of the following types:
    /// 1. Basic types and arrays of them
    /// 2. Aggregates of basic types and arrays of them
    /// 3. Opaque types (although they can not be in a uniform block, they are stored here for easy reference)
    std::map<string, uniformBlock_t>         mUniformBlocks;
    typedef map<string, uniformBlock_t> uniformBlockMap_t;
    const uniformBlockMap_t &           GetUniformBlocks(void)            const { FUN_ENTRY(GL_LOG_TRACE); return mUniformBlocks; }

public:
    GlslangShaderCompiler();
    ~GlslangShaderCompiler();

    bool CompileVertexShader(const char* const* source) override;
    bool CompileFragmentShader(const char* const* source) override;
    const char* GetProgramInfoLog(void) override;
    const char* GetShaderInfoLog(shader_type_t shaderType) override;
    bool PreprocessShaders(ShaderProgram& shaderProgram, bool isYInverted) override;
    bool LinkProgram(ShaderProgram& shaderProgram) override;
    void PrepareReflection(void) override;
    uint32_t SerializeReflection(void* binary) override;
    uint32_t DeserializeReflection(const void* binary) override;
    bool ValidateProgram(void) override;
    void DumpUniforms(void) override;

    ShaderReflection* GetShaderReflection(void)                        override { return mShaderReflection; }
    void EnableDumpVulkanShaderReflection(void)                        override { mDumpVulkanShaderReflection = true; }
    void EnableDumpInputShaderReflection(void)                         override { mDumpInputShaderReflection = true; }
    void EnableDumpProcessedShaderSource(void)                         override { mDumpProcessedShaderSource = true; }
    void EnableSaveBinaryToFiles(void)                                 override { mSaveBinaryToFiles = true; }
    void EnableSaveSourceToFiles(void)                                 override { mSaveSourceToFiles = true; }
    void EnableSaveSpvTextToFile(void)                                 override { mSaveSpvTextToFile = true; }
};

#endif // __GLSLANGSHADERCOMPILER_H__
