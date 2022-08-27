#include "gtest/gtest.h"

#include "rxmesh/kernels/update_dispatcher.cuh"
#include "rxmesh/rxmesh_dynamic.h"

template <uint32_t blockThreads>
__global__ static void dynamic_kernel(rxmesh::Context context)
{
    using namespace rxmesh;
    namespace cg           = cooperative_groups;
    cg::thread_block block = cg::this_thread_block();
    ShmemAllocator   shrd_alloc;
    PatchInfo        patch_info = context.get_patches_info()[blockIdx.x];
    Cavity<blockThreads, CavityOp::E> cavity(block, shrd_alloc, patch_info);

    for_each_dispatcher<Op::E, blockThreads>(context, [&](const EdgeHandle eh) {
        // TODO user-defined condition
        if (eh.unpack().second == 10) {
            cavity.add(eh);
        }
    });

    cavity.process(block, shrd_alloc, patch_info);

}

TEST(RXMeshDynamic, Cavity)
{
    using namespace rxmesh;
    cuda_query(rxmesh_args.device_id, rxmesh_args.quite);

    // RXMeshDynamic rx(STRINGIFY(INPUT_DIR) "sphere3.obj", rxmesh_args.quite);
    // rx.save(STRINGIFY(OUTPUT_DIR) "sphere3_patcher");

    RXMeshDynamic rx(STRINGIFY(INPUT_DIR) "sphere3.obj",
                     rxmesh_args.quite,
                     STRINGIFY(OUTPUT_DIR) "sphere3_patcher");

    auto coords = rx.get_input_vertex_coordinates();


    constexpr uint32_t      blockThreads = 256;
    LaunchBox<blockThreads> launch_box;
    rx.prepare_launch_box({}, launch_box, (void*)dynamic_kernel<blockThreads>);

    dynamic_kernel<blockThreads>
        <<<launch_box.blocks,
           launch_box.num_threads,
           launch_box.smem_bytes_dyn>>>(rx.get_context());

    CUDA_ERROR(cudaDeviceSynchronize());

    rx.update_host();
    EXPECT_TRUE(rx.validate());
}