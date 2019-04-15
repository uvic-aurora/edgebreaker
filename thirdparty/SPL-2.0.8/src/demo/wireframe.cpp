// Copyright (c) 2011, 2012, 2013 Michael D. Adams
// All rights reserved.

// __START_OF_LICENSE__
// 
// Copyright (c) 2015 Michael D. Adams
// All rights reserved.
// 
// This file is part of the Signal Processing Library (SPL).
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 3,
// or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with this program; see the file LICENSE.  If not,
// see <http://www.gnu.org/licenses/>.
// 
// __END_OF_LICENSE__

// A simple 3-D wireframe mesh viewer that uses both OpenGL and CGAL.

////////////////////////////////////////////////////////////////////////////////
// Header files
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <sstream>
#include <string>
#include <CGAL/Cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/IO/Polyhedron_iostream.h>
#include <CGAL/Bbox_3.h>
#include <SPL/cgalUtil.hpp>
#include <SPL/math.hpp>
#include <SPL/Arcball.hpp>
#if defined(SPL_HAVE_GL_GLUT_H)
#include <GL/glut.h>
#else
#include <glut.h>
#endif

using SPL::norm;
using SPL::radToDeg;
using SPL::degToRad;
using SPL::normalize;
using SPL::angleBetweenVectors;

////////////////////////////////////////////////////////////////////////////////
// Types
////////////////////////////////////////////////////////////////////////////////

// Basic types.
typedef double Real;
typedef CGAL::Cartesian<Real> Kernel;
typedef Kernel::Point_3 Point3;
typedef Kernel::Point_2 Point2;
typedef Kernel::Vector_3 Vector3;
typedef CGAL::Polyhedron_3<Kernel> Polyhedron;
typedef CGAL::Bbox_3 Bbox_3;
typedef SPL::Arcball<Kernel> ArcBall;
typedef SPL::Rotation_3<Kernel> Rotation3;

// The global state information for the program.
// In order to avoid a proliferation of global variables, all global
// state information for the program is kept in this structure.
struct Info
{
	//static constexpr Real sphereScale = 0.1;
	const Real sphereScale = 0.1;
	Info() : eyePos(0, 0, 10), sceneCenter(0, 0, 0), eyeUpDir(0, 1, 0),
	  scale(1.0), rot(Vector3(0, 0, 1), 0), trans(0, 0, 0), mode(0),
	  rotateMethod(0), arcBallRadius(1.0), displayArcBall(true),
	  displayAxes(false) {}
	Polyhedron mesh;  // The polyhedral mesh.
	Bbox_3 boundBox;  // The bounding box for the mesh.
	Real smallEdge;  // The length of the shortest edge in the mesh.
	int viewportWidth;  // The viewport width.
	int viewportHeight;  // The viewport height.
	Point3 eyePos;  // The eye position.
	Point3 sceneCenter;  // The center of the scene.
	Vector3 eyeUpDir;  // The eye's up direction.
	Real scale;  // The scaling used in drawing the mesh object.
	Rotation3 rot;  // The rotation used in drawing the mesh object.
	Vector3 trans;  // The translation used in drawing the mesh object.
	int mode;  // The mode for mouse-based operations.
	// The following modes are valid:
	static const int rotateMode = 1;
	static const int translateMode = 2;
	static const int scaleMode = 4;
	int rotateMethod; // The rotation method.
	Real arcBallRadius; // The arcball radius (for rotation).
	Point2 mousePos;  // The window coordinates of the last mouse event.
	bool displayArcBall; // Display the arcball?
	bool displayAxes; // Display the axes for the world frame of reference?
	ArcBall arcBall; // The arcball used for rotation.
};

////////////////////////////////////////////////////////////////////////////////
// OpenGL utility functions
////////////////////////////////////////////////////////////////////////////////

