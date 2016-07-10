#pragma once
#include <QtWidgets>
#include "screen.h"

class RenderWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
public:
	RenderWidget(QWidget *parent, GBScreen *scr)
        : QOpenGLWidget(parent), screen(scr) {}

protected:
   	void initializeGL()
   	{
   		printf("!\n");
   		initializeOpenGLFunctions();
   		glGenTextures(1, &tex);
   		glBindTexture(GL_TEXTURE_2D, tex);
   		glTexImage2D(GL_TEXTURE_2D, 0, 4, 160, 144, 0, GL_RGBA, GL_UNSIGNED_BYTE, screen->fb);
   		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
   		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
   		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
   		glClear(GL_COLOR_BUFFER_BIT);
   	}

   	void resizeGL(int w, int h)
   	{

   	}

   	void paintGL()
   	{
   		printf("!!\n");
   		initializeOpenGLFunctions();
   		glTexImage2D(GL_TEXTURE_2D, 0, 4, 160, 144, 0, GL_RGBA, GL_UNSIGNED_BYTE, screen->fb);
   		glClear(GL_COLOR_BUFFER_BIT);
   		glBegin(GL_QUADS);
   		glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f); 
   		glTexCoord2f(1.0f, 0.0f); glVertex2f( 1.0f, -1.0f); 
   		glTexCoord2f(1.0f, 1.0f); glVertex2f( 1.0f,  1.0f); 
   		glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, 1.0f); 
   		glEnd();
   	}

private:
	GLuint tex;

	GBScreen *screen;
};