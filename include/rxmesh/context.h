#pragma once

#include <stdint.h>
#include "rxmesh/patch_info.h"
#include "rxmesh/patch_scheduler.cuh"
#include "rxmesh/util/macros.h"

namespace rxmesh {

/**
 * @brief context for the mesh parameters and pointers. Everything is allocated
 * on and managed by RXMesh. This class is meant to be a vehicle to copy various
 * parameters to the device kernels.
 */
class Context
{
   public:
    friend class RXMesh;
    friend class RXMeshDynamic;

    /**
     * @brief Default constructor
     */
    __host__ __device__ Context()
        : m_num_edges(nullptr),
          m_num_faces(nullptr),
          m_num_vertices(nullptr),
          m_num_patches(nullptr),
          m_vertex_prefix(nullptr),
          m_edge_prefix(nullptr),
          m_face_prefix(nullptr),
          m_capacity_factor(0.0f),
          m_patches_info(nullptr)
    {
    }

    Context(const Context&) = default;

    /**
     * @brief Total number of edges in mesh
     */
    __device__ __forceinline__ uint32_t* get_num_edges()
    {
        return m_num_edges;
    }

    /**
     * @brief Total number of faces in mesh
     */
    __device__ __forceinline__ uint32_t* get_num_faces()
    {
        return m_num_faces;
    }

    /**
     * @brief Total number of vertices in mesh
     */
    __device__ __forceinline__ uint32_t* get_num_vertices()
    {
        return m_num_vertices;
    }

    /**
     * @brief Unpack an edge to its edge ID and direction
     * @param edge_dir The input packed edge as stored in PatchInfo and
     * internally in RXMesh
     * @param edge The unpacked edge ID
     * @param dir The unpacked edge direction
     */
    static __device__ __host__ __forceinline__ void
    unpack_edge_dir(const uint16_t edge_dir, uint16_t& edge, flag_t& dir)
    {
        dir  = (edge_dir & 1) != 0;
        edge = edge_dir >> 1;
    }


    /**
     * @brief initialize various members
     * @param num_vertices total number of vertices in the mesh
     * @param num_edges total number of edges in the mesh
     * @param num_faces total number of faces in the mesh
     * @param max_num_vertices max number of vertices in a patch
     * @param max_num_edges max number of edges in a patch
     * @param max_num_faces max number of faces in a patch
     * @param num_patches number of patches
     * @param patches pointer to PatchInfo that contains different info about
     * the patches
     */
    void init(const uint32_t num_vertices,
              const uint32_t num_edges,
              const uint32_t num_faces,
              const uint32_t max_num_vertices,
              const uint32_t max_num_edges,
              const uint32_t max_num_faces,
              const uint32_t num_patches,
              const float    capacity_factor,
              uint32_t*      vertex_prefix,
              uint32_t*      edge_prefix,
              uint32_t*      face_prefix,
              PatchInfo*     d_patches,
              PatchScheduler scheduler)
    {
        uint32_t* buffer = nullptr;
        CUDA_ERROR(cudaMalloc((void**)&buffer, 7 * sizeof(uint32_t)));
        m_num_vertices     = buffer + 0;
        m_num_edges        = buffer + 1;
        m_num_faces        = buffer + 2;
        m_num_patches      = buffer + 3;
        m_max_num_vertices = buffer + 4;
        m_max_num_edges    = buffer + 5;
        m_max_num_faces    = buffer + 6;
        m_capacity_factor  = capacity_factor;

        CUDA_ERROR(cudaMemcpy(m_num_vertices,
                              &num_vertices,
                              sizeof(uint32_t),
                              cudaMemcpyHostToDevice));
        CUDA_ERROR(cudaMemcpy(
            m_num_edges, &num_edges, sizeof(uint32_t), cudaMemcpyHostToDevice));
        CUDA_ERROR(cudaMemcpy(
            m_num_faces, &num_faces, sizeof(uint32_t), cudaMemcpyHostToDevice));
        CUDA_ERROR(cudaMemcpy(m_num_patches,
                              &num_patches,
                              sizeof(uint32_t),
                              cudaMemcpyHostToDevice));

        CUDA_ERROR(cudaMemcpy(m_max_num_vertices,
                              &max_num_vertices,
                              sizeof(uint32_t),
                              cudaMemcpyHostToDevice));
        CUDA_ERROR(cudaMemcpy(m_max_num_edges,
                              &max_num_edges,
                              sizeof(uint32_t),
                              cudaMemcpyHostToDevice));
        CUDA_ERROR(cudaMemcpy(m_max_num_faces,
                              &max_num_faces,
                              sizeof(uint32_t),
                              cudaMemcpyHostToDevice));

        m_vertex_prefix = vertex_prefix;
        m_edge_prefix   = edge_prefix;
        m_face_prefix   = face_prefix;

        m_patches_info = d_patches;

        m_patch_scheduler = scheduler;
    }

    void release()
    {
        CUDA_ERROR(cudaFree(m_num_vertices));
    }


    uint32_t *     m_num_edges, *m_num_faces, *m_num_vertices, *m_num_patches;
    uint32_t *     m_max_num_vertices, *m_max_num_edges, *m_max_num_faces;
    uint32_t *     m_vertex_prefix, *m_edge_prefix, *m_face_prefix;
    PatchInfo*     m_patches_info;
    float          m_capacity_factor;
    PatchScheduler m_patch_scheduler;
};
}  // namespace rxmesh