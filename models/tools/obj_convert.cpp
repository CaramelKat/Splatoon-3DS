//
// Created by ash on 9/16/22.
//

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <cmath>
#include <cstdint>
#include <vector>

using vec2 = struct { float x; float y; };
using vec3 = struct { float x; float y; float z; };
using vertex = struct { vec3 position; vec2 uv; vec3 normal; };

/*
 * https://github.com/opengl-tutorials/ogl/blob/master/common/vboindexer.cpp
 */

// Returns true iif v1 can be considered equal to v2
bool is_near(float v1, float v2){
    return std::fabs( v1-v2 ) < 0.01f;
}

// Searches through all already-exported vertices
// for a similar one.
// Similar = same position + same UVs + same normal
bool getSimilarVertexIndex(
    const vec3 & in_vertex,
    const vec2 & in_uv,
    const vec3 & in_normal,
    std::vector<vertex> & out_vertices,
    unsigned short & result
){
    // Lame linear search
    for ( unsigned int i=0; i<out_vertices.size(); i++ ){
        if (
            is_near( in_vertex.x , out_vertices[i].position.x ) &&
            is_near( in_vertex.y , out_vertices[i].position.y ) &&
            is_near( in_vertex.z , out_vertices[i].position.z ) &&
            is_near( in_uv.x     , out_vertices[i].uv.x ) &&
            is_near( in_uv.y     , out_vertices[i].uv.y ) &&
            is_near( in_normal.x , out_vertices[i].normal.x ) &&
            is_near( in_normal.y , out_vertices[i].normal.y ) &&
            is_near( in_normal.z , out_vertices[i].normal.z )
            ){
            result = i;
            return true;
        }
    }
    // No other vertex could be used instead.
    // Looks like we'll have to add it to the VBO.
    return false;
}

void indexVBO_slow(
    const std::vector<vec3> & in_vertices,
    const std::vector<vec2> & in_uvs,
    const std::vector<vec3> & in_normals,

    std::vector<unsigned short> & out_indices,
    std::vector<vertex> & out_vertices
) {
    // For each input vertex
    for ( unsigned int i=0; i<in_vertices.size(); i++ ){

        // Try to find a similar vertex in out_XXXX
        unsigned short index;
        bool found = getSimilarVertexIndex(
            in_vertices[i],
            in_uvs[i],
            in_normals[i],
            out_vertices,
            index
        );

        if ( found ){ // A similar vertex is already in the VBO, use it instead !
            out_indices.push_back( index );
        }else{ // If not, it needs to be added in the output data.
            out_vertices.push_back( { in_vertices[i], in_uvs[i], in_normals[i] } );
            //out_uvs     .push_back( in_uvs[i]);
            out_indices .push_back( (unsigned short)out_vertices.size() - 1 );
        }
    }
}

int main(int argc, char** argv) {
    if (argc < 3) {
        printf("no\n");
        return -1;
    }

    tinyobj::ObjReader reader;
    auto res = reader.ParseFromFile(argv[1]);
    if (!res) {
        printf("Couldn't parse model\n");
        return -1;
    }

    tinyobj::shape_t shape = reader.GetShapes()[0];
    const tinyobj::attrib_t& attrib = reader.GetAttrib();

/*  Get total vertex count for this shape */
    size_t total_vertex_count = 0;
    for (size_t face = 0; face < shape.mesh.num_face_vertices.size(); face++) {
        size_t vertex_count = shape.mesh.num_face_vertices[face];
        if (vertex_count != 3) {
            printf("Model is not triangulated\n");
            return -1;
        }

        total_vertex_count += vertex_count;
    }

    std::vector<vec3> flat_vertexes(total_vertex_count);
    std::vector<vec2> flat_uvs(total_vertex_count);
    std::vector<vec3> flat_normals(total_vertex_count);

    /*  We're going to de-indexify this model and make a flat buffer of attributes. */
    size_t out_vertex = 0;
/*  Each face might have a different number of vertexes, so we'll use an offset
    variable to index into mesh.indices instead of the usual i*size approach */
    size_t face_offset = 0;
    for (size_t face = 0; face < shape.mesh.num_face_vertices.size(); face++) {
        size_t vertex_count = shape.mesh.num_face_vertices[face];

        for (size_t vertex = 0; vertex < vertex_count; vertex++) {
            tinyobj::index_t indexes = shape.mesh.indices[face_offset + vertex];
            size_t vertex_idx = 3 * indexes.vertex_index;
            size_t uv_idx = 2 * indexes.texcoord_index;
            size_t normal_idx = 3 * indexes.normal_index;

            flat_vertexes[out_vertex].x = attrib.vertices[vertex_idx + 0];
            flat_vertexes[out_vertex].y = attrib.vertices[vertex_idx + 1];
            flat_vertexes[out_vertex].z = attrib.vertices[vertex_idx + 2];
            flat_uvs[out_vertex].x = attrib.texcoords[uv_idx + 0];
            flat_uvs[out_vertex].y = attrib.texcoords[uv_idx + 1];
            flat_normals[out_vertex].x = attrib.normals[normal_idx + 0];
            flat_normals[out_vertex].y = attrib.normals[normal_idx + 1];
            flat_normals[out_vertex].z = attrib.normals[normal_idx + 2];

            out_vertex++;
        }
        face_offset += vertex_count;
    }

    printf("unflattened: %ld\n", flat_vertexes.size());

    std::vector<unsigned short> indexes;
    std::vector<vertex> vertexes;
    indexVBO_slow(flat_vertexes, flat_uvs, flat_normals, indexes, vertexes);

    FILE* outfd = fopen(argv[2], "wb");
    if (!outfd) {
        printf("can't open outfile!\n");
        return -1;
    }

    struct {
        uint32_t dtype;
        uint32_t indexes_sz;
        uint32_t vertexes_sz;
    } header;

    printf("indexed: %ld\n", vertexes.size());

    std::string filename(argv[2]);

    if (vertexes.size() < flat_vertexes.size() && filename.find("level") == std::string::npos) {
        printf("saving indexed\n");
        header.dtype = 1;
        header.indexes_sz = (uint32_t)indexes.size();
        header.vertexes_sz = (uint32_t)vertexes.size();
        fwrite(&header, sizeof(header), 1, outfd);
        fwrite(indexes.data(), sizeof(indexes[0]), indexes.size(), outfd);
        fwrite(vertexes.data(), sizeof(vertexes[0]), vertexes.size(), outfd);
        fclose(outfd);
    } else {
        // well that was pointless, just write flattened
        printf("saving flat\n");
        std::vector<vertex> flat_data(flat_vertexes.size());
        for (int i = 0; i < flat_data.size(); i++) {
            flat_data[i] = { flat_vertexes[i], flat_uvs[i], flat_normals[i] };
        }
        header.dtype = 0;
        header.indexes_sz = 0;
        header.vertexes_sz = flat_data.size();
        fwrite(&header, sizeof(header), 1, outfd);
        fwrite(flat_data.data(), sizeof(flat_data[0]), flat_data.size(), outfd);
        fclose(outfd);
    }
}
