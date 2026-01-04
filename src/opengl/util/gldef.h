#ifndef CORE_GLDEF
#define CORE_GLDEF

#include <iostream>

//#define GLEW_STATIC
//#include "glew/include/GL/glew.h"
//#include "glew/include/GL/wglew.h"

//#ifndef GLEW_STATIC
#include "glext.h"
#include "wglext.h"
#include "glcorearb.h"

#if defined(_WIN32)
// dont bother adding library
#pragma comment(lib,"opengl32.lib")
#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>
#  define glGetProcAddress(name) (void *)wglGetProcAddress((LPCSTR)name)
HMODULE module = LoadLibraryA("opengl32.dll");
#endif 

auto loadExtension = [](const char* extensionName, void** functionPtr)
{
    auto GetAnyGLFuncAddress = [](const char* name)
    {
        void* p = (void*)wglGetProcAddress(name);
        if (p == 0 || (p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) || (p == (void*)-1))
        {

            p = (void*)GetProcAddress(module, name);
        }

        return p;
    };
    *functionPtr = GetAnyGLFuncAddress(extensionName);
    //*functionPtr = (void*)GetAnyGLFuncAddress((const GLubyte*)extensionName);

    return (*functionPtr != NULL);
};


//==================================================================================================
// WGL Extension Functions
//==================================================================================================
PFNWGLCHOOSEPIXELFORMATARBPROC         wglChoosePixelFormatARB = nullptr;
PFNWGLCREATECONTEXTATTRIBSARBPROC      wglCreateContextAttribsARB = nullptr;
PFNWGLSWAPINTERVALEXTPROC              wglSwapIntervalEXT = nullptr;
PFNWGLGETEXTENSIONSSTRINGARBPROC       wglGetExtensionsStringARB = nullptr;

//==================================================================================================
// Context & Capability Queries
//==================================================================================================
PFNGLGETSTRINGPROC                     glGetString = nullptr;
PFNGLGETSTRINGIPROC                    glGetStringi = nullptr;
PFNGLGETINTEGERVPROC                   glGetIntegerv = nullptr;
PFNGLGETFLOATVPROC                     glGetFloatv = nullptr;
PFNGLGETBOOLEANVPROC                   glGetBooleanv = nullptr;
PFNGLGETDOUBLEVPROC                    glGetDoublev = nullptr;
PFNGLGETERRORPROC                      glGetError = nullptr;

//==================================================================================================
// State Management
//==================================================================================================
PFNGLVIEWPORTPROC                      glViewport = nullptr;
PFNGLSCISSORPROC                       glScissor = nullptr;
PFNGLENABLEPROC                        glEnable = nullptr;
PFNGLDISABLEPROC                       glDisable = nullptr;
PFNGLENABLEIPROC                       glEnablei = nullptr;
PFNGLDISABLEIPROC                      glDisablei = nullptr;
PFNGLDEPTHFUNCPROC                     glDepthFunc = nullptr;
PFNGLDEPTHMASKPROC                     glDepthMask = nullptr;
PFNGLDEPTHRANGEPROC                    glDepthRange = nullptr;
PFNGLCOLORMASKPROC                     glColorMask = nullptr;
PFNGLCULLFACEPROC                      glCullFace = nullptr;
PFNGLFRONTFACEPROC                     glFrontFace = nullptr;
PFNGLPOLYGONMODEPROC                   glPolygonMode = nullptr;
PFNGLPOLYGONOFFSETPROC                 glPolygonOffset = nullptr;
PFNGLLINEWIDTHPROC                     glLineWidth = nullptr;
PFNGLPOINTSIZEPROC                     glPointSize = nullptr;
PFNGLBLENDFUNCPROC                     glBlendFunc = nullptr;
PFNGLBLENDFUNCSEPARATEPROC             glBlendFuncSeparate = nullptr;
PFNGLBLENDFUNCSEPARATEIPROC            glBlendFuncSeparatei = nullptr;
PFNGLBLENDEQUATIONPROC                 glBlendEquation = nullptr;
PFNGLBLENDEQUATIONSEPARATEPROC         glBlendEquationSeparate = nullptr;
PFNGLBLENDEQUATIONIPROC                glBlendEquationi = nullptr;
PFNGLBLENDCOLORPROC                    glBlendColor = nullptr;
PFNGLLOGICOPPROC                       glLogicOp = nullptr;
PFNGLSTENCILFUNCPROC                   glStencilFunc = nullptr;
PFNGLSTENCILMASKPROC                   glStencilMask = nullptr;
PFNGLSTENCILOPPROC                     glStencilOp = nullptr;
PFNGLSTENCILFUNCPROC                   glStencilFuncSeparate = nullptr;
PFNGLSTENCILOPSEPARATEPROC             glStencilOpSeparate = nullptr;
PFNGLPIXELSTOREIPROC                   glPixelStorei = nullptr;
PFNGLBLENDFUNCIPROC                     glBlendFunci = nullptr;
PFNGLCOLORMASKIPROC                     glColorMaski = nullptr;
PFNGLBLENDEQUATIONSEPARATEIPROC         glBlendEquationSeparatei = nullptr;
PFNGLSTENCILMASKSEPARATEPROC            glStencilMaskSeparate = nullptr;
PFNGLCLIPCONTROLPROC                    glClipControl = nullptr;

//==================================================================================================
// Shader & Program Management
//==================================================================================================
PFNGLCREATESHADERPROC                  glCreateShader = nullptr;
PFNGLSHADERSOURCEPROC                  glShaderSource = nullptr;
PFNGLCOMPILESHADERPROC                 glCompileShader = nullptr;
PFNGLGETSHADERIVPROC                   glGetShaderiv = nullptr;
PFNGLGETSHADERINFOLOGPROC              glGetShaderInfoLog = nullptr;
PFNGLGETSHADERSOURCEPROC               glGetShaderSource = nullptr;

PFNGLCREATEPROGRAMPROC                 glCreateProgram = nullptr;
PFNGLATTACHSHADERPROC                  glAttachShader = nullptr;
PFNGLLINKPROGRAMPROC                   glLinkProgram = nullptr;
PFNGLGETPROGRAMIVPROC                  glGetProgramiv = nullptr;
PFNGLGETPROGRAMINFOLOGPROC             glGetProgramInfoLog = nullptr;
PFNGLDETACHSHADERPROC                  glDetachShader = nullptr;
PFNGLDELETEPROGRAMPROC                 glDeleteProgram = nullptr;
PFNGLDELETEBUFFERSPROC                 glDeleteBuffers = nullptr;
PFNGLDELETEVERTEXARRAYSPROC            glDeleteVertexArrays = nullptr;
PFNGLISSHADERPROC                      glIsShader = nullptr;
PFNGLISPROGRAMPROC                     glIsProgram = nullptr;
PFNGLDELETESHADERPROC                   glDeleteShader = nullptr;
PFNGLUSEPROGRAMPROC                     glUseProgram = nullptr;