// Convert from window coordinates to world coordinates.
// If no viewing matrix is provided, the current modelview matrix is
// assumed to be the viewing matrix.
// Optionally, the depth buffer information may be used to add a
// z dimension to the window coordinates before conversion.
Point3 winToWorld(const Point2& p, bool useDepth = false,
  const GLdouble* viewMatrix = 0)
{
	GLint viewport[4];
	GLdouble projMatrix[16];
	GLdouble modelviewMatrix[16];

	// Get the current viewport matrix.
	glGetIntegerv(GL_VIEWPORT, viewport);

	// Get the current projection matrix.
	glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);

	// Get the current modelview matrix (if needed).
	if (!viewMatrix) {
		glGetDoublev(GL_MODELVIEW_MATRIX, modelviewMatrix);
	}

	GLdouble worldX;
	GLdouble worldY;
	GLdouble worldZ;
	GLdouble z;

	// Initialize the z component of the window coordinates.
	if (useDepth) {
		// Use the depth buffer information for the z value.
		glReadPixels(p.x(), p.y(), 1, 1, GL_DEPTH_COMPONENT, GL_DOUBLE, &z);
	} else {
		z = 0;
	}

	// Convert from window coordinates to object/world coordinates.
	gluUnProject(p.x(), p.y(), z, viewMatrix ? viewMatrix : modelviewMatrix,
	  projMatrix, viewport, &worldX, &worldY, &worldZ);

	return Point3(worldX, worldY, worldZ);
}

// Draw a circle in the xy-plane centered at the origin and with unit radius.
void drawCircle()
{
	glBegin(GL_LINE_LOOP);
	const Real twoPi = 2.0 * M_PI;
	const Real angleInc = degToRad(1.0);
	for (Real angle = 0.0; angle < twoPi; angle += angleInc) {
		glVertex3f(cos(angle), sin(angle), 0.0);
	}
	glEnd();
}

// Draw the x, y, and z axes.
void drawAxes()
{
	glLineWidth(4.0);
	glBegin(GL_LINES);

	// Draw the positive x axis (in bright red).
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(1.0, 0.0, 0.0);

	// Draw the negative x axis (in dark red).
	glColor3f(0.5, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(-1.0, 0.0, 0.0);

	// Draw the positive y axis (in bright green).
	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 1.0, 0.0);

	// Draw the negative y axis (in dark green).
	glColor3f(0.0, 0.5, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, -1.0, 0.0);

	// Draw the positive z axis (in bright blue).
	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 1.0);

	// Draw the negative z axis (in dark blue).
	glColor3f(0.0, 0.0, 0.5);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, -1.0);

	glEnd();
}

// Draw the arc ball.
void drawArcBall()
{
	glLineWidth(1.0);

	// Draw a circle in the yz-plane.
	glPushMatrix();
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glColor3f(1.0, 0.0, 0.0);
	drawCircle();
	glPopMatrix();

	// Draw a circle in the xz-plane.
	glPushMatrix();
	glRotatef(90.0, 0.0, 1.0, 0.0);
	glColor3f(0.0, 1.0, 0.0);
	drawCircle();
	glPopMatrix();

	// Draw a circle in the xy-plane.
	glPushMatrix();
	glRotatef(90.0, 0.0, 0.0, 1.0);
	glColor3f(0.0, 0.0, 1.0);
	drawCircle();
	glPopMatrix();
}

// Draw a sphere.
void drawSphere()
{
	glutSolidSphere(1.0, 32, 32);
}

// Draw a rectangular prism with the specified center of gravity, axis,
// axis length, and "radius".
void drawRectPrism(const Point3& center, Real length, Real radius,
  const Vector3& axis)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	Vector3 dir = axis / norm(axis);
	Real theta = angleBetweenVectors(dir, Vector3(0.0, 0.0, 1.0));
	glTranslatef(center.x(), center.y(), center.z());
	if (fabs(theta) > 1e-6) {
		Vector3 rotAxis = CGAL::cross_product(Vector3(0.0, 0.0, 1.0), dir);
		glRotatef(radToDeg(theta), rotAxis.x(), rotAxis.y(), rotAxis.z());
	}
	glScalef(radius, radius, length);
	glutSolidCube(1.0);

	glPopMatrix();
}

////////////////////////////////////////////////////////////////////////////////
// Global data
////////////////////////////////////////////////////////////////////////////////

// The default built-in mesh (in OFF format).
const std::string defaultMesh(
	"OFF\n"
	"5 4 0\n"
	"-1 -1  0\n"
	" 1 -1  0\n"
	" 1  1  0\n"
	"-1  1  0\n"
	" 0  0  1.5\n"
	"3 0 1 4\n"
	"3 1 2 4\n"
	"3 2 3 4\n"
	"3 0 4 3\n"
);

