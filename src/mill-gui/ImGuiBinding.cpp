/* 
This file is based on opengl3 example from official repository.
Rewritten to learn how it works & change it to a bit more friendly form.
*/

#include "ImGuiBinding.h"

#include <imgui.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace ImGuiBinding
{
    static GLFWwindow *gWindow;

    static GLuint gShaderProgram;
    static GLuint gVAO, gEBO, gVBO;
    static GLuint gShaderLocTexture; 
    static GLuint gShaderLocProjMtx; 
    static GLuint gShaderLocPosition; 
    static GLuint gShaderLocUV; 
    static GLuint gShaderLocColor; 
    static GLuint gFontTexture;

    static float gTime;
    static bool gMousePressed[3];
    static float gMouseWheel;

    static void setupIO();
    static void setupGLFWCallbacks();
    static void createFontsTexture();
    static void renderDrawLists(ImDrawData *drawData);
    static void setClipboardText(const char *text);
    static const char *getClipboardText();

    bool initialize(GLFWwindow *window, bool setGLFWCallbacks)
    {
        gWindow = window;

        setupIO();
        
        if (setGLFWCallbacks)
        {
            setupGLFWCallbacks();
        }

        return true;
    }

    static void setupIO()
    {
        ImGuiIO &io = ImGui::GetIO();

    	io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
		io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
		io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
		io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
		io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
		io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
		io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
		io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
		io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
		io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
		io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
		io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
		io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
		io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
		io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
		io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
		io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
		io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
		io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z; 

        io.RenderDrawListsFn = renderDrawLists;
        io.SetClipboardTextFn = setClipboardText;
        io.GetClipboardTextFn = getClipboardText;

#ifdef _WIN32
        io.ImeWindowHandle = glfwGetWin32Window(gWindow);
#endif
    }

    static void setupGLFWCallbacks()
    {
        glfwSetMouseButtonCallback(gWindow, mouseButtonCallback);
        glfwSetScrollCallback(gWindow, scrollCallback);
        glfwSetKeyCallback(gWindow, keyCallback);
        glfwSetCharCallback(gWindow, charCallback);
    }

    static void renderDrawLists(ImDrawData *drawData)
    {
        ImGuiIO &io = ImGui::GetIO();

        int fbWidth = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
        int fbHeight = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);

        if (fbWidth == 0 || fbHeight == 0)
        {
            return;
        }

        drawData->ScaleClipRects(io.DisplayFramebufferScale);

		GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
		GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
		GLint last_active_texture; glGetIntegerv(GL_ACTIVE_TEXTURE, &last_active_texture);
		GLint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
		GLint last_element_array_buffer; glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
		GLint last_vertex_array; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
		GLint last_blend_src; glGetIntegerv(GL_BLEND_SRC, &last_blend_src);
		GLint last_blend_dst; glGetIntegerv(GL_BLEND_DST, &last_blend_dst);
		GLint last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, &last_blend_equation_rgb);
		GLint last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &last_blend_equation_alpha);
		GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
		GLint last_scissor_box[4]; glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box); 
		GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
		GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
		GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
		GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_SCISSOR_TEST);
        glActiveTexture(GL_TEXTURE0);

        glViewport(0, 0, (GLsizei)fbWidth, (GLsizei)fbHeight);

        const float orthoProjectionMatrix[4][4] =
        {
            { 2.0f/(io.DisplaySize.x), 0.0f, 0.0f, 0.0f },
            { 0.0f, 2.0f/(-io.DisplaySize.y), 0.0f, 0.0f },
            { 0.0f, 0.0f, -1.0f, 0.0f },
            {-1.0f, 1.0f, 0.0f, 1.0f },
        };

        glUseProgram(gShaderProgram);
        glUniform1i(gShaderLocTexture, 0);
        glUniformMatrix4fv(gShaderLocProjMtx, 1, GL_FALSE, 
            &orthoProjectionMatrix[0][0]);
        glBindVertexArray(gVAO);

        for (int n = 0; n < drawData->CmdListsCount; ++n)
        {
            const ImDrawList *cmdList = drawData->CmdLists[n];
            const ImDrawIdx *idxBufferOffset = 0;

            glBindBuffer(GL_ARRAY_BUFFER, gVBO);
            glBufferData(GL_ARRAY_BUFFER, 
                (GLsizeiptr)cmdList->VtxBuffer.Size * sizeof(ImDrawVert),
                (GLvoid*)cmdList->VtxBuffer.Data, GL_STREAM_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gEBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                (GLsizeiptr)cmdList->IdxBuffer.Size * sizeof(ImDrawIdx),
                (GLvoid*)cmdList->IdxBuffer.Data, GL_STREAM_DRAW);

            for (int i = 0; i < cmdList->CmdBuffer.Size; ++i)
            {
                const ImDrawCmd *drawCmd = &cmdList->CmdBuffer[i];

                if (drawCmd->UserCallback)
                {
                    drawCmd->UserCallback(cmdList, drawCmd);
                }
                else
                {
                    glBindTexture(GL_TEXTURE_2D, 
                        (GLuint)(intptr_t)drawCmd->TextureId
					);
                    glScissor(
                        (int)drawCmd->ClipRect.x,
                        (int)(fbHeight - drawCmd->ClipRect.w),
                        (int)(drawCmd->ClipRect.z - drawCmd->ClipRect.x),
                        (int)(drawCmd->ClipRect.w - drawCmd->ClipRect.y)
                    );
                    glDrawElements(
                        GL_TRIANGLES,
                        (GLsizei)drawCmd->ElemCount,
                        sizeof(ImDrawIdx) == 2 
                            ? GL_UNSIGNED_SHORT
                            : GL_UNSIGNED_INT,
                        idxBufferOffset
                    );
                }
                
                idxBufferOffset += drawCmd->ElemCount;
            }
        }

		glUseProgram(last_program);
		glActiveTexture(last_active_texture);
		glBindTexture(GL_TEXTURE_2D, last_texture);
		glBindVertexArray(last_vertex_array);
		glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
		glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
		glBlendFunc(last_blend_src, last_blend_dst);
		if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
		if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
		if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
		if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
		glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
		glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
    }

    static void setClipboardText(const char *text)
    {
        glfwSetClipboardString(gWindow, text);
    }

    static const char *getClipboardText()
    {
        return glfwGetClipboardString(gWindow);
    }

    void shutdown()
    {
        invalidateDeviceObjects();
        ImGui::Shutdown();
    }

    bool newFrame()
    {
        if (!gFontTexture)
        {
            createDeviceObjects();
        }

        ImGuiIO &io = ImGui::GetIO();

        int width, height, displayWidth, displayHeight;
        glfwGetWindowSize(gWindow, &width, &height);
        glfwGetFramebufferSize(gWindow, &displayWidth, &displayHeight);
        io.DisplaySize = ImVec2((float)width, (float)height);
        io.DisplayFramebufferScale = ImVec2(
            width > 0 ? ((float)displayWidth/width) : 0,
            height > 0 ? ((float)displayHeight/height) : 0
        );

        double currentTime = glfwGetTime();
        io.DeltaTime = 
            gTime > 0.0f ? (float)(currentTime - gTime) : (float)(1.0f/60);
        gTime = currentTime;

        if (glfwGetWindowAttrib(gWindow, GLFW_FOCUSED))
        {
            double mouseX, mouseY;
            glfwGetCursorPos(gWindow, &mouseX, &mouseY);
            io.MousePos = ImVec2((float)mouseX, (float)mouseY);
        }
        else
        {
            io.MousePos = ImVec2(-1, -1);
        }

        for (int i = 0; i < 3; ++i)
        {
            io.MouseDown[i] = gMousePressed[i] 
                || glfwGetMouseButton(gWindow, i) != 0;
            gMousePressed[i] = false;
        }

        io.MouseWheel = gMouseWheel;
        gMouseWheel = 0.0f;

        glfwSetInputMode(gWindow, GLFW_CURSOR, 
            io.MouseDrawCursor ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);

        ImGui::NewFrame();

        return true;
    }

    bool createDeviceObjects()
    {
		const GLchar *vertex_shader =
			"#version 330\n"
			"uniform mat4 ProjMtx;\n"
			"in vec2 Position;\n"
			"in vec2 UV;\n"
			"in vec4 Color;\n"
			"out vec2 Frag_UV;\n"
			"out vec4 Frag_Color;\n"
			"void main()\n"
			"{\n"
			"	Frag_UV = UV;\n"
			"	Frag_Color = Color;\n"
			"	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
			"}\n";

		const GLchar* fragment_shader =
			"#version 330\n"
			"uniform sampler2D Texture;\n"
			"in vec2 Frag_UV;\n"
			"in vec4 Frag_Color;\n"
			"out vec4 Out_Color;\n"
			"void main()\n"
			"{\n"
			"	Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
			"}\n"; 

		gShaderProgram = glCreateProgram();
	    GLuint vert, frag;
        vert = glCreateShader(GL_VERTEX_SHADER);
        frag = glCreateShader(GL_FRAGMENT_SHADER);

        glShaderSource(vert, 1, &vertex_shader, 0);
        glShaderSource(frag, 1, &fragment_shader, 0);
        glCompileShader(vert);
        glCompileShader(frag);

        glAttachShader(gShaderProgram, vert);
        glAttachShader(gShaderProgram, frag);

        glLinkProgram(gShaderProgram);

        gShaderLocTexture = glGetUniformLocation(gShaderProgram, "Texture");
        gShaderLocProjMtx = glGetUniformLocation(gShaderProgram, "ProjMtx");
        gShaderLocPosition = glGetAttribLocation(gShaderProgram, "Position");
        gShaderLocUV = glGetAttribLocation(gShaderProgram, "UV");
        gShaderLocColor = glGetAttribLocation(gShaderProgram, "Color");

        glGenBuffers(1, &gVBO);
        glGenBuffers(1, &gEBO);

        glGenVertexArrays(1, &gVAO);

        glBindVertexArray(gVAO);
        glBindBuffer(GL_ARRAY_BUFFER, gVBO);
        glEnableVertexAttribArray(gShaderLocPosition);
        glEnableVertexAttribArray(gShaderLocUV);
        glEnableVertexAttribArray(gShaderLocColor);

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE*)0)->ELEMENT))
        glVertexAttribPointer(gShaderLocPosition, 2, GL_FLOAT, GL_FALSE,
            sizeof(ImDrawVert), (GLvoid*) OFFSETOF(ImDrawVert, pos));
        glVertexAttribPointer(gShaderLocUV, 2, GL_FLOAT, GL_FALSE,
            sizeof(ImDrawVert), (GLvoid*) OFFSETOF(ImDrawVert, uv));
        glVertexAttribPointer(gShaderLocColor, 4, GL_UNSIGNED_BYTE, GL_TRUE,
            sizeof(ImDrawVert), (GLvoid*) OFFSETOF(ImDrawVert, col));