//==================================================================================================
// Vertex Arrays & Buffer Objects
//==================================================================================================
PFNGLGENVERTEXARRAYSPROC               glGenVertexArrays = nullptr;
PFNGLCREATEVERTEXARRAYSPROC            glCreateVertexArrays = nullptr;
PFNGLBINDVERTEXARRAYPROC               glBindVertexArray = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC       glEnableVertexAttribArray = nullptr;
PFNGLDISABLEVERTEXATTRIBARRAYPROC      glDisableVertexAttribArray = nullptr;
PFNGLVERTEXATTRIBPOINTERPROC           glVertexAttribPointer = nullptr;
PFNGLVERTEXATTRIBFORMATPROC            glVertexAttribFormat = nullptr;
PFNGLVERTEXATTRIBBINDINGPROC           glVertexAttribBinding = nullptr;
PFNGLVERTEXATTRIBDIVISORPROC           glVertexAttribDivisor = nullptr;
PFNGLBINDVERTEXBUFFERPROC              glBindVertexBuffer = nullptr;
PFNGLVERTEXARRAYVERTEXBUFFERPROC       glVertexArrayVertexBuffer = nullptr;
PFNGLGETPROGRAMRESOURCEINDEXPROC       glGetProgramResourceIndex = nullptr;
PFNGLSHADERSTORAGEBLOCKBINDINGPROC     glShaderStorageBlockBinding = nullptr;
PFNGLENABLEVERTEXARRAYATTRIBPROC       glEnableVertexArrayAttrib = nullptr;
PFNGLVERTEXARRAYVERTEXATTRIBIFORMATEXTPROC glVertexArrayAttribIFormat = nullptr;
PFNGLVERTEXARRAYATTRIBFORMATPROC       glVertexArrayAttribFormat = nullptr;
PFNGLVERTEXARRAYATTRIBBINDINGPROC      glVertexArrayAttribBinding = nullptr;

//--------------------------------------------------------------------------------------------------
// Buffer Storage & Data
//--------------------------------------------------------------------------------------------------
PFNGLGENBUFFERSPROC                    glGenBuffers = nullptr;
PFNGLCREATEBUFFERSPROC                 glCreateBuffers = nullptr;
PFNGLBINDBUFFERPROC                    glBindBuffer = nullptr;
PFNGLBUFFERDATAPROC                    glBufferData = nullptr;
PFNGLBUFFERSTORAGEPROC                 glBufferStorage = nullptr;
PFNGLNAMEDBUFFERDATAPROC               glNamedBufferData = nullptr;
PFNGLNAMEDBUFFERSTORAGEPROC            glNamedBufferStorage = nullptr;
PFNGLBUFFERSUBDATAPROC                 glBufferSubData = nullptr;
PFNGLNAMEDBUFFERSUBDATAPROC            glNamedBufferSubData = nullptr;
PFNGLMAPBUFFERRANGEPROC                glMapBufferRange = nullptr;
PFNGLMAPBUFFERPROC                     glMapBuffer = nullptr;
PFNGLUNMAPBUFFERPROC                   glUnmapBuffer = nullptr;
PFNGLGETBUFFERSUBDATAARBPROC           glGetBufferSubData = nullptr;
PFNGLMAPNAMEDBUFFERRANGEPROC            glMapNamedBufferRange = nullptr;
PFNGLUNMAPNAMEDBUFFERPROC               glUnmapNamedBuffer = nullptr;
PFNGLMAPNAMEDBUFFERPROC                 glMapNamedBuffer = nullptr;
PFNGLCLEARNAMEDBUFFERDATAPROC           glClearNamedBufferData = nullptr;
PFNGLGETNAMEDBUFFERPARAMETERIVPROC      glGetNamedBufferParameteriv = nullptr;
PFNGLINVALIDATEBUFFERDATAPROC		  glInvalidateBufferData = nullptr;
PFNGLINVALIDATEBUFFERSUBDATAPROC       glInvalidateBufferSubData = nullptr;
PFNGLCLEARNAMEDBUFFERSUBDATAPROC 	 glClearNamedBufferSubData = nullptr;
PFNGLCOPYNAMEDBUFFERSUBDATAPROC     glCopyNamedBufferSubData = nullptr;
PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC	   glNamedFramebufferDrawBuffer = nullptr;
PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC  glNamedFramebufferDrawBuffers = nullptr;
PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC   glCheckNamedFramebufferStatus = nullptr;

//==================================================================================================
// Framebuffers & Renderbuffers
//==================================================================================================
PFNGLGENFRAMEBUFFERSPROC               glGenFramebuffers = nullptr;
PFNGLCREATEFRAMEBUFFERSPROC            glCreateFramebuffers = nullptr;
PFNGLBINDFRAMEBUFFERPROC               glBindFramebuffer = nullptr;
PFNGLFRAMEBUFFERTEXTURE2DPROC          glFramebufferTexture2D = nullptr;
PFNGLFRAMEBUFFERTEXTUREPROC            glFramebufferTexture = nullptr;
PFNGLFRAMEBUFFERRENDERBUFFERPROC       glFramebufferRenderbuffer = nullptr;
PFNGLRENDERBUFFERSTORAGEPROC           glRenderbufferStorage = nullptr;
PFNGLBINDRENDERBUFFERPROC              glBindRenderbuffer = nullptr;
PFNGLBLITFRAMEBUFFERPROC               glBlitFramebuffer = nullptr;
PFNGLCHECKFRAMEBUFFERSTATUSPROC        glCheckFramebufferStatus = nullptr;
PFNGLINVALIDATEFRAMEBUFFERPROC         glInvalidateFramebuffer = nullptr;
PFNGLINVALIDATESUBFRAMEBUFFERPROC      glInvalidateSubFramebuffer = nullptr;
PFNGLGENRENDERBUFFERSPROC               glGenRenderbuffers = nullptr;
PFNGLDELETERENDERBUFFERSPROC            glDeleteRenderbuffers = nullptr;
PFNGLDELETEFRAMEBUFFERSPROC             glDeleteFramebuffers = nullptr;
PFNGLNAMEDFRAMEBUFFERTEXTUREPROC        glNamedFramebufferTexture = nullptr;
PFNGLDRAWBUFFERPROC                     glDrawBuffer = nullptr;
PFNGLDRAWBUFFERSPROC                    glDrawBuffers = nullptr;
PFNGLREADBUFFERPROC                     glReadBuffer = nullptr;
PFNGLINVALIDATENAMEDFRAMEBUFFERDATAPROC glInvalidateNamedFramebufferData = nullptr;
PFNGLINVALIDATENAMEDFRAMEBUFFERSUBDATAPROC glInvalidateNamedFramebufferSubData = nullptr;
PFNGLNAMEDRENDERBUFFERSTORAGEPROC       glNamedRenderbufferStorage = nullptr;
PFNGLNAMEDFRAMEBUFFERRENDERBUFFERPROC glNamedFramebufferRenderbuffer = nullptr;
PFNGLCREATERENDERBUFFERSPROC glCreateRenderbuffers = nullptr;