// The global state information for the program.
Info info;

////////////////////////////////////////////////////////////////////////////////
// Transformation functions
////////////////////////////////////////////////////////////////////////////////

// Perform scaling.
void scale(const Point2& pos)
{
	const Real minScale = 1e-6;

	// Calculate the amount by which to scale.
	Real refDist = (1.0 / sqrt(2.0)) * norm(winToWorld(Point2(
	  info.viewportWidth - 1, info.viewportHeight - 1)) -
	  winToWorld(Point2(0, 0)));
	Vector3 upDir = normalize(info.eyeUpDir);
	Vector3 delta = winToWorld(pos) - winToWorld(info.mousePos);
	Real dist = norm((upDir * delta) * upDir);
	Real scale;
	if (upDir * delta > 0) {
		scale = 1.0 + 2.0 * dist / refDist;
	} else if (upDir * delta < 0) {
		scale = 1.0 / (1.0 + 2.0 * dist / refDist);
	} else {
		scale = 1.0;
	}
	scale = std::max(scale, minScale / info.scale);

	// Apply the scaling.
	info.scale *= scale;

	// Update the most recent mouse position.
	info.mousePos = pos;

	// Redraw the window with the new scaling applied.
	glutPostRedisplay();
}

// Perform rotation.
void rotate(const Point2& pos)
{
	// Select the arcball rotation mode.
	info.arcBall.setMode(info.rotateMethod);

	// Specify the starting position for the arcball movement.
	info.arcBall.start(winToWorld(info.mousePos));

	// Specify the current position for the arcball movement.
	info.arcBall.move(winToWorld(pos));

	// Add the arcball rotation to the current rotation.
	info.rot = ArcBall::combineRotations(info.rot, info.arcBall.getRotation());

	// Update the most recent mouse position.
	info.mousePos = pos;

	// Redraw the window with the new rotation applied.
	glutPostRedisplay();
}

// Perform translation.
void translate(const Point2& pos)
{
	// Calculate the amount by which to translate.
	Real refDist = (1.0 / sqrt(2.0)) * norm(winToWorld(Point2(
	  info.viewportWidth - 1, info.viewportHeight - 1)) -
	  winToWorld(Point2(0, 0)));
	Vector3 upDir = normalize(info.eyeUpDir);
	Vector3 rightDir = normalize(CGAL::cross_product(info.sceneCenter -
	  info.eyePos, info.eyeUpDir));
	Vector3 delta = winToWorld(pos) - winToWorld(info.mousePos);
	Vector3 trans = (2.0 / refDist) * ((upDir * delta) * upDir + (rightDir *
	  delta) * rightDir);

	// Apply the translation.
	info.trans = info.trans + trans;

	// Update the most recent mouse position.
	info.mousePos = pos;

	// Redraw the window with the new translation applied.
	glutPostRedisplay();
}

////////////////////////////////////////////////////////////////////////////////
// GLUT callback functions
////////////////////////////////////////////////////////////////////////////////

