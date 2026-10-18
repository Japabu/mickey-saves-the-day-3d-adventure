// Geodesic dome calculator (Geodesic.obj): console output through the classic iostreams.
#include "Geodesic.h"
#include <iostream.h>
#include <fstream.h>
#include <iomanip.h>
#include <math.h>
#include <stdlib.h>

const double DEG2RAD = 0.0174532925199433;
const double RAD2DEG = 57.2957795130824;

// 0x10108440 ??0Geodesic@@QAE@PAUparameters@@@Z
Geodesic::Geodesic(parameters *p)
{
    frequency = p->frequency;
    classtype = p->classtype;
    polyhedron = p->polyhedron;
    output_type = p->output_type;
    silent = p->silent;
    buckyball = p->buckyball;
    output_units = p->output_units;
    vertices_I = 0;
    faces_I = 0;
    edges_I = 0;
    if (p->ellipse != 1.0) {
        elliptical = 1;
        ellipse = p->ellipse;
    } else
        elliptical = 0;
    topology();
    init_crd();
    spherical();
    if (buckyball)
        bucky_factor();
    else {
        chord_factor();
        face_factor();
    }
}

// 0x101084e0 ??1Geodesic@@QAE@XZ
Geodesic::~Geodesic()
{
}

// 0x101084f0 ?topology@Geodesic@@QAEXXZ
void Geodesic::topology()
{
    long i;
    for (i = 1; i <= frequency; i++) {
        if (i == 1)
            vertices_I = 3;
        else
            vertices_I += i + 1;
    }
    faces_I = frequency * frequency;
    edges_I = vertices_I + faces_I - 1;
    if (classtype == 2) {
        for (i = 1; i <= frequency / 2; i++) {
            if (i == 1)
                vertices_II = 3;
            else
                vertices_II += i + 1;
        }
        faces_II = (frequency / 2) * (frequency / 2);
        edges_II = vertices_II + faces_II - 1;
    }
    if (buckyball) {
        vertices_B = 0;
        for (i = 0; i <= frequency; i += 3)
            vertices_B += i * 2;
        faces_B = 1;
        for (i = 3; i <= frequency; i += 3)
            faces_B += frequency - i;
        edges_B = vertices_B + faces_B - 1;
    }
}

// 0x101085d0 ?init_crd@Geodesic@@QAEXXZ
void Geodesic::init_crd()
{
    if (classtype == 1) {
        vertices = vertices_I;
        edges = edges_I;
        freq = frequency;
        faces = faces_I;
    } else if (classtype == 2) {
        vertices = vertices_II;
        edges = edges_II;
        freq = frequency / 2;
        faces = faces_II;
    }
}

// TODO 0x10108620 ?spherical@Geodesic@@QAEXXZ: (84% similar) all but the phi formulas: the original squares x before y and lets the class I branch share only the atan tail; VC6 squares y first (both operand orders, 2.0 tried)
void Geodesic::spherical()
{
    long i, j, n;
    label = new grid_label[vertices + 1];
    if (label == 0) {
        cout << "Insufficient memory for label array --- Execution Terminating" << '\n';
        exit(1);
    }
    sph = new sph_crd[vertices + 1];
    if (sph == 0) {
        cout << "Insufficient memory for coordinate array --- Execution Terminating" << '\n';
        exit(1);
    }
    sph_face = new sph_crd[vertices + 1];
    if (sph_face == 0) {
        cout << "Insufficient memory for face array --- Execution Terminating" << '\n';
        exit(1);
    }
    tri = new tri_crd[vertices + 1];
    if (tri == 0) {
        cout << "Insufficient memory for Spherical Array --- Execution Terminating" << '\n';
        exit(1);
    }
    n = 1;
    for (i = 0; i <= freq; i++) {
        for (j = 0; j <= i; j++) {
            label[n].row = i;
            label[n].col = j;
            tri[n].a = j;
            tri[n].b = i - j;
            tri[n].c = frequency - i;
            n++;
        }
    }
    if (!silent) {
        cout << "Calculating Spherical Coordinates... ";
        spin = 0;
    }
    for (n = 1; n <= vertices; n++) {
        if (!silent) {
            time_passage(spin);
            spin++;
            if (spin > 3)
                spin = 0;
        }
        if (polyhedron == 2) {
            x = tri[n].a;
            y = tri[n].b;
            z = tri[n].c;
        } else if (polyhedron == 1) {
            x = tri[n].a * sin(DEG2RAD * 72.0);
            y = tri[n].a * cos(DEG2RAD * 72.0) + tri[n].b;
            z = tri[n].c / (2 * cos(DEG2RAD * 36.0)) + frequency * 0.5;
        } else if (polyhedron == 3) {
            x = tri[n].a * pow(3.0, 0.5);
            y = tri[n].b * 2 - tri[n].a;
            z = (tri[n].c * 3.0 - tri[n].a - tri[n].b) / pow(2.0, 0.5);
        }
        if (y == 0.0 && x == 0.0)
            sph[n].theta = 0.0;
        else if (y == 0.0)
            sph[n].theta = 90.0;
        else
            sph[n].theta = atan(x / y) * RAD2DEG;
        if (sph[n].theta < 0.0)
            sph[n].theta += 180.0;
        if (z == 0.0)
            sph[n].phi = 90.0;
        else if (classtype == 1)
            sph[n].phi = atan(pow(pow(x, 2) + pow(y, 2), 0.5) / z) * RAD2DEG;
        else if (polyhedron == 2 && classtype == 2)
            sph[n].phi = atan(pow(2 * (pow(x, 2) + pow(y, 2)), 0.5) / z) * RAD2DEG;
        else if (polyhedron == 1 && classtype == 2)
            sph[n].phi = atan(pow(pow(x, 2) + pow(y, 2), 0.5) / (cos(DEG2RAD * 36.0) * z)) * RAD2DEG;
        else if (polyhedron == 3 && classtype == 2)
            sph[n].phi = atan(2 * pow(pow(x, 2) + pow(y, 2), 0.5) / z) * RAD2DEG;
        if (sph[n].phi < 0.0)
            sph[n].phi += 180.0;
        sph[n].radius = 1.0;
    }
    cout << '\r' << "                                     " << '\r';
    delete tri;
}