//==================================================================================================
// Textures & Samplers
//==================================================================================================
PFNGLGENTEXTURESPROC                   glGenTextures = nullptr;
PFNGLCREATETEXTURESPROC                glCreateTextures = nullptr;
PFNGLBINDTEXTUREPROC                   glBindTexture = nullptr;
PFNGLTEXIMAGE2DPROC                    glTexImage2D = nullptr;
PFNGLTEXSTORAGE2DPROC                  glTexStorage2D = nullptr;
PFNGLTEXSTORAGE3DPROC                  glTexStorage3D = nullptr;
PFNGLTEXTURESUBIMAGE2DPROC             glTextureSubImage2D = nullptr;
PFNGLTEXSUBIMAGE2DPROC                 glTexSubImage2D = nullptr;
PFNGLCOMPRESSEDTEXIMAGE2DPROC          glCompressedTexImage2D = nullptr;
PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC       glCompressedTexSubImage2D = nullptr;
PFNGLGENERATEMIPMAPPROC                glGenerateMipmap = nullptr;
PFNGLGENERATETEXTUREMIPMAPPROC         glGenerateTextureMipmap = nullptr;
PFNGLACTIVETEXTUREPROC                 glActiveTexture = nullptr;
PFNGLTEXTUREPARAMETERFPROC              glTextureParameterf = nullptr;
PFNGLTEXTUREPARAMETERIPROC             glTextureParameteri = nullptr;
PFNGLTEXPARAMETERIPROC                 glTexParameteri = nullptr;
PFNGLTEXPARAMETERFVPROC                glTexParameterfv = nullptr;
PFNGLTEXPARAMETERIIVPROC               glTexParameterIiv = nullptr;
PFNGLTEXPARAMETERIUIVPROC              glTexParameterIuiv = nullptr;
PFNGLBINDIMAGETEXTUREPROC              glBindImageTexture = nullptr;
PFNGLGETTEXIMAGEPROC                   glGetTexImage = nullptr;
PFNGLGETCOMPRESSEDTEXIMAGEPROC         glGetCompressedTexImage = nullptr;
PFNGLBINDTEXTUREUNITPROC                glBindTextureUnit = nullptr;
PFNGLDELETETEXTURESPROC                 glDeleteTextures = nullptr;
PFNGLTEXTURESTORAGE1DPROC               glTextureStorage1D = nullptr;
PFNGLTEXTURESTORAGE2DPROC               glTextureStorage2D = nullptr;
PFNGLTEXTURESTORAGE3DPROC               glTextureStorage3D = nullptr;
PFNGLCLEARTEXSUBIMAGEPROC               glClearTexSubImage = nullptr;
PFNGLCLEARTEXIMAGEPROC                  glClearTexImage = nullptr;

//--------------------------------------------------------------------------------------------------
// Sampler Objects
//--------------------------------------------------------------------------------------------------
PFNGLGENSAMPLERSPROC                   glGenSamplers = nullptr;
PFNGLDELETESAMPLERSPROC                glDeleteSamplers = nullptr;
PFNGLBINDSAMPLERPROC                   glBindSampler = nullptr;
PFNGLSAMPLERPARAMETERIPROC             glSamplerParameteri = nullptr;
PFNGLSAMPLERPARAMETERFPROC             glSamplerParameterf = nullptr;

//==================================================================================================
// Uniforms & Uniform Buffer Objects
//==================================================================================================
PFNGLGETUNIFORMLOCATIONPROC            glGetUniformLocation = nullptr;
PFNGLUNIFORM1IPROC                     glUniform1i = nullptr;
PFNGLUNIFORM2IPROC                     glUniform2i = nullptr;
PFNGLUNIFORM3IPROC                     glUniform3i = nullptr;
PFNGLUNIFORM4IPROC                     glUniform4i = nullptr;
PFNGLUNIFORM1FPROC                     glUniform1f = nullptr;
PFNGLUNIFORM2FPROC                     glUniform2f = nullptr;
PFNGLUNIFORM3FPROC                     glUniform3f = nullptr;
PFNGLUNIFORM4FPROC                     glUniform4f = nullptr;
PFNGLUNIFORM1DPROC                     glUniform1d = nullptr;
PFNGLUNIFORM2DPROC                     glUniform2d = nullptr;
PFNGLUNIFORM3DPROC                     glUniform3d = nullptr;
PFNGLUNIFORM4DPROC                     glUniform4d = nullptr;
PFNGLUNIFORMMATRIX2FVPROC              glUniformMatrix2fv = nullptr;
PFNGLUNIFORMMATRIX3FVPROC              glUniformMatrix3fv = nullptr;
PFNGLUNIFORMMATRIX4FVPROC              glUniformMatrix4fv = nullptr;
PFNGLUNIFORMMATRIX2X3FVPROC            glUniformMatrix2x3fv = nullptr;
PFNGLUNIFORMMATRIX3X2FVPROC            glUniformMatrix3x2fv = nullptr;
PFNGLUNIFORMMATRIX2X4FVPROC            glUniformMatrix2x4fv = nullptr;
PFNGLUNIFORMMATRIX4X2FVPROC            glUniformMatrix4x2fv = nullptr;
PFNGLUNIFORMMATRIX3X4FVPROC            glUniformMatrix3x4fv = nullptr;
PFNGLUNIFORMMATRIX4X3FVPROC            glUniformMatrix4x3fv = nullptr;

PFNGLGETUNIFORMFVPROC                  glGetUniformfv = nullptr;
PFNGLGETUNIFORMIVPROC                  glGetUniformiv = nullptr;
PFNGLGETUNIFORMBLOCKINDEXPROC          glGetUniformBlockIndex = nullptr;
PFNGLUNIFORMBLOCKBINDINGPROC           glUniformBlockBinding = nullptr;
PFNGLBINDBUFFERBASEPROC                glBindBufferBase = nullptr;
PFNGLBINDBUFFERRANGEPROC               glBindBufferRange = nullptr;
PFNGLGETACTIVEUNIFORMBLOCKIVPROC       glGetActiveUniformBlockiv = nullptr;
PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC     glGetActiveUniformBlockName = nullptr;

PFNGLPROGRAMUNIFORMMATRIX3FVPROC        glProgramUniformMatrix3fv = nullptr;
PFNGLUNIFORM2IVPROC                     glUniform2iv = nullptr;
PFNGLUNIFORM3IVPROC                     glUniform3iv = nullptr;
PFNGLUNIFORM4IVPROC                     glUniform4iv = nullptr;

//--------------------------------------------------------------------------------------------------
// Direct Program Uniforms (no glUseProgram)
//--------------------------------------------------------------------------------------------------
PFNGLPROGRAMUNIFORM1IPROC               glProgramUniform1i = nullptr;
PFNGLPROGRAMUNIFORM2IPROC               glProgramUniform2i = nullptr;
PFNGLPROGRAMUNIFORM3IPROC               glProgramUniform3i = nullptr;
PFNGLPROGRAMUNIFORM4IPROC               glProgramUniform4i = nullptr;
PFNGLPROGRAMUNIFORM1FPROC               glProgramUniform1f = nullptr;
PFNGLPROGRAMUNIFORM2FPROC               glProgramUniform2f = nullptr;
PFNGLPROGRAMUNIFORM3FPROC               glProgramUniform3f = nullptr;
PFNGLPROGRAMUNIFORM4FPROC               glProgramUniform4f = nullptr;
PFNGLPROGRAMUNIFORM1DVPROC              glProgramUniform1d = nullptr;
PFNGLPROGRAMUNIFORMMATRIX4FVPROC        glProgramUniformMatrix4fv = nullptr;

//==================================================================================================
// Drawing Commands
//==================================================================================================
PFNGLDRAWARRAYSPROC                    glDrawArrays = nullptr;
PFNGLDRAWELEMENTSPROC                  glDrawElements = nullptr;
PFNGLMULTIDRAWARRAYSPROC               glMultiDrawArrays = nullptr;
PFNGLMULTIDRAWELEMENTSPROC             glMultiDrawElements = nullptr;
PFNGLDRAWARRAYSINSTANCEDPROC           glDrawArraysInstanced = nullptr;
PFNGLDRAWELEMENTSINSTANCEDPROC         glDrawElementsInstanced = nullptr;
PFNGLDRAWARRAYSINDIRECTPROC            glDrawArraysIndirect = nullptr;
PFNGLDRAWELEMENTSINDIRECTPROC          glDrawElementsIndirect = nullptr;
PFNGLMULTIDRAWARRAYSINDIRECTPROC       glMultiDrawArraysIndirect = nullptr;
PFNGLMULTIDRAWELEMENTSINDIRECTPROC     glMultiDrawElementsIndirect = nullptr;

