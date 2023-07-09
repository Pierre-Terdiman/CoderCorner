

 Consolidation code from Flexporter, using Meshmerizer.

 -------------------------------------------------------------------------

   Flexporter is a free MAX exporter (www.codercorner.com/Flexporter.htm)
   Meshmerizer is the part of ICE responsible for all computational geometry.
   ICE is my private development framework.

 -------------------------------------------------------------------------

 Mesh consolidation is the process by which a mesh is cleaned before actually
 beeing used. Consolidation involves fixing as well as optimizing the input
 data set, and is a vital part of any production pipeline.

 The input data set usually comes from your modeler - 3DS MAX, Maya, whatever.
 It may be the modeler's native format if you're coding your own exporter, or
 the compulsory format from the exporter in use. In this last case the exporter
 probably already has transformed, optimized and cleaned meshes before leaving
 them to you. Transforming the input data set into hardware-friendly data (say
 vertex buffers or vertex arrays) is a tedious task, and your code must be
 flexible enough to follow the hardware evolution. One day you use indexed
 trilists, one day you want triangle strips with or without swaps, another day
 you need strided data, and on top of that you may want to take advantage of
 hardware-accelerated vertex blending for your characters, which again needs a
 different format - or at least extra data.

 MAX provides access to different sets of indices for vertices, UVs and colors.
 In other words, you have standard faces whose references are indices into a list
 of N1 vertices, texture-faces whose references are indices into a list of N2
 mapping coordinates, and color-faces whose references are indices into a list of
 N3 vertex colors. Then the job is to put all of them in a mixer and output,
 say indexed triangle strips referencing 9D entities (x,y,z for the position,
 Nx,Ny,Nz for the vertex normal, texture coordinates UVs and maybe a dword diffuse
 color).

 That's what this code do. This is the mixer.

 -------------------------------------------------------------------------

 Quick features:
 - Packs multiple independent topologies in a single hardware-friendly one.
 - Tracks and deletes unused or redundant vertices, mapping coordinates or
   vertex colors.
 - Kills zero-area faces.
 - Computes normals according to smoothing groups.
 - Outputs normal information used to recompute them easily at runtime.
 - Groups faces in submeshes, according to their rendering properties.
 - Duplicates as few vertices as necessary.

 -------------------------------------------------------------------------

 Need strips on top of that?
 Here: www.codercorner.com/Strips.htm

 -------------------------------------------------------------------------

 Use it freely in any commercial or non-commercial project. I like to believe
 free code can *also* be well-designed, robust and efficient.

 Quality rules. Courtesy of old Atari ST coders.


 Comments & bug reports are welcome.




 Pierre Terdiman

 February, 17, 2001
 p.terdiman@codercorner.com
 p.terdiman@wanadoo.fr