// EQUIV 0x10108b20 ?chord_factor@Geodesic@@QAEXXZ: reviewed: only register choices of the vertex searches and the placement of independent moves, lea and test instructions differ. Was: same code with the registers of the vertex searches swapped (the original keeps k in ecx and the label pointer in eax); declaration orders and a second index tried
void Geodesic::chord_factor()
{
    long n, k, c;
    if (!silent) {
        cout << "Calculating Chord Factors... ";
        spin = 0;
    }
    chords = new chord[edges_I + 1];
    if (chords == 0) {
        cout << "Insufficient memory for chord array --- Execution Terminating" << '\n';
        exit(1);
    }
    c = 1;
    for (n = 1; n < vertices; n++) {
        if (!silent) {
            time_passage(spin);
            spin++;
            if (spin > 3)
                spin = 0;
        }
        if (label[n].row == 0 && label[n].col == 0) {
            chords[c].start = n;
            for (k = n; k <= vertices; k++)
                if (label[k].row == label[n].row + 1 && label[k].col == label[n].col)
                    break;
            chords[c].end = k;
            c++;
            chords[c].start = n;
            for (k = n; k <= vertices; k++)
                if (label[k].row == label[n].row + 1 && label[k].col == label[n].col + 1)
                    break;
        } else if (label[n].row == freq && label[n].col == 0) {
            chords[c].start = n;
            for (k = n; k <= vertices; k++)
                if (label[k].row == label[n].row && label[k].col == label[n].col + 1)
                    break;
        } else if (label[n].row == label[n].col) {
            chords[c].start = n;
            for (k = n; k <= vertices; k++)
                if (label[k].row == label[n].row + 1 && label[k].col == label[n].col)
                    break;
            chords[c].end = k;
            c++;
            chords[c].start = n;
            for (k = n; k <= vertices; k++)
                if (label[k].row == label[n].row + 1 && label[k].col == label[n].col + 1)
                    break;
        } else if (label[n].row == freq) {
            chords[c].start = n;
            for (k = n; k <= vertices; k++)
                if (label[k].row == label[n].row && label[k].col == label[n].col + 1)
                    break;
        } else if (label[n].col == 0) {
            chords[c].start = n;
            for (k = n; k <= vertices; k++)
                if (label[k].row == label[n].row && label[k].col == label[n].col + 1)
                    break;
            chords[c].end = k;
            c++;
            chords[c].start = n;
            for (k = n; k <= vertices; k++)
                if (label[k].row == label[n].row + 1 && label[k].col == label[n].col)
                    break;
            chords[c].end = k;
            c++;
            chords[c].start = n;
            for (k = n; k <= vertices; k++)
                if (label[k].row == label[n].row + 1 && label[k].col == label[n].col + 1)
                    break;
        } else {
            chords[c].start = n;
            for (k = n; k <= vertices; k++)
                if (label[k].row == label[n].row && label[k].col == label[n].col + 1)
                    break;
            chords[c].end = k;
            c++;
            chords[c].start = n;
            for (k = n; k <= vertices; k++)
                if (label[k].row == label[n].row + 1 && label[k].col == label[n].col)
                    break;
            chords[c].end = k;
            c++;
            chords[c].start = n;
            for (k = n; k <= vertices; k++)
                if (label[k].row == label[n].row + 1 && label[k].col == label[n].col + 1)
                    break;
        }
        chords[c].end = k;
        c++;
    }
    cout << '\r' << "                             " << '\r';
}

// TODO 0x10108f80 ?face_factor@Geodesic@@QAEXXZ: 95% similar: the same searches, the original keeps the second index of some searches on the stack and the face pointer in other registers
void Geodesic::face_factor()
{
    long n, k, j, f;
    if (!silent) {
        cout << "Calculating Face Factors... ";
        spin = 0;
    }
    face_list = new face[faces + 1];
    if (face_list == 0) {
        cout << "Insufficient memory for face array --- Execution Terminating" << '\n';
        exit(1);
    }
    f = 1;
    for (n = 1; n <= vertices - freq - 1; n++) {
        if (!silent) {
            time_passage(spin);
            spin++;
            if (spin > 3)
                spin = 0;
        }
        if (label[n].row == 0 && label[n].col == 0) {
            face_list[f].v1 = n;
            for (k = n; k <= vertices; k++)
                if (label[k].row == label[n].row + 1 && label[k].col == label[n].col)
                    break;
            face_list[f].v2 = k;
            for (j = n; j <= vertices; j++)
                if (label[j].row == label[k].row && label[j].col == label[k].col + 1)
                    break;
            face_list[f].v3 = j;
        } else if (label[n].col == 0) {
            face_list[f].v1 = n;
            for (k = n; k <= vertices; k++)
                if (label[k].row == label[n].row + 1 && label[k].col == label[n].col)
                    break;
            face_list[f].v2 = k;
            for (j = n; j <= vertices; j++)
                if (label[j].row == label[k].row && label[j].col == label[k].col + 1)
                    break;
            face_list[f].v3 = j;
            f++;
            face_list[f].v1 = n;
            for (k = n; k <= vertices; k++)
                if (label[k].row == label[n].row + 1 && label[k].col == label[n].col + 1)
                    break;
            face_list[f].v2 = k;
            for (j = n; j <= vertices; j++)
                if (label[j].row == label[k].row - 1 && label[j].col == label[k].col)
                    break;
            face_list[f].v3 = j;
        } else if (label[n].row == label[n].col) {
            face_list[f].v1 = n;
            for (k = n; k <= vertices; k++)
                if (label[k].row == label[n].row + 1 && label[k].col == label[n].col)
                    break;
            face_list[f].v2 = k;
            for (j = n; j <= vertices; j++)
                if (label[j].row == label[k].row && label[j].col == label[k].col + 1)
                    break;
            face_list[f].v3 = j;
        } else {
            face_list[f].v1 = n;
            for (k = n; k <= vertices; k++)
                if (label[k].row == label[n].row + 1 && label[k].col == label[n].col)
                    break;
            face_list[f].v2 = k;
            for (j = n; j <= vertices; j++)
                if (label[j].row == label[k].row && label[j].col == label[k].col + 1)
                    break;
            face_list[f].v3 = j;
            f++;
            face_list[f].v1 = n;
            for (k = n; k <= vertices; k++)
                if (label[k].row == label[n].row + 1 && label[k].col == label[n].col + 1)
                    break;
            face_list[f].v2 = k;
            for (j = n; j <= vertices; j++)
                if (label[j].row == label[k].row - 1 && label[j].col == label[k].col)
                    break;
            face_list[f].v3 = j;
        }
        f++;
    }
    cout << '\r' << "                             " << '\r';
}

