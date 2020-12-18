///////////////////////////////////////////////////
//
//  Jonathan Alderson
//  November, 2020
//
//  ------------------------
//  Subdivide.cpp
//  ------------------------
//
// Allows subdivision of a Directed Edge Surface
//
///////////////////////////////////////////////////

#include "Subdivide.h"

// Subdivision operators for a surface
Subdivide::Subdivide(DirectedEdgeSurface *desIn)
{
  des = desIn;
}

  // Calls all the sub functions in sequence
void Subdivide::StartSubdivision()
{
  SmoothVertices();
  InsertEdgeVertices();
  CalculateFaces();
  CalculateOtherHalfs();
  ApplyVertexPositions();
}

// Creates a new vertex on every edge
void Subdivide::InsertEdgeVertices()
{
  // iterate over all other halfs
  // if smallest of pair, create new edgeVertex
  nVerticesOld = des->vertices.size(); // size before adding new edge vertices
  des->vertices.resize(nVerticesOld + des->otherHalf.size() / 2); // make enough room
  des->normals.resize(nVerticesOld + des->otherHalf.size() / 2); // make enough room

  // keeps track for every edge which new edge vetex they have
  edgeNewVertex.clear();
  edgeNewVertex.resize(des->otherHalf.size());
  int edgeVertexCount = 0;

  for(unsigned int e = 0; e < des->otherHalf.size(); e++)
  { // so we only create one new Vertex per edge
    if(e < des->otherHalf[e] )
    {
       // now compute the position/normal through weights
       des->vertices[nVerticesOld + edgeVertexCount] = CalculateEdgeVertexPosition(e);
       des->normals[nVerticesOld + edgeVertexCount] = CalculateEdgeVertexNormal(e);

       // also save which edge relates to which new edge vertex
       edgeNewVertex[e]                 = edgeVertexCount + nVerticesOld;
       edgeNewVertex[des->otherHalf[e]] = edgeVertexCount + nVerticesOld;

       // increment
       edgeVertexCount++;
    }
  }
}

// Finds the new position of the edge vertex with weighting
Cartesian3 Subdivide::CalculateEdgeVertexPosition(unsigned int e)
{
  Cartesian3 v0;  // next vertex
  Cartesian3 v1;  // prev vertex
  Cartesian3 v2;  // adjacent vertex
  Cartesian3 v3;  // adjacent vertex 2

  // other half
  unsigned int oH = des->otherHalf[e];

  // adjacent edges
  unsigned int aE0 = (3 * (e / 3)) + (e + 1) % 3;
  unsigned int aE1 = (3 * (oH / 3)) + (oH + 1) % 3;

  // find the locations of these
  v0 = des->vertices[des->faceVertices[e]];
  v1 = des->vertices[des->faceVertices[oH]];

  v2 = des->vertices[des->faceVertices[aE0]];
  v3 = des->vertices[des->faceVertices[aE1]];

  // take the weighted average
  Cartesian3 weightedAverage = (3. / 8.) * (v0 + v1)
                             + (1. / 8.) * (v2  + v3);

  // return value
  return weightedAverage;
}

// Finds the new normal of the edge vertex with weighting
Cartesian3 Subdivide::CalculateEdgeVertexNormal(unsigned int e)
{
  Cartesian3 n0;  // next normal
  Cartesian3 n1;  // prev normal
  Cartesian3 n2;  // adjacent normal
  Cartesian3 n3;  // adjacent normal

  // other half
  unsigned int oH = des->otherHalf[e];

  // adjacent edges
  unsigned int aE0 = (3 * (e / 3)) + (e + 1) % 3;
  unsigned int aE1 = (3 * (oH / 3)) + (oH + 1) % 3;

   // find the normals of these
   n0 = des->normals[des->faceVertices[e]];
   n1 = des->normals[des->faceVertices[oH]];

   n2 = des->normals[des->faceVertices[aE0]];
   n3 = des->normals[des->faceVertices[aE1]];

  // also find the normal in the same way
  Cartesian3 weightedNormal = (3. / 8.) * (n0 + n1)
                            + (1. / 8.) * (n2  + n3);

  weightedNormal = weightedNormal.unit();

  // return value
  return weightedNormal;
}

