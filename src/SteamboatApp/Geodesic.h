#pragma once
// Geodesic.h: geodesic dome calculator (Geodesic.obj, built without debug information: the
// layout and the names are reconstructed from the code). It splits a face of an icosahedron,
// octahedron or tetrahedron into a frequency-n triangle grid, projects it onto a sphere and
// computes the chord and face factors.

// what the caller asks for
struct parameters {
    long frequency;                                     // +0x00
    long classtype;                                     // +0x04 (1 = class I, 2 = class II)
    long polyhedron;                                    // +0x08 (1 icosa, 2 octa, 3 tetra)
    long unknown0c;                                     // +0x0c (not read by the constructor; 0 from the sphere)
    char filename[80];                                  // +0x10
    long unknown60;                                     // +0x60 (not read by the constructor; 1 from the sphere)
    long output_type;                                   // +0x64
    long buckyball;                                     // +0x68
    long output_units;                                  // +0x6c
    long silent;                                        // +0x70
    double ellipse;                                     // +0x78 (1.0 = a sphere; 4 bytes alignment before it)
};

// a vertex of the frequency grid: its row and position in the row
struct grid_label {
    long row;
    long col;
};

// a vertex in the triangle coordinates of the polyhedron face
struct tri_crd {
    long a;
    long b;
    long c;
};

// a vertex on the sphere
struct sph_crd {
    double theta;
    double phi;
    double radius;
};

// a chord: the two vertices it joins
struct chord {
    long start;
    long end;
};

// a face: its three vertices
struct face {
    long v1;
    long v2;
    long v3;
};

class Geodesic {
public:
    Geodesic(parameters *);
    double axial_angle_A(long);
    double axial_angle_B(long, double);
    double chord_length(double, double, double, double);
    double clean_float(double);
    double ellipse_radius(double, double);
    double face_angle_A(long);
    double face_angle_B(long, double);
    double face_angle_C(double, double);
    double root_E(double, double);
    double rotate_phi(double, double, double, double);
    double rotate_theta(double, double, double, double);
    long face_quantity(long, long);
    long tetra_angle();
    void bucky_factor();
    void chord_factor();
    void display_data();
    void face_factor();
    void icosa_sphere(long);
    void init_crd();
    void octa_sphere(long);
    void save_PRN(char *);
    void save_ascii(char *);
    void spherical();
    void tetra_sphere(long);
    void time_passage(int);
    void topology();
    ~Geodesic();

    double x;                                           // +0x00 the vertex being converted
    double y;                                           // +0x08
    double z;                                           // +0x10
    double ellipse;                                     // +0x18
    long spin;                                          // +0x20 the progress indicator's state
    long freq;                                          // +0x24 frequency of the computed triangle
    long vertices;                                      // +0x28
    long faces;                                         // +0x2c
    long edges;                                         // +0x30
    long frequency;                                     // +0x34
    long vertices_I;                                    // +0x38
    long faces_I;                                       // +0x3c
    long edges_I;                                       // +0x40
    long silent;                                        // +0x44
    long output_type;                                   // +0x48
    long output_units;                                  // +0x4c
    long elliptical;                                    // +0x50
    long vertices_II;                                   // +0x54
    long faces_II;                                      // +0x58
    long edges_II;                                      // +0x5c
    long faces_B;                                       // +0x60
    long vertices_B;                                    // +0x64
    long edges_B;                                       // +0x68
    long buckyball;                                     // +0x6c
    long classtype;                                     // +0x70
    long polyhedron;                                    // +0x74
    tri_crd *tri;                                       // +0x78
    grid_label *label;                                  // +0x7c
    sph_crd *sph;                                       // +0x80
    chord *chords;                                      // +0x84
    face *face_list;                                    // +0x88
    sph_crd *sph_face;                                  // +0x8c
};