// 0x101093d0 ?clean_float@Geodesic@@QAENN@Z
double Geodesic::clean_float(double value)
{
    if ((value < 0.0) & (value > -1e-13))
        return fabs(value);
    return value;
}

// 0x10109410 ?time_passage@Geodesic@@QAEXH@Z
void Geodesic::time_passage(int state)
{
    if (state == 0)
        cout << '\b' << "-";
    else if (state == 1)
        cout << '\b' << "/";
    else if (state == 2)
        cout << '\b' << "|";
    else if (state == 3)
        cout << '\b' << '\\';
}

// 0x10109470 ?face_quantity@Geodesic@@QAEJJJ@Z
long Geodesic::face_quantity(long poly, long cls)
{
    long quantity;
    if (poly == 1) {
        if (cls == 1)
            quantity = 19;
        else if (cls == 2)
            quantity = 7;
        else if (cls == 3)
            quantity = 3;
    } else if (poly == 2) {
        if (cls == 1)
            quantity = 59;
        else if (cls == 2)
            quantity = 39;
        else if (cls == 3)
            quantity = 11;
    }
    return quantity;
}

// 0x101094e0 ?save_PRN@Geodesic@@QAEXPAD@Z
void Geodesic::save_PRN(char *file)
{
    long i;
    double x1, y1, z1, x2, y2, z2;
    ofstream out(file);
    out << setiosflags(ios::fixed) << setw(8) << setprecision(6);
    out << "Spherical Vertexia Coordinates" << '\n';
    for (i = 1; i <= vertices; i++)
        out << sph[i].theta << ", " << sph[i].phi << '\n';
    out << '\n';
    out << "Cartesian Vertexia Coordinates" << '\n';
    for (i = 1; i <= vertices; i++) {
        x1 = clean_float(sin(DEG2RAD * sph[i].phi) * cos(DEG2RAD * sph[i].theta));
        y1 = clean_float(sin(DEG2RAD * sph[i].phi) * sin(DEG2RAD * sph[i].theta));
        z1 = clean_float(cos(DEG2RAD * sph[i].phi));
        out << x1 << ", " << y1 << ", " << z1 << '\n';
    }
    out << '\n';
    out << "Spherical Chord Coordinates" << '\n';
    for (i = 1; i <= edges; i++)
        out << sph[chords[i].start].theta << ", " << sph[chords[i].start].phi << ", "
            << sph[chords[i].end].theta << ", " << sph[chords[i].end].phi << '\n';
    out << '\n';
    out << "Cartesian Chord Coordinates" << '\n';
    for (i = 1; i <= edges; i++) {
        x1 = clean_float(sin(DEG2RAD * sph[chords[i].start].phi) * cos(DEG2RAD * sph[chords[i].start].theta));
        y1 = clean_float(sin(DEG2RAD * sph[chords[i].start].phi) * sin(DEG2RAD * sph[chords[i].start].theta));
        z1 = clean_float(cos(DEG2RAD * sph[chords[i].start].phi));
        x2 = clean_float(sin(DEG2RAD * sph[chords[i].end].phi) * cos(DEG2RAD * sph[chords[i].end].theta));
        y2 = clean_float(sin(DEG2RAD * sph[chords[i].end].phi) * sin(DEG2RAD * sph[chords[i].end].theta));
        z2 = clean_float(cos(DEG2RAD * sph[chords[i].end].phi));
        out << x1 << ", " << y1 << ", " << z1 << ", " << x2 << ", " << y2 << ", " << z2 << '\n';
    }
    out.close();
}

