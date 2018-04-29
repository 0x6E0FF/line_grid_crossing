#include "GL/gl.h"
#include <stdio.h>
#include <math.h>

#define GRID_SIZE 20.0f

extern int WIDTH;
extern int HEIGHT;

typedef struct
{
	float x;
	float y;
}
Point;

static int currentPoint = 0;
static Point line[2] = { {1.2f, 1.3f}, {5.1f, 1.5f} };
static int print = 0;
static void draw_line_crossing()
{
	Point p1 = line[currentPoint];
	Point p2 = line[(currentPoint+1)%2];
	Point p3;
			
	if (print == 1)
	{
		printf(" * init : %5.3f %5.3f -> %5.3f %5.3f\n", p1.x, p1.y, p2.x, p2.y);
	}
	float dx = p2.x - p1.x;
	float dy = p2.y - p1.y;

	int next_x = (int)(p1.x + ((dx < 0) ? 0.0 : 1.0));
	int next_y = (int)(p1.y + ((dy < 0) ? 0.0 : 1.0));
	int index = 0;
	while ( ((dx < 0 && p1.x > p2.x) || (dx >= 0 && p1.x < p2.x))
       	 || ((dy < 0 && p1.y > p2.y) || (dy >= 0 && p1.y < p2.y)))
	{
		if (dx == 0.0)
		{
			p3.x = p1.x;
			p3.y = next_y;
			next_y += (dy < 0) ? -1 : 1;
		}
		else if (dy == 0.0)
		{
			p3.x = next_x;
			p3.y = p1.y;
			next_x += (dx < 0) ? -1 : 1;
		}
		else 
		{
			/* general case : compute next intersection of line with the grid */
			
			/* first, try fixing x and compute y with line equation : */
			/*            dy
			 *  y - y1 = ---- . (x - x1)
			 *            dx 
			 */
			p3.x = next_x;
			p3.y = (dy / dx) * (p3.x - p1.x) + p1.y;

			/* if computed point doesn't lie on current cell border, retry with fixed y */
			if ((dy <  0 && p3.y <  next_y)
			 || (dy >= 0 && p3.y >= next_y))
			{
				p3.y = next_y;
				p3.x = (dx / dy) * (p3.y - p1.y) + p1.x;
				
				next_y += (dy < 0) ? -1 : 1;
			}
			else
			{
				next_x += (dx < 0) ? -1 : 1;
			}
		}
		
		/* draw the current cell as filled square */
		int x1 = (int)p1.x;
		int y1 = (int)p1.y;
		int x2,y2;
		
		if (dx < 0 && (float)x1 == p1.x)
			x1--;
		if (dy < 0 && (float)y1 == p1.y)
			y1--;
		x2 = x1 + 1;
		y2 = y1 + 1;
		glColor4f(0,0.3,0.8,0.5);
		glBegin(GL_TRIANGLE_FAN);
			glVertex2f(x1, y1);
			glVertex2f(x1, y2);
			glVertex2f(x2, y2);
			glVertex2f(x2, y1);
		glEnd();
		
		/* draw a yellow cross for computed intersection */
		glColor4f(0.5,0.5,0.2,1);
		glLineWidth(1);
		glBegin(GL_LINES);
			glVertex2f(p3.x - 0.2, p3.y);
			glVertex2f(p3.x + 0.2, p3.y);
			glVertex2f(p3.x, p3.y - 0.2);
			glVertex2f(p3.x, p3.y + 0.2);
		glEnd();
		
		
		p1 = p3;
		if (print == 1)
		{
			printf(" %05d : %5.3f %5.3f -> %5.3f %5.3f\n", index++, p1.x, p1.y, p2.x, p2.y);
		}
	}
	print = 0;
	
	/* draw original line */
	glEnable(GL_LINE_SMOOTH);
	glColor4f(0.1,0.5,0.2,1);
	glLineWidth(2);
	glBegin(GL_LINES);
		glVertex2f(line[0].x, line[0].y);
		glVertex2f(line[1].x, line[1].y);
	glEnd();
}

static void dumpErrors()
{
	GLenum err = GL_NO_ERROR;
	while((err = glGetError()) != GL_NO_ERROR)
	{
	  fprintf(stderr, "GL ERROR %x\n", err);
	}
}

GLvoid resize(GLsizei w, GLsizei h)
{
	
}


GLvoid initializeGL(GLsizei w, GLsizei h)
{	
	dumpErrors();
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0,0, WIDTH, HEIGHT);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, GRID_SIZE, 0, GRID_SIZE, -350, 350);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

GLvoid drawScene(GLvoid)
{
	int i;
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	/* GRID */
	glDisable(GL_LINE_SMOOTH);
	glColor4f(0,0.3,0.8,1);
	glLineWidth(1);
	glBegin(GL_LINES);
	for (i = 0; i < GRID_SIZE; i++)
	{
		glVertex2f(i, 0);
		glVertex2f(i, GRID_SIZE);
		
		glVertex2f(0, i);
		glVertex2f(GRID_SIZE, i);
	}
	glEnd();
	
	draw_line_crossing();
}

void key_pressed(char c)
{
}

void mouseClick(int x, int y)
{
	line[currentPoint].x = (float)x * GRID_SIZE / (float)WIDTH;
	line[currentPoint].y = (float)(HEIGHT - y) * GRID_SIZE / (float)HEIGHT;
	
	currentPoint = (currentPoint + 1) % 2;
	print = 1;
}
