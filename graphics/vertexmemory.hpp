#ifndef VERTEXMEMORY_H
#define VERTEXMEMORY_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <map>

class BufferObject
{
public:
	GLuint val;
	GLenum type;

	BufferObject(){}
	BufferObject(GLenum type)
		:type(type){}
	void generate()
	{
		glGenBuffers(1, &val);
	}

	void bind()
	{
		glBindBuffer(type, val);
	}
	template<typename T>
	void setData(GLuint noElements, T* data, GLenum usage)
	{
		glBufferData(type, noElements * sizeof(T), data, usage);
	}

	template<typename T>
	void updateData(GLintptr offset, GLuint noElements, T* data)
	{
		glBufferSubData(type, offset, noElements * sizeof(T), data);
	}

	template<typename T>
	void setAttPointer(GLuint idx, GLint size, GLenum type, GLuint stride, GLuint offset, GLuint divisor = 0)
	{
		glVertexAttribPointer(idx, size, type, GL_FALSE, stride * sizeof(T), (void*)(offset * sizeof(T)));
		glEnableVertexAttribArray(idx);
		if (divisor > 0)
		{
			glVertexAttribDivisor(idx, divisor);
		}
	}

	void clear()
	{
		glBindBuffer(type, 0);
	}

	void cleanup()
	{
		glDeleteBuffers(1, &val);
	}
};

class ArrayObject
{
public:
	GLuint val;
	std::map<const char*, BufferObject> buffers;

	BufferObject& operator[](const char* key)
	{
		return buffers[key];
	}
	void generate()
	{
		glGenVertexArrays(1, &val);
	}
	
	void bind()
	{
		glBindVertexArray(val);
	}

	void draw(GLenum mode, GLuint first, GLuint count)
	{
		glDrawArrays(mode, first, count);
	}

	void draw(GLenum mode, GLuint count, GLenum type, GLint indices, GLuint instancecount=1)
	{
		glDrawElementsInstanced(mode, count, type, (void*)indices, instancecount);
	}

	void cleanup()
	{
		glDeleteVertexArrays(1, &val);
		for (auto& pair : buffers)
		{
			pair.second.cleanup();
		}
	}

	static void clear()
	{
		glBindVertexArray(0);
	}
};
#endif