// TODO 0x10109af0 ?icosa_sphere@Geodesic@@QAEXJ@Z: 87% similar: same branches and calls; the original adds the constant terms in another order and keeps the shifted theta in locals
void Geodesic::icosa_sphere(long face)
{
    long n, f;
    double rot;
    if (classtype == 1) {
        if (face >= 0 && face <= 4) {
        for (n = 1; n <= vertices; n++) {
            if ((buckyball && fmod(label[n].row + label[n].col, 3.0) != 0) || !buckyball) {
                sph_face[n].theta = face * 72.0 + sph[n].theta;
                sph_face[n].phi = sph[n].phi;
                if (!elliptical)
                    sph_face[n].radius = 1.0;
                else {
                    sph_face[n].phi = root_E(ellipse, sph_face[n].phi);
                    sph_face[n].radius = ellipse_radius(ellipse, sph_face[n].phi);
                }
            }
        }
        } else if (face >= 5 && face <= 9) {
            f = face - 5;
        for (n = 1; n <= vertices; n++) {
            if ((buckyball && fmod(label[n].row + label[n].col, 3.0) != 0) || !buckyball) {
                sph_face[n].phi = rotate_theta(36.0, sph[n].theta, 180.0 - atan(2.0) * RAD2DEG, sph[n].phi);
                sph_face[n].theta = rotate_phi(36.0, sph[n].theta, sph[n].phi, sph_face[n].phi) + 36.0 + f * 72.0;
                if (!elliptical)
                    sph_face[n].radius = 1.0;
                else {
                    sph_face[n].phi = root_E(ellipse, sph_face[n].phi);
                    sph_face[n].radius = ellipse_radius(ellipse, sph_face[n].phi);
                }
            }
        }
        } else if (face >= 10 && face <= 14) {
            f = face - 10;
        for (n = 1; n <= vertices; n++) {
            if ((buckyball && fmod(label[n].row + label[n].col, 3.0) != 0) || !buckyball) {
                sph_face[n].phi = 180.0 - rotate_theta(36.0, sph[n].theta, 180.0 - atan(2.0) * RAD2DEG, sph[n].phi);
                sph_face[n].theta = rotate_phi(36.0, sph[n].theta, sph[n].phi, sph_face[n].phi) + 36.0 + f * 72.0 + 36.0;
                if (!elliptical)
                    sph_face[n].radius = 1.0;
                else {
                    sph_face[n].phi = root_E(ellipse, sph_face[n].phi);
                    sph_face[n].radius = ellipse_radius(ellipse, sph_face[n].phi);
                }
            }
        }
        } else if (face >= 15 && face <= 19) {
            f = face - 15;
        for (n = 1; n <= vertices; n++) {
            if ((buckyball && fmod(label[n].row + label[n].col, 3.0) != 0) || !buckyball) {
                sph_face[n].theta = f * 72.0 + sph[n].theta + 36.0;
                sph_face[n].phi = 180.0 - sph[n].phi;
                if (!elliptical)
                    sph_face[n].radius = 1.0;
                else {
                    sph_face[n].phi = root_E(ellipse, sph_face[n].phi);
                    sph_face[n].radius = ellipse_radius(ellipse, sph_face[n].phi);
                }
            }
        }
        }
    } else if (classtype == 2) {
        if (face >= 0 && face <= 4) {
        for (n = 1; n <= vertices; n++) {
            if ((buckyball && fmod(label[n].row + label[n].col, 3.0) != 0) || !buckyball) {
                sph_face[n].theta = face * 72.0 + sph[n].theta;
                sph_face[n].phi = sph[n].phi;
                if (!elliptical)
                    sph_face[n].radius = 1.0;
                else {
                    sph_face[n].phi = root_E(ellipse, sph_face[n].phi);
                    sph_face[n].radius = ellipse_radius(ellipse, sph_face[n].phi);
                }
            }
        }
        } else if (face >= 5 && face <= 29) {
        if (face >= 5 && face <= 9) {
            f = face - 5;
            rot = 0.0;
        }
        else if (face >= 10 && face <= 14) {
            f = face - 10;
            rot = 1.0;
        }
        else if (face >= 15 && face <= 19) {
            f = face - 15;
            rot = 2.0;
        }
        else if (face >= 20 && face <= 24) {
            f = face - 20;
            rot = 3.0;
        }
        else if (face >= 25 && face <= 29) {
            f = face - 25;
            rot = 4.0;
        }
        for (n = 1; n <= vertices; n++) {
            if ((buckyball && fmod(label[n].row + label[n].col, 3.0) != 0) || !buckyball) {
                sph_face[n].phi = rotate_theta(rot * 72.0 + 36.0, f * 72.0 + sph[n].theta, atan(2.0) * RAD2DEG, sph[n].phi);
                sph_face[n].theta = rotate_phi(rot * 72.0 + 36.0, f * 72.0 + sph[n].theta, sph[n].phi, sph_face[n].phi) + 36.0 + f * 72.0;
                if (!elliptical)
                    sph_face[n].radius = 1.0;
                else {
                    sph_face[n].phi = root_E(ellipse, sph_face[n].phi);
                    sph_face[n].radius = ellipse_radius(ellipse, sph_face[n].phi);
                }
            }
        }
        } else if (face >= 30 && face <= 34) {
            f = face - 30;
        for (n = 1; n <= vertices; n++) {
            if ((buckyball && fmod(label[n].row + label[n].col, 3.0) != 0) || !buckyball) {
                sph_face[n].theta = f * 72.0 + sph[n].theta + 36.0;
                sph_face[n].phi = 180.0 - sph[n].phi;
                if (!elliptical)
                    sph_face[n].radius = 1.0;
                else {
                    sph_face[n].phi = root_E(ellipse, sph_face[n].phi);
                    sph_face[n].radius = ellipse_radius(ellipse, sph_face[n].phi);
                }
            }
        }
        } else if (face >= 35 && face <= 59) {
        if (face >= 35 && face <= 39) {
            f = face - 35;
            rot = 0.0;
        }
        else if (face >= 40 && face <= 44) {
            f = face - 40;
            rot = 1.0;
        }
        else if (face >= 45 && face <= 49) {
            f = face - 45;
            rot = 2.0;
        }
        else if (face >= 50 && face <= 54) {
            f = face - 50;
            rot = 3.0;
        }
        else if (face >= 55 && face <= 59) {
            f = face - 55;
            rot = 4.0;
        }
        for (n = 1; n <= vertices; n++) {
            if ((buckyball && fmod(label[n].row + label[n].col, 3.0) != 0) || !buckyball) {
                sph_face[n].phi = 180.0 - rotate_theta(rot * 72.0 + 36.0, f * 72.0 + sph[n].theta, atan(2.0) * RAD2DEG, sph[n].phi);
                sph_face[n].theta = rotate_phi(rot * 72.0 + 36.0, f * 72.0 + sph[n].theta, sph[n].phi, sph_face[n].phi) + 36.0 + f * 72.0 + 36.0;
                if (!elliptical)
                    sph_face[n].radius = 1.0;
                else {
                    sph_face[n].phi = root_E(ellipse, sph_face[n].phi);
                    sph_face[n].radius = ellipse_radius(ellipse, sph_face[n].phi);
                }
            }
        }
        }
    }
}
// TODO 0x1010a800 ?octa_sphere@Geodesic@@QAEXJ@Z: 87% similar: same branches and calls; the original adds the constant terms in another order and keeps the shifted theta in locals (as icosa_sphere)
void Geodesic::octa_sphere(long face)
{
    long n, f;
    double rot;
    if (classtype == 1) {
        if (face >= 0 && face <= 3) {
        for (n = 1; n <= vertices; n++) {
            if ((buckyball && fmod(label[n].row + label[n].col, 3.0) != 0) || !buckyball) {
                sph_face[n].theta = face * 90.0 + sph[n].theta;
                sph_face[n].phi = sph[n].phi;
                if (!elliptical)
                    sph_face[n].radius = 1.0;
                else {
                    sph_face[n].phi = root_E(ellipse, sph_face[n].phi);
                    sph_face[n].radius = ellipse_radius(ellipse, sph_face[n].phi);
                }
            }
        }
        } else if (face >= 4 && face <= 7) {
            f = face - 4;
        for (n = 1; n <= vertices; n++) {
            if ((buckyball && fmod(label[n].row + label[n].col, 3.0) != 0) || !buckyball) {
                sph_face[n].theta = f * 90.0 + sph[n].theta;
                sph_face[n].phi = 180.0 - sph[n].phi;
                if (!elliptical)
                    sph_face[n].radius = 1.0;
                else {
                    sph_face[n].phi = root_E(ellipse, sph_face[n].phi);
                    sph_face[n].radius = ellipse_radius(ellipse, sph_face[n].phi);
                }
            }
        }
        }
    } else if (classtype == 2) {
        if (face >= 0 && face <= 3) {
        for (n = 1; n <= vertices; n++) {
            if ((buckyball && fmod(label[n].row + label[n].col, 3.0) != 0) || !buckyball) {
                sph_face[n].theta = face * 90.0 + sph[n].theta;
                sph_face[n].phi = sph[n].phi;
                if (!elliptical)
                    sph_face[n].radius = 1.0;
                else {
                    sph_face[n].phi = root_E(ellipse, sph_face[n].phi);
                    sph_face[n].radius = ellipse_radius(ellipse, sph_face[n].phi);
                }
            }
        }
        } else if (face >= 4 && face <= 19) {
        if (face >= 4 && face <= 7) {
            f = face - 4;
            rot = 0.0;
        }
        else if (face >= 8 && face <= 11) {
            f = face - 8;
            rot = 1.0;
        }
        else if (face >= 12 && face <= 15) {
            f = face - 12;
            rot = 2.0;
        }
        else if (face >= 16 && face <= 19) {
            f = face - 16;
            rot = 3.0;
        }
        for (n = 1; n <= vertices; n++) {
            if ((buckyball && fmod(label[n].row + label[n].col, 3.0) != 0) || !buckyball) {
                sph_face[n].phi = rotate_theta(rot * 90.0 + 45.0, f * 90.0 + sph[n].theta, 90.0, sph[n].phi);
                sph_face[n].theta = rotate_phi(rot * 90.0 + 45.0, f * 90.0 + sph[n].theta, sph[n].phi, sph_face[n].phi) + 45.0 + f * 90.0;
                if (!elliptical)
                    sph_face[n].radius = 1.0;
                else {
                    sph_face[n].phi = root_E(ellipse, sph_face[n].phi);
                    sph_face[n].radius = ellipse_radius(ellipse, sph_face[n].phi);
                }
            }
        }
        } else if (face >= 20 && face <= 23) {
            f = face - 20;
        for (n = 1; n <= vertices; n++) {
            if ((buckyball && fmod(label[n].row + label[n].col, 3.0) != 0) || !buckyball) {
                sph_face[n].theta = f * 90.0 + sph[n].theta;
                sph_face[n].phi = 180.0 - sph[n].phi;
                if (!elliptical)
                    sph_face[n].radius = 1.0;
                else {
                    sph_face[n].phi = root_E(ellipse, sph_face[n].phi);
                    sph_face[n].radius = ellipse_radius(ellipse, sph_face[n].phi);
                }
            }
        }
        } else if (face >= 24 && face <= 39) {
        if (face >= 24 && face <= 27) {
            f = face - 24;
            rot = 0.0;
        }
        else if (face >= 28 && face <= 31) {
            f = face - 28;
            rot = 1.0;
        }
        else if (face >= 32 && face <= 35) {
            f = face - 32;
            rot = 2.0;
        }
        else if (face >= 36 && face <= 39) {
            f = face - 36;
            rot = 3.0;
        }
        for (n = 1; n <= vertices; n++) {
            if ((buckyball && fmod(label[n].row + label[n].col, 3.0) != 0) || !buckyball) {
                sph_face[n].phi = 180.0 - rotate_theta(rot * 90.0 + 45.0, f * 90.0 + sph[n].theta, 90.0, sph[n].phi);
                sph_face[n].theta = rotate_phi(rot * 90.0 + 45.0, f * 90.0 + sph[n].theta, sph[n].phi, sph_face[n].phi) + 45.0 + f * 90.0;
                if (!elliptical)
                    sph_face[n].radius = 1.0;
                else {
                    sph_face[n].phi = root_E(ellipse, sph_face[n].phi);
                    sph_face[n].radius = ellipse_radius(ellipse, sph_face[n].phi);
                }
            }
        }
        }
    }
}
// TODO 0x1010b160 ?tetra_sphere@Geodesic@@QAEXJ@Z: 95% similar: same code; the original adds the 60 degree term right after rotate_phi and forms the array addresses per access (VC6 keeps the element pointers)
void Geodesic::tetra_sphere(long face)
{
    long n;
    long t = tetra_angle();
    double f;
    if (classtype == 1) {
        if (face >= 0 && face <= 2) {
        for (n = 1; n <= vertices; n++) {
            if ((buckyball && fmod(label[n].row + label[n].col, 3.0) != 0) || !buckyball) {
                sph_face[n].theta = face * 120.0 + sph[n].theta;
                sph_face[n].phi = sph[n].phi;
                if (!elliptical)
                    sph_face[n].radius = 1.0;
                else {
                    sph_face[n].phi = root_E(ellipse, sph_face[n].phi);
                    sph_face[n].radius = ellipse_radius(ellipse, sph_face[n].phi);
                }
            }
        }
        } else if (face == 3) {
        for (n = 1; n <= vertices; n++) {
            if ((buckyball && fmod(label[n].row + label[n].col, 3.0) != 0) || !buckyball) {
                sph_face[n].phi = rotate_theta(240.0, sph[n].theta, acos(-1.0 / 3.0) * RAD2DEG, sph[n].phi);
                sph_face[n].theta = rotate_phi(240.0, sph[n].theta, sph[n].phi, sph_face[n].phi);
                if (label[n].row >= t)
                    sph_face[n].theta = 180.0 - sph_face[n].theta;
                if (sph_face[n].theta < 0.0)
                    sph_face[n].theta += 360.0;
                if (!elliptical)
                    sph_face[n].radius = 1.0;
                else {
                    sph_face[n].phi = root_E(ellipse, sph_face[n].phi);
                    sph_face[n].radius = ellipse_radius(ellipse, sph_face[n].phi);
                }
            }
        }
        }
    } else if (classtype == 2) {
        if (face >= 0 && face <= 2) {
        for (n = 1; n <= vertices; n++) {
            if ((buckyball && fmod(label[n].row + label[n].col, 3.0) != 0) || !buckyball) {
                sph_face[n].theta = face * 120.0 + sph[n].theta;
                sph_face[n].phi = sph[n].phi;
                if (!elliptical)
                    sph_face[n].radius = 1.0;
                else {
                    sph_face[n].phi = root_E(ellipse, sph_face[n].phi);
                    sph_face[n].radius = ellipse_radius(ellipse, sph_face[n].phi);
                }
            }
        }
        } else if (face >= 3 && face <= 5) {
            f = face - 3;
        for (n = 1; n <= vertices; n++) {
            if ((buckyball && fmod(label[n].row + label[n].col, 3.0) != 0) || !buckyball) {
                sph_face[n].phi = rotate_theta(60.0, sph[n].theta, acos(-1.0 / 3.0) * RAD2DEG, sph[n].phi);
                sph_face[n].theta = rotate_phi(60.0, sph[n].theta, sph[n].phi, sph_face[n].phi) + 60.0 + f * 120.0;
                if (!elliptical)
                    sph_face[n].radius = 1.0;
                else {
                    sph_face[n].phi = root_E(ellipse, sph_face[n].phi);
                    sph_face[n].radius = ellipse_radius(ellipse, sph_face[n].phi);
                }
            }
        }
        } else if (face >= 6 && face <= 8) {
            f = face - 6;
        for (n = 1; n <= vertices; n++) {
            if ((buckyball && fmod(label[n].row + label[n].col, 3.0) != 0) || !buckyball) {
                sph_face[n].phi = rotate_theta(60.0, sph[n].theta, acos(-1.0 / 3.0) * RAD2DEG, sph[n].phi);
                sph_face[n].theta = rotate_phi(60.0, sph[n].theta, sph[n].phi, sph_face[n].phi) + f * 120.0;
                sph_face[n].phi = 180.0 - sph_face[n].phi;
                if (!elliptical)
                    sph_face[n].radius = 1.0;
                else {
                    sph_face[n].phi = root_E(ellipse, sph_face[n].phi);
                    sph_face[n].radius = ellipse_radius(ellipse, sph_face[n].phi);
                }
            }
        }
        } else if (face >= 9 && face <= 11) {
        for (n = 1; n <= vertices; n++) {
            if ((buckyball && fmod(label[n].row + label[n].col, 3.0) != 0) || !buckyball) {
                sph_face[n].theta = face * 120.0 + sph[n].theta + 60.0;
                sph_face[n].phi = 180.0 - sph[n].phi;
                if (!elliptical)
                    sph_face[n].radius = 1.0;
                else {
                    sph_face[n].phi = root_E(ellipse, sph_face[n].phi);
                    sph_face[n].radius = ellipse_radius(ellipse, sph_face[n].phi);
                }
            }
        }
        }
    }
}