// The window display callback function.
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	if (info.displayAxes) {
		// Draw axes for world frame of reference.
		glDisable(GL_LIGHTING);
		drawAxes();
		glEnable(GL_LIGHTING);
	}

	if (info.displayArcBall) {
		glDisable(GL_LIGHTING);
		glPushMatrix();
		if (!info.rotateMethod) {
			// Draw three circles as a visual cue for rotation.
			glTranslatef(info.sceneCenter.x(), info.sceneCenter.y(),
			  info.sceneCenter.z());
			glRotatef(radToDeg(info.rot.angle), info.rot.axis.x(),
			  info.rot.axis.y(), info.rot.axis.z());
			glScalef(info.arcBallRadius, info.arcBallRadius,
			  info.arcBallRadius);
			drawArcBall();
		} else {
			// Draw single circle as a visual cue for rotation.
			glTranslatef(info.sceneCenter.x(), info.sceneCenter.y(),
			  info.sceneCenter.z());
			glColor3f(0.5, 0.5, 0.5);
			glLineWidth(1.0);
			drawCircle();
		}
		glPopMatrix();
		glEnable(GL_LIGHTING);
	}

	glPushMatrix();

	// Establish the frame of reference (position and orientation)
	// for the mesh object.
	glTranslatef(info.trans.x(), info.trans.y(), info.trans.z());
	glRotatef(radToDeg(info.rot.angle), info.rot.axis.x(),
	  info.rot.axis.y(), info.rot.axis.z());
	glScalef(info.scale, info.scale, info.scale);

	// Draw axes showing the frame of reference for the mesh object.
	glDisable(GL_LIGHTING);
	glPushMatrix();
	glScalef(100.0, 100.0, 100.0);
	drawAxes();
	glPopMatrix();
	glEnable(GL_LIGHTING);

	// For each vertex in the mesh...
	for (Polyhedron::Vertex_const_iterator vertexIter =
	  info.mesh.vertices_begin(); vertexIter != info.mesh.vertices_end();
	  ++vertexIter) {
		// Draw a sphere at the vertex position.
		Point3 v = vertexIter->point();
		glPushMatrix();
		glTranslatef(v.x(), v.y(), v.z());
		glScalef(info.sphereScale * info.smallEdge, info.sphereScale *
		  info.smallEdge, info.sphereScale * info.smallEdge);
		drawSphere();
		glPopMatrix();
	}

	// For each edge in the mesh...
	for (Polyhedron::Edge_const_iterator edgeIter = info.mesh.edges_begin();
	  edgeIter != info.mesh.edges_end(); ++edgeIter) {
		// Draw a rectangular prism along the extent of the edge.
		Point3 v0 = edgeIter->vertex()->point();
		Point3 v1 = edgeIter->opposite()->vertex()->point();
		Point3 midpoint = CGAL::midpoint(v0, v1);
		Vector3 axis = v1 - v0;
		drawRectPrism(midpoint, norm(axis), 0.25 * info.sphereScale *
		  info.smallEdge, axis);
	}

	glPopMatrix();
	glutSwapBuffers();
}

// The window reshape callback function.
void reshape(GLint width, GLint height)
{
	info.viewportWidth = width;
	info.viewportHeight = height;

	// Compute the aspect ratio being careful to avoid the possibility of
	// division by zero (when the window width is zero).
	GLfloat aspectRatio = static_cast<GLfloat>(width) /
	  ((height) ? height : 1.0);

	// Set the viewport to the entire window.
	glViewport(0, 0, width, height);

	// Establish a perspective projection.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, aspectRatio, 1.0, 1000.0);

	// Establish a viewing transform.
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(info.eyePos.x(), info.eyePos.y(), info.eyePos.z(),
	  info.sceneCenter.x(), info.sceneCenter.y(), info.sceneCenter.z(),
	  info.eyeUpDir.x(), info.eyeUpDir.y(), info.eyeUpDir.z());
	info.arcBall.initialize(info.arcBallRadius, info.eyePos,
	  info.sceneCenter - info.eyePos, info.eyeUpDir, info.sceneCenter);
}

// The keyboard callback function.
void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'z':
		info.trans = info.trans + Vector3(0, 0, 0.5);
		glutPostRedisplay();
		break;
	case 'r':
		// Change the rotation method.
		info.rotateMethod ^= 1;
		glutPostRedisplay();
		break;
	case 'a':
		info.displayAxes ^= 1;
		glutPostRedisplay();
		break;
	case 'b':
		info.displayArcBall ^= 1;
		glutPostRedisplay();
		break;
	case 'q':
		// Quit the program.
		exit(0);
		break;
	}
}

// The mouse callback function.
// This function is called for each press and each release of a mouse button.
void mouse(int button, int state, int x, int y)
{
	// Compute the mouse position with the origin at the bottom left of the
	// window.
	Point2 mousePos(x, info.viewportHeight - 1 - y);

	switch (button) {
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN) {
			// Enter rotation mode and save the initial mouse position.
			info.mode = Info::rotateMode;
			info.mousePos = mousePos;
		} else if (state == GLUT_UP) {
			// Perform any remaining rotation and exit rotation mode.
			rotate(mousePos);
			info.mode = 0;
		}
		glutPostRedisplay();
		break;
	case GLUT_MIDDLE_BUTTON:
		if (state == GLUT_DOWN) {
			// Enter scaling mode and save the initial mouse position.
			info.mode = Info::scaleMode;
			info.mousePos = mousePos;
		} else if (state == GLUT_UP) {
			// Perform any remaining scaling and exit scaling mode.
			scale(mousePos);
			info.mode = 0;
		}
		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN) {
			// Enter translation mode and save the initial mouse position.
			info.mode = Info::translateMode;
			info.mousePos = mousePos;
		} else if (state == GLUT_UP) {
			// Perform any remaining translation and exit translation mode.
			translate(mousePos);
			info.mode = 0;
		}
		break;
	default:
		info.mode = 0;
		break;
	}
}