//==================================================================================================
// Queries & Synchronization
//==================================================================================================
PFNGLGENQUERIESPROC                    glGenQueries = nullptr;
PFNGLDELETEQUERIESPROC                 glDeleteQueries = nullptr;
PFNGLBEGINQUERYPROC                    glBeginQuery = nullptr;
PFNGLENDQUERYPROC                      glEndQuery = nullptr;
PFNGLGETQUERYIVPROC                    glGetQueryiv = nullptr;
PFNGLGETQUERYOBJECTIVPROC              glGetQueryObjectiv = nullptr;

PFNGLFENCESYNCPROC                     glFenceSync = nullptr;
PFNGLISSYNCPROC                        glIsSync = nullptr;
PFNGLDELETESYNCPROC                    glDeleteSync = nullptr;
PFNGLCLIENTWAITSYNCPROC                glClientWaitSync = nullptr;
PFNGLWAITSYNCPROC                      glWaitSync = nullptr;
PFNGLGETSYNCIVPROC                     glGetSynciv = nullptr;

//==================================================================================================
// Compute & Memory Barriers
//==================================================================================================
PFNGLDISPATCHCOMPUTEPROC               glDispatchCompute = nullptr;
PFNGLDISPATCHCOMPUTEGROUPSIZEARBPROC   glDispatchComputeGroupSizeARB = nullptr;
PFNGLMEMORYBARRIERPROC                 glMemoryBarrier = nullptr;
PFNGLMEMORYBARRIERBYREGIONPROC         glMemoryBarrierByRegion = nullptr;

//==================================================================================================
// Transform Feedback
//==================================================================================================
PFNGLGENTRANSFORMFEEDBACKSPROC         glGenTransformFeedbacks = nullptr;
PFNGLCREATETRANSFORMFEEDBACKSPROC      glCreateTransformFeedbacks = nullptr;
PFNGLBINDTRANSFORMFEEDBACKPROC         glBindTransformFeedback = nullptr;
PFNGLBEGINTRANSFORMFEEDBACKPROC        glBeginTransformFeedback = nullptr;
PFNGLENDTRANSFORMFEEDBACKPROC          glEndTransformFeedback = nullptr;
PFNGLPAUSETRANSFORMFEEDBACKPROC        glPauseTransformFeedback = nullptr;
PFNGLRESUMETRANSFORMFEEDBACKPROC       glResumeTransformFeedback = nullptr;
PFNGLTRANSFORMFEEDBACKBUFFERBASEPROC   glTransformFeedbackBufferBase = nullptr;
PFNGLTRANSFORMFEEDBACKBUFFERRANGEPROC  glTransformFeedbackBufferRange = nullptr;

//==================================================================================================
// Debug & Labeling (KHR_debug / ARB_debug_output)
//==================================================================================================
PFNGLDEBUGMESSAGECALLBACKPROC          glDebugMessageCallback = nullptr;
PFNGLDEBUGMESSAGECONTROLPROC           glDebugMessageControl = nullptr;
PFNGLGETDEBUGMESSAGELOGPROC            glGetDebugMessageLog = nullptr;
PFNGLPUSHDEBUGGROUPPROC                glPushDebugGroup = nullptr;
PFNGLPOPDEBUGGROUPPROC                 glPopDebugGroup = nullptr;
PFNGLOBJECTLABELPROC                   glObjectLabel = nullptr;
PFNGLGETOBJECTLABELPROC                glGetObjectLabel = nullptr;
PFNGLOBJECTPTRLABELPROC                glObjectPtrLabel = nullptr;
PFNGLGETOBJECTPTRLABELPROC             glGetObjectPtrLabel = nullptr;
PFNGLDEBUGMESSAGEINSERTPROC             glDebugMessageInsert = nullptr;

//==================================================================================================
// Extensions & Miscellaneous
//==================================================================================================
PFNGLGETINTERNALFORMATIVPROC           glGetInternalformativ = nullptr;
PFNGLPROGRAMPARAMETERIPROC             glProgramParameteri = nullptr;
PFNGLGENPROGRAMPIPELINESPROC           glGenProgramPipelines = nullptr;
PFNGLUSEPROGRAMSTAGESPROC              glUseProgramStages = nullptr;
PFNGLBINDPROGRAMPIPELINEPROC           glBindProgramPipeline = nullptr;
PFNGLGETATTRIBLOCATIONPROC             glGetAttribLocation = nullptr;
PFNGLBINDATTRIBLOCATIONPROC            glBindAttribLocation = nullptr;
PFNGLPROVOKINGVERTEXPROC               glProvokingVertex = nullptr;
PFNGLPATCHPARAMETERIPROC               glPatchParameteri = nullptr;
PFNGLFRAMEBUFFERTEXTURELAYERPROC       glFramebufferTextureLayer = nullptr;
PFNGLGETBUFFERPARAMETERIVPROC          glGetBufferParameteriv = nullptr;
PFNGLGETBUFFERPARAMETERI64VPROC        glGetBufferParameteri64v = nullptr;
PFNGLCLEARBUFFERFVPROC                 glClearBufferfv = nullptr;
PFNGLCLEARBUFFERIVPROC                 glClearBufferiv = nullptr;
PFNGLCLEARBUFFERUIVPROC                glClearBufferuiv = nullptr;
PFNGLCLEARBUFFERFIPROC                 glClearBufferfi = nullptr;
PFNGLCLEARPROC                          glClear = nullptr;
PFNGLCLEARCOLORPROC                     glClearColor = nullptr;
PFNGLFLUSHPROC                          glFlush = nullptr;
PFNGLTEXPAGECOMMITMENTARBPROC           glTexPageCommitmentARB = nullptr;
PFNGLGETTEXTUREHANDLEARBPROC            glGetTextureHandleARB = nullptr;
PFNGLMAKETEXTUREHANDLERESIDENTARBPROC    glMakeTextureHandleResidentARB = nullptr;
PFNGLMAKETEXTUREHANDLENONRESIDENTARBPROC  glMakeTextureHandleNonResidentARB = nullptr;