// EQUIV 0x1010ba20 ?root_E@Geodesic@@QAENNN@Z: reviewed: root + 180 is stored inside each branch instead of before the test, and the return tails are shared differently; same comparisons and results
double Geodesic::root_E(double e, double angle)
{
    if (angle == 90.0)
        return 90.0;
    if (angle == 270.0)
        return 270.0;
    double root = clean_float(atan(tan(DEG2RAD * angle) / pow(e, 0.5)) * RAD2DEG);
    double root1;
    if (root >= 0.0) {
        root1 = root + 180.0;
        if (fabs(root - angle) > fabs(root1 - angle))
            return root1;
        return root;
    }
    root1 = root + 180.0;
    root += 360.0;
    if (fabs(root1 - angle) > fabs(root - angle))
        return root;
    return root1;
}

// 0x1010bb00 ?ellipse_radius@Geodesic@@QAENNN@Z
double Geodesic::ellipse_radius(double a, double angle)
{
    angle = DEG2RAD * angle;
    a = pow(a, 2);
    return pow(a / (pow(sin(angle), 2) * a + pow(cos(angle), 2)), 0.5);
}

// 0x1010bb60 ?axial_angle_A@Geodesic@@QAENJ@Z
double Geodesic::axial_angle_A(long n)
{
    double theta1, phi1, theta2, phi2;
    theta1 = sph[chords[n].start].theta;
    phi1 = sph[chords[n].start].phi;
    theta2 = sph[chords[n].end].theta;
    phi2 = sph[chords[n].end].phi;
    double c = chord_length(theta1, phi1, theta2, phi2);
    return acos((sph[chords[n].start].radius * sph[chords[n].start].radius + c * c -
                 sph[chords[n].end].radius * sph[chords[n].end].radius) /
                (2 * c * sph[chords[n].start].radius)) * RAD2DEG;
}