// Recomputes face vertices vector
void Subdivide::CalculateFaces()
{
  // we will have 4x this many after adding the new vertexes
  int nFaceVerts = des->faceVertices.size();

  // save how many faces there were
  nFacesOld = nFaceVerts / 3;

  // save how many edges there are
  nEdgesOld = nFaceVerts;

  // create a new vector for all the new face vertices and resize
  std::vector<unsigned int> newFaceVertices;
  newFaceVertices.resize(4 * nFaceVerts);


  // init  vectors to track how our edges are split up
  firstHalfs.clear();
  secondHalfs.clear();
  firstHalfs.resize(nEdgesOld);
  secondHalfs.resize(nEdgesOld);

  // this is so we don't have to have nested for loops
  // in order to update our firstDirectedEdges
  // we can just increment instead of searching
  std::vector<unsigned int> newFDE;

  newFDE.resize(des->firstDirectedEdge.size() + (des->otherHalf.size() / 2.));

  //std::cout << "Size: " << newFDE.size() << '\n';
  // set all to -1, to compare later
  for(unsigned int i = 0; i < newFDE.size(); i++)
  {
    newFDE[i] = -1;
  }

  // vertexes that have just been created inside the face
  unsigned int ve0;
  unsigned int ve1;
  unsigned int ve2;

  // vertexes that initially belonged to the face
  unsigned int fv0;
  unsigned int fv1;
  unsigned int fv2;
  // we again use nVerticesOld to caluclate indexes without new edge vertices
  for(int f = 0; f < (nFaceVerts / 3); f++)
  {
    // recall the new vertex edges assosiated with this face
    ve0 = edgeNewVertex[f * 3    ];
    ve1 = edgeNewVertex[f * 3 + 1];
    ve2 = edgeNewVertex[f * 3 + 2];

    // recall the origional vertexes assosiated with this face
    fv0 = des->faceVertices[f * 3    ];
    fv1 = des->faceVertices[f * 3 + 1];
    fv2 = des->faceVertices[f * 3 + 2];

    // first face];
    // comprised of new edge vertexes in face
    // ve0, ve1, ve2
    newFaceVertices[(0. * nFaceVerts) + 3 * f    ] = ve0;
    newFaceVertices[(0. * nFaceVerts) + 3 * f + 1] = ve1;
    newFaceVertices[(0. * nFaceVerts) + 3 * f + 2] = ve2;

    // second face
    // comprised of one old vert and two new vertedges
    // fv0, ve1, ve0
    newFaceVertices[(1. * nFaceVerts) + 3 * f]     = fv0;
    newFaceVertices[(1. * nFaceVerts) + 3 * f + 1] = ve1;
    newFaceVertices[(1. * nFaceVerts) + 3 * f + 2] = ve0;

    // third face
    // comprised of one old vert and two new vertedges
    // fv1, ve2, ve1
    newFaceVertices[(2. * nFaceVerts) + 3 * f]     = fv1;
    newFaceVertices[(2. * nFaceVerts) + 3 * f + 1] = ve2;
    newFaceVertices[(2. * nFaceVerts) + 3 * f + 2] = ve1;

    // fourth face
    // comprised of old old verte and two new vertedges
    // fv2, ve0, ve2
    newFaceVertices[(3. * nFaceVerts) + 3 * f]     = fv2;
    newFaceVertices[(3. * nFaceVerts) + 3 * f + 1] = ve0;
    newFaceVertices[(3. * nFaceVerts) + 3 * f + 2] = ve2;

    // also fill in the first/second halfs
    // so we can reconstruct the other halfs correctly later
    secondHalfs[ 3 * f     ] = (1. * nFaceVerts) + 3 * f;
    secondHalfs[(3 * f) + 1] = (2. * nFaceVerts) + 3 * f;
    secondHalfs[(3 * f) + 2] = (3. * nFaceVerts) + 3 * f;

    firstHalfs[ 3 * f     ]  = (3. * nFaceVerts) + 3 * f + 1;
    firstHalfs[(3 * f) + 1]  = (1. * nFaceVerts) + 3 * f + 1;
    firstHalfs[(3 * f) + 2]  = (2. * nFaceVerts) + 3 * f + 1;


    // now update first directed edge for existing verticies
    for(unsigned int i = 0; i < des->firstDirectedEdge.size(); i++)
    {
      if((unsigned int)( 3 * f)      == des->firstDirectedEdge[i]){ newFDE[i] = firstHalfs[ 3 * f]     ; }
      if((unsigned int)((3 * f) + 1) == des->firstDirectedEdge[i]){ newFDE[i] = firstHalfs[(3 * f) + 1]; }
      if((unsigned int)((3 * f) + 2) == des->firstDirectedEdge[i]){ newFDE[i] = firstHalfs[(3 * f) + 2]; }
    }

    // finally update first directed edge for new vertices
    if(newFDE[ve0] == -1){ newFDE[ve0] = (3 * f) + 1; }
    if(newFDE[ve1] == -1){ newFDE[ve1] = (3 * f) + 2; }
    if(newFDE[ve2] == -1){ newFDE[ve2] =  3 * f     ; }
  }

  // assign our new face vertices
  des->faceVertices = newFaceVertices;

  // assign out first directed edges
  des->firstDirectedEdge = newFDE;
}


