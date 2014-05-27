/* 
 * File:   meshResource.h
 * Author: miles
 *
 * Created on April 2, 2014, 9:04 PM
 */

#ifndef __LS_MESH_RESOURCE_H__
#define	__LS_MESH_RESOURCE_H__

#include <cstdint>
#include "main.h"
#include "resource.h"
#include "vertex.h"
#include "textureAtlas.h"

/**
 * The mesh resource can be used to load a mesh or meshes from a file. It can
 * also be used to generate basic primitives such as a sphere, cube, cone, etc.
 */
class meshResource final : public resource {
    private:
        /**
         * Contains the number of vertices that were loaded into memory.
         */
        unsigned numMeshes = 0;
        
        /**
         * Contains the number of vertices used per mesh.
         * This is implemented as a single array of unsigned integers.
         */
        unsigned* numVertices = nullptr;
        
        /**
         * Contains the vertex data used by each mesh.
         * This is implemented as an array of vertex arrays.
         */
        vertex** vertices = nullptr;
        
        bool initArrays(unsigned meshCount);
        bool initMeshAt(unsigned meshIndex, unsigned vertCount);
        
    public:
        /**
         * Constructor
         */
        constexpr meshResource() {}
        
        /**
         * Copy Constructor -- DELETED
         */
        meshResource(const meshResource&) = delete;
        
        /**
         * Move Constructor
         * Moves all data from the input parameter into *this. No copies are
         * performed during the operation.
         */
        meshResource(meshResource&&);
        
        
        /**
         * Destructor
         * Unloads all data contain within *this.
         */
        virtual ~meshResource() {
            unload();
        }
        
        /**
         * Copy Constructor -- DELETED
         */
        meshResource& operator=(const meshResource&) = delete;
        
        /**
         * Move Operator
         * Moves all data from the input parameter into *this. No copies are
         * performed during the operation.
         */
        meshResource& operator=(meshResource&&);
        
        /**
         * Get the number of loaded meshes
         */
        inline unsigned getNumMeshes() const {
            return numMeshes;
        }
        
        /**
         * Get the number of loaded position variables in each mesh
         */
        inline unsigned getNumVertices(unsigned meshIndex) const {
            if (vertices != nullptr && numVertices != nullptr && meshIndex < numMeshes) {
                return numVertices[meshIndex];
            }
            return 0;
        }
        
        /**
         * Get the array of positions per mesh
         */
        inline vertex* getVertices(unsigned meshIndex) const {
            if (vertices != nullptr && meshIndex < numMeshes) {
                return vertices[meshIndex];
            }
            return nullptr;
        }
        
        /**
         * Unload
         * Free all memory used by *this.
         */
        virtual void unload();

        /**
         * Load a file using all vertex types (positions, normals, colors, UVs,
         * tangents and bi-tangents)
         * 
         * @param filename a c-style string containing the relative path name
         * to a file that should be loadable into memory.
         * 
         * @return true if the file was successfully loaded. False if not.
         */
        bool loadFile(const char* filename) override;

        /**
         * Save a file
         * 
         * @param filename a c-style string containing the relative path name
         * to a file that should be saved to the computer.
         * 
         * @return true if the file was successfully saved. False if not.
         */
        virtual bool saveFile(const char* filename) const {
            (void)filename;
            return false;
        }
        
        /**
         * N-Sided polygon primitive loading method
         * 
         * @param vertTypes
         * A bit-field of different vertex types that should be loaded.
         * @see vertex_desc_t from mesh.h
         * 
         * @param numPoints
         * The number of points that the polygon should contain. This number
         * must be greater than 1.
         * 
         * @return
         * TRUE if the function loaded the mesh correctly, or
         * FALSE if an error occurred during the load.
         */
        bool loadPolygon(unsigned numPoints);
        
        /**
         * Triangle primitive loading method
         * 
         * @param vertTypes
         * A bit-field of different vertex types that should be loaded.
         * @see vertex_desc_t from mesh.h
         * 
         * @return
         * TRUE if the function loaded the mesh correctly, or
         * FALSE if an error occurred during the load.
         * 
         * Triangle texture coordinates are as follows:
         * 
         * |----1---|
         * |   /\   |
         * |  /  \  |
         * | /    \ |
         * 0/______\2
         */
        bool loadTriangle();
        
        bool loadText(const textureAtlas&, const std::string&);
};

#endif	/* __LS_MESH_RESOURCE_H__ */