// 0x1010bc70 ?axial_angle_B@Geodesic@@QAENJN@Z
double Geodesic::axial_angle_B(long n, double angle)
{
    return asin(sin(DEG2RAD * angle) * sph[chords[n].start].radius / sph[chords[n].end].radius) * RAD2DEG;
}

// 0x1010bcb0 ?face_angle_A@Geodesic@@QAENJ@Z
double Geodesic::face_angle_A(long n)
{
    double theta1, phi1, theta2, phi2;
    theta1 = sph[face_list[n].v1].theta;
    phi1 = sph[face_list[n].v1].phi;
    theta2 = sph[face_list[n].v3].theta;
    phi2 = sph[face_list[n].v3].phi;
    double a = chord_length(theta1, phi1, theta2, phi2);
    theta1 = sph[face_list[n].v1].theta;
    phi1 = sph[face_list[n].v1].phi;
    theta2 = sph[face_list[n].v2].theta;
    phi2 = sph[face_list[n].v2].phi;
    double b = chord_length(theta1, phi1, theta2, phi2);
    theta1 = sph[face_list[n].v2].theta;
    phi1 = sph[face_list[n].v2].phi;
    theta2 = sph[face_list[n].v3].theta;
    phi2 = sph[face_list[n].v3].phi;
    double c = chord_length(theta1, phi1, theta2, phi2);
    return acos((b * b + a * a - c * c) / (2 * b * a)) * RAD2DEG;
}