#undef OFFSETOF

        createFontsTexture();

        return true;
    }

    static void createFontsTexture()
    {
        ImGuiIO &io = ImGui::GetIO();

        unsigned char *pixels;
        int width, height;

        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

        glGenTextures(1, &gFontTexture);
        glBindTexture(GL_TEXTURE_2D, gFontTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
            GL_UNSIGNED_BYTE, pixels);

        io.Fonts->TexID = (void*)(intptr_t) gFontTexture;
    }

    void invalidateDeviceObjects()
    {
        if (gVAO) glDeleteVertexArrays(1, &gVAO);
        if (gVBO) glDeleteBuffers(1, &gVBO);
        if (gEBO) glDeleteBuffers(1, &gEBO);
        gVAO = gVBO = gEBO = 0;

        if (gShaderProgram) glDeleteProgram(gShaderProgram);
        gShaderProgram = 0;

        // todo: cleanup other fields
    }

    void mouseButtonCallback(
        GLFWwindow *window,
        int button,
        int action,
        int mods
    )
    {
        if (action == GLFW_PRESS && button >= 0 & button < 3)
            gMousePressed[button] = true;
    }

    void scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
    {
        gMouseWheel += (float)yoffset;
    }

    void keyCallback(
        GLFWwindow *window,
        int key,
        int scancode,
        int action,
        int mods
    )
    {
        ImGuiIO &io = ImGui::GetIO();
        if (action == GLFW_PRESS)
            io.KeysDown[key] = true;
        if (action == GLFW_RELEASE)
            io.KeysDown[key] = false;

        io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] 
            || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
        io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT]
            || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
        io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT]
            || io.KeysDown[GLFW_KEY_RIGHT_ALT];
        io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER]
            || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
    }

    void charCallback(GLFWwindow *window, unsigned int c)
    {
        ImGuiIO &io = ImGui::GetIO();
        if (c > 0 && c < 0x10000)
            io.AddInputCharacter((unsigned short)c);
    }
}
