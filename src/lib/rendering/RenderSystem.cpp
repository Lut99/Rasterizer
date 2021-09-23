/* RENDER SYSTEM.cpp
 *   by Lut99
 *
 * Created:
 *   20/07/2021, 15:10:25
 * Last edited:
 *   9/20/2021, 10:51:53 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Defines the rendering system, which is capable of showing the entities
 *   that have a Render component on the screen. Also uses the Transform
 *   component to decide where to place the entity.
**/

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "tools/Common.hpp"
#include "ecs/components/Transform.hpp"
#include "ecs/components/Model.hpp"
#include "ecs/components/Camera.hpp"
#include "models/ModelSystem.hpp"

#include "auxillary/ErrorCodes.hpp"
#include "auxillary/Rectangle.hpp"
#include "auxillary/Vertex.hpp"
#include "auxillary/Index.hpp"

#include "RenderSystem.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::ECS;
using namespace Makma3D::Rendering;


/***** HELPER STRUCTS *****/
/* Used to store data for a single index buffer to render. */
struct IndexBufferRenderData {
    /* The entity to which the index buffer belongs. */
    ECS::entity_t entity;
    /* The vertex buffer of this entity. */
    Rendering::Buffer* vertex_buffer;
    /* The index buffer to render. */
    Rendering::Buffer* index_buffer;
    /* The number of indices to render. */
    uint32_t n_indices;

};





/***** HELPER FUNCTIONS *****/
/* Sorts given list of 'entities' (list of their Model components) in such a way that they can be rendered material-by-material efficiently. */
static std::unordered_map<Materials::MaterialType, std::unordered_map<Materials::material_t, Tools::Array<IndexBufferRenderData>>> sort_entities(const Materials::MaterialSystem& material_system, const ECS::ComponentList<ECS::Model>& entities) {
    // Delcare the result array
    std::unordered_map<Materials::MaterialType, std::unordered_map<Materials::material_t, Tools::Array<IndexBufferRenderData>>> result;

    // Loop through the entities in the given list to sort them
    for (uint32_t i = 0; i < entities.size(); i++) {
        // Get the entity & their model
        ECS::entity_t entity = entities.get_entity(i);
        const ECS::Model& model = entities[i];

        // Loop through the model's meshes and then per-mesh index buffers
        for (uint32_t j = 0; j < model.meshes.size(); j++) {
            const ECS::Mesh& mesh = model.meshes[j];
            for (uint32_t k = 0; k < mesh.indices.size(); k++) {
                // Get the data for this index buffer
                Rendering::Buffer* indices = mesh.indices[k];
                uint32_t n_indices = mesh.n_indices[k];
                Materials::material_t material = mesh.materials[k];
                // Also get the material type for this material
                Materials::MaterialType material_type = material_system.get_type(material);

                // With that collected, we first check if we have already seen a material with this type
                std::unordered_map<Materials::MaterialType, std::unordered_map<Materials::material_t, Tools::Array<IndexBufferRenderData>>>::iterator type_iter = result.find(material_type);
                if (type_iter == result.end()) {
                    type_iter = result.insert({ material_type, {} }).first;
                }

                // Next, check if we have already seen index buffers of this type
                std::unordered_map<Materials::material_t, Tools::Array<IndexBufferRenderData>>::iterator material_iter = (*type_iter).second.find(material);
                if (material_iter == (*type_iter).second.end()) {
                    material_iter = (*type_iter).second.insert({ material, Tools::Array<IndexBufferRenderData>(16) }).first;
                }

                // Populate the IndexBufferRenderData and add it to the list, resizing it in a more optimal fashion
                Tools::Array<IndexBufferRenderData>& indices_to_render = (*material_iter).second;
                if (indices_to_render.size() >= indices_to_render.capacity()) { indices_to_render.reserve(2 * indices_to_render.capacity()); }
                indices_to_render.push_back({
                    entity,
                    model.vertices,
                    indices,
                    n_indices
                });
            }
        }
    }

    // Done! Return the list
    return result;
}