// find the one ring of a vertex
std::vector<unsigned int> Subdivide::OneRing(unsigned int v)
{
  std::vector<unsigned int> thisOneRing; // one ring

  // first edge
  unsigned int fEdge = des->firstDirectedEdge[v];

  // current edge
  unsigned int cEdge = fEdge;

  // current vertex
  unsigned int cVert;

  // counter
  int c = 0;

  // now loop through till we arrive back at the start
  while(cEdge != fEdge || c == 0)
  {
    // find the vertex this edge is pointing to and add
    cVert = des->faceVertices[cEdge];
    thisOneRing.push_back(cVert);

    // move along the triangle two steps
    cEdge = (3 * (cEdge / 3)) + (cEdge + 2) % 3;

    // find the other half
    cEdge = des->otherHalf[cEdge];

    // increment
    c++;
  }
  return(thisOneRing);
}

void Subdivide::CalculateOtherHalfs()
{
    // create a new other half vector
    int ohVal = -1;
    std::vector<unsigned int> newOtherHalf;
    newOtherHalf.resize(des->faceVertices.size());

    // all the middle faces take up edges         0   to 3n - 1
    // all the first extra faces take up edges    3n  to 6n - 1
    // all the second extra faces take up edges   6n  to 9n - 1
    // all the final extra faces take up edges    9n  to 12n - 1
    for(unsigned int i = 0; i < firstHalfs.size(); i ++)
    {
      // very nice and neat
      newOtherHalf[firstHalfs[i]]  = secondHalfs[des->otherHalf[i]];
      newOtherHalf[secondHalfs[i]] =  firstHalfs[des->otherHalf[i]];
    }

    //finally we calculate all the 'other halfs' of the middle faces
    for(unsigned int e = 0; e < (3 * nFacesOld); e++)
    {
      // find other half value
      if(e % 3 == 0){ ohVal = (3 * nEdgesOld) + e + 2; }
      if(e % 3 == 1){ ohVal = (1 * nEdgesOld) + e + 1; }
      if(e % 3 == 2){ ohVal = (2 * nEdgesOld) + e + 0; }
      newOtherHalf[e] = ohVal;
      newOtherHalf[ohVal] = e;
    }
    // finally assign the new values
    des->otherHalf = newOtherHalf;
}

float Subdivide::FindAlpha(int n)
{

  float pi = 3.1415926;

  // Simple Case
  if(n == 3)
  {
    return(3. / 16.);
  }
  // Complex Case
  else
  {
    // how do you make lines like this readable
    float n1 = 1. / (float)n;
    float a = (5. / 8.);
    float b = (3. / 8.);
    float c = (1. / 4.);
    float cosComp = cos((2. * pi) / (n));

    // this is my best effort
    return (n1 * (a - pow(b + (c * cosComp), 2)));
  }

  // Stop any odd errors happening
  return 1.;
}

void Subdivide::SmoothVertices()
{
  std::vector<unsigned int> oneRing;   // one ring of current vertex
  Cartesian3 newPos;                   // new position of a single vertex
  unsigned int n;                      // neighbours
  float a;                             // alpha constant for weights
  unsigned int nVertices = des->vertices.size();

  // clear positions
  newVertexPositions.clear();
  newVertexPositions.resize(nVertices);

  for(unsigned int v = 0; v < nVertices; v++)
  {
    // find one ring for this vertex
    oneRing = OneRing(v);
    n = oneRing.size();

    // calculate a beta value
    a = FindAlpha(n);

    // first component of position
    newPos = (1. - (n * a)) * des->vertices[v];

    // add weights of one ring
    for(unsigned int i = 0; i < n; i++)
    {
      newPos = newPos + (a * des->vertices[oneRing[i]]);
    }

    // update position
    newVertexPositions[v] = newPos;
  }
}

// Done at the end to avoid the shape verticies
// affecting the results too soon
void Subdivide::ApplyVertexPositions()
{
  for(unsigned int i = 0; i < newVertexPositions.size(); i++)
  {
    des->vertices[i] = newVertexPositions[i];
  }
}