bool load_gl_extensions() {
#define LE(x) loadExtension(#x, (void**)&x)
#define ABORT_LOAD_EXT(x)  { std::cout << "ERR::GL_EXT::" << #x << "::Failed\n"; return false; };

    // WGL Extension Functions
    if (!LE(wglChoosePixelFormatARB))  ABORT_LOAD_EXT(wglChoosePixelFormatARB);
    if (!LE(wglCreateContextAttribsARB)) ABORT_LOAD_EXT(wglCreateContextAttribsARB);
    if (!LE(wglSwapIntervalEXT)) ABORT_LOAD_EXT(wglSwapIntervalEXT);
    if (!LE(wglGetExtensionsStringARB)) ABORT_LOAD_EXT(wglGetExtensionsStringARB);

    // Context & Capability Queries
    if (!LE(glGetString)) ABORT_LOAD_EXT(glGetString);
    if (!LE(glGetStringi)) ABORT_LOAD_EXT(glGetStringi);
    if (!LE(glGetIntegerv)) ABORT_LOAD_EXT(glGetIntegerv);
    if (!LE(glGetFloatv)) ABORT_LOAD_EXT(glGetFloatv);
    if (!LE(glGetBooleanv)) ABORT_LOAD_EXT(glGetBooleanv);
    if (!LE(glGetDoublev)) ABORT_LOAD_EXT(glGetDoublev);
    if (!LE(glGetError)) ABORT_LOAD_EXT(glGetError);

    // State Management
    if (!LE(glViewport)) ABORT_LOAD_EXT(glViewport);
    if (!LE(glScissor)) ABORT_LOAD_EXT(glScissor);
    if (!LE(glEnable)) ABORT_LOAD_EXT(glEnable);
    if (!LE(glDisable)) ABORT_LOAD_EXT(glDisable);
    if (!LE(glEnablei)) ABORT_LOAD_EXT(glEnablei);
    if (!LE(glDisablei)) ABORT_LOAD_EXT(glDisablei);
    if (!LE(glDepthFunc)) ABORT_LOAD_EXT(glDepthFunc);
    if (!LE(glDepthMask)) ABORT_LOAD_EXT(glDepthMask);
    if (!LE(glDepthRange)) ABORT_LOAD_EXT(glDepthRange);
    if (!LE(glColorMask)) ABORT_LOAD_EXT(glColorMask);
    if (!LE(glCullFace)) ABORT_LOAD_EXT(glCullFace);
    if (!LE(glFrontFace)) ABORT_LOAD_EXT(glFrontFace);
    if (!LE(glPolygonMode)) ABORT_LOAD_EXT(glPolygonMode);
    if (!LE(glPolygonOffset)) ABORT_LOAD_EXT(glPolygonOffset);
    if (!LE(glLineWidth)) ABORT_LOAD_EXT(glLineWidth);
    if (!LE(glPointSize)) ABORT_LOAD_EXT(glPointSize);
    if (!LE(glBlendFunc)) ABORT_LOAD_EXT(glBlendFunc);
    if (!LE(glBlendFuncSeparate)) ABORT_LOAD_EXT(glBlendFuncSeparate);
    if (!LE(glBlendFuncSeparatei)) ABORT_LOAD_EXT(glBlendFuncSeparatei);
    if (!LE(glBlendEquation)) ABORT_LOAD_EXT(glBlendEquation);
    if (!LE(glBlendEquationSeparate)) ABORT_LOAD_EXT(glBlendEquationSeparate);
    if (!LE(glBlendEquationi)) ABORT_LOAD_EXT(glBlendEquationi);
    if (!LE(glBlendColor)) ABORT_LOAD_EXT(glBlendColor);
    if (!LE(glLogicOp)) ABORT_LOAD_EXT(glLogicOp);
    if (!LE(glStencilFunc)) ABORT_LOAD_EXT(glStencilFunc);
    if (!LE(glStencilMask)) ABORT_LOAD_EXT(glStencilMask);
    if (!LE(glStencilOp)) ABORT_LOAD_EXT(glStencilOp);
    if (!LE(glStencilFuncSeparate)) ABORT_LOAD_EXT(glStencilFuncSeparate);
    if (!LE(glStencilOpSeparate)) ABORT_LOAD_EXT(glStencilOpSeparate);
    if (!LE(glPixelStorei)) ABORT_LOAD_EXT(glPixelStorei);
    if (!LE(glBlendFunci)) ABORT_LOAD_EXT(glBlendFunci);
    if (!LE(glColorMaski)) ABORT_LOAD_EXT(glColorMaski);
    if (!LE(glBlendEquationSeparatei)) ABORT_LOAD_EXT(glBlendEquationSeparatei);
    if (!LE(glStencilMaskSeparate)) ABORT_LOAD_EXT(glStencilMaskSeparate);
    if (!LE(glClipControl)) ABORT_LOAD_EXT(glClipControl);

    // Shader & Program Management
    if (!LE(glCreateShader)) ABORT_LOAD_EXT(glCreateShader);
    if (!LE(glShaderSource)) ABORT_LOAD_EXT(glShaderSource);
    if (!LE(glCompileShader)) ABORT_LOAD_EXT(glCompileShader);
    if (!LE(glGetShaderiv)) ABORT_LOAD_EXT(glGetShaderiv);
    if (!LE(glGetShaderInfoLog)) ABORT_LOAD_EXT(glGetShaderInfoLog);
    if (!LE(glGetShaderSource)) ABORT_LOAD_EXT(glGetShaderSource);
    if (!LE(glCreateProgram)) ABORT_LOAD_EXT(glCreateProgram);
    if (!LE(glAttachShader)) ABORT_LOAD_EXT(glAttachShader);
    if (!LE(glLinkProgram)) ABORT_LOAD_EXT(glLinkProgram);
    if (!LE(glGetProgramiv)) ABORT_LOAD_EXT(glGetProgramiv);
    if (!LE(glGetProgramInfoLog)) ABORT_LOAD_EXT(glGetProgramInfoLog);
    if (!LE(glDetachShader)) ABORT_LOAD_EXT(glDetachShader);
    if (!LE(glDeleteProgram)) ABORT_LOAD_EXT(glDeleteProgram);
    if (!LE(glDeleteBuffers)) ABORT_LOAD_EXT(glDeleteBuffers);
    if (!LE(glDeleteVertexArrays)) ABORT_LOAD_EXT(glDeleteVertexArrays);
    if (!LE(glIsShader)) ABORT_LOAD_EXT(glIsShader);
    if (!LE(glIsProgram)) ABORT_LOAD_EXT(glIsProgram);
    if (!LE(glDeleteShader)) ABORT_LOAD_EXT(glDeleteShader);
    if (!LE(glUseProgram)) ABORT_LOAD_EXT(glUseProgram);
    if (!LE(glGetProgramResourceIndex)) ABORT_LOAD_EXT(glGetProgramResourceIndex);
    if (!LE(glShaderStorageBlockBinding)) ABORT_LOAD_EXT(glShaderStorageBlockBinding);
    if (!LE(glEnableVertexArrayAttrib)) ABORT_LOAD_EXT(glEnableVertexArrayAttrib);
    if (!LE(glVertexArrayAttribIFormat)) ABORT_LOAD_EXT(glVertexArrayAttribIFormat);
    if (!LE(glVertexArrayAttribFormat)) ABORT_LOAD_EXT(glVertexArrayAttribFormat);
    if (!LE(glVertexArrayAttribBinding)) ABORT_LOAD_EXT(glVertexArrayAttribBinding);

    // Vertex Arrays & Buffer Objects
    if (!LE(glGenVertexArrays)) ABORT_LOAD_EXT(glGenVertexArrays);
    if (!LE(glCreateVertexArrays)) ABORT_LOAD_EXT(glCreateVertexArrays);
    if (!LE(glBindVertexArray)) ABORT_LOAD_EXT(glBindVertexArray);
    if (!LE(glEnableVertexAttribArray)) ABORT_LOAD_EXT(glEnableVertexAttribArray);
    if (!LE(glDisableVertexAttribArray)) ABORT_LOAD_EXT(glDisableVertexAttribArray);
    if (!LE(glVertexAttribPointer)) ABORT_LOAD_EXT(glVertexAttribPointer);
    if (!LE(glVertexAttribFormat)) ABORT_LOAD_EXT(glVertexAttribFormat);
    if (!LE(glVertexAttribBinding)) ABORT_LOAD_EXT(glVertexAttribBinding);
    if (!LE(glVertexAttribDivisor)) ABORT_LOAD_EXT(glVertexAttribDivisor);
    if (!LE(glBindVertexBuffer)) ABORT_LOAD_EXT(glBindVertexBuffer);
    if (!LE(glVertexArrayVertexBuffer)) ABORT_LOAD_EXT(glVertexArrayVertexBuffer);
    if (!LE(glDrawArrays)) ABORT_LOAD_EXT(glDrawArrays);
    if (!LE(glDrawArraysInstanced)) ABORT_LOAD_EXT(glDrawArraysInstanced);
    if (!LE(glMultiDrawArrays)) ABORT_LOAD_EXT(glMultiDrawArrays);
    if (!LE(glMultiDrawArraysIndirect)) ABORT_LOAD_EXT(glMultiDrawArraysIndirect);
    if (!LE(glDrawArraysIndirect)) ABORT_LOAD_EXT(glDrawArraysIndirect);

    // Buffer Storage & Data
    if (!LE(glGenBuffers)) ABORT_LOAD_EXT(glGenBuffers);
    if (!LE(glCreateBuffers)) ABORT_LOAD_EXT(glCreateBuffers);
    if (!LE(glBindBuffer)) ABORT_LOAD_EXT(glBindBuffer);
    if (!LE(glBufferData)) ABORT_LOAD_EXT(glBufferData);
    if (!LE(glBufferStorage)) ABORT_LOAD_EXT(glBufferStorage);
    if (!LE(glNamedBufferData)) ABORT_LOAD_EXT(glNamedBufferData);
    if (!LE(glNamedBufferStorage)) ABORT_LOAD_EXT(glNamedBufferStorage);
    if (!LE(glBufferSubData)) ABORT_LOAD_EXT(glBufferSubData);
    if (!LE(glNamedBufferSubData)) ABORT_LOAD_EXT(glNamedBufferSubData);
    if (!LE(glMapBufferRange)) ABORT_LOAD_EXT(glMapBufferRange);
    if (!LE(glMapBuffer)) ABORT_LOAD_EXT(glMapBuffer);
    if (!LE(glUnmapBuffer)) ABORT_LOAD_EXT(glUnmapBuffer);
    if (!LE(glGetBufferSubData)) ABORT_LOAD_EXT(glGetBufferSubData);
    if (!LE(glMapNamedBufferRange)) ABORT_LOAD_EXT(glMapNamedBufferRange);
    if (!LE(glUnmapNamedBuffer)) ABORT_LOAD_EXT(glUnmapNamedBuffer);
    if (!LE(glMapNamedBuffer)) ABORT_LOAD_EXT(glMapNamedBuffer);
    if (!LE(glClearNamedBufferData)) ABORT_LOAD_EXT(glClearNamedBufferData);
    if (!LE(glGetNamedBufferParameteriv)) ABORT_LOAD_EXT(glGetNamedBufferParameteriv);
    if (!LE(glInvalidateBufferData)) ABORT_LOAD_EXT(glInvalidateBufferData);
    if (!LE(glInvalidateBufferSubData)) ABORT_LOAD_EXT(glInvalidateBufferSubData);
    if (!LE(glClearNamedBufferSubData)) ABORT_LOAD_EXT(glClearNamedBufferSubData);
    if (!LE(glCopyNamedBufferSubData)) ABORT_LOAD_EXT(glCopyNamedBufferSubData);

    // Framebuffers & Renderbuffers
    if (!LE(glGenFramebuffers)) ABORT_LOAD_EXT(glGenFramebuffers);
    if (!LE(glCreateFramebuffers)) ABORT_LOAD_EXT(glCreateFramebuffers);
    if (!LE(glBindFramebuffer)) ABORT_LOAD_EXT(glBindFramebuffer);
    if (!LE(glFramebufferTexture2D)) ABORT_LOAD_EXT(glFramebufferTexture2D);
    if (!LE(glFramebufferTexture)) ABORT_LOAD_EXT(glFramebufferTexture);
    if (!LE(glFramebufferRenderbuffer)) ABORT_LOAD_EXT(glFramebufferRenderbuffer);
    if (!LE(glRenderbufferStorage)) ABORT_LOAD_EXT(glRenderbufferStorage);
    if (!LE(glBindRenderbuffer)) ABORT_LOAD_EXT(glBindRenderbuffer);
    if (!LE(glBlitFramebuffer)) ABORT_LOAD_EXT(glBlitFramebuffer);
    if (!LE(glCheckFramebufferStatus)) ABORT_LOAD_EXT(glCheckFramebufferStatus);
    if (!LE(glInvalidateFramebuffer)) ABORT_LOAD_EXT(glInvalidateFramebuffer);
    if (!LE(glInvalidateSubFramebuffer)) ABORT_LOAD_EXT(glInvalidateSubFramebuffer);
    if (!LE(glGenRenderbuffers)) ABORT_LOAD_EXT(glGenRenderbuffers);
    if (!LE(glDeleteRenderbuffers)) ABORT_LOAD_EXT(glDeleteRenderbuffers);
    if (!LE(glDeleteFramebuffers)) ABORT_LOAD_EXT(glDeleteFramebuffers);
    if (!LE(glNamedFramebufferTexture)) ABORT_LOAD_EXT(glNamedFramebufferTexture);
    if (!LE(glDrawBuffer)) ABORT_LOAD_EXT(glDrawBuffer);
    if (!LE(glDrawBuffers)) ABORT_LOAD_EXT(glDrawBuffers);
    if (!LE(glReadBuffer)) ABORT_LOAD_EXT(glReadBuffer);
    if (!LE(glInvalidateNamedFramebufferData)) ABORT_LOAD_EXT(glInvalidateNamedFramebufferData);
    if (!LE(glInvalidateNamedFramebufferSubData)) ABORT_LOAD_EXT(glInvalidateNamedFramebufferSubData);
    if (!LE(glNamedRenderbufferStorage)) ABORT_LOAD_EXT(glNamedRenderbufferStorage);
    if (!LE(glNamedFramebufferRenderbuffer)) ABORT_LOAD_EXT(glNamedFramebufferRenderbuffer);
    if (!LE(glCreateRenderbuffers)) ABORT_LOAD_EXT(glCreateRenderbuffers);
    if (!LE(glNamedFramebufferDrawBuffer)) ABORT_LOAD_EXT(glNamedFramebufferDrawBuffer);
    if (!LE(glNamedFramebufferDrawBuffers)) ABORT_LOAD_EXT(glNamedFramebufferDrawBuffers);
    if (!LE(glCheckNamedFramebufferStatus)) ABORT_LOAD_EXT(glCheckNamedFramebufferStatus);

    // Textures & Samplers
    if (!LE(glGenTextures)) ABORT_LOAD_EXT(glGenTextures);
    if (!LE(glCreateTextures)) ABORT_LOAD_EXT(glCreateTextures);
    if (!LE(glBindTexture)) ABORT_LOAD_EXT(glBindTexture);
    if (!LE(glTexImage2D)) ABORT_LOAD_EXT(glTexImage2D);
    if (!LE(glTexStorage2D)) ABORT_LOAD_EXT(glTexStorage2D);
    if (!LE(glTexStorage3D)) ABORT_LOAD_EXT(glTexStorage3D);
    if (!LE(glTexSubImage2D)) ABORT_LOAD_EXT(glTexSubImage2D);
    if (!LE(glTextureSubImage2D)) ABORT_LOAD_EXT(glTextureSubImage2D);
    if (!LE(glCompressedTexImage2D)) ABORT_LOAD_EXT(glCompressedTexImage2D);
    if (!LE(glCompressedTexSubImage2D)) ABORT_LOAD_EXT(glCompressedTexSubImage2D);
    if (!LE(glGenerateMipmap)) ABORT_LOAD_EXT(glGenerateMipmap);
    if (!LE(glGenerateTextureMipmap)) ABORT_LOAD_EXT(glGenerateTextureMipmap);
    if (!LE(glActiveTexture)) ABORT_LOAD_EXT(glActiveTexture);
    if (!LE(glTexParameteri)) ABORT_LOAD_EXT(glTextureParameteri);
    if (!LE(glTextureParameterf)) ABORT_LOAD_EXT(glTextureParameterf);
    if (!LE(glTexParameteri)) ABORT_LOAD_EXT(glTexParameteri);
    if (!LE(glTexParameterfv)) ABORT_LOAD_EXT(glTexParameterfv);
    if (!LE(glTexParameterIiv)) ABORT_LOAD_EXT(glTexParameterIiv);
    if (!LE(glTexParameterIuiv)) ABORT_LOAD_EXT(glTexParameterIuiv);
    if (!LE(glBindImageTexture)) ABORT_LOAD_EXT(glBindImageTexture);
    if (!LE(glGetTexImage)) ABORT_LOAD_EXT(glGetTexImage);
    if (!LE(glGetCompressedTexImage)) ABORT_LOAD_EXT(glGetCompressedTexImage);
    if (!LE(glBindTextureUnit)) ABORT_LOAD_EXT(glBindTextureUnit);
    if (!LE(glDeleteTextures)) ABORT_LOAD_EXT(glDeleteTextures);
    if (!LE(glTextureStorage1D)) ABORT_LOAD_EXT(glTextureStorage1D);
    if (!LE(glTextureStorage2D)) ABORT_LOAD_EXT(glTextureStorage2D);
    if (!LE(glTextureStorage3D)) ABORT_LOAD_EXT(glTextureStorage3D);
    if (!LE(glClearTexSubImage)) ABORT_LOAD_EXT(glClearTexSubImage);
    if (!LE(glClearTexImage)) ABORT_LOAD_EXT(glClearTexImage);

    // Sampler Objects
    if (!LE(glGenSamplers)) ABORT_LOAD_EXT(glGenSamplers);
    if (!LE(glDeleteSamplers)) ABORT_LOAD_EXT(glDeleteSamplers);
    if (!LE(glBindSampler)) ABORT_LOAD_EXT(glBindSampler);
    if (!LE(glSamplerParameteri)) ABORT_LOAD_EXT(glSamplerParameteri);
    if (!LE(glSamplerParameterf)) ABORT_LOAD_EXT(glSamplerParameterf);

    // Uniforms & UBOs
    if (!LE(glGetUniformLocation)) ABORT_LOAD_EXT(glGetUniformLocation);
    if (!LE(glUniform1i)) ABORT_LOAD_EXT(glUniform1i);
    if (!LE(glUniform2i)) ABORT_LOAD_EXT(glUniform2i);
    if (!LE(glUniform3i)) ABORT_LOAD_EXT(glUniform3i);
    if (!LE(glUniform4i)) ABORT_LOAD_EXT(glUniform4i);
    if (!LE(glUniform1f)) ABORT_LOAD_EXT(glUniform1f);
    if (!LE(glUniform2f)) ABORT_LOAD_EXT(glUniform2f);
    if (!LE(glUniform3f)) ABORT_LOAD_EXT(glUniform3f);
    if (!LE(glUniform4f)) ABORT_LOAD_EXT(glUniform4f);
    if (!LE(glUniform1d)) ABORT_LOAD_EXT(glUniform1d);
    if (!LE(glUniform2d)) ABORT_LOAD_EXT(glUniform2d);
    if (!LE(glUniform3d)) ABORT_LOAD_EXT(glUniform3d);
    if (!LE(glUniform4d)) ABORT_LOAD_EXT(glUniform4d);
    if (!LE(glUniformMatrix2fv)) ABORT_LOAD_EXT(glUniformMatrix2fv);
    if (!LE(glUniformMatrix3fv)) ABORT_LOAD_EXT(glUniformMatrix3fv);
    if (!LE(glUniformMatrix4fv)) ABORT_LOAD_EXT(glUniformMatrix4fv);
    if (!LE(glUniformMatrix2x3fv)) ABORT_LOAD_EXT(glUniformMatrix2x3fv);
    if (!LE(glUniformMatrix3x2fv)) ABORT_LOAD_EXT(glUniformMatrix3x2fv);
    if (!LE(glUniformMatrix2x4fv)) ABORT_LOAD_EXT(glUniformMatrix2x4fv);
    if (!LE(glUniformMatrix4x2fv)) ABORT_LOAD_EXT(glUniformMatrix4x2fv);
    if (!LE(glUniformMatrix3x4fv)) ABORT_LOAD_EXT(glUniformMatrix3x4fv);
    if (!LE(glUniformMatrix4x3fv)) ABORT_LOAD_EXT(glUniformMatrix4x3fv);
    if (!LE(glGetUniformfv)) ABORT_LOAD_EXT(glGetUniformfv);
    if (!LE(glGetUniformiv)) ABORT_LOAD_EXT(glGetUniformiv);
    if (!LE(glGetUniformBlockIndex)) ABORT_LOAD_EXT(glGetUniformBlockIndex);
    if (!LE(glUniformBlockBinding)) ABORT_LOAD_EXT(glUniformBlockBinding);
    if (!LE(glBindBufferBase)) ABORT_LOAD_EXT(glBindBufferBase);
    if (!LE(glBindBufferRange)) ABORT_LOAD_EXT(glBindBufferRange);
    if (!LE(glGetActiveUniformBlockiv)) ABORT_LOAD_EXT(glGetActiveUniformBlockiv);
    if (!LE(glGetActiveUniformBlockName)) ABORT_LOAD_EXT(glGetActiveUniformBlockName);
    if (!LE(glProgramUniformMatrix3fv)) ABORT_LOAD_EXT(glProgramUniformMatrix3fv);
    if (!LE(glUniform2iv)) ABORT_LOAD_EXT(glUniform2iv);
    if (!LE(glUniform3iv)) ABORT_LOAD_EXT(glUniform3iv);
    if (!LE(glUniform4iv)) ABORT_LOAD_EXT(glUniform4iv);

    // Direct Program Uniforms
    if (!LE(glProgramUniform1i)) ABORT_LOAD_EXT(glProgramUniform1i);
    if (!LE(glProgramUniform2i)) ABORT_LOAD_EXT(glProgramUniform2i);
    if (!LE(glProgramUniform3i)) ABORT_LOAD_EXT(glProgramUniform3i);
    if (!LE(glProgramUniform4i)) ABORT_LOAD_EXT(glProgramUniform4i);
    if (!LE(glProgramUniform1f)) ABORT_LOAD_EXT(glProgramUniform1f);
    if (!LE(glProgramUniform2f)) ABORT_LOAD_EXT(glProgramUniform2f);
    if (!LE(glProgramUniform3f)) ABORT_LOAD_EXT(glProgramUniform3f);
    if (!LE(glProgramUniform4f)) ABORT_LOAD_EXT(glProgramUniform4f);
    if (!LE(glProgramUniform1d)) ABORT_LOAD_EXT(glProgramUniform1d);
    if (!LE(glProgramUniformMatrix4fv)) ABORT_LOAD_EXT(glProgramUniformMatrix4fv);

    // Drawing Commands
    if (!LE(glDrawArrays)) ABORT_LOAD_EXT(glDrawArrays);
    if (!LE(glDrawElements)) ABORT_LOAD_EXT(glDrawElements);
    if (!LE(glMultiDrawArrays)) ABORT_LOAD_EXT(glMultiDrawArrays);
    if (!LE(glMultiDrawElements)) ABORT_LOAD_EXT(glMultiDrawElements);
    if (!LE(glDrawArraysInstanced)) ABORT_LOAD_EXT(glDrawArraysInstanced);
    if (!LE(glDrawElementsInstanced)) ABORT_LOAD_EXT(glDrawElementsInstanced);
    if (!LE(glDrawArraysIndirect)) ABORT_LOAD_EXT(glDrawArraysIndirect);
    if (!LE(glDrawElementsIndirect)) ABORT_LOAD_EXT(glDrawElementsIndirect);
    if (!LE(glMultiDrawArraysIndirect)) ABORT_LOAD_EXT(glMultiDrawArraysIndirect);
    if (!LE(glMultiDrawElementsIndirect)) ABORT_LOAD_EXT(glMultiDrawElementsIndirect);

    // Queries & Synchronization
    if (!LE(glGenQueries))             ABORT_LOAD_EXT(glGenQueries);
    if (!LE(glDeleteQueries))          ABORT_LOAD_EXT(glDeleteQueries);
    if (!LE(glBeginQuery))             ABORT_LOAD_EXT(glBeginQuery);
    if (!LE(glEndQuery))               ABORT_LOAD_EXT(glEndQuery);
    if (!LE(glGetQueryiv))             ABORT_LOAD_EXT(glGetQueryiv);
    if (!LE(glGetQueryObjectiv))       ABORT_LOAD_EXT(glGetQueryObjectiv);
    if (!LE(glFenceSync))              ABORT_LOAD_EXT(glFenceSync);
    if (!LE(glIsSync))                 ABORT_LOAD_EXT(glIsSync);
    if (!LE(glDeleteSync))             ABORT_LOAD_EXT(glDeleteSync);
    if (!LE(glClientWaitSync))         ABORT_LOAD_EXT(glClientWaitSync);
    if (!LE(glWaitSync))               ABORT_LOAD_EXT(glWaitSync);
    if (!LE(glGetSynciv))              ABORT_LOAD_EXT(glGetSynciv);

    // Compute & Memory Barriers
    if (!LE(glDispatchCompute))                ABORT_LOAD_EXT(glDispatchCompute);
    //if (!LE(glDispatchComputeGroupSizeARB))    ABORT_LOAD_EXT(glDispatchComputeGroupSizeARB);
    if (!LE(glMemoryBarrier))                  ABORT_LOAD_EXT(glMemoryBarrier);
    if (!LE(glMemoryBarrierByRegion))          ABORT_LOAD_EXT(glMemoryBarrierByRegion);

    // Transform Feedback
    if (!LE(glGenTransformFeedbacks))          ABORT_LOAD_EXT(glGenTransformFeedbacks);
    if (!LE(glCreateTransformFeedbacks))       ABORT_LOAD_EXT(glCreateTransformFeedbacks);
    if (!LE(glBindTransformFeedback))          ABORT_LOAD_EXT(glBindTransformFeedback);
    if (!LE(glBeginTransformFeedback))         ABORT_LOAD_EXT(glBeginTransformFeedback);
    if (!LE(glEndTransformFeedback))           ABORT_LOAD_EXT(glEndTransformFeedback);
    if (!LE(glPauseTransformFeedback))         ABORT_LOAD_EXT(glPauseTransformFeedback);
    if (!LE(glResumeTransformFeedback))        ABORT_LOAD_EXT(glResumeTransformFeedback);
    if (!LE(glTransformFeedbackBufferBase))    ABORT_LOAD_EXT(glTransformFeedbackBufferBase);
    if (!LE(glTransformFeedbackBufferRange))   ABORT_LOAD_EXT(glTransformFeedbackBufferRange);

    // Debug & Labeling (KHR_debug / ARB_debug_output)
    if (!LE(glDebugMessageCallback))           ABORT_LOAD_EXT(glDebugMessageCallback);
    if (!LE(glDebugMessageControl))            ABORT_LOAD_EXT(glDebugMessageControl);
    if (!LE(glGetDebugMessageLog))             ABORT_LOAD_EXT(glGetDebugMessageLog);
    if (!LE(glPushDebugGroup))                 ABORT_LOAD_EXT(glPushDebugGroup);
    if (!LE(glPopDebugGroup))                  ABORT_LOAD_EXT(glPopDebugGroup);
    if (!LE(glObjectLabel))                    ABORT_LOAD_EXT(glObjectLabel);
    if (!LE(glGetObjectLabel))                 ABORT_LOAD_EXT(glGetObjectLabel);
    if (!LE(glObjectPtrLabel))                 ABORT_LOAD_EXT(glObjectPtrLabel);
    if (!LE(glGetObjectPtrLabel))              ABORT_LOAD_EXT(glGetObjectPtrLabel);
    if (!LE(glDebugMessageInsert))             ABORT_LOAD_EXT(glDebugMessageInsert);

    // Extensions & Miscellaneous
    if (!LE(glGetInternalformativ))            ABORT_LOAD_EXT(glGetInternalformativ);
    if (!LE(glProgramParameteri))              ABORT_LOAD_EXT(glProgramParameteri);
    if (!LE(glGenProgramPipelines))            ABORT_LOAD_EXT(glGenProgramPipelines);
    if (!LE(glUseProgramStages))               ABORT_LOAD_EXT(glUseProgramStages);
    if (!LE(glBindProgramPipeline))            ABORT_LOAD_EXT(glBindProgramPipeline);
    if (!LE(glGetAttribLocation))              ABORT_LOAD_EXT(glGetAttribLocation);
    if (!LE(glBindAttribLocation))             ABORT_LOAD_EXT(glBindAttribLocation);
    if (!LE(glProvokingVertex))                ABORT_LOAD_EXT(glProvokingVertex);
    if (!LE(glPatchParameteri))                ABORT_LOAD_EXT(glPatchParameteri);
    if (!LE(glFramebufferTextureLayer))        ABORT_LOAD_EXT(glFramebufferTextureLayer);
    if (!LE(glGetBufferParameteriv))           ABORT_LOAD_EXT(glGetBufferParameteriv);
    if (!LE(glGetBufferParameteri64v))         ABORT_LOAD_EXT(glGetBufferParameteri64v);
    if (!LE(glInvalidateSubFramebuffer))       ABORT_LOAD_EXT(glInvalidateSubFramebuffer);
    if (!LE(glClearBufferfv))                  ABORT_LOAD_EXT(glClearBufferfv);
    if (!LE(glClearBufferiv))                  ABORT_LOAD_EXT(glClearBufferiv);
    if (!LE(glClearBufferuiv))                 ABORT_LOAD_EXT(glClearBufferuiv);
    if (!LE(glClearBufferfi))                  ABORT_LOAD_EXT(glClearBufferfi);
    if (!LE(glClear))                          ABORT_LOAD_EXT(glClear);
    if (!LE(glClearColor))                     ABORT_LOAD_EXT(glClearColor);
    if (!LE(glFlush))                          ABORT_LOAD_EXT(glFlush);
    if (!LE(glTexPageCommitmentARB))           ABORT_LOAD_EXT(glTexPageCommitmentARB);
    if (!LE(glGetTextureHandleARB))            ABORT_LOAD_EXT(glGetTextureHandleARB);
    if (!LE(glMakeTextureHandleResidentARB))    ABORT_LOAD_EXT(glMakeTextureHandleResidentARB);
    if (!LE(glMakeTextureHandleNonResidentARB))ABORT_LOAD_EXT(glMakeTextureHandleNonResidentARB);

    return true;
}

//#else
//bool load_gl_extensions() {
//    glewExperimental = GL_TRUE;
//    if (glewInit() != GLEW_OK)
//        throw "Failed to initialze GLEW";
//}
//#endif

#endif CORE_GLDEF