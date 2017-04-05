/*
  Copyright ©2013 The Regents of the University of California
  (Regents). All Rights Reserved. Permission to use, copy, modify, and
  distribute this software and its documentation for educational,
  research, and not-for-profit purposes, without fee and without a
  signed licensing agreement, is hereby granted, provided that the
  above copyright notice, this paragraph and the following two
  paragraphs appear in all copies, modifications, and
  distributions. Contact The Office of Technology Licensing, UC
  Berkeley, 2150 Shattuck Avenue, Suite 510, Berkeley, CA 94720-1620,
  (510) 643-7201, for commercial licensing opportunities.

  IN NO EVENT SHALL REGENTS BE LIABLE TO ANY PARTY FOR DIRECT,
  INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING
  LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS
  DOCUMENTATION, EVEN IF REGENTS HAS BEEN ADVISED OF THE POSSIBILITY
  OF SUCH DAMAGE.

  REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
  FOR A PARTICULAR PURPOSE. THE SOFTWARE AND ACCOMPANYING
  DOCUMENTATION, IF ANY, PROVIDED HEREUNDER IS PROVIDED "AS
  IS". REGENTS HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT,
  UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
*/

#ifndef MESH_HPP
#define MESH_HPP

#include "transformation.hpp"
#include "vectors.hpp"
#include <utility>
#include <vector>

// material space (not fused at seams)
struct Vert;
struct Face;
// world space (fused)
struct Node;
struct Edge;

struct Sizing; // for dynamic remeshing

struct Vert {
    int label;
    Vec2 u; // material space
    Node *node; // world space
    // topological data
    std::vector<Face*> adjf; // adjacent faces
    int index; // position in mesh.verts
    // derived material-space data that only changes with remeshing
    double a, m; // area, mass
    // remeshing data
    Sizing *sizing;
    // constructors
    Vert () {}
    explicit Vert (const Vec2 &u, int label=0):
        label(label), u(u) {}
    explicit Vert (const Vec3 &x, int label=0):
        label(label), u(project<2>(x)) {}
};

struct Node {
    int label;
    std::vector<Vert*> verts;
    Vec3 y; // plastic embedding
    Vec3 x, x0, v; // position, old (collision-free) position, velocity
    bool preserve; // don't remove this node
    // topological data
    int index; // position in mesh.nodes
    std::vector<Edge*> adje; // adjacent edges
    // derived world-space data that changes every frame
    Vec3 n; // local normal, approximate
    // derived material-space data that only changes with remeshing
    double a, m; // area, mass
    // pop filter data
    Vec3 acceleration;
    Node () {}
    explicit Node (const Vec3 &y, const Vec3 &x, const Vec3 &v, int label=0):
        label(label), y(y), x(x), x0(x), v(v) {}
    explicit Node (const Vec3 &x, const Vec3 &v, int label=0):
        label(label), y(x), x(x), x0(x), v(v) {}
    explicit Node (const Vec3 &x, int label=0):
        label(label), y(x), x(x), x0(x), v(Vec3(0)) {}
};

struct Edge {
    Node *n[2]; // nodes
    int label;
    // topological data
    Face *adjf[2]; // adjacent faces
    int index; // position in mesh.edges
    // derived world-space data that changes every frame
    double theta; // actual dihedral angle
    // derived material-space data
    double l; // length
    // plasticity data
    double theta_ideal, damage; // rest dihedral angle, damage parameter
    double reference_angle; // just to get sign of dihedral_angle() right
    // constructors
    Edge () {}
    explicit Edge (Node *node0, Node *node1, double theta_ideal, int label=0):
        label(label), theta_ideal(theta_ideal), damage(0),
        reference_angle(theta_ideal), l(0) {
        n[0] = node0;
        n[1] = node1;
    }
    explicit Edge (Node *node0, Node *node1, int label=0):
        label(label), theta_ideal(0), damage(0), reference_angle(0), l(0) {
        n[0] = node0;
        n[1] = node1;
    }
};

struct Face {
    Vert* v[3]; // verts
    int label;
    // topological data
    Edge *adje[3]; // adjacent edges
    int index; // position in mesh.faces
    // derived world-space data that changes every frame
    Vec3 n; // local normal, exact
    // derived material-space data that only changes with remeshing
    double a, m; // area, mass
    Mat2x2 Dm, invDm; // finite element matrix
    // plasticity data
    Mat2x2 S_plastic; // plastic strain
    double damage; // accumulated norm of S_plastic/S_yield
    // constructors
    Face () {}
    explicit Face (Vert *vert0, Vert *vert1, Vert *vert2, int label=0):
        label(label), S_plastic(0), damage(0), a(0), m(0) {
        v[0] = vert0;
        v[1] = vert1;
        v[2] = vert2;
    }
};

struct Mesh {
    std::vector<Vert*> verts;
    std::vector<Node*> nodes;
    std::vector<Edge*> edges;
    std::vector<Face*> faces;
    // These do *not* assume ownership, so no deletion on removal
    void add (Vert *vert);
    void add (Node *node);
    void add (Edge *edge);
    void add (Face *face);
    void remove (Vert *vert);
    void remove (Node *node);
    void remove (Edge *edge);
    void remove (Face *face);
};

template <typename Prim> const std::vector<Prim*> &get (const Mesh &mesh);

void connect (Vert *vert, Node *node); // assign vertex to node

bool check_that_pointers_are_sane (const Mesh &mesh);
bool check_that_contents_are_sane (const Mesh &mesh);

void compute_ms_data (Mesh &mesh); // call after mesh topology changes
void compute_ws_data (Mesh &mesh); // call after vert positions change

Edge *get_edge (const Node *node0, const Node *node1);
Vert *edge_vert (const Edge *edge, int side, int i);
Vert *edge_opp_vert (const Edge *edge, int side);

void update_indices (Mesh &mesh);
void mark_nodes_to_preserve (Mesh &mesh);

inline Vec2 derivative (double a0, double a1, double a2, const Face *face) {
    return face->invDm.t() * Vec2(a1-a0, a2-a0);
}
template <int n>
Mat<n,2> derivative (Vec<n> w0, Vec<n> w1, Vec<n> w2, const Face *face) {
    return Mat<n,2>(w1-w0, w2-w0) * face->invDm;
}
inline Mat2x3 derivative (const Face *face) {
    return face->invDm.t()*Mat2x3::rows(Vec3(-1,1,0), Vec3(-1,0,1));
}

void apply_transformation_onto(const Mesh& start_state, Mesh& onto,
                               const Transformation& tr);
void apply_transformation(Mesh& mesh, const Transformation& tr);

void update_x0 (Mesh &mesh);

Mesh deep_copy (const Mesh &mesh);
void delete_mesh (Mesh &mesh);

#endif