// 0x1010bea0 ?face_angle_B@Geodesic@@QAENJN@Z
double Geodesic::face_angle_B(long n, double angle)
{
    double theta1, phi1, theta2, phi2;
    theta1 = sph[face_list[n].v1].theta;
    phi1 = sph[face_list[n].v1].phi;
    theta2 = sph[face_list[n].v3].theta;
    phi2 = sph[face_list[n].v3].phi;
    double a = chord_length(theta1, phi1, theta2, phi2);
    theta1 = sph[face_list[n].v2].theta;
    phi1 = sph[face_list[n].v2].phi;
    theta2 = sph[face_list[n].v3].theta;
    phi2 = sph[face_list[n].v3].phi;
    double c = chord_length(theta1, phi1, theta2, phi2);
    return asin(sin(DEG2RAD * angle) * a / c) * RAD2DEG;
}

// 0x1010bff0 ?face_angle_C@Geodesic@@QAENNN@Z
double Geodesic::face_angle_C(double A, double B)
{
    return 180.0 - A - B;
}

// 0x1010c010 ?save_ascii@Geodesic@@QAEXPAD@Z
void Geodesic::save_ascii(char *file)
{
    int i;
    double theta1, phi1, theta2, phi2;
    ofstream out(file);
    out << "Geodesic Dome Data" << '\n';
    out << "----------------Dome Parameters-------------------------" << '\n';
    out << "Polyhedron Type: ";
    if (polyhedron == 1)
        out << "Icosahedron" << '\n';
    else if (polyhedron == 2)
        out << "Octahedron" << '\n';
    else
        out << "Tetrahedron" << '\n';
    out << "Class ";
    if (classtype == 1)
        out << "I" << '\n';
    else
        out << "II" << '\n';
    out << "Frequency: " << frequency << '\n';
    out << "----------------Symmetry Triangle Data------------------" << '\n';
    out << "Vertexia: " << vertices_I << '\n';
    out << "Edges: " << edges_I << '\n';
    out << "Faces: " << faces_I << '\n';
    out << "-----------Symmetry Triangle Spherical Coordinates------" << '\n';
    out << "Vertex #" << '\t' << "phi" << '\t' << "theta" << '\n';
    for (i = 1; i <= vertices; i++) {
        out << setiosflags(ios::right) << setw(3) << i << '\t';
        out << setw(11) << setprecision(8) << setiosflags(ios::fixed) << sph[i].theta << '\t';
        out << sph[i].phi << '\n';
    }
    out << '\n';
    out << "----------Symmetry Triangle Chord Data------------------" << '\n';
    out << "Vertex" << '\n' << "start/end" << '\t' << "Chord Length" << '\n';
    for (i = 1; i <= edges; i++) {
        out << setiosflags(ios::right) << setw(3) << setw(12) << setprecision(8) << setiosflags(ios::fixed)
            << chords[i].start << "-" << chords[i].end << '\t';
        theta1 = sph[chords[i].start].theta;
        phi1 = sph[chords[i].start].phi;
        theta2 = sph[chords[i].end].theta;
        phi2 = sph[chords[i].end].phi;
        out << chord_length(theta1, phi1, theta2, phi2) << '\n';
    }
    out << "---------Symmetry Triangle Axial Angle Data-------------" << '\n';
    out << "Vertex" << '\n' << "start/end" << '\t' << "Axial #1" << '\t' << "Axial #2" << '\n';
    for (i = 1; i <= edges; i++) {
        out << setiosflags(ios::right) << setw(3) << setw(12) << setprecision(8) << setiosflags(ios::fixed)
            << chords[i].start << "-" << chords[i].end << '\t';
        out << axial_angle_A(i) << '\t' << axial_angle_B(i, axial_angle_A(i)) << '\n';
    }
    out << "---------Symmetry Triangle Face Angle Data-------------" << '\n';
    out << "Face" << '\t' << "Face Angle #1" << '\t' << "Face Angle #2" << '\t' << "Face Angle #3" << '\n';
    for (i = 1; i <= faces; i++) {
        out << setiosflags(ios::right) << setw(3) << setw(12) << setprecision(8) << setiosflags(ios::fixed)
            << i << '\t';
        out << face_angle_A(i) << '\t' << face_angle_B(i, face_angle_A(i)) << '\t';
        out << face_angle_C(face_angle_A(i), face_angle_B(i, face_angle_A(i))) << '\n';
    }
    out << "End Dome Data" << '\n';
    out.close();
}


// TODO 0x1010c8b0 ?chord_length@Geodesic@@QAENNNNN@Z: (55% similar) the original keeps both radian angles in memory (a new local and the phi1 slot) and loads them for every sin/cos; VC6 keeps them on the FPU stack (rotate_theta, same shape, matches)
double Geodesic::chord_length(double theta1, double phi1, double theta2, double phi2)
{
    double p1 = DEG2RAD * phi1;
    double p2 = DEG2RAD * phi2;
    return pow(2 - 2 * (cos(DEG2RAD * (theta1 - theta2)) * sin(p2) * sin(p1) + cos(p2) * cos(p1)), 0.5);
}

// 0x1010c920 ?rotate_phi@Geodesic@@QAENNNNN@Z
double Geodesic::rotate_phi(double theta1, double theta2, double phi1, double phi2)
{
    double value = sin((theta1 - theta2) * DEG2RAD) * sin(DEG2RAD * phi1) / sin(DEG2RAD * phi2);
    if (value > 1.0)
        value = 1.0;
    else if (value < -1.0)
        value = -1.0;
    return asin(value) * RAD2DEG;
}

// 0x1010c9b0 ?rotate_theta@Geodesic@@QAENNNNN@Z
double Geodesic::rotate_theta(double theta1, double theta2, double phi1, double phi2)
{
    double p1 = DEG2RAD * phi1;
    double p2 = DEG2RAD * phi2;
    double value = cos(DEG2RAD * (theta1 - theta2)) * sin(p2) * sin(p1) + cos(p2) * cos(p1);
    if (value > 1.0)
        value = 1.0;
    else if (value < -1.0)
        value = -1.0;
    return acos(value) * RAD2DEG;
}

