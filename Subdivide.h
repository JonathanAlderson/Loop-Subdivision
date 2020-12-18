///////////////////////////////////////////////////
//
//  Jonathan Alderson
//  November, 2020
//
//  ------------------------
//  Subdivide.h
//  ------------------------
//
// Allows subdivision of a Directed Edge Surface
//
///////////////////////////////////////////////////

// include guard for DirectedEdgeSurface
#ifndef _SUBDIVIDE_H
#define _SUBDIVIDE_H

// needed as we are manipulating these objects
#include "DirectedEdgeSurface.h"


// include the C++ standard libraries we want
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <math.h>
#include <utility>
#include <array>

class DirectedEdgeSurface; // forward declaration

class Subdivide
{
private:

  // how many vertices before subdivision
  unsigned int nVerticesOld;

  // how many faces before subdivison
  unsigned int nFacesOld;

  // how many edges before subdivision
  unsigned int nEdgesOld;

  // keeps track of the vertex added at the midpoint of edges
  std::vector<unsigned int> edgeNewVertex;

  // keeps track of the new edge positions
  std::vector<Cartesian3> newVertexPositions;

  // keeps track of where the new other halfs go
  std::vector<unsigned int> firstHalfs;
  std::vector<unsigned int> secondHalfs;

public:
  Subdivide(DirectedEdgeSurface *desIn);

  // surface we are operating on
  DirectedEdgeSurface *des;

  // Calls all the sub functions in sequence
  void StartSubdivision();

  // Creates a new vertex on every edge
  void InsertEdgeVertices();

  // Finds the new position of the edge vertex with weighting
  Cartesian3 CalculateEdgeVertexPosition(unsigned int e);

  // Finds the new normal of the edge vertex with weighting
  Cartesian3 CalculateEdgeVertexNormal(unsigned int e);

  // Recomputes face vertices vector
  void CalculateFaces();

  // Computes the new other halfs
  void CalculateOtherHalfs();

  // Updates Positions for New Edge Vertices
  void SmoothEdgeVertices();

  // Finds the weights needed for a given one ring size
  float FindAlpha(int n);

  // Updates Positions for all other Vertrices
  void SmoothVertices();

  // applys the positions we found at the start
  void ApplyVertexPositions();

  // find the one ring of a vertex
  std::vector<unsigned int> OneRing(unsigned int v);



};

#endif //Subdivide.h
