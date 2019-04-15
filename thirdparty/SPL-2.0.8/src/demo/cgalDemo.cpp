// Copyright (c) 2011 Michael D. Adams
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

// Read a mesh from standard input in OFF format and print various information
// about the mesh to standard output.

#include <iostream>
#include <string>
#include <CGAL/Cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/IO/Polyhedron_iostream.h>

#include <SPL/cgalUtil.hpp>

typedef double Real;
typedef CGAL::Cartesian<Real> Kernel;
typedef CGAL::Polyhedron_3<Kernel> Polyhedron;
typedef Kernel::Point_3 Point;

int main(int argc, char **argv)
{

	Polyhedron mesh;

	// Read the input mesh from standard input in OFF format.
	if (!(std::cin >> mesh)) {
		std::cerr << "Cannot read input mesh\n";
		return 1;
	}

	// Determine the mesh type.
	std::string meshType;
	if (mesh.is_pure_triangle()) {
		meshType = std::string("triangle");
	} else if (mesh.is_pure_quad()) {
		meshType = std::string("quad");
	} else {
		meshType = std::string("general");
	}

	// Loop over all of the vertices in the mesh.
	// In the process of doing so, compute the minimum,
	// maximum, and average valences of the vertices in the mesh.
	Real valenceSum = 0;
	int minValence = -1;
	int maxValence = -1;
	// For each vertex in the mesh...
	for (Polyhedron::Vertex_const_iterator vertexIter = mesh.vertices_begin();
	  vertexIter != mesh.vertices_end(); ++vertexIter) {

		// Get the valence of the current vertex.
		int valence = vertexIter->degree();

		// Update the minimum valence value.
		if (minValence < 0 || valence < minValence) {
			minValence = valence;
		}

		// Update the maximum valence value.
		if (maxValence < 0 || valence > maxValence) {
			maxValence = valence;
		}

		valenceSum += valence;

	}
	Real meanValence = valenceSum / mesh.size_of_vertices();

	// Check for nonplanar faces.
	int numNonplanarFaces = 0;
	Real degreeSum = 0;
	int minDegree = -1;
	int maxDegree = -1;
	// For each face in the mesh...
	for (Polyhedron::Facet_const_iterator faceIter = mesh.facets_begin();
	  faceIter != mesh.facets_end(); ++faceIter) {

		// Get the degree of the face.
		int degree = faceIter->facet_degree();
		// The face can only be nonplanar if its degree exceeds three.
		if (degree >= 4) {

			// Get a circulator that can be used to visit all of the
			// halfedges around the face in CCW order (where the halfedge
			// has the face on its left side).
			Polyhedron::Facet::Halfedge_around_facet_const_circulator
			  halfEdgeCirc = faceIter->facet_begin();

			// Get the first vertex of the face.
			const Point& v0 = halfEdgeCirc->vertex()->point();
			++halfEdgeCirc;

			// Get the second vertex of the face.
			const Point& v1 = halfEdgeCirc->vertex()->point();
			++halfEdgeCirc;

			// Get the third vertex of the face.
			const Point& v2 = halfEdgeCirc->vertex()->point();
			++halfEdgeCirc;

			// Check that each remaining vertex is coplanar with the first
			// three vertices.
			for (int i = 3; i < degree; ++i) {

				// Get the next vertex of the face.
				const Point& v = halfEdgeCirc->vertex()->point();
				++halfEdgeCirc;

				// Check if the vertex is coplanar with the first three.
				if (!CGAL::coplanar(v0, v1, v2, v)) {
					++numNonplanarFaces;
					std::cout << "nonplanar face detected: "
					  << "(" << v0.x() << "," << v0.y() << "," << v0.z() << ") "
					  << "(" << v1.x() << "," << v1.y() << "," << v1.z() << ") "
					  << "(" << v2.x() << "," << v2.y() << "," << v2.z() << ") "
					  << "(" << v.x() << "," << v.y() << "," << v.z() << ")\n";
				}
			}
		}

		// Update the minimum degree.
		if (minDegree < 0 || degree < minDegree) {
			minDegree = degree;
		}

		// Update the maximum degree.
		if (maxDegree < 0 || degree > maxDegree) {
			maxDegree = degree;
		}

		degreeSum += degree;

	}
	Real meanDegree = degreeSum / mesh.size_of_facets();

	// Output the mesh information.
	std::cout
	  << "mesh type: " << meshType << "\n"
	  << "number of vertices: " << mesh.size_of_vertices() << "\n"
	  << "number of edges: " << mesh.size_of_halfedges() / 2 << "\n"
	  << "number of border edges: " << mesh.size_of_border_edges() << "\n"
	  << "number of faces: " << mesh.size_of_facets() << "\n"
	  << "number of halfedges: " << mesh.size_of_halfedges() << "\n"
	  << "mean vertex valence: " << meanValence << "\n"
	  << "minimum vertex valence: " << minValence << "\n"
	  << "maximum vertex valence: " << maxValence << "\n"
	  << "mean face degree: " << meanDegree << "\n"
	  << "minimum face degree: " << minDegree << "\n"
	  << "maximum face degree: " << maxDegree << "\n"
	  << "number of nonplanar faces: " << numNonplanarFaces << "\n"
	  ;

	return 0;
}
