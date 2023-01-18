# INplot3d_to_tets
A small and portable utility to turn a single-block structured 3D-grid to
tetrahedra. Written by IN <nompelis@nobelware.com>


SUMMARY

This is a portable utility (written entirely in C) to turn a structured grid,
given as a single-block of k,j,i-ordered nodes, to an unstructured format made
of entirely tetrahedra. The tetrahedra are created by subdivisions of the
hexahedral cells in the structured grid; care is taken to make sure that the
faces of the tetrahedra conform. The result is a Fluent case-file format file,
which contains the zones with the nodes, faces (in 7 groups), and cells. The
face groups consist of 1 group for interior faces and 6 groups corresponding
to the six faces of the k,j,i structured block.

The utility can be incorporated into software by being compiled in it or as a
shared object (.so / .dll).

An example driver is included, along with a plot3d file to use.

LIMITATIONS

This utility treats only a _single_ structured block. It can only use a 3D grid,
given that the output is in three-dimensional simplex objects (tetrahedra).


IN 2018/03/28

