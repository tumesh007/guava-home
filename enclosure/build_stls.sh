#!/usr/bin/env bash
set -e
mkdir -p stl
echo "Rendering Base STL..."
openscad -o stl/guava_home_base.stl export_base.scad
echo "Rendering Lid STL..."
openscad -o stl/guava_home_lid.stl export_lid.scad
echo "Rendering Fit-Test Slice STL..."
openscad -o stl/guava_home_test_slice.stl export_test_slice.scad
echo "Done! STLs saved in ./stl/"