/***** RENDERSYSTEM CLASS *****/
/* Constructor for the RenderSystem, which takes a window, a memory manager to render (to and draw memory from, respectively), a material system to create pipelines with, a model system to schedule the model buffers with and a texture system to schedule texture images with. */
RenderSystem::RenderSystem(Window& window, MemoryManager& memory_manager, const Materials::MaterialSystem& material_system, const Models::ModelSystem& model_system/*, const Textures::TextureSystem& texture_system*/) :
    window(window),
    memory_manager(memory_manager),
    material_system(material_system),
    model_system(model_system),
    // texture_system(texture_system),

    global_descriptor_layout(this->window.gpu()),
    material_descriptor_layout(this->window.gpu()),
    object_descriptor_layout(this->window.gpu()),

    depth_stencil(this->window.gpu(), this->memory_manager.draw_pool, this->window.swapchain().extent()),

    shader_pool(this->window.gpu()),

    render_pass(this->window.gpu()),

    pipeline_cache(this->window.gpu(), get_executable_path() + "/pipeline.cache"),
    pipeline_constructor(this->window.gpu(), this->pipeline_cache)
{
    // Initialize the descriptor set layout for the global data
    this->global_descriptor_layout.add_binding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);
    this->global_descriptor_layout.finalize();

    // Initialize the descritpor set layout for the per-material data
    Materials::MaterialSystem::init_layout(this->material_descriptor_layout);

    // Initialize the descriptor set layout for the per-object data
    this->object_descriptor_layout.add_binding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);
    this->object_descriptor_layout.finalize();

    // Initialize the render pass
    uint32_t col_index = this->render_pass.add_attachment(this->window.swapchain().format(), VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    uint32_t dep_index = this->render_pass.add_attachment(this->depth_stencil.format(), VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    this->render_pass.add_subpass({ std::make_pair(col_index, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) }, std::make_pair(dep_index, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL));
    this->render_pass.add_dependency(VK_SUBPASS_EXTERNAL, col_index, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, 0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
    this->render_pass.finalize();

    // Prepare pipeline construction by settings the constructor properties
    this->pipeline_constructor.vertex_input_state = VertexInputState(
        { VertexBinding(0, sizeof(Vertex)) },
        {
            VertexAttribute(0, 0, offsetof(Vertex, pos), VK_FORMAT_R32G32B32_SFLOAT),
            VertexAttribute(0, 1, offsetof(Vertex, colour), VK_FORMAT_R32G32B32_SFLOAT),
            VertexAttribute(0, 2, offsetof(Vertex, texel), VK_FORMAT_R32G32_SFLOAT)
        }
    );
    this->pipeline_constructor.input_assembly_state = InputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    this->pipeline_constructor.depth_testing = DepthTesting(VK_TRUE, VK_COMPARE_OP_LESS);
    this->pipeline_constructor.viewport_transformation = ViewportTransformation(VkOffset2D{ 0, 0 }, this->window.swapchain().extent(), VkOffset2D{ 0, 0 }, this->window.swapchain().extent());
    this->pipeline_constructor.rasterization = Rasterization(VK_TRUE, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE);
    // this->pipeline_constructor.rasterization = Rasterization(VK_TRUE, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE);
    this->pipeline_constructor.multisampling = Multisampling();
    this->pipeline_constructor.color_logic = ColorLogic(
        VK_FALSE, VK_LOGIC_OP_NO_OP,
        { ColorBlending(0, VK_FALSE, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD) }
    );
    this->pipeline_constructor.pipeline_layout = PipelineLayout({ this->global_descriptor_layout, this->material_descriptor_layout, this->object_descriptor_layout }, {});

    // Create the pipeline for all materials
    for (uint32_t i = 0; i < Materials::MaterialSystem::n_types; i++) {
        // Modify the pipeline constructor for this type
        this->material_system.init_props(Materials::MaterialSystem::types[i], this->shader_pool, this->pipeline_constructor);

        // Construct the new pipeline and insert it into the list
        this->pipelines.insert({
            Materials::MaterialSystem::types[i],
            this->pipeline_constructor.construct(this->render_pass, 0, VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT)
        });
    }

    // Initialize the frame manager
    this->frame_manager = new FrameManager(this->memory_manager, this->window.swapchain(), this->global_descriptor_layout, this->material_descriptor_layout, this->object_descriptor_layout);
    this->frame_manager->bind(this->render_pass, this->depth_stencil);

    // Done initializing
    logger.logc(Verbosity::important, RenderSystem::channel, "Init success.");
}

/* Move constructor for the RenderSystem class. */
RenderSystem::RenderSystem(RenderSystem&& other)  :
    window(other.window),
    memory_manager(other.memory_manager),
    material_system(other.material_system),
    model_system(other.model_system),
    // texture_system(other.texture_system),

    global_descriptor_layout(std::move(other.global_descriptor_layout)),
    material_descriptor_layout(std::move(other.material_descriptor_layout)),
    object_descriptor_layout(std::move(other.object_descriptor_layout)),

    depth_stencil(std::move(other.depth_stencil)),

    shader_pool(std::move(other.shader_pool)),
    
    render_pass(std::move(other.render_pass)),

    pipeline_cache(std::move(other.pipeline_cache)),
    pipeline_constructor(std::move(other.pipeline_constructor)),
    pipelines(other.pipelines),

    frame_manager(other.frame_manager)
{
    // Prevent the frame manager from being deallocated
    other.pipelines.clear();
    other.frame_manager = nullptr;
}

/* Destructor for the RenderSystem class. */
RenderSystem::~RenderSystem() {
    logger.logc(Verbosity::important, RenderSystem::channel, "Cleaning...");

    // Deallocate the frame manager if needed
    if (this->frame_manager != nullptr) {
        delete this->frame_manager;
    }
    // Deallocate the pipelines
    if (!this->pipelines.empty()) {
        for (const auto& p : this->pipelines) {
            delete p.second;
        }
    }

    logger.logc(Verbosity::important, RenderSystem::channel, "Cleaned.");
}



/* Private helper function that resizes all required structures for a new window size. */
void RenderSystem::_resize() {
    // First, wait until the device is idle
    this->window.gpu().wait_for_idle();

    // Then, resize the window
    this->window.resize();

    // Re-create the depth stencil with a new size
    logger.debug("Window size: ", this->window.real_extent().width, 'x', this->window.real_extent().height);
    this->depth_stencil.resize(this->window.real_extent());

    // Re-create all frames in the frame manager
    this->frame_manager->bind(this->render_pass, this->depth_stencil);

    // Update the pipeline constructor with the new viewport data
    this->pipeline_constructor.viewport_transformation.viewport.width  = (float) this->window.swapchain().extent().width;
    this->pipeline_constructor.viewport_transformation.viewport.height = (float) this->window.swapchain().extent().height;
    this->pipeline_constructor.viewport_transformation.scissor.extent = this->window.swapchain().extent();

    // Recreate our pipelines
    for (auto& p : this->pipelines) {
        // Update the constructo to this type's preferences
        this->material_system.init_props(p.first, this->shader_pool, this->pipeline_constructor);
        // Set the current pipeline as the base
        this->pipeline_constructor.set_base_pipeline(p.second);
        
        // Now, recreate it
        Pipeline* new_pipeline = this->pipeline_constructor.construct(this->render_pass, 0);
        // Replace it with the current one
        delete p.second;
        p.second = new_pipeline;
    }
}



/* Runs a single iteration of the game loop. Returns whether or not the RenderSystem is asked to close the window (false) or not (true). */
bool RenderSystem::render_frame(const ECS::EntityManager& entity_manager) {
    /* PREPARATION */
    // First, handle window events
    bool can_continue = this->window.loop();
    if (!can_continue) {
        return false;
    }

    // Next, get a conceptual frame to render to
    ConceptualFrame* frame = this->frame_manager->get_frame();
    if (frame == nullptr) {
        // Resize, then stop this iteration
        this->_resize();
        return true;
    }

    // Prepare rendering to the frame
    const ECS::ComponentList<ECS::Model>& entities = entity_manager.get_list<ECS::Model>();
    frame->prepare_render(this->material_system.size(), entities.size());

    // Sort the objects by material type
    std::unordered_map<Materials::MaterialType, std::unordered_map<Materials::material_t, Tools::Array<IndexBufferRenderData>>> sorted_entities = sort_entities(this->material_system, entities);

    // Populate the frame's camera data
    const Camera& cam = entity_manager.get_list<Camera>()[0];
    frame->upload_camera_data(cam.proj, cam.view);

    // Populate the object datas in advance
    for (uint32_t i = 0; i < entities.size(); i++) {
        // Get the entity's transform data
        ECS::entity_t entity = entities.get_entity(i);
        const ECS::Transform& transform = entity_manager.get_component<ECS::Transform>(entity);

        // Upload it to the GPU
        frame->upload_entity_data(entity, EntityData{ transform.translation });
    }





    /* RECORDING */
    // Start recording the frame's command buffer
    frame->schedule_start();

    // Loop through all present material types
    for (const auto& material_types : sorted_entities) {
        // Schedule the pipeline for this material
        frame->schedule_pipeline(this->pipelines.at(material_types.first));
        // Schedule the frame global data on it
        frame->schedule_global();

        // Get the list of data for this material type
        const Tools::AssociativeArray<Materials::material_t, MaterialData>& material_data = this->material_system.get_list(material_types.first);

        // Loop through all specific materials for this type
        for (const auto& materials : material_types.second) {
            // Upload & schedule the data for this material
            frame->upload_material_data(materials.first, material_data.get(materials.first));
            frame->schedule_material(materials.first);

            // Next, loop through all index buffers of this material to render them
            for (uint32_t i = 0; i < materials.second.size(); i++) {
                // Get a shortcut to the data we'll need
                const IndexBufferRenderData& render_data = materials.second[i];

                // Schedule the object data & its vertex buffer
                frame->schedule_entity(render_data.entity);
                frame->schedule_vertex_buffer(render_data.vertex_buffer);

                // Draw the given index buffer
                frame->schedule_draw(render_data.index_buffer, render_data.n_indices);
            }
        }
    }

    // Close off recording
    frame->schedule_stop();





    /* PRESENTING */
    // 'Render' the frame by submitting it
    VkQueue graphics_queue = this->window.gpu().queues(QueueType::graphics)[0];
    frame->submit(graphics_queue);

    // Schedule the frame for presentation once it's done rendering
    if (this->window.needs_resize() || this->frame_manager->present_frame(frame)) {
        // Resize the window
        this->_resize();
    }





    /* RETURNING */
    return true;
    // return false;
}



/* Swap operator for the RenderSystem class. */
void Rendering::swap(RenderSystem& rs1, RenderSystem& rs2) {
    #ifndef NDEBUG
    if (&rs1.window != &rs2.window) { logger.fatalc(RenderSystem::channel, "Cannot swap render systems with different windows"); }
    if (&rs1.memory_manager != &rs2.memory_manager) { logger.fatalc(RenderSystem::channel, "Cannot swap render systems with different memory managers"); }
    if (&rs1.material_system != &rs2.material_system) { logger.fatalc(RenderSystem::channel, "Cannot swap render systems with different material systems"); }
    if (&rs1.model_system != &rs2.model_system) { logger.fatalc(RenderSystem::channel, "Cannot swap render systems with different model systems"); }
    // if (&rs1.texture_system != &rs2.texture_system) { logger.fatalc(RenderSystem::channel, "Cannot swap render systems with different texture systems"); }
    #endif

    // Simply swap everything
    using std::swap;

    swap(rs1.global_descriptor_layout, rs2.global_descriptor_layout);
    swap(rs1.material_descriptor_layout, rs2.material_descriptor_layout);
    swap(rs1.object_descriptor_layout, rs2.object_descriptor_layout);

    swap(rs1.depth_stencil, rs2.depth_stencil);

    swap(rs1.shader_pool, rs2.shader_pool);

    swap(rs1.render_pass, rs2.render_pass);

    swap(rs1.pipeline_cache, rs2.pipeline_cache);
    swap(rs1.pipeline_constructor, rs2.pipeline_constructor);
    swap(rs1.pipelines, rs2.pipelines);

    swap(rs1.frame_manager, rs2.frame_manager);
}
