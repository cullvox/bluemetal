#!/bin/bash

for shader in *.frag *.vert 
do 
        glslc "$shader" -o "$shader.spv"
done