// The motion callback function.
// This function is called when the mouse moves within the window while
// one or more mouse buttons are pressed.
void motion(int x, int y)
{
	// Compute the mouse position with the origin at the bottom left of the
	// window.
	Point2 mousePos(x, info.viewportHeight - 1 - y);

	// Perform the appropriate processing for the current transformation mode.
	switch (info.mode) {
	case Info::rotateMode:
		// We are in rotation mode.  Perform the necessary rotation.
		rotate(mousePos);
		break;
	case Info::scaleMode:
		// We are in scale mode.  Perform the necessary scaling.
		scale(mousePos);
		break;
	case Info::translateMode:
		// We are in translation mode.  Perform the necessary translation.
		translate(mousePos);
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////
// Main program
////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
	const int winWidth = 1024;
	const int winHeight = 1024;

	// Initialize the GLUT library.
	glutInit(&argc, argv);

	// Decide whether to use the built-in mesh, based on command-line arguments.
	bool useBuiltinMesh = (argc <= 1);

	// Load the mesh.
	if (!useBuiltinMesh) {
		// Read the mesh in OFF format from standard input.
		if (!(std::cin >> info.mesh)) {
			std::cerr << "cannot read mesh from standard input\n";
			exit(1);
		}
	} else {
		// Use the default built-in mesh.
		std::stringstream inStream(defaultMesh);
		inStream >> info.mesh;
	}

	// Compute the bounding box of the mesh.
	if (info.mesh.size_of_vertices() > 0) {
		Point3 v = info.mesh.vertices_begin()->point();
		info.boundBox = Bbox_3(v.x(), v.y(), v.z(), v.x(), v.y(), v.z());
	} else {
		info.boundBox = Bbox_3(0, 0, 0, 0, 0, 0);
	}
	for (Polyhedron::Vertex_const_iterator vertexIter =
	  info.mesh.vertices_begin(); vertexIter != info.mesh.vertices_end();
	  ++vertexIter) {
		const Point3& v = vertexIter->point();
		info.boundBox = info.boundBox + Bbox_3(v.x(), v.y(), v.z(), v.x(),
		  v.y(), v.z());
	}

	// Compute the length of the shortest edge in the mesh.
	info.smallEdge = -1.0;
	for (Polyhedron::Edge_const_iterator edgeIter = info.mesh.edges_begin();
	  edgeIter != info.mesh.edges_end(); ++edgeIter) {
		Point3 v0 = edgeIter->vertex()->point();
		Point3 v1 = edgeIter->opposite()->vertex()->point();
		Real length = norm(v1 - v0);
		if (info.smallEdge < 0.0 || length < info.smallEdge) {
			info.smallEdge = length;
		}
	}

	// Print some information about the mesh.
	std::cout << "bounding box: "
	  << "[" << info.boundBox.xmin() << "," << info.boundBox.xmax() << "] x "
	  << "[" << info.boundBox.ymin() << "," << info.boundBox.ymax() << "] x "
	  << "[" << info.boundBox.zmin() << "," << info.boundBox.zmax() << "]"
	  << "\n";
	std::cout << "length of shortest edge: " << info.smallEdge << "\n";

	// Initialize the display mode and create a graphics window.
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(winWidth, winHeight);
	glutCreateWindow(argv[0]);

	// Register callback functions for event handling.
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	// Initialize some additional OpenGL state.
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHT0);
	glClearColor(0.0, 0.0, 0.0, 0.0);

	// Enter main event-processing loop in GLUT.
	glutMainLoop();

	// This line is never reached.
	return 0;
}