// 0x1010ca60 ?tetra_angle@Geodesic@@QAEJXZ
long Geodesic::tetra_angle()
{
    double theta = rotate_theta(240.0, sph[1].theta, acos(-1.0 / 3.0) * RAD2DEG, sph[1].phi);
    double phi = rotate_phi(240.0, sph[1].theta, sph[1].phi, theta);
    double last = clean_float(cos(DEG2RAD * phi) * sin(DEG2RAD * theta));
    long n = 1;
    for (long i = 2; i <= freq; i++) {
        n += i - 1;
        theta = rotate_theta(240.0, sph[n].theta, acos(-1.0 / 3.0) * RAD2DEG, sph[n].phi);
        phi = rotate_phi(240.0, sph[n].theta, sph[n].phi, theta);
        double z = clean_float(cos(DEG2RAD * phi) * sin(DEG2RAD * theta));
        if (z - last > 0.0)
            return label[n].row;
        last = z;
    }
    if (freq == 4)
        return 3;
    if (freq == 1)
        return 1;
    return 2;
}


// 0x1010cc20 ?display_data@Geodesic@@QAEXXZ
void Geodesic::display_data()
{
    if (classtype == 1) {
        vertices = vertices_I;
        edges = edges_I;
    } else if (classtype == 2) {
        vertices = vertices_II;
        edges = edges_II;
    }
    cout << "Geodesic Dome Data" << '\n';
    cout << "----------------Dome Parameters-------------------------" << '\n';
    cout << "Polyhedron Type: ";
    if (polyhedron == 1)
        cout << "Icosahedron" << '\n';
    else if (polyhedron == 2)
        cout << "Octahedron" << '\n';
    else
        cout << "Tetrahedron" << '\n';
    cout << "Class ";
    if (classtype == 1)
        cout << "I" << '\n';
    else
        cout << "II" << '\n';
    cout << "Frequency: " << frequency << '\n';
    if (buckyball)
        cout << "----------------Buckyball Data--------------------------" << '\n';
    else
        cout << "----------------Symmetry Triangle Data------------------" << '\n';
    if (buckyball) {
        cout << "Vertexia: " << vertices_B << '\n';
        cout << "Edges: " << edges_B << '\n';
        cout << "Faces: " << faces_B << '\n' << '\n';
    } else {
        cout << "Vertexia: " << vertices << '\n';
        cout << "Edges: " << edges << '\n';
        cout << "Faces: " << faces << '\n' << '\n';
    }
}

// 0x1010ce00 ?bucky_factor@Geodesic@@QAEXXZ
void Geodesic::bucky_factor()
{
    long n, k, c;
    if (!silent) {
        cout << "Calculating Buckyball Chord Factors... ";
        spin = 0;
    }
    chords = new chord[edges_I + 1];
    if (chords == 0) {
        cout << "Insufficient memory for chord array --- Execution Terminating" << '\n';
        exit(1);
    }
    c = 1;
    for (n = 1; n < vertices; n++) {
        if (!silent) {
            time_passage(spin);
            spin++;
            if (spin > 3)
                spin = 0;
        }
        if (fmod(label[n].row + label[n].col, 3.0) == 0)
            continue;
        if (label[n].row == freq && label[n].col == 0) {
            chords[c].start = n;
            for (k = n; k <= vertices; k++)
                if (label[k].row == label[n].row && label[k].col == label[n].col + 1)
                    break;
            if (fmod(label[k].row + label[k].col, 3.0) != 0) {
                chords[c].end = k;
                c++;
            }
        } else if (label[n].row == label[n].col) {
            chords[c].start = n;
            for (k = n; k <= vertices; k++)
                if (label[k].row == label[n].row + 1 && label[k].col == label[n].col)
                    break;
            if (fmod(label[k].row + label[k].col, 3.0) != 0) {
                chords[c].end = k;
                c++;
            }
            chords[c].start = n;
            for (k = n; k <= vertices; k++)
                if (label[k].row == label[n].row + 1 && label[k].col == label[n].col + 1)
                    break;
            if (fmod(label[k].row + label[k].col, 3.0) != 0) {
                chords[c].end = k;
                c++;
            }
        } else if (label[n].row == freq) {
            chords[c].start = n;
            for (k = n; k <= vertices; k++)
                if (label[k].row == label[n].row && label[k].col == label[n].col + 1)
                    break;
            if (fmod(label[k].row + label[k].col, 3.0) != 0) {
                chords[c].end = k;
                c++;
            }
        } else if (label[n].col == 0) {
            chords[c].start = n;
            for (k = n; k <= vertices; k++)
                if (label[k].row == label[n].row && label[k].col == label[n].col + 1)
                    break;
            if (fmod(label[k].row + label[k].col, 3.0) != 0) {
                chords[c].end = k;
                c++;
            }
            chords[c].start = n;
            for (k = n; k <= vertices; k++)
                if (label[k].row == label[n].row + 1 && label[k].col == label[n].col)
                    break;
            if (fmod(label[k].row + label[k].col, 3.0) != 0) {
                chords[c].end = k;
                c++;
            }
            chords[c].start = n;
            for (k = n; k <= vertices; k++)
                if (label[k].row == label[n].row + 1 && label[k].col == label[n].col + 1)
                    break;
            if (fmod(label[k].row + label[k].col, 3.0) != 0) {
                chords[c].end = k;
                c++;
            }
        } else {
            chords[c].start = n;
            for (k = n; k <= vertices; k++)
                if (label[k].row == label[n].row && label[k].col == label[n].col + 1)
                    break;
            if (fmod(label[k].row + label[k].col, 3.0) != 0) {
                chords[c].end = k;
                c++;
            }
            chords[c].start = n;
            for (k = n; k <= vertices; k++)
                if (label[k].row == label[n].row + 1 && label[k].col == label[n].col)
                    break;
            if (fmod(label[k].row + label[k].col, 3.0) != 0) {
                chords[c].end = k;
                c++;
            }
            chords[c].start = n;
            for (k = n; k <= vertices; k++)
                if (label[k].row == label[n].row + 1 && label[k].col == label[n].col + 1)
                    break;
            if (fmod(label[k].row + label[k].col, 3.0) != 0) {
                chords[c].end = k;
                c++;
            }
        }
    }
    cout << '\r' << "                                        " << '\r';
}


// Matched by this code (iostream inline instances):
// 0x10109a70 ?__setiosflags@@YAAAVios@@AAV1@J@Z
// 0x10109ad0 ??_Dofstream@@QAEXXZ
